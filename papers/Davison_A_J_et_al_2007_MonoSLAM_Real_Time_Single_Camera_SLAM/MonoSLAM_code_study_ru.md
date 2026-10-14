# MonoSLAM — изучение исходного кода

## Что скачано

Папка с кодом:

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy`

Скачаны:

- официальный `SceneLib 1.0`;
- официальный `MonoSLAMGlow`;
- зависимости `VW34` и `GLOW`;
- тестовая последовательность `TestSeqMonoSLAM`;
- GitHub-реализация `marcromani/MonoSLAM`;
- ROS-реализация `rrg-polito/mono-slam`.

Важно: в самой статье ссылки на GitHub нет. В статье указана официальная страница SceneLib:

[https://www.doc.ic.ac.uk/~ajd/Scene/](https://www.doc.ic.ac.uk/~ajd/Scene/)

На странице Andrew Davison отдельно написано, что SceneLib 1.0 уже не поддерживается и сложно собирается на современных системах:

[https://www.doc.ic.ac.uk/~ajd/software.html](https://www.doc.ic.ac.uk/~ajd/software.html)

## Главная структура официального кода

Официальный код устроен так:

```text
SceneLib/
  Scene/        общий EKF-SLAM: состояние, ковариации, фильтр Калмана
  MonoSLAM/     модели одной камеры: движение, проекция, признаки
  SceneImproc/  корреляция, поиск признаков, обработка изображений

MonoSLAMGlow/
  демо-приложение с GUI, OpenGL/GLOW и offline/live режимом

TestSeqMonoSLAM/
  тестовая последовательность изображений
```

## Основная идея архитектуры

MonoSLAM разделён на две части:

1. **Общий SLAM-движок**  
   Он не знает, что сенсор — камера. Он просто хранит состояние, ковариации и делает EKF prediction/update.

2. **Модели конкретной задачи**  
   Они объясняют движку:
   - как движется камера;
   - как 3D-точка проецируется в изображение;
   - как инициализировать новый признак;
   - как считать якобианы.

Это ровно соответствует статье: общая вероятностная карта плюс специальные модели для single camera SLAM.

## Где смотреть основные части

### 1. Карта и состояние

Файл:

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/SceneLib/Scene/scene_single.h`

Класс:

```cpp
class Scene_Single
```

Он хранит:

- состояние камеры `xv`;
- ковариацию камеры `Pxx`;
- список признаков `feature_list`;
- выбранные для измерения признаки `selected_feature_list`;
- частично инициализированные признаки `feature_init_info_vector`.

Это кодовая версия формулы из статьи:

$$
\hat{x} =
\begin{pmatrix}
\hat{x}_v\\
\hat{y}_1\\
\hat{y}_2\\
\vdots
\end{pmatrix}
$$

и полной ковариационной матрицы:

$$
P =
\begin{pmatrix}
P_{xx} & P_{xy_1} & \cdots\\
P_{y_1x} & P_{y_1y_1} & \cdots\\
\vdots & \vdots & \ddots
\end{pmatrix}
$$

### 2. EKF prediction/update

Файл:

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/SceneLib/Scene/kalman.cpp`

Главные функции:

```cpp
Kalman::predict_filter_fast(...)
Kalman::total_update_filter_slow(...)
```

Prediction:

$$
P_{k+1|k} = F P_{k|k} F^\top + Q
$$

Update:

$$
S = HPH^\top + R
$$

$$
K = PH^\top S^{-1}
$$

$$
x_{new} = x + K\nu
$$

$$
P_{new} = P - KSK^\top
$$

В коде Kalman gain называется `W`.

### 3. Модель движения камеры

Файл:

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/SceneLib/MonoSLAM/models_impulse.h`

Класс:

```cpp
Impulse_ThreeD_Motion_Model
```

Это модель constant velocity / constant angular velocity:

$$
x_v =
\begin{pmatrix}
r\\
q\\
v\\
\omega
\end{pmatrix}
$$

Обновление:

$$
r_{new} = r + (v + V)\Delta t
$$

$$
q_{new} = q \times q((\omega + \Omega)\Delta t)
$$

$$
v_{new} = v + V
$$

$$
\omega_{new} = \omega + \Omega
$$

### 4. Настройки запуска

Файл:

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/MonoSLAMGlow/monoslam_state.ini`

Там задаётся:

```ini
MotionModel=IMPULSE_THREED
NewFeatureMeasurementModel=CAMERA_WIDE_LINE_INIT
CameraParameters=320 240 162 125 195 195 9e-06 1
```

Также там есть начальное состояние камеры и четыре известных признака `KnownFeature1`-`KnownFeature4`.

Это и есть стартовая мишень из статьи: известные 3D-точки задают масштаб и начальную систему координат.

### 5. Проекция 3D-точки в изображение

Файл:

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/SceneLib/MonoSLAM/models_wideangle.cpp`

Класс:

```cpp
Fully_Init_Wide_Point_Feature_Measurement_Model
```

Главная функция:

```cpp
func_hi_and_dhi_by_dxp_and_dhi_by_dyi(...)
```

Она делает:

