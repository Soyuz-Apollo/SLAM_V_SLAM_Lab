---
aliases:
  - Запуск MonoSLAM
  - Сборка MonoSLAM на macOS
tags:
  - MonoSLAM
  - SLAM
  - OpenCV
  - macOS
---

# MonoSLAM: сборка, калибровка и запуск

## Связанные заметки

- [[Davison_2007_MonoSLAM_translation_ru|Перевод статьи Davison et al. 2007]]
- [[MonoSLAM_code_study_ru|Изучение кода MonoSLAM]]
- [[Tello_MonoSLAM_test_plan_ru|План тестирования с DJI Tello]]

## Используемые параметры

Для калибровки используется распечатанная шахматная доска:

| Параметр | Значение |
|---|---:|
| Количество клеток по вертикали | 8 |
| Количество клеток по горизонтали | 8 |
| Сторона клетки | 33,5 мм |
| Сторона клетки в метрах | 0,0335 м |
| Количество внутренних углов | 7 x 7 |

> [!important]
> Программе передаётся количество **клеток**, а не количество внутренних углов.
> Для доски `8 x 8` в командной строке указывается `8 8`, а OpenCV внутри
> программы ищет `7 x 7` внутренних углов.

## Расположение проекта

```text
papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy
```

Основные программы после сборки:

```text
cmake-build-debug/bin/calibration
cmake-build-debug/bin/monoslam
```

- `calibration` определяет внутренние параметры камеры и коэффициенты искажения;
- `monoslam` запускает алгоритм и использует шахматную доску для начальной позы,
  начала мировой системы координат и метрического масштаба.

## 1. Переход в папку проекта

```bash
cd papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy
```

Все последующие команды выполняются из этой папки.

## 2. Проверка зависимостей

```bash
cmake --version
brew --prefix opencv@4
```

Если CMake или OpenCV 4 ещё не установлены:

```bash
brew install cmake opencv@4
```

## 3. Настройка Debug-сборки

```bash
cmake -S . -B cmake-build-debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$(brew --prefix opencv@4)"
```

Обозначения:

- `-S .` — исходный код находится в текущей папке;
- `-B cmake-build-debug` — файлы сборки помещаются в отдельную папку;
- `CMAKE_BUILD_TYPE=Debug` — сборка с отладочной информацией;
- `CMAKE_PREFIX_PATH` — путь, по которому CMake найдёт OpenCV 4.

## 4. Сборка программ

Собрать калибратор и MonoSLAM:

```bash
cmake --build cmake-build-debug \
  --target calibration monoslam \
  -j "$(sysctl -n hw.logicalcpu)"
```

Собрать только калибратор:

```bash
cmake --build cmake-build-debug --target calibration
```

Собрать только MonoSLAM:

```bash
cmake --build cmake-build-debug --target monoslam
```

Проверить допустимые аргументы:

```bash
./cmake-build-debug/bin/calibration --help
./cmake-build-debug/bin/monoslam -h
```

Код возврата `255` после вывода справки для этих программ является ожидаемым.

## 5. Калибровка камеры Mac

### Формат команды

```text
calibration rows cols square_size [tolerance] output_file
```

Где:

- `rows` — количество клеток по вертикали;
- `cols` — количество клеток по горизонтали;
- `square_size` — сторона клетки в метрах;
- `tolerance` — допустимая ошибка перепроекции отдельного кадра в пикселях;
- `output_file` — имя файла без обязательного расширения `.xml`.

### Команда для нашей доски

```bash
./cmake-build-debug/bin/calibration \
  8 8 0.0335 1.5 \
  papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm
```

Программа автоматически добавит расширение `.xml`. Результат должен появиться
здесь:

```text
papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm.xml
```

### Как снимать доску

1. Держать доску ровной: бумага не должна изгибаться.
2. Показать её в центре изображения и около краёв кадра.
3. Менять расстояние до камеры.
4. Наклонять доску по горизонтальной и вертикальной осям.
5. Избегать смазанных кадров, бликов и слишком сильных наклонов.
6. Набрать примерно 15–20 успешно распознанных положений.

При успешном распознавании в терминале увеличивается счётчик:

```text
1 samples captured
2 samples captured
3 samples captured
```

После 20 принятых кадров калибровка запускается автоматически. Завершить сбор
раньше можно клавишей `S`, `Enter` или `Esc`, когда активно окно камеры.

> [!warning]
> Если счётчик `samples captured` не увеличивается, OpenCV не распознаёт доску.
> В этом случае файл калибровки создать невозможно. Нужно проверить число клеток,
> освещение, резкость и то, что вся доска находится в кадре.

## 6. Проверка файла калибровки

```bash
ls -lh papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm.xml
```

Посмотреть содержимое:

