# RPNG OpenVINS Dataset

## Коротко

**RPNG OpenVINS Dataset** - набор датасетов, используемых авторами OpenVINS для тестирования visual-inertial estimation.

## Характеристики

- **Платформа:** разные visual-inertial sensor rigs.
- **Среда:** indoor и outdoor.
- **Сенсоры:** камеры + IMU; в части последовательностей есть GPS / ArUco / ground truth.
- **Типичные задачи:** VIO, тестирование OpenVINS, visual-inertial estimation.
- **Сильная сторона:** хорошо подходит для OpenVINS, есть готовые конфиги.
- **Ограничение:** это не один единый большой датасет, а набор поддерживаемых OpenVINS последовательностей и форматов.

## Официальные ссылки

- OpenVINS docs: https://docs.openvins.com/
- Supported datasets: https://docs.openvins.com/gs-datasets.html
- GitHub: https://github.com/rpng/open_vins

## Скачивание

Обычно данные скачиваются по ссылкам из документации OpenVINS для конкретного датасета.

Пример общего процесса:

```bash
mkdir -p ~/datasets/openvins
cd ~/datasets/openvins

# затем открыть docs и скачать нужный rosbag по ссылке
# https://docs.openvins.com/gs-datasets.html
```

Если работаешь именно с OpenVINS:

```bash
git clone https://github.com/rpng/open_vins.git
```

Дальше смотреть `config/` и `ov_data/` под выбранный датасет.

## Практическая заметка

Для изучения VIO полезнее как companion-набор к OpenVINS, чем как самостоятельный benchmark.
