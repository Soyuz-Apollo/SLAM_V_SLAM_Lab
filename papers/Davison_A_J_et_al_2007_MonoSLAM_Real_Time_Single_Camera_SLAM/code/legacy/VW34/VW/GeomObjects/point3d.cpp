/** 
 * @file     point3d.cc
 * @author   BJ Tordoff
 * @date     02/08/2002
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

#include "point3d.h"


// NB Inline functions given in .h, not in the .cc file

using namespace std;
using namespace VW;

////////////////////////////
// Creation / Destruction //
////////////////////////////

Point3D::Point3D() : 
  _x(0), 
  _y(0),
  _z(0)
{
  // Nothing much to create...
}


Point3D::Point3D(const double x_coord, const double y_coord, const double z_coord) :
  _x(x_coord),
  _y(y_coord),
  _z(z_coord)
{
  // Nothing much to create...
}


Point3D &Point3D::operator=(const Point3D &p) 
{
  if (this != &p) {
    _x=p._x;
    _y=p._y;
    _z=p._z;
  } else {
    cout << "Error: copying point into itself!" << endl;
  }
  return *this;
}


Point3D::~Point3D() 
{
  // Nothing useful to delete, so a bit empty really.
}


bool Point3D::operator==(const Point3D &p) const 
{
  return (_x==p._x && _y==p._y && _z==p._z);
}


bool Point3D::operator!=(const Point3D &p) const 
{
  return (_x!=p._x || _y!=p._y || _z!=p._z);
}

Point3D VW::operator+(const Point3D &p1, const Point3D &p2) {
  Point3D p3;
  p3._x = p1._x + p2._x;
  p3._y = p1._y + p2._y;
  p3._z = p1._z + p2._z;
  return p3;
}


Point3D VW::operator-(const Point3D &p)
{
  Point3D p2(-p._x,-p._y,-p._z);
  return(p2);
}

Point3D VW::operator-(const Point3D &p1, const Point3D &p2) {
  Point3D p3;
  p3._x = p1._x - p2._x;
  p3._y = p1._y - p2._y;
  p3._z = p1._z - p2._z;
  return p3;
}


double VW::operator*(const Point3D &p1, const Point3D &p2) {
  return p1._x*p2._x + p1._y*p2._y + p1._z*p2._z;
}


Point3D VW::operator*(const Point3D &p1, const double a) {
  Point3D p3;
  p3._x = a*p1._x;
  p3._y = a*p1._y;
  p3._z = a*p1._z;
  return p3;
}


Point3D VW::operator*(const double a, const Point3D &p1) {
  Point3D p3;
  p3._x = a*p1._x;
  p3._y = a*p1._y;
  p3._z = a*p1._z;
  return p3;
}

Point3D VW::operator*(const VNL::MatrixFixed<3,3,double>& M, const Point3D &p1) {
  Point3D p3;
  p3._x = M[0][0]*p1._x + M[0][1]*p1._y + M[0][2]*p1._z;
  p3._y = M[1][0]*p1._x + M[1][1]*p1._y + M[1][2]*p1._z;
  p3._z = M[2][0]*p1._x + M[2][1]*p1._y + M[2][2]*p1._z;
  return p3;
}


Point3D VW::operator/(const Point3D &p1, const double a) {
  Point3D p3;
  p3._x = p1._x/a;
  p3._y = p1._y/a;
  p3._z = p1._z/a;
  return p3;
}


void Point3D::Get(double &x_coord, double &y_coord, double &z_coord) const 
{
  z_coord=_z;
  y_coord=_y;
  x_coord=_x;
}

