/** 
 * @file   projectionmatrix.cpp
 * @author BJ Tordoff
 * @date   14/08/2003		   		
 *
 *              (c) Copyright University of Oxford
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


#include <VNL/Algo/svd.h>
#include <VNL/Algo/qr.h>
#include <VNL/matrix.h>
#include <VNL/Algo/matrixinverse.h>

#include "projectionmatrix.h"
#include "rotationmatrix.h"
#include "calibrationmatrix.h"


std::ostream& 
VW::ProjectionMatrix::operator<< (std::ostream& stream) const
{
  const VNL::Matrix<double>* base = (static_cast<const VNL::Matrix<double>* > (this));
  return stream << base;
}

void VW::ProjectionMatrix::Compose(const VW::CalibrationMatrix& K,
				   const VW::RotationMatrix& R,
				   const VW::Vector3D& t)
{
  // Normalise by calibration
  const VNL::MatrixFixed<3,3,double> R2 = K*R;
  const VW::Vector3D t2 = K*t;

  // Now build matrix
  data[0][3] = t2.GetX ();
  data[1][3] = t2.GetY ();
  data[2][3] = t2.GetZ ();

  this->SetColumns (0, R2);
  
}

bool VW::ProjectionMatrix::Decompose(VW::CalibrationMatrix& result_K,
				     VW::RotationMatrix& result_R,
				     VW::Vector3D& result_t) const
{
  /* This routine takes the left hand 3x3 block of the 3x4 projection  
   * matrix and splits it into KR */
  
  // Get 3x3 block
  const VNL::MatrixFixed <3,3,double> C = this->Extract (3, 3);
  
  // and translation part
  VNL::VectorFixed <3,double> t = this->GetColumn (3);
  

  
  VNL::Matrix < double > Cinv = VNL::MatrixInverse<double> (C);
  
  /* we do QR decomp on CRotinv.  
   * Q will be Rotinv, R will be Cinv  
   */
  VNL::QR<double> qr (Cinv);
  

  // On rare occasions, the decomposition fails and gives zero determinant
  const double a = qr.R()[0][0];
  const double b = qr.R()[0][1];
  const double c = qr.R()[0][2];
  const double d = qr.R()[1][1];
  const double e = qr.R()[1][2];
  const double f = qr.R()[2][2];
  const double det = a * d * f;
  if (det==0) {
    result_K.SetIdentity();
    result_R.SetIdentity();
    result_t.Set(0,0,0);
    return false;
  }

  // Now the calibration: invert by hand, taking advantage of triangularness
  const double invdet = 1.0 / det;
  VNL::MatrixFixed <3,3,double> K(0.0);
  K[0][0] =  d * f * invdet;
  K[0][1] = -b * f * invdet;
  K[0][2] = (b * e - d * c) * invdet;
  K[1][1] = a * f * invdet;
  K[1][2] = -e * a * invdet;
  K[2][2] = a * d * invdet;
  result_K.Set(K);


  // Get the rotation part
  result_R.Set(VW::RotationMatrix(qr.Q().Transpose()));
  


  // Also convert t using inv(K)
  t = qr.R () * t;
  
  result_t.Set (t[0], t[1], t[2]);
  return true;
}
