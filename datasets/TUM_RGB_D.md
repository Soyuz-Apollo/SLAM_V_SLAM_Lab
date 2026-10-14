# TUM RGB-D SLAM Dataset

## Коротко

**TUM RGB-D** - классический датасет для visual odometry и RGB-D SLAM. Данные записаны Microsoft Kinect: цветное изображение + карта глубины + ground truth.

## Характеристики

- **Платформа:** ручная RGB-D камера.
- **Среда:** indoor.
- **Сенсоры:** Microsoft Kinect, RGB + depth, accelerometer.
- **Частота:** 30 Hz.
- **Разрешение:** 640x480.
- **Ground truth:** motion capture, 100 Hz.
- **Типичные задачи:** RGB-D SLAM, visual odometry, оценка траектории.
- **Сильная сторона:** простой формат, много инструментов оценки, удобен для первых экспериментов.
- **Ограничение:** старый Kinect, indoor-only, не про автономное вождение.

## Официальные ссылки

- Основная страница: https://cvg.cit.tum.de/data/datasets/rgbd-dataset
- Страница скачивания: https://cvg.cit.tum.de/data/datasets/rgbd-dataset/download
- Форматы файлов: https://cvg.cit.tum.de/data/datasets/rgbd-dataset/file_formats
- Инструменты: https://cvg.cit.tum.de/data/datasets/rgbd-dataset/tools

## Скачивание

Ручной путь:

1. Открыть https://cvg.cit.tum.de/data/datasets/rgbd-dataset/download
2. Выбрать последовательность, например `freiburg1`, `freiburg2` или `freiburg3`.
3. Скачать `.tgz` архив.
4. Распаковать:

```bash
mkdir -p ~/datasets/tum_rgbd
cd ~/datasets/tum_rgbd
tar -xzf /path/to/rgbd_dataset_freiburg*_*.tgz
```

## Практическая заметка

Для RGB-D алгоритмов это один из самых удобных стартовых датасетов. Для Visual SLAM без depth можно использовать только RGB-часть, но смысл датасета именно в наличии глубины.
