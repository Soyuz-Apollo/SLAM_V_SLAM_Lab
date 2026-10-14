#include <algorithm>
#include <cmath>

#include "opencv2/imgproc/imgproc.hpp"

#include "util.hpp"

using namespace cv;
using namespace std;

/*
 * Считает матрицу гомографии H между двумя видами плоскости для камеры-обскуры.
 * В частности, p2 = H * p1, где p1 — пиксель в первом виде, соответствующий
 * точке p на плоскости, а p2 — пиксель этой же точки во втором виде.
 *
 * p    Точка на плоскости в мировой системе координат
 * n    Нормаль плоскости в мировой системе координат
 * K    Матрица внутренних параметров камеры
 * R1   Поворот первой позы из мира в камеру
 * t1   Положение первой камеры в мировой системе координат
 * R2   Поворот второй позы из мира в камеру
 * t2   Положение второй камеры в мировой системе координат
 */
Mat computeHomography(const Mat& p, const Mat& n, const Mat& K,
                      const Mat& R1, const Mat& t1, const Mat& R2, const Mat& t2) {

    Mat m = n.t() * (p - t1);
    Mat H = K * R2 * (m.at<double>(0, 0) * Mat::eye(3, 3, CV_64FC1) + (t1 - t2) * n.t()) * R1.t() * K.inv();

    return H;
}

/*
 * Возвращает квадратную область интереса по пиксельным координатам центра
 * и размеру. Квадрат будет точно центрирован в нужной точке только при
 * нечётном размере.
 *
 * center       Центр квадрата
 * size         Размер квадрата
 */
Rect buildSquare(const Point2i& center, int size) {

    int left = center.x - size / 2;
    int top = center.y - size / 2;

    return Rect(left, top, size, size);
}

/*
 * Удаляет n-ю строку и n-й столбец квадратной матрицы. Исходная матрица
 * не меняется, возвращается глубокая копия.
 *
 * mat      Матрица, которую нужно уменьшить
 * idx      Удаляемые строка и столбец
 */
Mat removeRowCol(const Mat& mat, int idx) {

    int n = mat.rows;

    Mat reduced(n - 1, n - 1, mat.type());

    if (idx == 0)
        mat(Rect(1, 1, n - 1, n - 1)).copyTo(reduced(Rect(0, 0, n - 1, n - 1)));

    else if (idx == n - 1)
        mat(Rect(0, 0, n - 1, n - 1)).copyTo(reduced(Rect(0, 0, n - 1, n - 1)));

    else {
        mat(Rect(0, 0, idx, idx)).copyTo(reduced(Rect(0, 0, idx, idx)));
        mat(Rect(idx+1, 0, n-idx-1, idx)).copyTo(reduced(Rect(idx, 0, n-idx-1, idx)));
        mat(Rect(0, idx+1, idx, n-idx-1)).copyTo(reduced(Rect(0, idx, idx, n-idx-1)));
        mat(Rect(idx+1, idx+1, n-idx-1, n-idx-1)).copyTo(reduced(Rect(idx, idx, n-idx-1, n-idx-1)));
    }

    return reduced;
}

/*
 * Удаляет заданные строки и столбцы квадратной матрицы. Исходная матрица
 * не меняется, возвращается глубокая копия. Индексы для удаления должны
 * быть отсортированы по возрастанию. Параметр stride показывает, как строки
 * и столбцы группируются и соотносятся с индексами. Например, для индексов
 * 0, 3, 7 и stride = 2 будут удалены строки и столбцы 0, 1, 6, 7, 14, 15.
 *
 * mat          Матрица, которую нужно уменьшить
 * indices      Отсортированные строки и столбцы для удаления
 * stride       Шаг группировки строк и столбцов
 */
Mat removeRowsCols(const Mat& mat, vector<int>& indices, int stride) {

    Mat result = mat.clone();

    for (unsigned int i = 0; i < indices.size(); i++) {

        int idx = stride * (indices[i] - i);

        for (int j = 0; j < stride; j++)
            result = removeRowCol(result, idx);
    }

    return result;
}

/*
 * Удаляет n-ю строку матрицы. Исходная матрица не меняется,
 * возвращается глубокая копия.
 *
 * mat      Матрица, которую нужно уменьшить
 * idx      Удаляемая строка
 */
Mat removeRow(const Mat& mat, int idx) {

    Mat result;

    int n = mat.rows;

    if (idx == 0)
        result = mat.rowRange(1, n).clone();

    else if (idx == n - 1)
        result = mat.rowRange(0, n - 1).clone();

    else
        vconcat(mat.rowRange(0, idx), mat.rowRange(idx + 1, n), result);

    return result;
}

/*
 * Удаляет заданные строки матрицы. Исходная матрица не меняется,
 * возвращается глубокая копия. Индексы для удаления должны быть
 * отсортированы по возрастанию. Параметр stride показывает, как строки
 * группируются и соотносятся с индексами. Например, для индексов
 * 0, 3, 7 и stride = 2 будут удалены строки 0, 1, 6, 7, 14, 15.
 *
 * mat          Матрица, которую нужно уменьшить
 * indices      Отсортированные строки для удаления
 * stride       Шаг группировки строк
 */
Mat removeRows(const Mat& mat, vector<int>& indices, int stride) {

    Mat result = mat.clone();

    for (unsigned int i = 0; i < indices.size(); i++) {

        int idx = stride * (indices[i] - i);

        for (int j = 0; j < stride; j++)
            result = removeRow(result, idx);
    }

    return result;
}

/*
 * Возвращает эллипсы, внутри которых с высокой вероятностью должны находиться
 * предсказанные видимые признаки. Эллипсы строятся как доверительные области
 * уровня 0.86.
 *
 * means    Предсказанные пиксельные положения видимых признаков
 * S        Ковариационная матрица инновации для предсказанных видимых признаков
 */
