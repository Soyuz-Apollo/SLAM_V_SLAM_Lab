/** 
 * @file   point2d.h
 * @author ID Reid and BJ Tordoff
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
 * @class    VW::Point2D 
 * @ingroup  GeometricObjects
 * @author   ID Reid and BJ Tordoff
 * @date     02/08/2002
 * @brief    Representation of a 2D position or vector.
 *
 * Point2D represents a position in the 2D plane, and provides the
 * expected operations of addition, subtraction, multiplication by
 * scalars etc.
 * In addition it inherits from the IOObject and provides the standard
 * streaming capabilities.
 */


#ifndef _point2d_h_
#define _point2d_h_

//#include <VNL/vectorfixed.h>
#include <math.h> // for sqrt function
#include <iostream>

// A dependency. Aaargh!
#include "VW/iobasics.h"

#include <VNL/vectorfixed.h>

namespace VW {


  class Point2D : public VW::IOObject
  {
  public:

    
    Point2D (void);
    Point2D (const double x_coord, const double y_coord);
    Point2D (const VNL::VectorFixed<2>& v);
    //Point2D (const VNL::VectorFixed<3,double>& homg_point);
    //Point2D (const VNL::VectorFixed<3,float>& homg_point);
    virtual ~Point2D();



    Point2D &operator=(const Point2D &p);
    Point2D &operator+=(const Point2D &p);
    Point2D &operator-=(const Point2D &p);
    Point2D &operator*=(double scale);
    Point2D &operator/=(double scale);

    bool operator==(const Point2D &p) const;
    bool operator!=(const Point2D &p) const;
    friend Point2D operator+(const Point2D &p1, const Point2D &p2);
    friend Point2D operator-(const Point2D &p);
    friend Point2D operator-(const Point2D &p1, const Point2D &p2);
    friend double  operator*(const Point2D &p1, const Point2D &p2);
    friend Point2D operator*(const Point2D &p1, const double a);
    friend Point2D operator*(const double a, const Point2D &p1);
    friend Point2D operator/(const Point2D &p1, const double a);
    
    void Get(double &x_coord, double &y_coord) const;
    double GetX(void) const;
    double GetY(void) const;
    
    /** treat as an array (no bound checking!) */
    double operator[](unsigned int index) const;

    void Set(const double x_coord, const double y_coord);
    void SetX(const double x_coord);
    void SetY(const double y_coord);
    
    double Norm() const;
    double NormSquared() const;
    void Normalise();

    double GetDistance(const Point2D &testpoint) const {
      return sqrt((testpoint._x - _x)*(testpoint._x - _x) + 
		  (testpoint._y - _y)*(testpoint._y - _y));
    }		  

    //VNL::VectorFixed<3,double> Homg();
    
    friend double ScalarProduct(const Point2D &p1, 
				const Point2D &p2);

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
  double _x;
  double _y;


};


typedef Point2D Vector2D;

}; // end namespace VW





// Inline functions given here, not in the .cc file

///////////////////////////////////////////////////////////

/**
Constructor from a VNL vector.
**/
inline VW::Point2D::Point2D(const VNL::VectorFixed<2,double>& v)
  : _x(v[0]), _y(v[1])
{
}

/** Get the X position.
 * @return the x position as a double.
 */
inline 
double VW::Point2D::GetX(void) const 
{
  return(_x);
}

/** Get the Y position.
 * @return the y position as a double.
 */
inline 
double VW::Point2D::GetY(void) const 
{
  return(_y);
}

/** Set both the X and Y positions.
 * @param x_coord the new X position.
 * @param y_coord the new Y position.
 */
inline 
void VW::Point2D::Set(const double x_coord, const double y_coord) 
{
  _x=x_coord;
  _y=y_coord;
}

/** Set the X position.
 * @param x_coord the new X position.
 */
inline 
void VW::Point2D::SetX(const double x_coord) 
{
  _x=x_coord;
}

