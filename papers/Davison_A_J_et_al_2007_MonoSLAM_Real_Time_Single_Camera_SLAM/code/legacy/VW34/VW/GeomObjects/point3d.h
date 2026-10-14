/** 
 * @file     point3d.h
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
/**
 * @class    VW::Point3D 
 * @ingroup  GeometricObjects
 * @author   BJ Tordoff
 * @date     02/08/2002
 * @brief    Representation of a 3D position or vector.
 *
 * Point2D represents a position in 3D Euclidean space, and provides the
 * expected operations of addition, subtraction, multiplication by
 * scalars etc.
 * In addition it inherits from the IOObject and provides the standard
 * streaming capabilities.
 */


#ifndef _point3d_h_
#define _point3d_h_


// System includes
#include <iostream>
#include <math.h> // for sqrt function

// VW includes
#include <VNL/matrixfixed.h>
#include <VNL/vectorfixed.h>
//#include <VW/math.h>
#include <VW/iobasics.h>

// Local includes



namespace VW {

  class Point3D : public VW::IOObject
  {
  public:
    Point3D (void);
    Point3D (const double x_coord, const double y_coord, const double z_coord);
    Point3D (const VNL::VectorFixed<3,double>& v);
    virtual ~Point3D();
    Point3D &operator=(const Point3D &p);
    Point3D &operator+=(const Point3D &p);
    Point3D &operator-=(const Point3D &p);
    Point3D &operator*=(double scale);
    Point3D &operator/=(double scale);

    bool operator==(const Point3D &p) const;
    bool operator!=(const Point3D &p) const;
    friend Point3D operator+(const Point3D &p1, const Point3D &p2);
    friend Point3D operator-(const Point3D &p);
    friend Point3D operator-(const Point3D &p1, const Point3D &p2);
    friend double  operator*(const Point3D &p1, const Point3D &p2);
    friend Point3D operator*(const Point3D &p1, const double a);
    friend Point3D operator*(const double a, const Point3D &p1);
    friend Point3D operator*(const VNL::MatrixFixed<3,3,double>& M, 
			     const Point3D &p1);
    friend Point3D operator/(const Point3D &p1, const double a);
    friend double ScalarProduct(const Point3D &p1, 
				const Point3D &p2);
    friend Point3D VectorProduct(const Point3D &p1, 
				 const Point3D &p2);
    /** p1 . (p2 x p3) */
    friend double  ScalarTripleProduct(const Point3D &p1, 
				       const Point3D &p2, 
				       const Point3D &p3);
  
    void Get(double &x_coord, double &y_coord, double &z_coord) const;
    double GetX(void) const;
    double GetY(void) const;
    double GetZ(void) const;
    // Get into VNL 3-vector
    VNL::VectorFixed<3,double> GetVNL3() const;

    /** treat as an array (no bound checking!) */
    double operator[](unsigned int index) const;
  
    void Set(const double x_coord, const double y_coord, const double z_coord);
    void SetX(const double x_coord);
    void SetY(const double y_coord);
    void SetZ(const double z_coord);
    // Set from VNL 3-vector
    void SetVNL3(const VNL::VectorFixed<3,double> &vnl3);


    double Norm() const;
    double NormSquared() const;
    void Normalise();

    double GetDistance(const Point3D &testpoint) const {
      return sqrt((testpoint._x - _x)*(testpoint._x - _x) + 
		  (testpoint._y - _y)*(testpoint._y - _y) + 
		  (testpoint._z - _z)*(testpoint._z - _z));
    }		  

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
    double _z;


  };

  typedef Point3D Vector3D;

}; // end namespace VW











// Inline functions given here, not in the .cc file

///////////////////////////////////////////////////////////
inline 
    
VW::Point3D::Point3D (const VNL::VectorFixed<3,double>& v)
  : _x(v[0]), _y(v[1]), _z(v[2])
{
}

inline double 
VW::Point3D::GetX(void) const 
{
  return(_x);
}

inline double 
VW::Point3D::GetY(void) const 
{
  return(_y);
}

inline double 
VW::Point3D::GetZ(void) const 
{
  return(_z);
}

inline VNL::VectorFixed<3,double> 
VW::Point3D::GetVNL3() const
{
  VNL::VectorFixed<3,double> vnl3;
  vnl3[0] = _x;
  vnl3[1] = _y;
  vnl3[2] = _z;

  return vnl3;
}


inline void 
VW::Point3D::Set(const double x_coord, 
		 const double y_coord, 
		 const double z_coord) 
{
  _x=x_coord;
  _y=y_coord;
  _z=z_coord;
}

inline void 
VW::Point3D::SetX(const double x_coord) 
{
  _x=x_coord;
}

inline void 
VW::Point3D::SetY(const double y_coord) 
{
  _y=y_coord;
}

inline void 
VW::Point3D::SetZ(const double z_coord) 
{
  _z=z_coord;
}

inline void 
VW::Point3D::SetVNL3(const VNL::VectorFixed<3,double> &vnl3)
{
  _x = vnl3[0];
  _y = vnl3[1];
  _z = vnl3[2];
}

