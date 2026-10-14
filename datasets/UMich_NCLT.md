# University of Michigan NCLT Dataset

## Коротко

**NCLT** - North Campus Long-Term Vision and LIDAR Dataset. Это большой долгосрочный датасет, собранный Segway-роботом на кампусе University of Michigan.

## Характеристики

- **Платформа:** Segway robot.
- **Среда:** indoor и outdoor на кампусе.
- **Сенсоры:** Ladybug3 omnidirectional camera, Velodyne HDL-32E, Hokuyo lidars, Microstrain IMU, KVH fiber optic gyro, wheel odometry, GPS/RTK GPS.
- **Масштаб:** 27 sessions, около 15 месяцев, около 147.4 км.
- **Ground truth:** pose estimates / ground truth для всех сессий в общей системе координат.
- **Типичные задачи:** long-term SLAM, place recognition, lidar SLAM, visual-lidar localization.
- **Сильная сторона:** долгосрочные сезонные и погодные изменения, много сенсоров.
- **Ограничение:** очень большие данные; одна сессия может занимать десятки/сотни GB.

## Официальная ссылка

- https://robots.engin.umich.edu/nclt/index.html

## Скачивание

На официальной странице есть таблица по сессиям. Для каждой сессии отдельно доступны:

- images;
- sensors;
- Velodyne;
- Hokuyo;
- ground truth pose;
- covariance.

Пример ручного процесса:

1. Открыть https://robots.engin.umich.edu/nclt/index.html
2. Выбрать дату сессии.
3. Скачать только нужные компоненты, например `sen.tar.gz`, `vel.tar.gz`, `groundtruth.csv`.
4. Распаковать:

```bash
mkdir -p ~/datasets/nclt/2012-01-08
cd ~/datasets/nclt/2012-01-08
tar -xzf /path/to/sen.tar.gz
tar -xzf /path/to/vel.tar.gz
```

## Практическая заметка

Не стоит качать весь NCLT “просто посмотреть”: объем огромный. Лучше выбрать одну дату и один-два sensor streams.
