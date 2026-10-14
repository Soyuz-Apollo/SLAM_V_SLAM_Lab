/** 
 * @file     pdfgaussian2d.cpp
 * @author   BJ Tordoff
 * @date     30/06/2003
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
#include "pdfgaussian2d.h"

#include <iostream>
#include <VNL/Algo/cholesky.h>

const double VW::PDF::Gaussian2D::_TWOPI = 6.28318530717959;

VW::PDF::Gaussian2D::Gaussian2D(const VNL::VectorFixed<2,double>& mean, 
				const VNL::MatrixFixed<2,2,double>& covariance)
  : PDFBase2D(mean,covariance)
{
  // For 2x2, decomposition is easy
  double det = covariance[0][0] * covariance[1][1] 
    - covariance[0][1] * covariance[1][0];

  assert(det>0);

  double invdet = 1.0 / det;

  _invC[0][0] = invdet * covariance[1][1];
  _invC[1][1] = invdet * covariance[0][0];
  _invC[0][1] = -1 * invdet * covariance[0][1];
  _invC[1][0] = -1 * invdet * covariance[1][0];

  _rootC[0][0] = std::sqrt(covariance[0][0]);
  _rootC[0][1] = covariance[0][1] / _rootC[0][0];
  _rootC[1][0] = 0;
  _rootC[1][1] = std::sqrt(covariance[1][1] - (_rootC[0][1]*_rootC[0][1]));

  _prefix = invdet / _TWOPI;

  std::cerr << "Check decom:\nC="<<covariance<<"\nrootC="<<_rootC<<"\nrootC^2="<<_rootC.Transpose()*_rootC << std::endl;


}
VNL::VectorFixed<2,double> VW::PDF::Gaussian2D::Sample() const
{
  std::cerr << "Sorry, cannot sample from a VW::PDF::Gaussian2D just yet." << std::endl;
  assert(false);
  return false;
}
