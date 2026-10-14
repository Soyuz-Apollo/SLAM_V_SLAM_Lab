/** 
 * @file   fmatrixaffine.cpp
 * @author BJ Tordoff
 * @date   08/04/2005		   		
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


#include <math.h> // for sqrt
#include "fmatrixaffine.h"

VW::Line2D 
VW::FMatrixAffine::Transfer(const VW::Point2D& point) const
{
  const double scale = 1.0 / sqrt(_a*_a + _b*_b);
  const double dist = _c*point.GetX() + _d*point.GetY() + _e;
  return VW::Line2D(scale*_a, scale*_b, scale*dist);
}

VW::Line2D 
VW::FMatrixAffine::ReverseTransfer(const VW::Point2D& point) const
{
  const double scale = 1.0 / sqrt(_c*_c + _d*_d);
  const double dist = _a*point.GetX() + _b*point.GetY() + _e;
  return VW::Line2D(scale*_c, scale*_d, scale*dist);
}
