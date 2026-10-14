# Команды сборки, калибровки и запуска MonoSLAM

## 1. Перейти в проект

```bash
cd papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy
```

## 2. Проверить зависимости

```bash
cmake --version
brew --prefix opencv@4
```

Если команда для OpenCV сообщает, что пакет не установлен:

```bash
brew install cmake opencv@4
```

## 3. Настроить и собрать проект

```bash
cmake -S . -B cmake-build-debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$(brew --prefix opencv@4)"

cmake --build cmake-build-debug \
  --target calibration monoslam \
  -j "$(sysctl -n hw.logicalcpu)"
```

Проверка аргументов программ:

```bash
./cmake-build-debug/bin/calibration --help
./cmake-build-debug/bin/monoslam -h
```

Обе справочные команды завершаются с кодом `255` — для этого кода это нормальное
поведение, а не ошибка сборки.

## 4. Задать размеры шахматной доски

Программа ожидает количество **клеток**, а не количество внутренних углов.

Для используемой доски из 8 клеток по вертикали и 8 клеток по горизонтали:

```bash
ROWS=8
COLS=8
SQUARE_SIZE=0.0335
```

`0.0335` метра — это 33,5 миллиметра. Для такой доски OpenCV будет искать
`(COLS - 1) x (ROWS - 1)`, то есть `7 x 7` внутренних углов.

Если размеры доски другие, нужно изменить только `ROWS` и `COLS`.

## 5. Откалибровать камеру Mac

```bash
./cmake-build-debug/bin/calibration \
  "$ROWS" "$COLS" "$SQUARE_SIZE" \
  1.5 papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm
```

Здесь `1.5` — допустимая ошибка перепроецирования одного снимка в пикселях.
Программа сама добавит расширение `.xml` и создаст файл:

```text
papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm.xml
```

Во время калибровки:

1. Показывать доску камере под разными углами и на разных расстояниях.
2. Добиться примерно 15–25 успешно принятых кадров.
3. Доска должна быть плоской, резкой и целиком попадать в изображение.
4. После набора кадров нажать `S`, `Enter` или `Esc` в окне камеры.
5. Не завершать процесс кнопкой `Stop` в CLion: она убивает программу до сохранения.

После 20 успешно принятых кадров программа завершит сбор и сохранит файл
автоматически.

Проверить, что файл создан:

```bash
ls -lh mac_camera_33_5mm.xml
sed -n '1,80p' mac_camera_33_5mm.xml
```

В конце калибровки программа напечатает `RMS reprojection error`. Чем он меньше,
тем лучше; ориентир для первого теста — меньше 1 пикселя, хотя результат немного
выше 1 пикселя тоже может быть пригоден.

## 6. Запустить MonoSLAM

```bash
./cmake-build-debug/bin/monoslam \
  papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm.xml \
  "$ROWS" "$COLS" "$SQUARE_SIZE"
```

При запуске нужно снова показать камере ту же доску. Алгоритм использует её для
начальной позы, начала мировой системы координат и метрического масштаба. После
инициализации доску можно убрать и плавно перемещать камеру. Для завершения
нажать `Esc`.

## 7. Сборка Release для более быстрого запуска

Когда Debug-вариант заработает:

```bash
cmake -S . -B cmake-build-release \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$(brew --prefix opencv@4)"

cmake --build cmake-build-release \
  --target calibration monoslam \
  -j "$(sysctl -n hw.logicalcpu)"
```

Запуск уже скомпилированной Release-версии:

```bash
./cmake-build-release/bin/monoslam \
  mac_camera_33_5mm.xml \
  "$ROWS" "$COLS" "$SQUARE_SIZE"
```

## Аргументы для CLion

Рабочая папка обеих конфигураций:

```text
papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy
```

Для цели `calibration`:

```text
8 8 0.0335 1.5 papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm
```

Для цели `monoslam`:

```text
papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm.xml 8 8 0.0335
```

Камера в обеих программах сейчас жёстко задана как устройство `0`:
`VideoCapture(0)`. При первом запуске macOS может запросить доступ к камере для
Terminal или CLion.
