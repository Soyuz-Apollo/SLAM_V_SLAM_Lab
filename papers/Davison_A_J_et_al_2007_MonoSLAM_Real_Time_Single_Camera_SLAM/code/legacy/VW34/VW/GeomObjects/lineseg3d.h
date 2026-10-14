/** 
 * @file   lineseg2d.h
 * @author ID Reid
 * @date   02/08/2002		   		
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
/**
 * @class   VW::LineSeg3D
 * @ingroup GeometricObjects
 * @author  BJ Tordoff
 * @date    27/05/2003
 * @brief   Representation of a 3D line segment (ie a line with endpoints).
 * 
 * (See Line3D for representing infinite 3D lines.)
 */


#ifndef _lineseg3d_h_
#define _lineseg3d_h_

#include <iostream>
#include "point3d.h"

namespace VW {

class LineSeg3D 
{
 public:
  LineSeg3D (void);
  LineSeg3D (const double x0, const double y0, const double z0, 
	     const double x1, const double y1, const double z1);
  LineSeg3D (const Point3D& p0, const Point3D& p1);
  LineSeg3D (const Point3D *p0, const Point3D *p1);
  ~LineSeg3D();
  LineSeg3D &operator=(const LineSeg3D &ls);

  bool operator==(const LineSeg3D &ls);
  LineSeg3D &operator+=(const Point3D &p);
  LineSeg3D &operator-=(const Point3D &p);
  
  /** l1*l2*cos(theta), where l1=length of line 1, l2 = length of line 2, theta=angle between. */
  friend double operator*(const LineSeg3D &ls1, const LineSeg3D &ls2);
  friend double Norm2(const LineSeg3D &ls);
  friend double Norm(const LineSeg3D &ls);
  friend std::ostream& operator<<(std::ostream& s, const LineSeg3D &ls);
  friend std::istream& operator>>(std::istream& s, LineSeg3D &ls);

  void GetEndPoints(double &x0, double &y0, double &z0,
		    double &x1, double &y1, double &z1) const;
  void GetEndPoints(Point3D &p0, Point3D &p1) const;

  double GetLength() const;
  double GetLength2() const;

  // Get by value
  Point3D GetStartPoint(void) const;
  Point3D GetEndPoint(void) const;

  // Get by reference
  Point3D& StartPoint();
  Point3D& EndPoint();
  const Point3D& StartPoint() const;
  const Point3D& EndPoint() const;

  void SetEndPoints(const double x0, const double y0, const double z0, 
		    const double x1, const double y1, const double z1);
  void SetEndPoints(const Point3D& p0, const Point3D& p1);
  void SetStartPoint(const Point3D& p);
  void SetEndPoint(const Point3D& p);

 protected:
  // Data
  Point3D _sp, _ep;

};

  // Other funcs
VW::LineSeg3D operator+(const VW::LineSeg3D &line, const VW::Point3D &point);
VW::LineSeg3D operator-(const VW::LineSeg3D &line, const VW::Point3D &point);

}; // end namespace VW

















// Inline functions given here, not in the .cc file

///////////////////////////////////////////////////////////

inline double VW::LineSeg3D::GetLength() const
{
  return  sqrt(GetLength2());
}

inline double VW::LineSeg3D::GetLength2() const
{
   return ((_ep._x-_sp._x)*(_ep._x-_sp._x) 
	  + (_ep._y-_sp._y)*(_ep._y-_sp._y) 
	  + (_ep._z-_sp._z)*(_ep._z-_sp._z)); 
}

inline void 
VW::LineSeg3D::GetEndPoints(double &x0, double &y0, double &z0,
			    double &x1, double &y1, double &z1) const 
{ 
  _sp.Get(x0,y0,z0); 
  _ep.Get(x1,y1,z1); 
}

inline void 
VW::LineSeg3D::GetEndPoints(VW::Point3D &p0, VW::Point3D &p1) const 
{ 
  p0=_sp; 
  p1=_ep; 
}

inline VW::Point3D 
VW::LineSeg3D::GetStartPoint(void) const 
{ 
  return _sp; 
}

inline VW::Point3D 
VW::LineSeg3D::GetEndPoint(void) const 
{ 
  return _ep; 
}

inline VW::Point3D& 
VW::LineSeg3D::StartPoint()
{
  return _sp; 
}

inline VW::Point3D& 
VW::LineSeg3D::EndPoint()
{
  return _ep; 
}

inline const VW::Point3D& 
VW::LineSeg3D::StartPoint() const
{
  return _sp; 
}

inline const VW::Point3D& 
VW::LineSeg3D::EndPoint() const
{
  return _ep; 
}

inline void 
VW::LineSeg3D::SetEndPoints(const double x0, const double y0, const double z0,
			    const double x1, const double y1, const double z1) 
{ 
  _sp.Set(x0,y0,z0); 
  _ep.Set(x1,y1,z1); 
}

inline void 
VW::LineSeg3D::SetEndPoints(const VW::Point3D& p0, const VW::Point3D& p1) 
{ 
  _sp=p0; 
  _ep=p1; 
}

inline void 
VW::LineSeg3D::SetStartPoint(const VW::Point3D& p) 
{ 
  _sp=p;
}

inline void 
VW::LineSeg3D::SetEndPoint(const VW::Point3D& p) 
{ 
  _ep=p;
}

inline VW::LineSeg3D&
VW::LineSeg3D::operator+=(const Point3D &p)
{
  _sp += p;
  _ep += p;
  return *this;
}

inline VW::LineSeg3D&
VW::LineSeg3D::operator-=(const Point3D &p)
{
  _sp -= p;
  _ep -= p;
  return *this;
}

inline VW::LineSeg3D 
VW::operator+(const VW::LineSeg3D &line, const VW::Point3D &point)
{
  VW::LineSeg3D l(line);
  l += point;
  return l;
}

inline VW::LineSeg3D 
VW::operator-(const VW::LineSeg3D &line, const VW::Point3D &point)
{
  VW::LineSeg3D l(line);
  l -= point;
  return l;
}

#endif /* _lineseg3d_h */












