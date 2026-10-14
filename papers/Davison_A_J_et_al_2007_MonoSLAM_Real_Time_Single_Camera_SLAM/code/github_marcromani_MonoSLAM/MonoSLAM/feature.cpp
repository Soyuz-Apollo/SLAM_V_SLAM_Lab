#include "feature.hpp"

using namespace cv;

/*
 * Конструктор полностью инициализированного признака. Это вспомогательный
 * метод для этапа инициализации карты, когда вручную добавленные признаки
 * считаются известными без неопределённости положения.
 *
 * image_    Кадр камеры в момент инициализации признака
 * roi_      Патч признака
 * normal_   Вектор нормали патча
 * R_        Поворот камеры из мира в камеру при инициализации признака
 * t_        Положение камеры в мировой системе координат при инициализации
 */
Feature::Feature(const Mat& image_, const Rect& roi_, const Mat& normal_,
                 const Mat& R_, const Mat& t_) {

    image_.copyTo(image);
    roi = roi_;
    normal_.copyTo(normal);
    R_.copyTo(R);
    t_.copyTo(t);

    matchingFails = 0;
    matchingAttempts = 1;
}

/*
 * Конструктор предварительно инициализированного признака-кандидата.
 *
 * image_            Кадр камеры в момент инициализации признака
 * roi_              Патч признака
 * R_                Поворот камеры из мира в камеру при инициализации
 * t_                Положение камеры в мировой системе координат при инициализации
 * dir_              Единичный вектор луча признака в мировой системе координат
 * depthInterval     Интервал, содержащий гипотезы глубины
 * depthSamples      Количество гипотез глубины
 */
Feature::Feature(const Mat& image_, const Rect& roi_, const Mat& R_, const Mat& t_,
                 const Mat& dir_, const Point2d& depthInterval, int depthSamples) {

    image_.copyTo(image);
    roi = roi_;
    R_.copyTo(R);
    t_.copyTo(t);
    dir_.copyTo(dir);

    depths.resize(depthSamples);

    double step = (depthInterval.y - depthInterval.x) / (depthSamples - 1);

    for (int i = 0; i < depthSamples; i++)
        depths[i] = depthInterval.x + step * i;

    probs.resize(depthSamples, 1.0 / depthSamples);
}
