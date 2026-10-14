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
 * @class   VW::LineSeg2D
 * @ingroup GeometricObjects
 * @author  ID Reid
 * @date    02/08/2002
 * @brief   Representation of a 2D line segment.
 */


#ifndef _lineseg2d_h_
#define _lineseg2d_h_

#include <iostream>
#include "point2d.h"
#include "line2d.h"

namespace VW {

class LineSeg2D 
{
 public:
  LineSeg2D (void);
  LineSeg2D (const double x0, const double y0, 
	     const double x1, const double y1);
  LineSeg2D (const Point2D& p0, const Point2D& p1);
  LineSeg2D (const Point2D *p0, const Point2D *p1);
  virtual ~LineSeg2D();
  LineSeg2D &operator=(const LineSeg2D &ls);


 bool operator==(const LineSeg2D &ls);
  friend double operator*(const LineSeg2D &ls1, const LineSeg2D &ls2);
  friend std::ostream& operator<<(std::ostream& s, const LineSeg2D &ls);
  friend std::istream& operator>>(std::istream& s, LineSeg2D &ls);

  void GetEndPoints(double &x0, double &y0, double &x1, double &y1) const;
  void GetEndPoints(Point2D &p0, Point2D &p1) const;
  Point2D GetStartPoint(void) const;
  double GetStartX(void) const;
  double GetStartY(void) const;
  Point2D GetEndPoint(void) const;
  double GetEndX(void) const;
  double GetEndY(void) const;
  Point2D GetMidPoint(void) const;

  void SetEndPoints(const double x0, const double y0, const double x1, const double y1);
  void SetEndPoints(const Point2D p0, const Point2D p1);
  void SetStartPoint(const Point2D p);
  void SetEndPoint(const Point2D p);

  /** Truncate to lie within a rectangle (x0,y0)->(x1,y1) 
   * (eg for displaying) 
   */
  LineSeg2D Truncate(const double x0, const double y0,
		     const double x1, const double y1) const;

  /** Get the direction. */
  Vector2D GetDirection() const;

  /** Get the length of the segment. */
  double GetLength() const;
 /** Get the sqaured length of the segment. */
 double GetLength2() const;
 /** The norm (length) of the segment. */
  double Norm() const;
 /** The squared norm (length) of the segment. */
  double Norm2() const;

/** Return the angle between the +ve X-axis and the line, measured 
   * clockwise (\f$-\pi\le\theta\le\pi\f$). 
   */
  double GetAngle() const;

  /** Return the perpendicular distance of line from origin. 
   */
  double GetDistanceToOrigin() const;

  /** The distance to the left of the line to reach the testpoint (negative if to right). 
   */
  double DistanceToLeft(const VW::Point2D& testpoint) const;

  /** Is a point to the left of the line. */
  bool IsToLeft(const VW::Point2D& testpoint) const;

  /** The distance to the right of the line to reach the testpoint (negative if to left). 
   */
  double DistanceToRight(const VW::Point2D& testpoint) const;

  /** Is a point to the right of the line. */
  bool IsToRight(const VW::Point2D& testpoint) const;

  /** Return the infinite line that this segment is part of. */
  Line2D GetLine() const;

  /** distance to infinite line. */
  double GetDistanceToLine(const VW::Point2D& testpoint) const;
   /** distance to line segment
      including endpoints */
  double GetDistanceToLineSeg(const VW::Point2D& testpoint) const;

  /** Get the cosine of the angle between two line-segments. */
  double GetCosAngle(const VW::LineSeg2D& lineseg) const;

  /** Find the point of intersection of this lineseg with another. If there is no
   * intersection, or it's beyond the end of the line(s) it returns false.
   */
  bool GetIntersection(const VW::LineSeg2D& lineseg, 
		       VW::Point2D& result) const;
  // Streaming methods
  virtual 
  bool WriteASCII(std::ostream& stream) const;
  virtual 
  bool WriteXML(std::ostream& stream,
			 const std::string& name="") const;
  virtual 
  bool WriteBinary(std::ostream& stream) const;


  virtual 
  bool ReadASCII(std::istream& stream);
  virtual 
  bool ReadXML(std::istream& stream, 
	       const std::string& name="");
  virtual 
  bool ReadBinary(std::istream& stream);


  // Data
  Point2D sp, ep;

};

}; // end namespace VW
























// Inline functions given here, not in the .cc file

///////////////////////////////////////////////////////////

