#include <cmath>

#include "quaternion.hpp"

using namespace cv;

/*
 * Строит кватернион поворота по ненулевому вектору поворота.
 */
Mat computeQuaternion(const Mat& rotv) {

    double x = rotv.at<double>(0, 0);
    double y = rotv.at<double>(1, 0);
    double z = rotv.at<double>(2, 0);

    double angle = sqrt(x*x + y*y + z*z);

    double c = cos(0.5 * angle);
    double s = sin(0.5 * angle);

    double q[] = {c, s*x/angle, s*y/angle, s*z/angle};

    return Mat(4, 1, CV_64FC1, q).clone();
}

/*
 * Считает произведение Гамильтона для двух кватернионов.
 */
Mat quaternionMultiply(const Mat& q1, const Mat& q2) {

    double a1 = q1.at<double>(0, 0);
    double b1 = q1.at<double>(1, 0);
    double c1 = q1.at<double>(2, 0);
    double d1 = q1.at<double>(3, 0);

    double a2 = q2.at<double>(0, 0);
    double b2 = q2.at<double>(1, 0);
    double c2 = q2.at<double>(2, 0);
    double d2 = q2.at<double>(3, 0);

    double q[] = {a1*a2 - b1*b2 - c1*c2 - d1*d2,
                  a1*b2 + b1*a2 + c1*d2 - d1*c2,
                  a1*c2 - b1*d2 + c1*a2 + d1*b2,
                  a1*d2 + b1*c2 - c1*b2 + d1*a2
                 };

    return Mat(4, 1, CV_64FC1, q).clone();
}

/*
 * Считает матрицу поворота по кватерниону поворота. Если кватернион
 * задаёт ориентацию и поворачивает систему отсчёта A в конечную позу B,
 * то матрица является переходом базиса из B в A.
 */
Mat getRotationMatrix(const Mat& q) {

    double a = q.at<double>(0, 0);
    double b = q.at<double>(1, 0);
    double c = q.at<double>(2, 0);
    double d = q.at<double>(3, 0);

    double m[] = {a*a+b*b-c*c-d*d, 2*b*c-2*a*d,     2*b*d+2*a*c,
                  2*b*c+2*a*d,     a*a-b*b+c*c-d*d, 2*c*d-2*a*b,
                  2*b*d-2*a*c,     2*c*d+2*a*b,     a*a-b*b-c*c+d*d
                 };

    return Mat(3, 3, CV_64FC1, m).clone();
}

/*
 * Считает обратный кватернион.
 */
Mat quaternionInv(const Mat& q) {

    Mat inv = Mat(4, 1, CV_64FC1);

    inv.at<double>(0, 0) =  q.at<double>(0, 0);
    inv.at<double>(1, 0) = -q.at<double>(1, 0);
    inv.at<double>(2, 0) = -q.at<double>(2, 0);
    inv.at<double>(3, 0) = -q.at<double>(3, 0);

    return inv;
}

/*
 * Считает матрицы производных R(q)^t по компонентам кватерниона.
 * То есть элементы Ri — это производные элементов транспонированной матрицы,
 * возвращаемой getRotationMatrix(q), по i-й компоненте кватерниона.
 */
void getRotationMatrixDerivatives(const Mat& q, Mat& R1, Mat& R2, Mat& R3, Mat& R4) {

    double a = q.at<double>(0, 0);
    double b = q.at<double>(1, 0);
    double c = q.at<double>(2, 0);
    double d = q.at<double>(3, 0);

    double R1_[] = {a, d, -c, -d, a, b, c, -b, a};
    R1 = 2 * Mat(3, 3, CV_64FC1, R1_).clone();

    double R2_[] = {b, c, d, c, -b, a, d, -a, -b};
    R2 = 2 * Mat(3, 3, CV_64FC1, R2_).clone();

    double R3_[] = {-c, b, -a, b, c, d, a, d, -c};
    R3 = 2 * Mat(3, 3, CV_64FC1, R3_).clone();

    double R4_[] = {-d, a, b, -a, -d, c, b, c, d};
    R4 = 2 * Mat(3, 3, CV_64FC1, R4_).clone();
}
