# DRE Dataset

## Коротко

**DRE dataset** - датасет авторов DRE-SLAM для differential-drive робота Redbot в динамических indoor-сценах.

## Характеристики

- **Платформа:** Redbot, differential-drive robot.
- **Среда:** indoor.
- **Сенсоры:** RGB-D камера Kinect 2.0, wheel encoders.
- **Динамика:** есть low dynamic и high dynamic последовательности.
- **Ground truth:** downview camera + markers, точность в статье явно не стандартизирована как у TUM/EuRoC.
- **Типичные задачи:** dynamic RGB-D SLAM, SLAM с колесными энкодерами, differential-drive robot.
- **Сильная сторона:** есть колесные энкодеры, что редко встречается в популярных visual SLAM датасетах.
- **Ограничение:** скачивание через Dropbox/Baidu; алгоритм и датасет старые, завязаны на ROS Kinetic / Ubuntu 16.04.

## Официальные ссылки

- GitHub DRE-SLAM: https://github.com/ydsf16/dre_slam
- Статья: https://www.mdpi.com/2072-4292/11/4/380

## Скачивание

На GitHub авторы пишут, что датасет доступен через Pan.Baidu или Dropbox.

Ручной путь:

1. Открыть https://github.com/ydsf16/dre_slam
2. Найти раздел `Example -> Dataset`.
3. Перейти по ссылке Dropbox или Pan.Baidu.
4. Скачать `.bag` файлы.
5. Запуск примера:

```bash
roslaunch dre_slam comparative_test.launch
rosbag play <bag_name>.bag
```

## Практическая заметка

Интересен именно для dynamic SLAM и роботов с колесной одометрией. Для современного ROS2-проекта потребует портирования или конвертации.
