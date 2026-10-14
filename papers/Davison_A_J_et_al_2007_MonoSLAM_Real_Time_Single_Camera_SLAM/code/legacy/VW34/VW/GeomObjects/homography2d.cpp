/** 
 * @file   homography2d.cpp
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

#include "homography2d.h"

#include <VNL/Algo/svd.h>
#include <VW/MathBits/covariance2d.h>

const char *const VW::Homography2D::NAME = "Homography2D";

VW::Homography2D VW::Homography2D::Inverse() const
{
  VNL::SVD<double> svd(*this);
  VW::Homography2D inv(svd.Inverse());
  return inv;
}

void VW::TransferPoint(const VW::Point2D& source_point,
		       const VW::Homography2D& H,
		       const VW::Homography2DCovariance& HCovar,
		       const float image_noise_variance,
		       VW::Point2D& dest_point,
		       VW::Covariance2D& dest_covar)
{
  VNL::VectorFixed<3,double> tp;
  tp[0] = H[0][0]*source_point.GetX() + H[0][1]*source_point.GetY() + H[0][2];
  tp[1] = H[1][0]*source_point.GetX() + H[1][1]*source_point.GetY() + H[1][2];
  tp[2] = H[2][0]*source_point.GetX() + H[2][1]*source_point.GetY() + H[2][2];

  const double w = 1.0 / tp[2];
  const double w2 = w*w;

  dest_point.Set(tp[0]*w, tp[1]*w);

  // Covariance has three parts, due to noise in image 1, noise in image 2 and
  // noise in H
  const Covariance3D Cx(image_noise_variance, 0, 0, image_noise_variance, 0, 0);
  const Covariance3D Cxt = VW::MapCovariance(H,Cx);
  const Covariance3D CHt = HCovar.TransferUncertainty(source_point);
  const Covariance3D C =CHt + Cxt;

  // Now project around the homg point
  const double x = dest_point.GetX();
  const double y = dest_point.GetY();
  dest_covar[0] = C[0] - 2.0*C[2]*x*w + C[5]*x*x*w2;
  dest_covar[1] = C[1] - C[2]*y*w - C[4]*x*w + C[5]*x*y*w2;
  dest_covar[2] = C[3] - 2.0*C[4]*y*w + C[5]*y*y*w2;
  dest_covar *= w2;
}