/** Set the Y position.
 * @param y_coord the new Y position.
 */
inline 
void VW::Point2D::SetY(const double y_coord) 
{
  _y=y_coord;
}

/** The 2-norm - root sum squared of the elements.
 * @return the 2-norm as a double.
 */
inline 
double VW::Point2D::Norm(void) const 
{
  return sqrt(_x*_x + _y*_y);
}

/** The 2-norm squared - sum squared of the elements.
 * @return the square of the 2-norm as a double.
 */
inline 
double VW::Point2D::NormSquared(void) const 
{
  return (_x*_x + _y*_y);
}

inline void
VW::Point2D::Normalise()
{
  double n=1/Norm();
  _x*=n;
  _y*=n;
}

/** Add a point to this point - the result is another point.
 * @return the point formed by vector addition
 */
inline 
VW::Point2D & VW::Point2D::operator+=(const VW::Point2D &p) 
{
  _x += p._x;
  _y += p._y;
  return(*this);
}

/** Subtract a point from this point - the result is another point.
 * @return the point formed by vector subtraction
 */
inline 
VW::Point2D & VW::Point2D::operator-=(const VW::Point2D &p) 
{
  _x -= p._x;
  _y -= p._y;
  return(*this);
}

inline VW::Point2D&
VW::Point2D::operator*=(double scale)
{
  _x *= scale;
  _y *= scale;
  return(*this);
}

inline VW::Point2D&
VW::Point2D::operator/=(double scale)
{
  _x /= scale;
  _y /= scale;
  return(*this);
}

inline
bool  VW::Point2D::WriteASCII(std::ostream& stream) const
{
  stream << _x << "\t" << _y << "\t";
  return(true);
}

inline
bool  VW::Point2D::WriteXML(std::ostream& stream, 
				 const std::string& name) const
{
  
  // Start tag
  if (name.size()>0)
    stream << "<Point2D NAME=\"" << name << "\"> ";
  else
    stream << "<Point2D> ";
  
  // Contents
  stream << "<X>" << _x << "</X> ";
  stream << "<Y>" << _y << "</Y> ";
  
    // End tag
  stream << "</Point2D>";
  
  return(true);
}

inline
bool  VW::Point2D::WriteBinary(std::ostream& stream) const
{
  VW::WriteBinary(stream,_x);
  VW::WriteBinary(stream,_y);
  return(true);
}

inline
bool  VW::Point2D::ReadASCII(std::istream& stream)
{
  stream >> _x >> _y;
  return(true);
}

inline
bool VW::Point2D::ReadXML(std::istream& stream, 
			  const std::string&)
{
  std::string tagname;
  std::istringstream contents(" ");
  if (VW::FindXMLBlock("Point2D",stream,tagname)) {
    if (VW::GetXMLBlock("X",stream,contents,tagname))
      contents >> _x;
    else
      std::cerr << "VW::Point2D::ReadXML - error reading x-component.\n";
    if (VW::GetXMLBlock("Y",stream,contents,tagname))
      contents >> _y;
    else
      std::cerr << "VW::Point2D::ReadXML - error reading y-component.\n";
  } else {
    return(false);
  }
  return(true);
}

inline
bool VW::Point2D::ReadBinary(std::istream& stream)
{
  VW::ReadBinary(stream,_x);
  VW::ReadBinary(stream,_y);
  return(true);
}

#if 0
inline
VNL::VectorFixed<3,double> VW::Point2D::Homg () 
{
  return VNL::VectorFixed<3,double> (_x, _y, 1.0);
}
#endif

namespace VW {
  inline double 
  ScalarProduct(const VW::Point2D &p1, const VW::Point2D &p2)
  {
    return(p1._x * p2._x
	   + p1._y * p2._y);
  }
}

inline double
VW::Point2D::operator[](unsigned int index) const
{
  // This makes use of the C++ gaurantee of memory alignment
  return *(&_x + index);
}

#endif /* _point2d_h */












