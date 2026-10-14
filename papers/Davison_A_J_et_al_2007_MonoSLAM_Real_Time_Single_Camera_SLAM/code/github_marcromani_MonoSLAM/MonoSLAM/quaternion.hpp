#ifndef QUATERNION_H
#define QUATERNION_H

#include "opencv2/core/core.hpp"

/*
 * Строит кватернион поворота по ненулевому вектору поворота.
 */
cv::Mat computeQuaternion(const cv::Mat&);

/*
 * Считает произведение Гамильтона для двух кватернионов.
 */
cv::Mat quaternionMultiply(const cv::Mat&, const cv::Mat&);

/*
 * Считает матрицу поворота по кватерниону поворота. Если кватернион
 * задаёт ориентацию и поворачивает систему отсчёта A в конечную позу B,
 * то матрица является переходом базиса из B в A.
 */
cv::Mat getRotationMatrix(const cv::Mat&);

/*
 * Считает обратный кватернион.
 */
cv::Mat quaternionInv(const cv::Mat&);

/*
 * Считает матрицы производных R(q)^t по компонентам кватерниона.
 * То есть элементы Ri — это производные элементов транспонированной матрицы,
 * возвращаемой getRotationMatrix(q), по i-й компоненте кватерниона.
 */
void getRotationMatrixDerivatives(const cv::Mat& q, cv::Mat& R1, cv::Mat& R2, cv::Mat& R3, cv::Mat& R4);

#endif
