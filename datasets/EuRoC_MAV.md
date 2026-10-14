# EuRoC MAV Dataset

## Коротко

**EuRoC MAV** - один из самых популярных датасетов для visual-inertial odometry и visual SLAM. Данные записаны на micro aerial vehicle, то есть небольшом летательном аппарате, в помещениях.

## Характеристики

- **Платформа:** MAV / квадрокоптер.
- **Среда:** indoor.
- **Сенсоры:** стерео monochrome камеры, IMU.
- **Ground truth:** высокоточная система motion capture / laser tracker, в зависимости от последовательности.
- **Типичные задачи:** visual-inertial odometry, stereo SLAM, monocular SLAM, калибровка.
- **Сильная сторона:** стандартный benchmark, много алгоритмов уже имеют конфиги под EuRoC.
- **Ограничение:** данные в основном про полеты в помещении; для колесного робота и уличной навигации переносимость ограничена.

## Официальные ссылки

- Список датасетов ETH ASL: https://projects.asl.ethz.ch/datasets/
- Страница EuRoC MAV в ETH Research Collection: https://doi.org/10.3929/ethz-b-000690084

## Скачивание

На странице ETH Research Collection доступны крупные ZIP-архивы:

- `Machine Hall Datasets`
- `Vicon Room 1 Datasets`
- `Vicon Room 2 Datasets`
- `Calibration Datasets`

Самый надежный способ:

1. Открыть https://doi.org/10.3929/ethz-b-000690084
2. Скачать нужный ZIP из блока `Downloads`.
3. Распаковать:

```bash
mkdir -p ~/datasets/euroc
cd ~/datasets/euroc
unzip /path/to/downloaded_archive.zip
```

## Практическая заметка

Для первого теста Visual-Inertial SLAM обычно берут последовательности `MH_01_easy`, `V1_01_easy` или `V2_01_easy`. Для ORB-SLAM3, VINS, OpenVINS, Basalt и Kimera почти всегда есть готовые конфиги под EuRoC.
