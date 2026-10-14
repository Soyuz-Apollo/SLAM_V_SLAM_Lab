/** 
 * @file    linesegfitted.cpp
 * @author  ID Reid and BJ Tordoff
 * @date    02/08/2002
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
 
 #include "linesegfitted.h"


using namespace std;
using namespace VW;

#ifndef PI
#define PI 3.1415927
#endif


LineSegFitted::LineSegFitted() 
: LineSeg2D()
{
  edgels.clear();

  info.sumx = 0;
  info.sumy = 0;
  info.sumxx = 0;
  info.sumxy = 0;
  info.sumyy = 0;

  info.R=1.0;
  info.n=0;
}


LineSegFitted::LineSegFitted(Edgel *e1, Edgel *e2) 
: LineSeg2D(e1, e2)
{
  edgels.clear();
  edgels.push_back(e1);
  edgels.push_back(e2);

  info.sumx = e1->_x + e2->_x;
  info.sumy = e1->_y + e2->_y;
  info.sumxx = (e1->_x)*(e1->_x) + (e2->_x)*(e2->_x);
  info.sumxy = e1->_x*e1->_y + e2->_x*e2->_y;
  info.sumyy = (e1->_y)*(e1->_y) + (e2->_y)*(e2->_y);

  info.R=1.0;
  info.n=2;
}

bool LineSegFitted::CorrectEndpoints()
{
  double a, b, c, cs, sn;

  // the endpoints can be rather unreliable so the tactic here is to
  // remove the first and last endpoint.  really we could/should compute 
  // line and then remove only outlying end points, but for now the 
  // simpler heuristic does a reasonable job

	if (edgels.size() <= 2) {
		
		// Too small, so just empty it and set to 0,0
		edgels.clear();
		info.sumx=0;
		info.sumy=0;
		info.sumxx=0;
		info.sumyy=0;
		info.sumxy=0;
		info.n=0;
		
		SetEndPoints(VW::Point2D(0,0),
								VW::Point2D(0,0));
		cout << "VW::LineSegFitted::CorrectEndpoints: Line is no more!" << endl;
		return false;
	
	} else {
	
  // remove first and last edgels since these tend to be unreliable
  Edgel oldsp = *(edgels.front()), oldep = *(edgels.back());
  edgels.pop_front();
  edgels.pop_back();
	

  info.sumx -= (oldsp._x + oldep._x);
  info.sumy -= (oldsp._y + oldep._y);
  info.sumxx -= (oldsp._x*oldsp._x + oldep._x*oldep._x);
  info.sumxy -= (oldsp._x*oldsp._y + oldep._x*oldep._y);
  info.sumyy -= (oldsp._y*oldsp._y + oldep._y*oldep._y);
  info.n -= 2;

  // now compute second moments
  a = info.sumxx - (info.sumx * info.sumx) / info.n;
  b = info.sumxy - info.sumx * info.sumy / info.n;
  c = info.sumyy - (info.sumy * info.sumy) / info.n;

  orient = 0.5*atan2(2*b, a-c);
  // Check if we're in the right direction
  if (fabs(this->GetAngle() - orient) < PI/2.0) {
    orient = PI+orient;
  } 

  const double new_length = edgels.front()->GetDistance( *edgels.back() );

  //cout << "Length = " << new_length << endl;
  midpoint._x = info.sumx/info.n;
  midpoint._y = info.sumy/info.n;

  // and adjust end points
  cs = cos(orient);
  sn = sin(orient);
  SetEndPoints(midpoint + 0.5*new_length*Vector2D(cs,sn),
	       midpoint - 0.5*new_length*Vector2D(cs,sn));
}
	return true;
}
