#ifndef MAP_H
#define MAP_H

#include <vector>

#include "opencv2/core/core.hpp"

#include "camera.hpp"
#include "feature.hpp"

class Map {

public:

    Camera camera;                          // Камера

    std::vector<Feature> features;          // Инициализированные признаки карты, используемые для отслеживания
    std::vector<Feature> candidates;        // Предварительно инициализированные признаки, обновляемые фильтром частиц

    int patchSize;                          // Размер плоского патча признака в пикселях

    int minFeatureDensity;                  // Минимальное число признаков на кадр
    int maxFeatureDensity;                  // Максимальное число признаков на кадр

    double failTolerance;                   // Максимальная доля неудачных сопоставлений до удаления признака

    int numVisibleFeatures;                 // Число признаков, реально сопоставленных в текущем кадре
    std::vector<cv::Point2d> inviewPos;     // Предсказанные позиции видимых признаков на изображении

    cv::Mat x;                              // Состояние карты: состояние камеры и состояния признаков
    cv::Mat P;                              // Ковариационная матрица состояния карты

    cv::Mat A;                              // Ковариация шума линейных и угловых ускорений
    cv::Mat R;                              // Дисперсии шума измерений в пикселях

    /*
     * Конструктор для модели камеры с дисторсией объектива.
     */
    Map(const cv::Mat& K,
        const cv::Mat& distCoeffs,
        const cv::Size& frameSize,
        int patchSize_,
        int minFeatureDensity_,
        int maxFeatureDensity_,
        double failTolerance_,
        const cv::Mat& accelerationVariances,
        const cv::Mat& measurementNoiseVariances) :
        camera(K, distCoeffs, frameSize),
        patchSize(patchSize_),
        minFeatureDensity(minFeatureDensity_),
        maxFeatureDensity(maxFeatureDensity_),
        failTolerance(failTolerance_),
        numVisibleFeatures(0),
        x(13, 1, CV_64FC1, cv::Scalar(0)),
        P(13, 13, CV_64FC1, cv::Scalar(0)) {

        A = cv::Mat::diag(accelerationVariances);
        measurementNoiseVariances.copyTo(R);
    }

    /*
     * Конструктор для модели камеры-обскуры без дисторсии.
     */
    Map(const cv::Mat& K,
        const cv::Size& frameSize,
        int patchSize_,
        int minFeatureDensity_,
        int maxFeatureDensity_,
        double failTolerance_,
        const cv::Mat& accelerationVariances,
        const cv::Mat& measurementNoiseVariances) :
        camera(K, frameSize),
        patchSize(patchSize_),
        minFeatureDensity(minFeatureDensity_),
        maxFeatureDensity(maxFeatureDensity_),
        failTolerance(failTolerance_),
        numVisibleFeatures(0),
        x(13, 1, CV_64FC1, cv::Scalar(0)),
        P(13, 13, CV_64FC1, cv::Scalar(0)) {

        A = cv::Mat::diag(accelerationVariances);
        measurementNoiseVariances.copyTo(R);
    }

    /*
     * Инициализирует карту по известному шахматному шаблону. Он задаёт
     * абсолютный масштаб, несколько вручную инициализированных признаков
     * для отслеживания, начало мировой системы координат и начальную позу
     * камеры. Начало мировой системы координат помещается в верхний левый
     * угол шаблона: ось x идёт вдоль высоты шаблона, ось y — вдоль ширины.
     * Ось z задаётся правилом правой руки (см. solvePnP).
     * Предполагается, что начальные линейная и угловая скорости камеры равны нулю.
     *
     * frame            Кадр в градациях серого
     * patternSize      Размеры шаблона (см. findChessboardCorners)
     * squareSize       Размер клетки шахматного шаблона в метрах
     * var              Дисперсии начальных компонент состояния камеры
     */
    bool initMap(const cv::Mat& frame, const cv::Size& patternSize, double squareSize,
                 const std::vector<double>& var);

    /*
     * Обновляет данные камеры и признаков из общего вектора состояния карты
     * и ковариационной матрицы. Выполняются только поверхностные копии.
     * Эту функцию нужно вызывать, когда текущее состояние системы надо
     * раздать её отдельным частям. Метод нужен потому, что EKF постоянно
     * меняет x и P, а объекты камеры и признаков сами по себе этого не видят.
     */
    void broadcastData();

    /*
     * Детектирует углы и создаёт новых кандидатов в признаки, если видимых
     * признаков недостаточно и сейчас не отслеживаются другие кандидаты.
     * Возвращает true, если новые кандидаты были найдены.
     *
     * frame    Кадр в градациях серого
     */
    bool trackNewCandidates(const cv::Mat& frame);