```text
3D-точка в мире
-> точка относительно камеры
-> проекция в изображение
-> якобианы по позе камеры и 3D-точке
```

То есть это кодовая реализация:

$$
\hat{z}_{ij} = \pi(K, T_i, X_j)
$$

### 6. Активный выбор признаков

Файл:

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/SceneLib/Scene/scene_single.cpp`

Функция:

```cpp
Scene_Single::auto_select_n_features(...)
```

Она:

- предсказывает положение каждого признака;
- проверяет, видим ли он;
- считает score;
- выбирает несколько самых информативных признаков.

Score в официальном коде:

```cpp
return VNL::Trace(Si);
```

То есть выбираются признаки с большой ковариацией инновации $S_i$, потому что их измерение сильнее уменьшает неопределённость.

### 7. Области поиска признаков

Файлы:

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/SceneLib/Scene/scene_single.cpp`

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/SceneLib/MonoSLAM/robot.cpp`

Для каждого признака считается:

$$
S_i = H_i P H_i^\top + R_i
$$

Затем `robot.cpp` вызывает:

```cpp
elliptical_search(...)
```

То есть признак ищется не по всему изображению, а внутри эллипса неопределённости.

### 8. Корреляционный поиск патча

Файл:

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/SceneLib/SceneImproc/correlate.cpp`

Сопоставление признака основано на сравнении маленьких патчей изображения.

В статье это описано как template matching / normalized cross-correlation. В коде используется функция сравнения патчей, где учитываются среднее и стандартное отклонение яркости.

### 9. Инициализация нового признака

Файлы:

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/SceneLib/MonoSLAM/models_wideangle.h`

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/SceneLib/MonoSLAM/models_wideangle.cpp`

Класс:

```cpp
Line_Init_Wide_Point_Feature_Measurement_Model
```

Смысл:

после первого наблюдения признак ещё не является 3D-точкой. Он задаётся лучом:

$$
y_{pi} =
\begin{pmatrix}
r_i^W\\
\hat{h}_{Li}^W
\end{pmatrix}
$$

Любая возможная точка лежит на этом луче:

$$
y_i = r_i^W + \lambda \hat{h}_{Li}^W
$$

Глубина $\lambda$ неизвестна.

### 10. Частицы по глубине

Файлы:

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/SceneLib/Scene/init_feature.h`

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/SceneLib/Scene/init_feature.cpp`

Классы:

```cpp
Particle
FeatureInitInfo
```

Они хранят дискретные гипотезы глубины $\lambda$ и их вероятности.

В `MonoSLAMGlow` параметры такие:

```cpp
MIN_LAMBDA = 0.5
MAX_LAMBDA = 5.0
NUMBER_OF_PARTICLES = 100
STANDARD_DEVIATION_DEPTH_RATIO = 0.3
```

То есть новый признак сначала имеет 100 гипотез глубины от 0.5 до 5 м.

Когда распределение по глубине сжимается:

$$
\frac{\sigma_\lambda}{\mu_\lambda} < 0.3
$$

признак переводится в обычную 3D-точку.

### 11. Основной цикл MonoSLAM

Файл:

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/legacy/SceneLib/MonoSLAM/monoslam.cpp`

Функция:

```cpp
MonoSLAM::GoOneStep(...)
```

Она делает один шаг по новому кадру:

```text
1. prediction EKF
2. выбор признаков
3. измерение признаков
4. update EKF
5. удаление плохих признаков
6. добавление нового признака, если видимых мало
7. обновление частично инициализированных признаков
```

Это центральное место, откуда удобно начинать чтение.

## Современная GitHub-реализация

Папка:

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/github_marcromani_MonoSLAM`

Этот код компактнее и проще для чтения, потому что использует OpenCV и не тянет старую инфраструктуру SceneLib/VW/GLOW.

Главный файл:

`papers/Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM/code/github_marcromani_MonoSLAM/MonoSLAM/map.cpp`

Там в одном классе `Map` видно почти всё:

- `initMap(...)` — старт по шахматной доске;
- `predict(...)` — prediction EKF;
- `update(...)` — измерение признаков и update EKF;
- `trackNewCandidates(...)` — добавление кандидатов;
- `computeMeasurementMatrix(...)` — якобиан измерений;
- `removeBadFeatures(...)` — удаление плохих признаков.

Для первого чтения я бы смотрел именно его, а потом сверял с оригинальным SceneLib.

## Что важно понять

MonoSLAM не строит плотную карту. Он строит маленькую карту хороших ориентиров.

Его логика:

```text
предсказать позу камеры
-> предсказать, где должны быть ориентиры
-> искать их только в маленьких эллипсах
-> обновить камеру и карту через EKF
-> добавить новый ориентир, если старых мало
```

Главная инженерная сила MonoSLAM — активный поиск. Он не обрабатывает всё изображение, а использует вероятностную оценку, чтобы искать только там, где признак ожидается.

Главное ограничение — масштабируемость. Полная ковариация EKF делает карту дорогой при росте числа признаков:

$$
O(N^2)
$$

Поэтому MonoSLAM хорошо подходит для комнатного масштаба, но не для больших карт вроде целого здания.
