/** 
 * @file     pdfgaussian1d.cpp
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
#include "pdfgaussian1d.h"

static const double PI = 3.1415926535897932384626433832795; // 32 digits
const double VW::PDF::Gaussian1D::_ROOTTWOPI = 2.506628274631;


double VW::PDF::Gaussian1D::Sample() const
{
  // Prob must lie between 0 and 1, so match a uniformly selected prob
  // to the value that corresponds, then re-weight using cos

  double u     = VW::Rand(1.0);

  double r = std::sqrt(-2*std::log(u));

  double theta = VW::Rand(2 * PI);
  return( _mu + _sigma * r * std::cos(theta) );
}
