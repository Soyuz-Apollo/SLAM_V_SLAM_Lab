# KAIST Complex Urban Dataset

## Коротко

**KAIST Complex Urban Dataset** - автомобильный датасет для сложной городской среды с большим количеством сенсоров.

## Характеристики

- **Платформа:** автомобиль.
- **Среда:** complex urban outdoor: город, много полос, здания, динамические объекты.
- **Сенсоры:** 2D/3D LiDAR, stereo camera, GPS, VRS-GPS, IMU, fiber optic gyro, wheel encoders, altimeter.
- **Ground truth / reference:** pose information / baseline trajectories, полученные с использованием SLAM и навигационных сенсоров.
- **Типичные задачи:** autonomous driving, localization, lidar SLAM, visual-lidar SLAM, sensor fusion.
- **Сильная сторона:** очень богатый multi-sensor setup.
- **Ограничение:** крупные данные; лицензия некоммерческая CC BY-NC-SA 4.0.

## Официальные ссылки

- Основная страница: https://sites.google.com/view/complex-urban-dataset
- Download LiDAR: https://sites.google.com/view/complex-urban-dataset/download-lidar
- Download LiDAR + Stereo: https://sites.google.com/view/complex-urban-dataset/download-lidar-stereo

## Скачивание

Ручной путь:

1. Открыть основную страницу.
2. Перейти в `Download (LiDAR)` или `Download (LiDAR + Stereo)`.
3. Скачать нужные последовательности.
4. Распаковать в отдельную папку:

```bash
mkdir -p ~/datasets/kaist_complex_urban
cd ~/datasets/kaist_complex_urban
unzip /path/to/urban_sequence.zip
```

## Практическая заметка

Если нужен датасет именно для городской автономной навигации с несколькими типами сенсоров, KAIST гораздо ближе к реальной машине, чем EuRoC или TUM RGB-D.
