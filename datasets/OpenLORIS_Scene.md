# OpenLORIS-Scene Dataset

## Коротко

**OpenLORIS-Scene** - датасет для оценки SLAM в реалистичных indoor-сценах с изменениями освещения, динамическими объектами и повторными посещениями одних и тех же мест.

## Характеристики

- **Платформа:** реальные сервисные роботы.
- **Среда:** indoor: офис, дом, кафе, коридоры и похожие сцены.
- **Сенсоры:** visual, inertial, odometry, RGB-D / RealSense, lidar в части сцен.
- **Ground truth:** OptiTrack для office-сцены; для других сцен - offline LiDAR SLAM на основе Hokuyo.
- **Типичные задачи:** lifelong SLAM, robust SLAM, scene understanding, проверка динамики и изменений условий.
- **Сильная сторона:** ближе к реальной эксплуатации сервисных роботов, чем стерильные indoor benchmarks.
- **Ограничение:** данные могут быть объемными; часть скачивания идет через Google Drive/Baidu, не всегда удобно автоматизировать.

## Официальные ссылки

- Основная страница: https://lifelong-robotic-vision.github.io/dataset/scene.html
- Download page: https://lifelong-robotic-vision.github.io/dataset/download

## Скачивание

На официальной странице указано, что данные доступны через Google Drive или Baidu Pan.

Ручной путь:

1. Открыть https://lifelong-robotic-vision.github.io/dataset/scene.html
2. Перейти на download page.
3. Выбрать нужную сцену и скачать архивы через Google Drive или Baidu.
4. Распаковать:

```bash
mkdir -p ~/datasets/openloris
cd ~/datasets/openloris
unzip /path/to/openloris_scene_archive.zip
```

Если данные старые, скачанные между Nov 2019 и May 2020, на странице есть scripts `fix_bags.py` / `fix_packages.py`.

## Практическая заметка

Хороший выбор, если надо проверять SLAM не только на “красивом” benchmark, а на сценах с людьми, сменой света и повторными проходами.
