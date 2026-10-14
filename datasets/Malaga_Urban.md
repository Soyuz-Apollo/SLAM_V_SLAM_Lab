# Malaga Urban Dataset

## Коротко

**Malaga Urban Dataset** - городской автомобильный датасет со стереокамерой и несколькими лазерными сканерами.

## Характеристики

- **Платформа:** автомобиль.
- **Среда:** outdoor urban.
- **Сенсоры:** stereo camera Bumblebee2, пять laser scanners, GPS, IMU.
- **Камеры:** stereo images 1024x768, 20 fps.
- **Маршрут:** около 36.8 км.
- **Типичные задачи:** stereo visual odometry, visual SLAM, lidar/stereo fusion.
- **Сильная сторона:** высокочастотные high-resolution stereo images.
- **Ограничение:** старый формат данных; для полного набора нужны большие архивы.

## Официальная ссылка

- https://www.mrpt.org/MalagaUrbanDataset

## Скачивание

На странице MRPT доступны:

- plain text архив;
- raw images;
- rectified images;
- отдельные stereo video files;
- binary files и инструменты парсинга.

Пример:

```bash
mkdir -p ~/datasets/malaga_urban
cd ~/datasets/malaga_urban

# скачать нужный архив со страницы:
# https://www.mrpt.org/MalagaUrbanDataset

unzip /path/to/malaga-urban-dataset-plain-text.zip
```

## Практическая заметка

Если нужен автомобильный stereo dataset, но KITTI не подходит, Malaga может быть полезной альтернативой. Для быстрого теста лучше начать с selected extracts или видео, а не полного архива.
