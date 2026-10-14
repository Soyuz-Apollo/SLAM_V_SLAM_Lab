# Segway DRIVE Benchmark

## Коротко

**Segway DRIVE** - benchmark, собранный fleet delivery-роботами Segway в реальных офисах и торговых центрах.

## Характеристики

- **Платформа:** Segway delivery robots.
- **Среда:** indoor, реальные офисные здания и shopping malls.
- **Сенсоры:** global-shutter fisheye camera, consumer-grade IMU, wheel encoders, removable high-precision lidar.
- **Масштаб:** первая публикация описывает сотни последовательностей и более 50 км indoor floors.
- **Типичные задачи:** place recognition, SLAM, indoor robot navigation.
- **Сильная сторона:** реальные сервисные роботы, динамичные сцены с пешеходами.
- **Ограничение:** официальная download-страница может быть нестабильной/недоступной; источник лучше проверять вручную.

## Официальные ссылки

- Страница статьи arXiv: https://arxiv.org/abs/1907.03424
- Указанная авторами download-ссылка: http://drive.segwayrobotics.com/#/dataset/download

## Скачивание

Авторы статьи указывают download URL:

```text
http://drive.segwayrobotics.com/#/dataset/download
```

Рекомендуемый путь:

1. Открыть ссылку в браузере.
2. Проверить доступность download-раздела.
3. Скачать нужные sequences.
4. Если сайт недоступен, искать mirror по названию статьи или писать авторам.

## Практическая заметка

По смыслу датасет очень подходит для indoor service robot SLAM, но доступность данных надо проверять отдельно. Это не такой “железобетонный” источник, как KITTI или TUM.
