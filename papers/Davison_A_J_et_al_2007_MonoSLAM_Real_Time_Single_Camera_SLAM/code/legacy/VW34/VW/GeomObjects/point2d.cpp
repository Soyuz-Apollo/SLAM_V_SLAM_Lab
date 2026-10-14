/** 
 * @file   point2d.cc
 * @author   ID Reid and BJ Tordoff
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


#include <stdlib.h>
#include <cmath>

#include <string>
#include <iostream>

//#include <VNL/math.h>


#include "point2d.h"


// NB Inline functions given in .h, not in the .cc file

using namespace VW;

////////////////////////////
// Creation / Destruction //
////////////////////////////

Point2D::Point2D() : 
  _x(0), 
  _y(0)
{
  // Nothing much to create...
}


Point2D::Point2D(const double x_coord, const double y_coord) :
  _x(x_coord),
  _y(y_coord)
{
  // Nothing much to create...
}

#if 0
Point2D::Point2D (const VNL::VectorFixed<3,double>& homg_point) :
  _x(homg_point[0]/homg_point[2]),
  _y(homg_point[1]/homg_point[2])
{
  if (VNL::IsNaN(_x)) _x = 1e100;
  if (VNL::IsNaN(_y)) _y = 1e100;
}

Point2D::Point2D (const VNL::VectorFixed<3,float>& homg_point) :
  _x(homg_point[0]/homg_point[2]),
  _y(homg_point[1]/homg_point[2])
{
	  if (VNL::IsNaN(_x)) _x = 1e100;
  if (VNL::IsNaN(_y)) _y = 1e100;
}
#endif

Point2D &Point2D::operator=(const Point2D &p) 
{
  if (this != &p) {
    _x=p._x;
    _y=p._y;
  } else {
    std::cout << "Error: copying point into itself!" << std::endl;
  }
  return *this;
}


Point2D::~Point2D() 
{
  // Nothing useful to delete, so a bit empty really.
}


bool Point2D::operator==(const Point2D &p) const
{
  return (_x==p._x && _y==p._y);
}


bool Point2D::operator!=(const Point2D &p) const 
{
  return (_x!=p._x || _y!=p._y);
}


Point2D VW::operator+(const Point2D &p1, const Point2D &p2) {
  Point2D p3;
  p3._x = p1._x + p2._x;
  p3._y = p1._y + p2._y;
  return p3;
}


Point2D VW::operator-(const Point2D &p)
{
  Point2D p2(-p._x,-p._y);
  return(p2);
}

Point2D VW::operator-(const Point2D &p1, const Point2D &p2) {
  Point2D p3;
  p3._x = p1._x - p2._x;
  p3._y = p1._y - p2._y;
  return p3;
}


double VW::operator*(const Point2D &p1, const Point2D &p2) {
  return p1._x*p2._x + p1._y*p2._y;
}


Point2D VW::operator*(const Point2D &p1, const double a) {
  Point2D p3;
  p3._x = a*p1._x;
  p3._y = a*p1._y;
  return p3;
}


Point2D VW::operator*(const double a, const Point2D &p1) {
  Point2D p3;
  p3._x = a*p1._x;
  p3._y = a*p1._y;
  return p3;
}


Point2D VW::operator/(const Point2D &p1, const double a) {
  Point2D p3;
  p3._x = p1._x/a;
  p3._y = p1._y/a;
  return p3;
}


void Point2D::Get(double &x_coord, double &y_coord) const 
{
  y_coord=_y;
  x_coord=_x;
}