inline double 
VW::Point3D::Norm(void) const 
{
  return sqrt(_x*_x + _y*_y + _z*_z);
}

inline double 
VW::Point3D::NormSquared(void) const 
{
  return (_x*_x + _y*_y + _z*_z);
}

inline void
VW::Point3D::Normalise()
{
  double n=1/Norm();
  _x*=n;
  _y*=n;
  _z*=n;
}

inline VW::Point3D& 
VW::Point3D::operator+=(const VW::Point3D &p) 
{
  _x+=p._x;
  _y+=p._y;
  _z+=p._z;
  return(*this);
}

inline VW::Point3D& 
VW::Point3D::operator-=(const VW::Point3D &p) 
{
  _x-=p._x;
  _y-=p._y;
  _z-=p._z;
  return(*this);
}

inline VW::Point3D&
VW::Point3D::operator*=(double scale)
{
  _x *= scale;
  _y *= scale;
  _z *= scale;
  return(*this);
}

inline VW::Point3D&
VW::Point3D::operator/=(double scale)
{
  _x /= scale;
  _y /= scale;
  _z /= scale;
  return(*this);
}



inline
bool VW::Point3D::WriteASCII(std::ostream& stream) const
{
  stream << _x << "\t" << _y << "\t" << _z << "\t";
  return(true);
}

inline
bool VW::Point3D::WriteXML(std::ostream& stream, 
			   const std::string& name) const
{
  
  // Start tag
  if (name.size()>0)
    stream << "<Point3D NAME=\"" << name << "\"> ";
  else
    stream << "<Point3D> ";
  
  // Contents
  stream << "<X>" << _x << "</X> ";
  stream << "<Y>" << _y << "</Y> ";
  stream << "<Z>" << _z << "</Z> ";
  
  // End tag
  stream << "</Point3D>";
  
  return(true);
}

inline
bool  VW::Point3D::WriteBinary(std::ostream& stream) const
{
  VW::WriteBinary(stream,_x);
  VW::WriteBinary(stream,_y);
  VW::WriteBinary(stream,_z);
  return(true);
}

inline
bool VW::Point3D::ReadASCII(std::istream& stream)
{
  stream >> _x >> _y >> _z;
  return(true);
}

inline
bool VW::Point3D::ReadXML(std::istream& stream, 
			  const std::string& name)
{
  std::string tagname;
  std::istringstream contents(" ");
  if (!VW::FindXMLBlock("Point3D",stream,tagname)) {
    std::cerr << "VW::Point3D::ReadXML - Could not find opening tag.\n";
    return(false);
  } else {
    // Check name is correct
    if (name.size()>0) {
      if (tagname!=name) {
	std::cerr << "VW::Point3D::ReadXML - Name \""<<tagname<<"\" is not as expected (\""<<name<<"\").\n";
	return(false);
      }
    }

    if (!VW::GetXMLBlockAsNumber("X",stream,_x,tagname)) {
      std::cerr << "VW::Point3D::ReadXML - error reading x-component.\n";
      return(false);
    }
    if (!VW::GetXMLBlockAsNumber("Y",stream,_y,tagname)) {
      std::cerr << "VW::Point3D::ReadXML - error reading y-component.\n";
      return(false);
    }
    if (!VW::GetXMLBlockAsNumber("Z",stream,_z,tagname)) {
      std::cerr << "VW::Point3D::ReadXML - error reading z-component.\n";
      return(false);
    }
  }
  return(true);
}

inline
bool VW::Point3D::ReadBinary(std::istream& stream)
{
  VW::ReadBinary(stream,_x);
  VW::ReadBinary(stream,_y);
  VW::ReadBinary(stream,_z);
  return(true);
}

//friends should be in namespace...
namespace VW
{
	inline VW::Point3D 
	VectorProduct(const VW::Point3D &p1, const VW::Point3D &p2)
	{
	  return(VW::Point3D(p1._y*p2._z - p1._z*p2._y,
				 p1._z*p2._x - p1._x*p2._z,
				 p1._x*p2._y - p1._y*p2._x));
	}

	inline double 
	ScalarProduct(const VW::Point3D &p1, const VW::Point3D &p2)
	{
	  return(p1._x * p2._x
		 + p1._y * p2._y
		 + p1._z * p2._z);
	}

	inline double ScalarTripleProduct(const VW::Point3D &p1, 
					  const VW::Point3D &p2, 
					  const VW::Point3D &p3)
	{
	  //         | :  :  :  |
	  // s = det | p1 p2 p3 |
	  //         | :  :  :  |

	  double result=0;
	  result += p1._x * (p2._y*p3._z - p3._y*p2._z);
	  result -= p2._x * (p1._y*p3._z - p3._y*p1._z);
	  result += p3._x * (p1._y*p2._z - p2._y*p1._z);
	  return(result);
	}

}
inline double
VW::Point3D::operator[](unsigned int index) const
{
  // This makes use of the C++ gaurantee of memory alignment
  return *(&_x + index);
}



#endif /* _point3d_h */












