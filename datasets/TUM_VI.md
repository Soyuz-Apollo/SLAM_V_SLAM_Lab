# TUM VI Dataset

## Коротко

**TUM VI** - benchmark для visual-inertial odometry. Он сложнее и разнообразнее EuRoC: есть комнаты, коридоры, большие холлы и outdoor-последовательности.

## Характеристики

- **Платформа:** ручная sensor rig.
- **Среда:** indoor и outdoor.
- **Сенсоры:** стерео grayscale камеры, IMU.
- **Камеры:** изображения 1024x1024, 20 Hz.
- **IMU:** 200 Hz.
- **Ground truth:** motion capture; полный ground truth есть для `room`, для многих длинных/outdoor-последовательностей точные позы есть только в начале и конце.
- **Типичные задачи:** VIO, visual-inertial SLAM, проверка устойчивости на длинных траекториях.
- **Сильная сторона:** более реалистичные и длинные траектории, чем EuRoC.
- **Ограничение:** не для всех последовательностей есть полный ground truth.

## Официальная ссылка

- https://cvg.cit.tum.de/data/datasets/visual-inertial-dataset

## Скачивание

Официальная страница предлагает несколько форматов:

- `Bag 1024x1024`
- `Bag 512x512`
- `Euroc/DSO 1024x1024`
- `Euroc/DSO 512x512`

Пример массового скачивания папки из официальной инструкции:

```bash
mkdir -p ~/datasets/tumvi
cd ~/datasets/tumvi

wget -R "index.*" -m -np -nH --no-check-certificate -e robots=off \
  https://cdn3.vision.in.tum.de/tumvi/exported/euroc/512_16/
```

Проверка md5, если рядом скачались `.md5`:

```bash
cd tumvi/exported/euroc/512_16
md5sum -c *.md5
```

## Практическая заметка

Если алгоритм уже умеет читать EuRoC-формат, проще начать с `Euroc/DSO 512x512`. Для тяжелых экспериментов можно переходить на 1024x1024.
