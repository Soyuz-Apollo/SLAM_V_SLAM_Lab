/** 
 * @file     pdfgaussian2diso.cpp
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


#include <cmath> // for sqrt, log

#include "random.h"
#include "pdfgaussian2diso.h"

const double VW::PDF::Gaussian2DIso::_TWOPI = 6.28318530717959;


VNL::VectorFixed<2,double> VW::PDF::Gaussian2DIso::Sample() const
{
  // Prob must lie between 0 and 1, so match a uniformly selected prob
  // to the value that corresponds, then re-weight using cos

  double u     = VW::Rand(1.0);

  double r = std::sqrt(-2*std::log(u));

  double theta = VW::Rand(_TWOPI);
  VNL::VectorFixed<2,double> e;
  e[0] = r * std::cos(theta);
  e[1] = r * std::sin(theta);

  // As it's isotropic, can multiply by uniform std dev
  return( _mu + _sigma * e );
}
