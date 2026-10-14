# Настройка проекта в CLion

## Что открывать

В CLion нужно открыть папку:

```text
papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/github_marcromani_MonoSLAM
```

В этой папке лежит `CMakeLists.txt`, поэтому CLion должен автоматически увидеть CMake-проект.

## Цели сборки

После индексации в CLion должны появиться две цели:

- `calibration` — калибровка камеры по шахматному шаблону;
- `monoslam` — запуск MonoSLAM.

## Аргументы запуска

Для `calibration` формат аргументов такой:

```text
rows cols square_size [tol] filename
```

Пример:

```text
8 11 0.025 camera_mac
```

После калибровки будет создан файл `camera_mac.xml`.

Для `monoslam` формат аргументов такой:

```text
cameraFile patternRows patternCols squareSize
```

Пример:

```text
camera_mac.xml 8 11 0.025
```

Значения `rows`, `cols` и `squareSize` должны соответствовать реальному шахматному шаблону. Размер клетки задаётся в метрах.

## Если CLion не нашёл OpenCV

На этом Mac OpenCV 4 установлен через Homebrew:

```text
/opt/homebrew/opt/opencv@4
```

Если CLion попросит указать CMake options, можно добавить:

```text
-DCMAKE_PREFIX_PATH=/opt/homebrew/opt/opencv@4
```

## Проверка из терминала

```bash
cd papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/github_marcromani_MonoSLAM
cmake --preset macos-clion-debug
cmake --build --preset macos-clion-debug
```

Собранные бинарники появятся здесь:

```text
cmake-build-debug/bin/calibration
cmake-build-debug/bin/monoslam
```
