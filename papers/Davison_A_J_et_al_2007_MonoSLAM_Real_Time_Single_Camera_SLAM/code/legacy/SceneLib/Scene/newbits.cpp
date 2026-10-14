/*  SceneLib: software for real-time SLAM

    newbits.cpp
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

#include "newbits.h"

/**
@addtogroup gScene
@{
*/

void and_pi_range(double &angle)
{
  while(angle >= M_PI)
    angle -= 2 * M_PI;
  while(angle < -M_PI)
    angle += 2 * M_PI;

  return;
}

VNL::MatrixFixed<4,4,double> dq3_by_dq1(const VW::Quaternion &q2)
{
  double a[16] = {q2.R(), -q2.X(), -q2.Y(), -q2.Z(),
		  q2.X(),  q2.R(), -q2.Z(),  q2.Y(),
		  q2.Y(),  q2.Z(),  q2.R(), -q2.X(),
		  q2.Z(), -q2.Y(),  q2.X(),  q2.R()};

  VNL::MatrixFixed<4,4,double> M(a);
  return M;
}

VNL::MatrixFixed<4,4,double> dq3_by_dq2(const VW::Quaternion &q1)
{
  double a[16] = {q1.R(), -q1.X(), -q1.Y(), -q1.Z(),
		  q1.X(),  q1.R(),  q1.Z(), -q1.Y(),
		  q1.Y(), -q1.Z(),  q1.R(),  q1.X(),
		  q1.Z(),  q1.Y(), -q1.X(),  q1.R()};
 
  VNL::MatrixFixed<4,4,double> M(a);
  return M;
}

VNL::MatrixFixed<4,4,double> dqbar_by_dq()
{
  double a[16] = {1.0,  0.0,  0.0,  0.0,
		  0.0, -1.0,  0.0,  0.0,
		  0.0,  0.0, -1.0,  0.0,
		  0.0,  0.0,  0.0, -1.0};
 
  VNL::MatrixFixed<4,4,double> M(a);
  return M;
};


VNL::MatrixFixed<3,4,double> dRq_times_a_by_dq (const VW::Quaternion &q, 
						const VW::Vector3D &a)
{
  VNL::MatrixFixed<3,1,double> aMat;
  aMat(0, 0) = a.GetX(); 
  aMat(1, 0) = a.GetY(); 
  aMat(2, 0) = a.GetZ(); 

  VNL::MatrixFixed<3,3,double> TempR;
  VNL::MatrixFixed<3,1,double> Temp31;
  VNL::MatrixFixed<3,4,double> dRq_times_a_by_dq;

  // Строим якобиан, складывая четыре вектора рядом друг с другом.
  TempR = dR_by_dq0(q);
  Temp31 = TempR * aMat;
  dRq_times_a_by_dq.Update(Temp31, 0, 0);

  TempR = dR_by_dqx(q);
  Temp31 = TempR * aMat;
  dRq_times_a_by_dq.Update(Temp31, 0, 1);

  TempR = dR_by_dqy(q);
  Temp31 = TempR * aMat;
  dRq_times_a_by_dq.Update(Temp31, 0, 2);

  TempR = dR_by_dqz(q);
  Temp31 = TempR * aMat;
  dRq_times_a_by_dq.Update(Temp31, 0, 3);

  return dRq_times_a_by_dq;
}


VNL::MatrixFixed<3,3,double> dR_by_dq0(const VW::Quaternion &q)
{
  double q0 = q.R();
  double qx = q.X();
  double qy = q.Y();
  double qz = q.Z();

  double a[9] = {2*q0, -2*qz,  2*qy,
		 2*qz,  2*q0, -2*qx,
		-2*qy,  2*qx,  2*q0};
 
  VNL::MatrixFixed<3,3,double> M(a);
  return M;
}

VNL::MatrixFixed<3,3,double> dR_by_dqx(const VW::Quaternion &q)
{
  double q0 = q.R();
  double qx = q.X();
  double qy = q.Y();
  double qz = q.Z();

  double a[9] = {2*qx, 2*qy, 2*qz,
		 2*qy, -2*qx, -2*q0,
		 2*qz, 2*q0, -2*qx};
 
  VNL::MatrixFixed<3,3,double> M(a);
  return M;

}