```bash
sed -n '1,100p' \
  papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm.xml
```

В файле должны присутствовать поля:

```text
Image_Size
Intrinsic_Matrix
Distortion_Coefficients
```

В конце работы калибратор также выводит среднеквадратичную ошибку перепроекции:

```text
RMS reprojection error (pixels): ...
```

Желательно получить значение меньше `1` пикселя. Для первого эксперимента может
подойти и немного большее значение, но высокая ошибка ухудшит проекцию признаков
и работу MonoSLAM.

## 7. Запуск MonoSLAM

### Формат команды

```text
monoslam camera_file pattern_rows pattern_cols square_size
```

### Команда для нашей камеры и доски

```bash
./cmake-build-debug/bin/monoslam \
  papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm.xml \
  8 8 0.0335
```

### Порядок запуска

1. До запуска расположить шахматную доску целиком в поле зрения камеры.
2. Запустить команду.
3. Дождаться распознавания доски и инициализации карты.
4. После инициализации доску можно убрать.
5. Перемещать камеру плавно, сохраняя в кадре текстурированные неподвижные объекты.
6. Для завершения нажать `Esc` в окне MonoSLAM.

На начальном кадре доска задаёт:

- известные трёхмерные координаты первых ориентиров;
- начальную позу камеры относительно доски;
- начало мировой системы координат;
- метрический масштаб карты, полученный из размера клетки `0,0335` м.

## 8. Быстрая Release-сборка

После проверки Debug-варианта можно собрать оптимизированную версию:

```bash
cmake -S . -B cmake-build-release \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$(brew --prefix opencv@4)"

cmake --build cmake-build-release \
  --target calibration monoslam \
  -j "$(sysctl -n hw.logicalcpu)"
```

Запуск Release-версии:

```bash
./cmake-build-release/bin/monoslam \
  papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm.xml \
  8 8 0.0335
```

## 9. Конфигурации запуска в CLion

### Калибровка

Цель:

```text
calibration
```

Program arguments:

```text
8 8 0.0335 1.5 papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm
```

### MonoSLAM

Цель:

```text
monoslam
```

Program arguments:

```text
papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm.xml 8 8 0.0335
```

Рабочая папка для обеих конфигураций:

```text
papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy
```

## 10. Типичные проблемы

### Не создаётся XML-файл

Проверить, появляется ли сообщение `samples captured`.

- Счётчика нет — доска не распознаётся. Для нашей доски должны быть аргументы
  `8 8`, соответствующие `7 x 7` внутренним углам.
- Счётчик есть — дождаться автоматического сохранения после 20 кадров либо нажать
  `S`, `Enter` или `Esc` в активном окне камеры.
- Использовать абсолютный путь к выходному файлу, как в приведённой команде.
- После сохранения найти строку `Camera settings saved to ...`.

### Камера не открывается

В коде используется первая камера системы:

```cpp
VideoCapture cap(0);
```

Нужно разрешить доступ к камере для Terminal или CLion в настройках конфиденциальности
macOS. Также следует закрыть другие приложения, которые могут занимать камеру.

### Ошибка `_src.empty()` в `cvtColor`

Сообщение вида:

```text
(-215:Assertion failed) !_src.empty() in function 'cvtColor'
```

означает, что камера открылась, но не передала очередной кадр. В текущую версию
кода добавлены проверка пустых кадров и 30 повторных попыток чтения. После изменения
нужно обязательно пересобрать обе программы:

```bash
cmake --build cmake-build-debug \
  --target calibration monoslam \
  -j "$(sysctl -n hw.logicalcpu)"
```

Если после пересборки камера всё равно не передаёт кадры, следует закрыть программы,
использующие камеру, и проверить разрешение на доступ к камере для Terminal или CLion.

### MonoSLAM не начинает работу

- Убедиться, что XML-файл существует и содержит параметры камеры.
- Показать доску полностью и без сильного наклона.
- Использовать те же параметры `8 8 0.0335`, что и при калибровке.
- Обеспечить равномерное освещение и резкое изображение.

### После инициализации быстро теряются признаки

- Перемещать камеру медленнее.
- Избегать однотонных стен и бликов.
- Направлять камеру на неподвижную сцену с углами и текстурой.
- Не закрывать большую часть изображения рукой или доской.

## Краткая последовательность команд

```bash
cd papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy

cmake -S . -B cmake-build-debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$(brew --prefix opencv@4)"

cmake --build cmake-build-debug \
  --target calibration monoslam \
  -j "$(sysctl -n hw.logicalcpu)"

./cmake-build-debug/bin/calibration \
  8 8 0.0335 1.5 \
  papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm

ls -lh papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm.xml

./cmake-build-debug/bin/monoslam \
  papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/mac_camera_33_5mm.xml \
  8 8 0.0335
```
