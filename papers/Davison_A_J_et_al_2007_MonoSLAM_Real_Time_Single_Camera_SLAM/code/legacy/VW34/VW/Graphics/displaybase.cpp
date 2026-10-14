/** @file displaybase.cpp
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
#include "displaybase.h"

#define NUM_CIRC_POINTS 32
#define PI 3.14159265358979323846f

static float circ_x[NUM_CIRC_POINTS];
static float circ_y[NUM_CIRC_POINTS];
static bool circ_made = false;


static void 
_MakeCircPoints()
{
  circ_made = true;
  const float step = 2.0 * PI / float(NUM_CIRC_POINTS);
  float theta = 0.0;
  for (unsigned int i=0;i<NUM_CIRC_POINTS;i++, theta += step) {
    circ_x[i] = sin(theta);
    circ_y[i] = cos(theta);
  }
}


void 
VW::DisplayBase::DrawRightEllipse(const float x0, const float y0, 
				  const float rx, const float ry)
{
  if (!circ_made) _MakeCircPoints();

  this->DrawLine(x0 + rx*circ_x[NUM_CIRC_POINTS-1], 
		 y0 + ry*circ_y[NUM_CIRC_POINTS-1],
		 x0 + rx*circ_x[0], 
		 y0 + ry*circ_y[0]);

  for (unsigned int i=1;i<NUM_CIRC_POINTS;i++) {
    this->DrawLine(x0 + rx*circ_x[i-1], y0 + ry*circ_y[i-1],
		   x0 + rx*circ_x[i],   y0 + ry*circ_y[i]  );    
  }
		 

}
void 
VW::DisplayBase::DrawEllipse(const float x0, const float y0, 
			     const float rx, const float ry, const float skew)
{
  if (!circ_made) _MakeCircPoints();

  this->DrawLine(x0 + rx*circ_x[NUM_CIRC_POINTS-1], 
		 y0 + skew*circ_x[NUM_CIRC_POINTS-1] + ry*circ_y[NUM_CIRC_POINTS-1],
		 x0 + rx*circ_x[0], 
		 y0 + skew*circ_x[0] + ry*circ_y[0]);

  for (unsigned int i=1;i<NUM_CIRC_POINTS;i++) {
    this->DrawLine(x0 + rx*circ_x[i-1], 
		   y0 + skew*circ_x[i-1] + ry*circ_y[i-1],
		   x0 + rx*circ_x[i],   
		   y0 + skew*circ_x[i] + ry*circ_y[i]  );    
  }
		 

}
