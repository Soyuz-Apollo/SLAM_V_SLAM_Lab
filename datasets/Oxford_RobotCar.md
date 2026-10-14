# Oxford RobotCar Dataset

## Коротко

**Oxford RobotCar** - долгосрочный автомобильный датасет: один и тот же маршрут по Оксфорду записывался более года в разных погодных, световых и дорожных условиях.

## Характеристики

- **Платформа:** автомобиль.
- **Среда:** outdoor urban.
- **Сенсоры:** камеры, lidar, GPS/INS, IMU и другие датчики автомобиля.
- **Масштаб:** более 100 повторов маршрута, около 1000 км.
- **Ground truth:** есть отдельный выпуск RTK ground truth для части задач.
- **Типичные задачи:** long-term localization, place recognition, autonomous driving, robust mapping.
- **Сильная сторона:** долгосрочные изменения, погода, освещение, трафик.
- **Ограничение:** для скачивания требуется регистрация; данные очень большие.

## Официальные ссылки

- Основная страница: https://mrg.robots.ox.ac.uk/
- Dataset / downloads: https://robotcar-dataset.robots.ox.ac.uk/
- SDK: https://github.com/ori-mrg/robotcar-dataset-sdk

## Скачивание

Официальный путь:

1. Открыть https://robotcar-dataset.robots.ox.ac.uk/
2. Зарегистрироваться для доступа к download.
3. Выбрать нужные traversal / sensor streams.
4. Скачать архивы через интерфейс сайта.
5. Для парсинга использовать RobotCar Dataset SDK:

```bash
git clone https://github.com/ori-mrg/robotcar-dataset-sdk.git
```

## Практическая заметка

Это хороший датасет для long-term localization. Для простого первого запуска SLAM он тяжеловат: лучше брать только небольшой sample или одну поездку и ограниченный набор сенсоров.
