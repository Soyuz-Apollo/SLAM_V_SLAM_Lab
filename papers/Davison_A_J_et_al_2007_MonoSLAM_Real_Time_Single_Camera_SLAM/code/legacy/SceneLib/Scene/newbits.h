/*  SceneLib: software for real-time SLAM

    newbits.h
    Copyright (C) 2005 University of Oxford

    Author
    Andrew Davison
    ajd@doc.ic.ac.uk

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _scene_bits_h_
#define _scene_bits_h_

#include <VNL/vector.h>
#include <VNL/matrix.h>
#include <VNL/matrixfixed.h>
#include <VNL/vectorfixed.h>
#include <VW/GeomObjects/quaternion.h>
#include <VW/GeomObjects/point3d.h>

// and_pi_range: простая функция, которая получает double, представляющий угол,
// и приводит его к диапазону -pi -> pi.
void and_pi_range(double &angle);

// Доступ к элементам матрицы и вектора с индексацией от (1, 1),
// где это верхний левый элемент.
//inline double& matel(const VNL::Matrix<double>& M, const int &r, const int &c) 
//{
//  return M(r - 1, c - 1);
//}
//inline double& vecel(const VNL::Vector<double>& V, const int &r) 
//{
//  return V(r - 1);
//}


// Якобианы произведения кватернионов по аргументам.
// Предполагается произведение q3 = q2 * q1.
// Получается, что dq3_by_dq1 зависит только от q2, а dq3_by_dq2 — от q1.
VNL::MatrixFixed<4,4,double> dq3_by_dq1(const VW::Quaternion& q2);
VNL::MatrixFixed<4,4,double> dq3_by_dq2(const VW::Quaternion& q1);

// Якобиан обратного кватерниона: это постоянная матрица,
// не зависящая от самого кватерниона.
VNL::MatrixFixed<4,4,double> dqbar_by_dq();

// Якобиан поворота, полученного из кватерниона и умноженного на вектор.
// Это непросто, потому что для аккуратной записи здесь фактически нужен тензор.
// Здесь обходимся без него.
// Возвращает матрицу 3x4.
VNL::MatrixFixed<3,4,double> dRq_times_a_by_dq (const VW::Quaternion &q, 
						const VW::Vector3D &a);

// Компонентные якобианы: каждый возвращает матрицу 3x3, являющуюся производной
// матрицы поворота, полученной из кватерниона, по соответствующему элементу
// кватерниона.
VNL::MatrixFixed<3,3,double> dR_by_dq0(const VW::Quaternion &q);
VNL::MatrixFixed<3,3,double> dR_by_dqx(const VW::Quaternion &q);
VNL::MatrixFixed<3,3,double> dR_by_dqy(const VW::Quaternion &q);
VNL::MatrixFixed<3,3,double> dR_by_dqz(const VW::Quaternion &q);

// Нормализация кватерниона и её якобиан.
VW::Quaternion normalise_quaternion(const VW::Quaternion &q);
VNL::MatrixFixed<4,4,double> dqnorm_by_dq(const VW::Quaternion &q);

// Нормализация 3D-вектора.
VNL::MatrixFixed<3,3,double> dvnorm_by_dv(VW::Vector3D v);

#endif
