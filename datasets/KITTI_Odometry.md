# KITTI Odometry Dataset

## Коротко

**KITTI Odometry** - один из главных benchmark-датасетов для visual odometry, lidar odometry и SLAM в автомобильной среде.

## Характеристики

- **Платформа:** автомобиль.
- **Среда:** outdoor: город, сельская местность, автомагистрали.
- **Сенсоры:** стереокамеры, Velodyne lidar, GPS/IMU.
- **Данные:** 22 odometry-последовательности.
- **Ground truth:** доступен для training-последовательностей.
- **Типичные задачи:** stereo visual odometry, lidar SLAM, visual-lidar SLAM, autonomous driving benchmarks.
- **Сильная сторона:** дорожная среда, leaderboard, огромное количество сравнений.
- **Ограничение:** старый, но все еще стандартный benchmark; для некоторых форматов данных нужны отдельные dev kit / parsers.

## Официальные ссылки

- KITTI main: https://www.cvlibs.net/datasets/kitti/
- Odometry benchmark: https://www.cvlibs.net/datasets/kitti/eval_odometry.php

## Скачивание

На странице odometry benchmark доступны архивы:

- grayscale images;
- color images;
- velodyne laser data;
- calibration files;
- ground truth poses;
- development kit.

Ручной путь:

1. Открыть https://www.cvlibs.net/datasets/kitti/eval_odometry.php
2. Скачать нужные архивы из блока download.
3. Распаковать:

```bash
mkdir -p ~/datasets/kitti_odometry
cd ~/datasets/kitti_odometry
unzip /path/to/data_odometry_gray.zip
unzip /path/to/data_odometry_calib.zip
unzip /path/to/data_odometry_poses.zip
```

## Практическая заметка

Для Visual SLAM чаще всего начинают с stereo images + calibration + poses. Для lidar SLAM добавляют `velodyne`.
