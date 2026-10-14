/** 
 * @file   normalisation.cpp
 * @author JGH Knight
 * @date   02/11/2002		   		
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


#ifndef _normalisation_cc_
#define _normalisation_cc_

#include <VNL/copy.h>

#include "calibrationmatrix.h"
#include "normalisation.h"

using namespace std;

 const float VW::Normalisation::DEFAULT_WIDTH_HEIGHT_RATIO = 0.75;

double VW::Normalisation::NormaliseX(double x) const
{
  return (x - _centre_x) * _scale;
}

double VW::Normalisation::NormaliseY(double y) const
{
  return (y - _centre_y) * _scale;
}

double VW::Normalisation::UnNormaliseX(double x) const
{
  return x/_scale + _centre_x;
}

double VW::Normalisation::UnNormaliseY(double y) const
{
  return y/_scale + _centre_y;
}


VW::Point2D VW::Normalisation::Normalise(const VW::Point2D& point) const
{
  const double tx = (point.GetX() - _centre_x) * _scale;
  const double ty = (point.GetY() - _centre_y) * _scale;
  return VW::Point2D(tx,ty);
}

VW::Line2D VW::Normalisation::Normalise(const VW::Line2D& line) const
{
  VW::Line2D lout = line;
  lout[2] += lout[0]*_centre_x + lout[1]*_centre_y;
  lout[0] /= _scale;
  lout[1] /= _scale;
  return lout;
}

VW::LineSeg2D VW::Normalisation::Normalise(const VW::LineSeg2D& lineseg) const
{
  return VW::LineSeg2D(Normalise(lineseg.GetStartPoint()),
		       Normalise(lineseg.GetEndPoint()));
}


VW::CalibrationMatrix 
VW::Normalisation::Normalise(const VW::CalibrationMatrix& K) const
{
  return CalibrationMatrix(K.GetFocalLength()*_scale,
			   K.GetAspectRatio(),
			   K.GetSkew(),
			   (K.GetPrincipalPointX() - _centre_x) * _scale,
			   (K.GetPrincipalPointY() - _centre_y) * _scale);
			   
}


VW::Point2D VW::Normalisation::UnNormalise(const VW::Point2D& point) const
{
  const double tx = point.GetX() / _scale + _centre_x;
  const double ty = point.GetY() / _scale + _centre_y;
  return VW::Point2D(tx,ty);
}

VW::Line2D VW::Normalisation::UnNormalise(const VW::Line2D& line) const
{
  VW::Line2D lout = line;
  lout[2] -= _scale * (lout[0]*_centre_x + lout[1]*_centre_y);
  lout[0] *= _scale;
  lout[1] *= _scale;
  return lout;
}

VW::LineSeg2D VW::Normalisation::UnNormalise(const VW::LineSeg2D& lineseg) const
{
  return VW::LineSeg2D(UnNormalise(lineseg.GetStartPoint()),
		       UnNormalise(lineseg.GetEndPoint()));
}


VW::CalibrationMatrix 
VW::Normalisation::UnNormalise(const VW::CalibrationMatrix& K) const
{
  return CalibrationMatrix(K.GetFocalLength()/_scale,
			   K.GetAspectRatio(),
			   K.GetSkew(),
			   K.GetPrincipalPointX()/_scale + _centre_x,
			   K.GetPrincipalPointY()/_scale + _centre_y);
			   
}







// Template stuff


#ifndef _WIN32
// Instantiations for Normalisation (const vector<Feature>& feature_list, point/line)
template VW::Normalisation::Normalisation (const vector<VW::Point2D>&, VW::Point2D);
// template VW::Normalisation::Normalisation (const vector<VW::PlesseyCorner>&, VW::Point2D);
template VW::Normalisation::Normalisation (const vector<VW::Line2D>&, VW::Line2D);
#endif


// Instantiations for Init (const vector<Feature>& feature_list, point/line)
template void VW::Normalisation::Init (const vector<VW::Point2D>&, VW::Point2D);
// template void VW::Normalisation::Init (const vector<VW::PlesseyCorner>&, VW::Point2D);
template void VW::Normalisation::Init (const vector<VW::Line2D>&, VW::Line2D);

// Instantiations for Normalise (feature[_list])
template void VW::Normalisation::Normalise (vector<VW::Point2D>&, VW::Point2D) const;
// template void VW::Normalisation::Normalise (vector<VW::PlesseyCorner>&, VW::Point2D) const;
template void VW::Normalisation::Normalise (vector<VW::Line2D>&, VW::Line2D) const;
template VW::Point2D VW::Normalisation::Normalise (const VW::Point2D&, VW::Point2D) const;
// template VW::PlesseyCorner VW::Normalisation::Normalise (const VW::PlesseyCorner&, VW::Point2D) const;
template VW::Line2D VW::Normalisation::Normalise (const VW::Line2D&, VW::Line2D) const;

// {{{ vnl_matrix_fixed<float,3,3,double> VW::Normalisation::Normalise (const vnl_matrix_fixed<float,3,3,double>& M)

VNL::MatrixFixed<3,3,float> 
VW::Normalisation::Normalise (const VNL::MatrixFixed<3,3,float>& M) const
{
  VNL::Matrix<float> C;
  VNL::MatrixFixed<3,3,double> Casdouble = this->AsMatrix();
  VNL::Copy (dynamic_cast<VNL::Matrix<double>&> (Casdouble), C);
  return C * M;
}

// }}}

// Instantiations of UnNormalise (feature[_list])
template void VW::Normalisation::UnNormalise (vector<VW::Point2D>&, VW::Point2D) const;
// template void VW::Normalisation::UnNormalise (vector<VW::PlesseyCorner>&, VW::Point2D) const;
template void VW::Normalisation::UnNormalise (vector<VW::Line2D>&, VW::Line2D) const;
template VW::Point2D VW::Normalisation::UnNormalise (const VW::Point2D&, VW::Point2D) const;
// template VW::PlesseyCorner VW::Normalisation::UnNormalise (const VW::PlesseyCorner&, VW::Point2D) const;
template VW::Line2D VW::Normalisation::UnNormalise (const VW::Line2D&, VW::Line2D) const;

// {{{ VW::Normalisation::UnNormalise (const MatrixFixed<3,3,float>& M)

VNL::MatrixFixed<3,3,float> 
VW::Normalisation::UnNormalise (const VNL::MatrixFixed<3,3,float>& M) const
{
  using namespace VNL;
  Matrix<float> Cinv;
  MatrixFixed<3,3,double> Cinvasdouble = this->InverseMatrix ();
  VNL::Copy (dynamic_cast<Matrix<double>&> (Cinvasdouble), Cinv);
  return Cinv * M;
}

// }}}

VNL::MatrixFixed<3,3,double> 
VW::Normalisation::AsMatrix () const
{
  VNL::MatrixFixed<3,3,double> C (0.0);
  C(0,0) = _scale;
  C(1,1) = _scale;
  C(0,2) = -_centre_x * _scale;
  C(1,2) = -_centre_y * _scale;
  C(2,2) = 1.0;
  return C;
}


VNL::MatrixFixed<3,3,double> 
VW::Normalisation::InverseMatrix () const
{
  VNL::MatrixFixed<3,3,double> Cinv (0.0);
  Cinv(0,0) = 1/_scale;
  Cinv(1,1) = 1/_scale;
  Cinv(0,2) = _centre_x;
  Cinv(1,2) = _centre_y;
  Cinv(2,2) = 1.0;
  return Cinv;
}



#endif // #ifndef _normalisation_cc_
