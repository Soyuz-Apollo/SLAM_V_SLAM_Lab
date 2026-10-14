# Zurich Urban MAV Dataset

## Коротко

**Zurich Urban MAV** - городской датасет с micro aerial vehicle, летящим на малой высоте по улицам Цюриха.

## Характеристики

- **Платформа:** MAV / небольшой летательный аппарат.
- **Среда:** outdoor urban.
- **Сенсоры:** high-resolution aerial images, GPS, IMU, ground-level street-view images.
- **Камера:** GoPro Hero 4, rolling shutter.
- **Масштаб:** около 2 км, полный датасет примерно 28 GB.
- **Типичные задачи:** appearance-based localization, visual odometry, SLAM, online 3D reconstruction.
- **Сильная сторона:** реальная городская среда для MAV.
- **Ограничение:** rolling shutter, aerial viewpoint; не автомобильный и не indoor датасет.

## Официальная ссылка

- https://rpg.ifi.uzh.ch/zurichmavdataset.html

## Скачивание

На официальной странице доступны:

- sample dataset около 200 MB;
- entire dataset около 28 GB.

Ручной путь:

1. Открыть https://rpg.ifi.uzh.ch/zurichmavdataset.html
2. В разделе `Download` скачать sample или full ZIP.
3. Распаковать:

```bash
mkdir -p ~/datasets/zurich_urban_mav
cd ~/datasets/zurich_urban_mav
unzip /path/to/zurich_mav_dataset.zip
```

## Практическая заметка

Хороший датасет для визуальной навигации дронов в городе. Для колесного робота или RoboCross полезен скорее концептуально, чем напрямую.
