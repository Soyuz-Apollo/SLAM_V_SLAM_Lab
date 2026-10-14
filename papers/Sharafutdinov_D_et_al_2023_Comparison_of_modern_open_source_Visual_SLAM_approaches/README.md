# Sharafutdinov et al., 2023: сравнение Visual SLAM

## Статья и пояснения

- [Оригинальный PDF](Sharafutdinov_D_et_al_2023_Comparison_of_modern_open_source_Visual_SLAM_approaches.pdf)
- [[Sharafutdinov_2023_Visual_SLAM_translation_ru|Русский перевод]]
- [[slam_probability_error_derivation_ru|Вероятностная постановка и функция ошибки]]
- [[slam_metrics_explained_ru|Метрики с формулами]]
- [[slam_basic_terms_ground_truth_se3_rms|Основные понятия для оценки траекторий]]

## Датасеты

Общий каталог: [[Datasets_Index]]. Выборки для экспериментов из имеющегося перевода: [[EuRoC_MAV]], [[TUM_VI]], [[KITTI_Odometry]], [[OpenLORIS_Scene]], [[DRE_Dataset]].

Описание датасета не означает, что его записи уже скачаны. Ссылки и условия скачивания требуют проверки перед запуском.

## Воспроизведение

[Код](code/README.md) алгоритмов из сравнения пока не импортирован. Для каждого выбранного алгоритма нужно закрепить исходный commit, окружение, адаптацию датасета и параметры оценки. Результаты сохраняются в [experiments/](experiments/README.md); общие сравнения нескольких статей могут храниться в корневой `experiments/`.

Перевод импортирован в прежнем виде: в нём указано, что точные численные таблицы следует сверять с PDF. Источник файлов: [metadata.json](metadata.json).
