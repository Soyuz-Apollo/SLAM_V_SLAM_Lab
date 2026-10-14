/** 
 * @file   essentialmatrix.cpp
 * @author BJ Tordoff
 * @date   19/04/2005		   		
 *
 *              (c) Copyright University of Cambridge
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#include <iostream>
#include <VNL/matrix.h>
#include <VNL/Algo/svd.h>
#include <VNL/Algo/determinant.h>

#include "essentialmatrix.h"


using namespace std;

std::ostream& VW::EssentialMatrix::operator<< (std::ostream& stream) const
{
  const VNL::Matrix<double>* base = (static_cast<const VNL::Matrix<double>* > (this));
  return stream << base;
}


void VW::EssentialMatrix::GetEpipoles (VW::Point2D& e1, VW::Point2D& e2) const
{
  const VNL::SVD<double> svd (*this);

  // Right null vector
  const double w1 = 1.0/svd.V()[2][2];
  e1.Set(w1*svd.V()[0][2], 
			w1*svd.V()[1][2]);

  // Left null vector
  const double w2 = 1.0/svd.U()[2][2];
  e2.Set(w2*svd.U()[0][2], 
			w2*svd.U()[1][2]);
}


/** EssentialMatrix::Constrain () -- Constrain to rank 2 (singularity condition) with two unity singular values.
 *
 * Enforces the singularity condition on the essential matrix using the SVD.
 * It is left to the user to call this, in certain circumstances it is best
 * to allow the user to decide when the constraint should occur.
 */

void VW::EssentialMatrix::Constrain ()
{
  const VNL::SVD<double> svd(*this);

  const VNL::Matrix<double>& U = svd.U();
  const VNL::Matrix<double>& V = svd.V();

  // Reform as U*diag(1,1,0)*V'
  data[0][0] = U[0][0]*V[0][0] + U[0][1]*V[0][1];
  data[0][1] = U[0][0]*V[1][0] + U[0][1]*V[1][1];
  data[0][2] = U[0][0]*V[2][0] + U[0][1]*V[2][1];

  data[1][0] = U[1][0]*V[0][0] + U[1][1]*V[0][1];
  data[1][1] = U[1][0]*V[1][0] + U[1][1]*V[1][1];
  data[1][2] = U[1][0]*V[2][0] + U[1][1]*V[2][1];

  data[2][0] = U[2][0]*V[0][0] + U[2][1]*V[0][1];
  data[2][1] = U[2][0]*V[1][0] + U[2][1]*V[1][1];
  data[2][2] = U[2][0]*V[2][0] + U[2][1]*V[2][1];

}

//function ex=vec(e)
//  ex=[0 -e(3) e(2); e(3) 0 -e(1); -e(2) e(1) 0];

static inline void SelectRt(VW::RotationMatrix& R,
			    VW::Vector3D& t)
{
 // function [R,t]=SelectRt(R,t)
// Rtp=eye(3)+2*vec(t/norm(t))^2;
// l1=[0 0 1]';
// O1=zeros(3,1);
// for i=1:4
//   if i==3 
//     R=Rtp*R;
//   end
//   l2=R'*[0 0 1]';
//   O2=inv([R t;0 0 0 1])*[0 0 0 1]';
//   O2=O2(1:3);
//
//   p1=vec(l1)^2*l2;
//   k2=p1'*(O1-O2)/(p1'*l2);
//
//   p2=vec(l2)^2*l1;
//   k1=p2'*(O2-O1)/(p2'*l1);
//   if k1>0 & k2>0 
//     break;
//   end
//   t=-t;
// end

  const double xy = 2*t.GetX()*t.GetY();
  const double xz = 2*t.GetX()*t.GetZ();
  const double yz = 2*t.GetY()*t.GetZ();
  const double x2 = 2*t.GetX()*t.GetX();
  const double y2 = 2*t.GetY()*t.GetY();
  const double z2 = 2*t.GetZ()*t.GetZ();

  VNL::MatrixFixed<3,3,double> Rtp;
  Rtp[0][0]=1-y2-z2; Rtp[0][1]=     xy; Rtp[0][2]=     xz; 
  Rtp[1][0]=     xy; Rtp[1][1]=1-x2-z2; Rtp[1][2]=     yz; 
  Rtp[2][0]=     xz; Rtp[2][1]=     yz; Rtp[2][2]=1-x2-y2; 
  
  const VW::Vector3D l1(0,0,1);

  for (unsigned int i=0;i<3;i++) {
    if (i==2) {
      R=Rtp*R;
    }
    const VW::Vector3D l2(R[2][0],R[2][1],R[2][2]), o2=-R.Transpose()*t;
    t=-t;

    const VW::Vector3D p1(-l2.GetX(), -l2.GetY(), 0);
    const double depth2 = -(p1*o2) / (p1*l2);
    
    if (depth2<0) continue;

    const VW::Vector3D p2(l2[0]*l2[1],
			  l2[1]*l2[2],
			  -(l2[0]*l2[0]+l2[1]*l2[1]));
    const double depth1 = (p1*o2) / (p2*l1);
    
    if ((depth1>0) && (depth2>0)) {
      break;
    }
  }


   
}

void VW::EssentialMatrix::Decompose(VW::RotationMatrix& R,
				    VW::Vector3D& t) const
{
  // Matlab code from Carlos
// function [R,t]=Extrinsics(E)
// R0=[0 1 0;-1 0 0;0 0 1];
// t0=[0 -1 0;1 0 0;0 0 0];
// [U,S,V]=svd(E);
// [tmp,I]=sort(diag(S));
// sigma=(S(I(3),I(3))+S(I(2),I(2)))/2;
// R=U*R0*V';
// tx=sigma*U*t0*U';
// t=[-tx(2,3);tx(1,3);-tx(1,2)];
// R=R*sign(det(R));
// t=t*sign(det(R));

// [R,t]=SelectRt(R,t);

  VNL::MatrixFixed<3,3,double> R0,t0;
  R0[0][0]= 0; R0[0][1]= 1; R0[0][2]= 0;
  R0[1][0]=-1; R0[1][1]= 0; R0[1][2]= 0;
  R0[2][0]= 0; R0[2][1]= 0; R0[2][2]= 1;

  t0[0][0]= 0; t0[0][1]=-1; t0[0][2]= 0;
  t0[1][0]= 1; t0[1][1]= 0; t0[1][2]= 0;
  t0[2][0]= 0; t0[2][1]= 0; t0[2][2]= 0;

  VNL::SVD<double> svd(*this);

  // Average the two non-zero singular values
  const double sigma = 0.5*(svd.W()[0] + svd.W()[1]);

  R0 = svd.U() * R0 * svd.V().Transpose();
  t0 = sigma * svd.U() * t0 * svd.U().Transpose();

  const double det = VNL::Determinant(R0);
  if (det>0) {
    R = R0;
    t.Set(-t0[1][2], t0[0][2], -t0[0][1]);
  } else {
    R = -R0;
    t.Set(t0[1][2], -t0[0][2], t0[0][1]);
  }

  SelectRt(R,t);

}