VNL::MatrixFixed<3,3,double> dR_by_dqy(const VW::Quaternion &q)
{
  double q0 = q.R();
  double qx = q.X();
  double qy = q.Y();
  double qz = q.Z();

  double a[9] = {-2*qy, 2*qx, 2*q0,
		 2*qx, 2*qy, 2*qz,
		 -2*q0, 2*qz, -2*qy};
 
  VNL::MatrixFixed<3,3,double> M(a);
  return M;
}

VNL::MatrixFixed<3,3,double> dR_by_dqz(const VW::Quaternion &q)
{
  double q0 = q.R();
  double qx = q.X();
  double qy = q.Y();
  double qz = q.Z();

  double a[9] = {-2*qz, -2*q0, 2*qx,
		 2*q0, -2*qz, 2*qy,
		 2*qx, 2*qy, 2*qz};
 
  VNL::MatrixFixed<3,3,double> M(a);
  return M;

}

VW::Quaternion normalise_quaternion(const VW::Quaternion &q)
{
  // qq — текущая длина q.
  double qq = sqrt(q.GetR()*q.GetR() + 
		   q.GetX()*q.GetX() + q.GetY()*q.GetY() + q.GetZ()*q.GetZ());

  VW::Quaternion qnorm(q.GetX() / qq,
		       q.GetY() / qq,
		       q.GetZ() / qq,
		       q.GetR() / qq);
  return qnorm;
}


// Вспомогательные функции, используемые dqnorm_by_dq().
// Значение диагонального элемента якобиана.
static double dqi_by_dqi(double qi, double qq)
{
  return (1 - qi*qi / (qq*qq)) / qq;
}

// Значение внедиагонального элемента якобиана.
static double dqi_by_dqj(double qi, double qj, double qq)
{
  return -qi * qj / (qq*qq*qq);
}


VNL::MatrixFixed<4,4,double> dqnorm_by_dq(const VW::Quaternion &q)
{
  double qq = q.GetR()*q.GetR() + 
              q.GetX()*q.GetX() + q.GetY()*q.GetY() + q.GetZ()*q.GetZ();

  double a[16] = {
    dqi_by_dqi(q.GetR(), qq), 
    dqi_by_dqj(q.GetR(), q.GetX(), qq), 
    dqi_by_dqj(q.GetR(), q.GetY(), qq), 
    dqi_by_dqj(q.GetR(), q.GetZ(), qq), 
	
    dqi_by_dqj(q.GetX(), q.GetR(), qq), 
    dqi_by_dqi(q.GetX(), qq), 
    dqi_by_dqj(q.GetX(), q.GetY(), qq), 
    dqi_by_dqj(q.GetX(), q.GetZ(), qq), 
	
    dqi_by_dqj(q.GetY(), q.GetR(), qq), 
    dqi_by_dqj(q.GetY(), q.GetX(), qq), 
    dqi_by_dqi(q.GetY(), qq),
    dqi_by_dqj(q.GetY(), q.GetZ(), qq), 
	
    dqi_by_dqj(q.GetZ(), q.GetR(), qq), 
    dqi_by_dqj(q.GetZ(), q.GetX(), qq), 
    dqi_by_dqj(q.GetZ(), q.GetY(), qq), 
    dqi_by_dqi(q.GetZ(), qq)
  };

  VNL::MatrixFixed<4,4,double> M(a);
  return M;
}

VNL::MatrixFixed<3,3,double> dvnorm_by_dv(VW::Vector3D v)
{
  double vv = v.GetX()*v.GetX() + v.GetY()*v.GetY() + v.GetZ()*v.GetZ();

  // Можно использовать dqi_by_dqi и dqi_by_dqj, потому что формулы совпадают.
  double a[9] = {
    dqi_by_dqi(v.GetX(), vv), 
    dqi_by_dqj(v.GetX(), v.GetY(), vv), 
    dqi_by_dqj(v.GetX(), v.GetZ(), vv), 
	
    dqi_by_dqj(v.GetY(), v.GetX(), vv), 
    dqi_by_dqi(v.GetY(), vv),
    dqi_by_dqj(v.GetY(), v.GetZ(), vv), 
	
    dqi_by_dqj(v.GetZ(), v.GetX(), vv), 
    dqi_by_dqj(v.GetZ(), v.GetY(), vv), 
    dqi_by_dqi(v.GetZ(), vv)
  };

  VNL::MatrixFixed<3,3,double> M(a);
  return M;
}

/**
@}
*/
