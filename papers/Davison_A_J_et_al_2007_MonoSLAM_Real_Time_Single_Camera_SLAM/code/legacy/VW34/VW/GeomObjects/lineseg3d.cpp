/** 
 * @file   lineseg3d.cpp
 * @author BJ Tordoff
 * @date   27/05/2003		   		
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



#include <string>
#include <iostream>

// Local includes
#include "point3d.h"
#include "lineseg3d.h"

// NB Inline functions given in .h, not in the .cc file

using namespace std;
using namespace VW;

////////////////////////////
// Creation / Destruction //
////////////////////////////

LineSeg3D::LineSeg3D() : 
  _sp(0,0,0), _ep(0,0,0)
{
  // Nothing much to create...
}


LineSeg3D::LineSeg3D(const double x0, const double y0, const double z0, 
		     const double x1, const double y1, const double z1)
 : _sp(x0, y0, z0), _ep(x1, y1, z1)
{
  // Nothing much to create...
}

LineSeg3D::LineSeg3D(const Point3D& p0, const Point3D& p1) :
  _sp(p0), _ep(p1)
{
  // Nothing much to create...
}

LineSeg3D::LineSeg3D(const Point3D *p0, const Point3D *p1) :
  _sp(*p0), _ep(*p1)
{
  // Nothing much to create...
}

LineSeg3D &LineSeg3D::operator=(const LineSeg3D &ls) 
{
  if (this != &ls) {
    _sp=ls._sp;
    _ep=ls._ep;
  } else {
    cerr << "Error: copying LineSeg3D into itself!" << endl;
  }
  return *this;
}


LineSeg3D::~LineSeg3D() 
{
  // Nothing useful to delete, so a bit empty really.
}


bool LineSeg3D::operator==(const LineSeg3D &ls) 
{
  return (_sp==ls._sp && _ep==ls._ep);
}


double VW::operator*(const LineSeg3D &ls1, const LineSeg3D &ls2) 
{
  VW::Vector3D d1(ls1._ep - ls1._sp);
  VW::Vector3D d2(ls2._ep - ls2._sp);

  return (d1.GetX() * d2.GetX() +
	  d1.GetY() * d2.GetY() +
	  d1.GetZ() * d2.GetZ());
}


double VW::Norm2(const LineSeg3D &ls)
{
  return ((ls._ep._x-ls._sp._x)*(ls._ep._x-ls._sp._x) 
	  + (ls._ep._y-ls._sp._y)*(ls._ep._y-ls._sp._y) 
	  + (ls._ep._z-ls._sp._z)*(ls._ep._z-ls._sp._z));
}


double VW::Norm(const LineSeg3D &ls)
{
  return sqrt((ls._ep._x-ls._sp._x)*(ls._ep._x-ls._sp._x) 
	      + (ls._ep._y-ls._sp._y)*(ls._ep._y-ls._sp._y) 
	      + (ls._ep._z-ls._sp._z)*(ls._ep._z-ls._sp._z));
}


ostream& VW::operator<<(ostream& s, const LineSeg3D& ls) {
  return s << ls._sp << "    " << ls._ep;
}


istream& VW::operator>>(istream& s, LineSeg3D& ls) {
  s >> ls._sp >> ls._ep;
  return s;
}
