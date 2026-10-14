/** 
 * @file   fmatrix.cpp
 * @author JGH Knight
 * @date   02/08/2002		   		
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


#ifndef _fmatrix_cpp_
#define _fmatrix_cpp_

#include <iostream>
#include <VNL/matrix.h>
#include <VNL/Algo/svd.h>

#include "fmatrix.h"


using namespace std;

const char * const VW::FMatrix::NAME = "FMatrix";

std::ostream& VW::FMatrix::operator<< (std::ostream& stream) const
{
  const VNL::Matrix<double>* base = (static_cast<const VNL::Matrix<double>* > (this));
  return stream << base;
}


void VW::FMatrix::GetEpipoles (VW::Point2D& e1, VW::Point2D& e2) const
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


/** FMatrix::Constrain () -- Constrain to rank 2 (singularity condition)
 *
 * Enforces the singularity condition on the fundamental matrix using the SVD.
 * It is left to the user to call this, in certain circumstances it is best
 * to allow the user to decide when the constraint should occur.
 */

void VW::FMatrix::Constrain ()
{
  VNL::SVD<double> svdF (*this);
  svdF.W()(2,2) = 0.0;
  *this = svdF.Recompose ();
}


void VW::TransferPoint(const VW::Point2D& source_point,
		       const VW::FMatrix& F,
		       const VW::FMatrixCovariance& FCovar,
		       const float image_noise_variance,
		       VW::Line2D& dest_line,
		       VW::Covariance3D& dest_line_covar)
{
  const double x=source_point.GetX();
  const double y=source_point.GetY();
  dest_line[0] = F[0][0]*x + F[0][1]*y + F[0][2];
  dest_line[1] = F[1][0]*x + F[1][1]*y + F[1][2];
  dest_line[2] = F[2][0]*x + F[2][1]*y + F[2][2];

  // Covariance has three parts, due to noise in image 1, noise in image 2 and
  // noise in H
  const Covariance3D Cx(image_noise_variance, 0, 0, image_noise_variance, 0, 0);
  const Covariance3D Cxt = VW::MapCovariance(F,Cx);
  const Covariance3D CFt = FCovar.TransferUncertainty(source_point);
  dest_line_covar = CFt + Cxt;

  /** Normalise the line so that distances are correct */
  VW::NormaliseLine(dest_line, dest_line_covar);

}
double VW::GetDistanceVariance(const VW::Point2D& dest_point,
			       const VW::Line2D& epipolar_line,
			       const VW::Covariance3D& epipolar_line_covar,
			       const float image_noise_variance)
{
  const VW::Covariance3D& C = epipolar_line_covar;
  const double x = dest_point.GetX(), y=dest_point.GetY();
  const double v = image_noise_variance 
    + C[0]*x*x + C[3]*y*y + C[5]
    + 2.0*(C[1]*x*y + C[2]*x + C[4]*y); 
  return v;
}

#endif // #ifndef _fmatrix_cpp_
