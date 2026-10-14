#include "opencv2/imgproc/imgproc.hpp"

#include "camera.hpp"
#include "util.hpp"

using namespace cv;
using namespace std;

/*
 * Конструктор для модели камеры с дисторсией объектива.
 */
Camera::Camera(const Mat& K_, const Mat& distCoeffs_, const Size& frameSize_) {

    K_.copyTo(K);
    frameSize = frameSize_;

    distCoeffs_.copyTo(distCoeffs);
}

/*
 * Конструктор для модели камеры-обскуры без дисторсии
 * (distCoeffs заполняется нулями).
 */
Camera::Camera(const Mat& K_, const Size& frameSize_) {

    K_.copyTo(K);
    frameSize = frameSize_;

    distCoeffs = Mat::zeros(1, 5, CV_64FC1);
}

/*
 * Переносит прямоугольный патч из одного вида камеры в другой.
 * Предполагается, что патч является проекцией небольшой области плоскости
 * в пространстве. Вычисляется преобразование M такое, что s2 = M * s1,
 * где s1 — патч в первом виде, а s2 — вид того же патча из второй позы.
 * Используется модель камеры-обскуры, то есть нулевые коэффициенты
 * дисторсии, поэтому преобразование является плоской гомографией.
 * Это хорошее приближение, если патч мал или дисторсия объектива умеренная.
 *
 * p            Точка на плоскости в мировой системе координат
 * n            Нормаль плоскости в мировой системе координат
 * view1        Исходный вид
 * patch1       Исходный патч
 * R1           Поворот первой позы из мира в камеру
 * t1           Положение первой камеры в мировой системе координат
 * R2           Поворот второй позы из мира в камеру
 * t2           Положение второй камеры в мировой системе координат
 * u            Первая пиксельная координата проекции центра исходного патча
 * v            Вторая пиксельная координата проекции центра исходного патча
 */
Mat Camera::warpPatch(const Mat& p, const Mat& n, const Mat& view1, const Rect& patch1,
                      const Mat& R1, const Mat& t1, const Mat& R2, const Mat& t2,
                      int& u, int& v) {

    // Чёрный фон.
    Mat black(view1.rows, view1.cols, view1.type(), Scalar(0));

    // Копируем исходный патч на чёрный фон.
    view1(patch1).copyTo(black(patch1));

    // Считаем матрицу гомографии между первым и вторым видами.
    Mat H = computeHomography(p, n, K, R1, t1, R2, t2);

    // Применяем гомографию.
    Mat warped;
    warpPerspective(black, warped, H, view1.size());

    // Делаем каждый нечёрный пиксель перенесённого изображения белым.
    Mat binary;
    threshold(warped, binary, 0, 255, THRESH_BINARY);

    // По бинарной маске обрезаем перенесённое изображение до области патча.
    Mat points;
    findNonZero(binary, points);
    Rect patchBox = boundingRect(points);

    // Считаем проекцию центра патча.
    int x = patch1.x + patch1.width / 2;
    int y = patch1.y + patch1.height / 2;

    Mat centerProj = H * (Mat_<double>(3, 1) << x, y, 1);
    centerProj /= centerProj.at<double>(2, 0);

    u = centerProj.at<double>(0, 0) - patchBox.x;
    v = centerProj.at<double>(1, 0) - patchBox.y;

    return warped(patchBox);
}

/*
 * Проецирует 3D-точки на кадр камеры.
 *
 * R            Матрица поворота из мира в камеру
 * t            Положение камеры в мировой системе координат
 * points3D     Проецируемые точки в мировой системе координат
 * points2D     Проекции точек в пикселях
 */
void Camera::projectPoints(const Mat& R, const Mat& t, const vector<Point3d>& points3D,
                           vector<Point2d>& points2D) {

    double fx = K.at<double>(0, 0);
    double fy = K.at<double>(1, 1);
    double cx = K.at<double>(0, 2);
    double cy = K.at<double>(1, 2);

    double k1 = distCoeffs.at<double>(0, 0);
    double k2 = distCoeffs.at<double>(0, 1);
    double p1 = distCoeffs.at<double>(0, 2);
    double p2 = distCoeffs.at<double>(0, 3);
    double k3 = distCoeffs.at<double>(0, 4);

    points2D.resize(points3D.size());

    for (unsigned int i = 0; i < points3D.size(); i++) {

        Mat pCam = R * (Mat(points3D[i]) - t);

        double xn = pCam.at<double>(0, 0) / pCam.at<double>(2, 0);
        double yn = pCam.at<double>(1, 0) / pCam.at<double>(2, 0);

        double r2 = xn*xn + yn*yn;
        double r4 = r2 * r2;
        double r6 = r4 * r2;

        double u = xn * (1 + k1*r2 + k2*r4 + k3*r6) + 2*p1*xn*yn + p2*(r2 + 2*xn*xn);
        double v = yn * (1 + k1*r2 + k2*r4 + k3*r6) + 2*p2*xn*yn + p1*(r2 + 2*yn*yn);

        u = fx*u + cx;
        v = fy*v + cy;

        points2D[i] = Point2d(u, v);
    }
}