inline void 
VW::LineSeg2D::GetEndPoints(double &x0, double &y0,
			    double &x1, double &y1) const 
{ 
  sp.Get(x0,y0); 
  ep.Get(x1,y1); 
}

inline void 
VW::LineSeg2D::GetEndPoints(VW::Point2D &p0, VW::Point2D &p1) const 
{ 
  p0=sp; 
  p1=ep; 
}

inline VW::Point2D 
VW::LineSeg2D::GetStartPoint(void) const 
{ 
  return sp; 
}

inline double 
VW::LineSeg2D::GetStartX(void) const
{
  return sp.GetX();
}

inline double 
VW::LineSeg2D::GetStartY(void) const
{
  return sp.GetY();
}

inline VW::Point2D 
VW::LineSeg2D::GetEndPoint(void) const 
{ 
  return ep; 
}

inline double 
VW::LineSeg2D::GetEndX(void) const
{
  return ep.GetX();
}

inline double 
VW::LineSeg2D::GetEndY(void) const
{
  return ep.GetY();
}

inline VW::Point2D 
VW::LineSeg2D::GetMidPoint(void) const 
{ 
  return 0.5*(sp+ep); 
}

inline void 
VW::LineSeg2D::SetEndPoints(const double x0, const double y0,
			    const double x1, const double y1) 
{ 
  sp.Set(x0,y0); 
  ep.Set(x1,y1); 
}

inline void 
VW::LineSeg2D::SetEndPoints(const VW::Point2D p0, const VW::Point2D p1) 
{ 
  sp=p0; 
  ep=p1; 
}

inline void 
VW::LineSeg2D::SetStartPoint(const VW::Point2D p) 
{ 
  sp=p;
}

inline void 
VW::LineSeg2D::SetEndPoint(const VW::Point2D p) 
{ 
  ep=p;
}

inline VW::Vector2D 
VW::LineSeg2D::GetDirection() const
{
  return (ep-sp)/GetLength();
}

inline double VW::LineSeg2D::GetLength() const
{
  using namespace std; // for sqrt
  return sqrt((ep._x - sp._x)*(ep._x - sp._x) 
		   + (ep._y - sp._y)*(ep._y - sp._y));
}

inline double VW::LineSeg2D::GetLength2() const
{
  return (ep._x - sp._x)*(ep._x - sp._x) 
		   + (ep._y - sp._y)*(ep._y - sp._y);
}

inline double 
VW::LineSeg2D::DistanceToLeft(const VW::Point2D& testpoint) const
{
  VW::Point2D delta = ep - sp; 
  delta.Normalise();
  VW::Point2D vect = testpoint - sp;

  // The Z component of the cross product is negative for left-handed set
  return delta.GetY()*vect.GetX() - delta.GetX()*vect.GetY();
}

inline bool 
VW::LineSeg2D::IsToLeft(const VW::Point2D& testpoint) const
{
  return(DistanceToLeft(testpoint)>0);
}

inline double 
VW::LineSeg2D::DistanceToRight(const VW::Point2D& testpoint) const
{
  VW::Point2D delta = ep - sp; 
  delta.Normalise();
  VW::Point2D vect = testpoint - sp;

  // The Z component of the cross product is negative for left-handed set
  return delta.GetX()*vect.GetY() - delta.GetY()*vect.GetX();
}

inline bool 
VW::LineSeg2D::IsToRight(const VW::Point2D& testpoint) const
{
  return(DistanceToRight(testpoint)>0);

}

inline VW::Line2D 
VW::LineSeg2D::GetLine() const
{
  VW::Line2D result;
  result.Set(sp.GetY() - ep.GetY(),
	     ep.GetX() - sp.GetX(),
	     sp.GetX()*ep.GetY() - sp.GetY()*ep.GetX());
  const double d = sqrt(result[0]*result[0] + result[1]*result[1]);
  return(result/d);
}


inline double 
VW::LineSeg2D::Norm2() const
{
  return this->GetLength2();
}


inline double 
VW::LineSeg2D::Norm() const
{
  return this->GetLength();
}

inline double 
VW::LineSeg2D::GetCosAngle(const VW::LineSeg2D& lineseg) const
{
  VW::Vector2D d1 = ep-sp; 
  d1.Normalise();
  VW::Vector2D d2 = lineseg.ep-lineseg.sp; 
  d2.Normalise();
  return VW::ScalarProduct(d1,d2);
}

#endif /* _lineseg2d_h */