    /*
     * Выполняет первый шаг расширенного фильтра Калмана: предсказание.
     * По текущей оценке состояния x_k|k и ковариации P_k|k вычисляет
     * априорную оценку x_k+1|k через модель движения камеры и соответствующую
     * ковариацию P_k+1|k.
     *
     * dt    Интервал времени между текущим и предыдущим кадрами
     */
    void predict(double dt);

    /*
     * Выполняет шаг обновления расширенного фильтра Калмана.
     * По априорной оценке x_k+1|k и ковариации P_k+1|k вычисляет
     * исправленную апостериорную оценку x_k+1|k+1 и её ковариацию P_k+1|k+1,
     * измеряя известные признаки карты, которые сейчас видны.
     *
     * gray     Кадр в градациях серого для сопоставления признаков
     * frame    Цветной кадр, на котором рисуются признаки
     */
    void update(const cv::Mat& gray, cv::Mat& frame);

private:

    /*
     * Добавляет начальный признак в карту. Его положение известно без
     * неопределённости, поэтому соответствующая ковариация нулевая.
     * Поскольку патч признака лежит на шахматном шаблоне, его нормаль
     * принимается равной единичному вектору оси z. Признак сразу считается
     * видимым. Если патч выходит за границы кадра, признак не добавляется.
     *
     * frame    Кадр в градациях серого
     * pos2D    Положение признака на изображении в пикселях
     * pos3D    Положение признака в мировой системе координат
     * R        Поворот камеры из мировой системы в систему камеры
     * t        Положение камеры в мировой системе координат
     */
    bool addInitialFeature(const cv::Mat& frame, const cv::Point2f& pos2D, const cv::Point3f& pos3D,
                           const cv::Mat& R, const cv::Mat& t);

    /*
     * Сбрасывает карту к состоянию сразу после создания.
     */
    void reset();

    /*
     * Возвращает углы, найденные детектором Shi-Tomasi. Число найденных углов
     * выбирается так, чтобы после добавления соответствующих предварительно
     * инициализированных признаков число видимых признаков в текущем кадре
     * по возможности достигло разрешённого максимума.
     *
     * frame    Кадр в градациях серого
     */
    cv::Mat findCorners(const cv::Mat& frame);

    /*
     * Применяет функцию перехода состояния к текущей оценке. Этот метод должен
     * вызываться после computeProcessMatrix и computeProcessNoiseMatrix, потому
     * что они ожидают в x последнюю апостериорную, уже исправленную, оценку.
     *
     * dt    Интервал времени между текущим и предыдущим кадрами
     */
    void applyMotionModel(double dt);

    /*
     * Возвращает якобиан функции перехода состояния по полному состоянию
     * (r, q, v, w, f1, ..., fn). Матрица считается в текущей оценке состояния
     * при предположении нулевого шума процесса. Метод должен вызываться перед
     * applyMotionModel, потому что applyMotionModel обновляет x.
     *
     * dt    Интервал времени между текущим и предыдущим кадрами
     */
    cv::Mat computeProcessMatrix(double dt);

    /*
     * Возвращает произведение W * Q * W^t, где Q — ковариация шума процесса,
     * а W — якобиан функции перехода состояния по вектору шума
     * (V1, V2, V3, W1, W2, W3). Он вычисляется в текущей оценке состояния
     * при предположении нулевого шума процесса. Метод должен вызываться перед
     * applyMotionModel, потому что applyMotionModel обновляет x.
     *
     * dt    Интервал времени между текущим и предыдущим кадрами
     * F     Якобиан функции перехода состояния по x
     */
    cv::Mat computeProcessNoiseMatrix(double dt, const cv::Mat& F);

    /*
     * Возвращает якобиан функции измерения признаков по полному состоянию
     * (r, q, v, w, f1, ..., fn). Матрица вычисляется в предсказанной,
     * априорной, оценке состояния, поэтому метод должен вызываться только
     * после applyMotionModel. Также он обновляет вектор предсказанных
     * пиксельных положений видимых признаков inviewPos и заполняет индексы
     * признаков, которые должны быть видны.
     */
    cv::Mat computeMeasurementMatrix(std::vector<int>& inviewIndices);

    void drawFeatures(cv::Mat& frame,
                      const std::vector<int>& matchedInviewIndices, const std::vector<int>& failedInviewIndices,
                      const std::vector<cv::RotatedRect>& ellipses, bool drawEllipses = true);

    void removeBadFeatures(const std::vector<int>& failedInviewIndices, const std::vector<int>& failedIndices);

    /*
     * Нормализует кватернион ориентации в состоянии системы и соответствующим
     * образом обновляет ковариационную матрицу через распространение ошибки
     * первого порядка. Функцию обязательно вызывать после обновления EKF, потому
     * что исправленный апостериорный кватернион может перестать быть единичным.
     */
    void renormalizeQuaternion();
};

#endif
