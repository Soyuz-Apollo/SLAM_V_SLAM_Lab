/** 
 * @file   normalisation3d.cpp
 * @author BJ Tordoff
 * @date   15/08/2003		   		
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


#ifndef _normalisation3d_cc_
#define _normalisation3d_cc_

#include <VNL/copy.h>

#include "normalisation3d.h"

using namespace std;


// Instantiations for Init (const vector<Feature>& feature_list, point/line)
template void 
VW::Normalisation3D::Init (const vector<VW::Point3D>&);



// Instantiations for Normalise (feature[_list])
template void 
VW::Normalisation3D::Normalise (vector<VW::Point3D>&, 
				VW::Point3D) const;

template VW::Point3D 
VW::Normalisation3D::Normalise (const VW::Point3D&, 
				VW::Point3D) const;



VNL::MatrixFixed<4,4,float> 
VW::Normalisation3D::Normalise (const VNL::MatrixFixed<4,4,float>& M) const
{
  VNL::Matrix<float> C;
  VNL::MatrixFixed<4,4,double> Casdouble = this->AsMatrix();
  VNL::Copy (dynamic_cast<VNL::Matrix<double>&> (Casdouble), C);
  return C * M;
}

// Instantiations of UnNormalise (feature[_list])
template void 
VW::Normalisation3D::UnNormalise (vector<VW::Point3D>&, 
				  VW::Point3D) const;

template VW::Point3D 
VW::Normalisation3D::UnNormalise (const VW::Point3D&, 
				  VW::Point3D) const;


VNL::MatrixFixed<4,4,float> 
VW::Normalisation3D::UnNormalise (const VNL::MatrixFixed<4,4,float>& M) const
{
  using namespace VNL;
  Matrix<float> Cinv;
  MatrixFixed<4,4,double> Cinvasdouble = this->InverseMatrix ();
  VNL::Copy (dynamic_cast<Matrix<double>&> (Cinvasdouble), Cinv);
  return Cinv * M;
}


VNL::MatrixFixed<4,4,double> 
VW::Normalisation3D::AsMatrix () const
{
  VNL::MatrixFixed<4,4,double> C (0.0);
  C(0,0) = _scale;
  C(1,1) = _scale;
  C(2,2) = _scale;
  C(0,3) = -_centre_x * _scale;
  C(1,3) = -_centre_y * _scale;
  C(2,3) = -_centre_z * _scale;
  C(3,3) = 1.0;
  return C;
}


VNL::MatrixFixed<4,4,double> 
VW::Normalisation3D::InverseMatrix () const
{
  VNL::MatrixFixed<4,4,double> Cinv (0.0);
  Cinv(0,0) = 1/_scale;
  Cinv(1,1) = 1/_scale;
  Cinv(2,2) = 1/_scale;
  Cinv(0,3) = _centre_x;
  Cinv(1,3) = _centre_y;
  Cinv(2,3) = _centre_z;
  Cinv(3,3) = 1.0;
  return Cinv;
}



#endif // #ifndef _normalisation_cc_
