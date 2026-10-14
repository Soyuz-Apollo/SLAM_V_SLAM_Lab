/** @file atan2.cc
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

#include <math.h>
#include "atan2.h"


/**
 * @param y numerator of the tan(y/x) expression
 * @param x denominator of the tan(y/x) expression
 * @returns the four-quadrant arc-tangent: atan(y/x)
 */
#define M_PI 3.14159265358979323846

double VW::LinearAtan2(double y, double x)
{
  double a, absx, absy, orn;
  absx = fabs(x);
  absy = fabs(y);

  if(100.*absx < absy) {a=101;} else {a=absy/absx;}

  if ( a < 1.000000) { orn = 0.785398*a + 0.000000; }
  else {
    if (a>100)            { orn = M_PI/2;}
    else if ( a > 3.000000) { orn = 0.003214*a + 1.239404; }
    else if ( a > 2.000000) { orn = 0.141897*a + 0.823355; }
    else                    { orn = 0.321751*a + 0.463648; }
 }

  // quadrants
  if(x >= 0) {
    if( y> 0 ) {
      return(orn);
    } else if (y< 0) {
      return (-orn);
    }
    else {
      return(0);
    }
  } else { // x is  negative
    if( y> 0 ) {
      return(M_PI-orn);
    } else if (y< 0) {
      return (-M_PI+orn);
    }
    else {
      return(M_PI);
    }
  }
}