vector<RotatedRect> computeEllipses(const vector<Point2d>& means, const Mat& S) {

    vector<RotatedRect> ellipses;

    for (unsigned int i = 0; i < means.size(); i++) {

        Mat Si = S(Rect(2*i, 2*i, 2, 2));

        // Считаем собственные значения и собственные векторы.
        Mat eigenval, eigenvec;
        eigen(Si, eigenval, eigenvec);

        // Считаем угол между наибольшим собственным вектором и осью x.
        double angle = atan2(eigenvec.at<double>(0, 1), eigenvec.at<double>(0, 0));

        // Переносим угол из диапазона [-pi, pi] в [0, 2pi].
        if (angle < 0)
            angle += PI_DOUBLE;

        // Переводим в градусы.
        angle *= RAD_TO_DEG;

        // Считаем размеры большой и малой осей.
        double majorAxis = 4 * sqrt(eigenval.at<double>(0));
        double minorAxis = 4 * sqrt(eigenval.at<double>(1));

        ellipses.push_back(RotatedRect(means[i], Size(majorAxis, minorAxis), -angle));
    }

    return ellipses;
}

/*
 * Возвращает минимальный прямой прямоугольник, содержащий заданный эллипс.
 * Если прямоугольник выходит за границы изображения, он обрезается.
 *
 * ellipse      Эллипс, для которого ищется ограничивающий прямоугольник
 * imageSize    Размер изображения
 */
Rect getBoundingBox(const RotatedRect& ellipse, const Size& imageSize) {

    double angle = ellipse.angle * DEG_TO_RAD;
    double x = ellipse.center.x;
    double y = ellipse.center.y;
    double a = 0.5 * ellipse.size.width;
    double b = 0.5 * ellipse.size.height;

    double xmax, ymax;

    if (angle == 0 || a == b) {

        xmax = a;
        ymax = b;

    } else {

        double a2inv = 1 / (a*a);
        double b2inv = 1 / (b*b);

        double c = cos(angle);
        double s = sin(angle);
        double c2 = c*c;
        double s2 = s*s;

        double A = c2 * a2inv + s2 * b2inv;
        double B = c2 * b2inv + s2 * a2inv;
        double C = 2 * c * s * (b2inv - a2inv);

        double r = (4 * A * B / (C * C) - 1);
        double x0 = 1 / sqrt(A * r);
        double y0 = 1 / sqrt(B * r);

        if (C < 0) {

            x0 = - x0;
            y0 = - y0;
        }

        xmax = 2 * B * y0 / C;
        ymax = 2 * A * x0 / C;
    }

    double x0 = x - xmax;
    double y0 = y - ymax;
    double w = 2 * xmax + 1;
    double h = 2 * ymax + 1;

    if (x0 + w > imageSize.width)
        w = imageSize.width - x0;
    if (y0 + h > imageSize.height)
        h = imageSize.height - y0;

    return Rect(max(x - xmax, 0.), max(y - ymax, 0.), w, h);
}

/*
 * Рисует прямоугольник на изображении. Функция меняет изображение,
 * поэтому для сохранения оригинала нужно заранее сделать глубокую копию.
 *
 * image        Серое или цветное изображение для рисования
 * r            Прямоугольник
 * color        Цвет прямоугольника
 */
void drawRectangle(Mat& image, const Rect& r, const Scalar& color) {

    rectangle(image, r, color, 1, LINE_AA);
}

/*
 * Рисует квадрат на изображении. Функция меняет изображение,
 * поэтому для сохранения оригинала нужно заранее сделать глубокую копию.
 *
 * image        Серое или цветное изображение для рисования
 * center       Центр квадрата
 * width        Ширина квадрата
 * color        Цвет квадрата
 */
void drawSquare(Mat& image, const Point2i& center, int width, const Scalar& color) {

    rectangle(image, buildSquare(center, width), color, 1, LINE_AA);
}

/*
 * Рисует эллипс на изображении. Функция меняет изображение,
 * поэтому для сохранения оригинала нужно заранее сделать глубокую копию.
 *
 * image        Серое или цветное изображение для рисования
 * e            Эллипс
 * color        Цвет эллипса
 */
void drawEllipse(Mat& image, const RotatedRect& e, const Scalar& color) {

    ellipse(image, e, color, 1, LINE_AA);
}

/*
 * Рисует окружность на изображении. Функция меняет изображение,
 * поэтому для сохранения оригинала нужно заранее сделать глубокую копию.
 *
 * image        Серое или цветное изображение для рисования
 * center       Центр окружности
 * radius       Радиус окружности
 * color        Цвет окружности
 */
void drawCircle(Mat& image, const Point2i& center, int radius, const Scalar& color) {

    circle(image, center, radius, color, 1, LINE_AA);
}

void drawTemplate(Mat& image, const Mat& templ, const Point2d& position, int cx, int cy) {

    int templ_x = 0;
    int image_x = position.x - cx;

    if (image_x > image.cols - 1)
        return;

    if (image_x < 0) {

        templ_x = - image_x;
        image_x = 0;
    }

    int templ_y = 0;
    int image_y = position.y - cy;

    if (image_y > image.rows - 1)
        return;

    if (image_y < 0) {

        templ_y = - image_y;
        image_y = 0;
    }

    int w = templ.cols - templ_x;

    if (w <= 0)
        return;

    if (image_x + w > image.cols)
        w = image.cols - image_x;

    int h = templ.rows - templ_y;

    if (h <= 0)
        return;

    if (image_y + h > image.rows)
        h = image.rows - image_y;

    templ(Rect(templ_x, templ_y, w, h)).copyTo(image(Rect(image_x, image_y, w, h)));
}
