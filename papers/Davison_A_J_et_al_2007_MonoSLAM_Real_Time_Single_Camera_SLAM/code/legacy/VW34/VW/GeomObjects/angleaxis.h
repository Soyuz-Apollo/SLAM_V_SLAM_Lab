/** 
 * @file     angleaxis.h
 * @author   BJ Tordoff
 * @date     10/12/2004
 *
 *              (c) Copyright University of Cambridge
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
 * @class    VW::AngleAxis
 * @ingroup  GeometricObjects
 * @author   BJ Tordoff
 * @date     02/11/2002
 * @brief    Representation of a 3D rotation as an angle-axis or axial vector.
 *
 * Although useful for somethings, angleaxis vectors are the poor mans
 * alternative to using genuine quaternions mainly because the mathematics
 * of products is much less simple (and more computationally costly).
 */



#ifndef _vw_angleaxis_h_
#define _vw_angleaxis_h_

// System
#include <iostream>
#include <cmath>

// VW includes
#include <VW/IOBasics/ioobject.h>
#include <VW/IOBasics/miscioutils.h>
#include <VW/IOBasics/xmlstuff.h>

// Local includes


//////////////////////////
// Forward Declarations //
//////////////////////////
namespace VW {
  class RotationMatrix;
  class Quaternion;
  class Point3D; typedef Point3D Vector3D;
  class LineSeg3D;
}


/////////////////////////


namespace VW {

  class AngleAxis : public IOObject {
  public:
    /** Construct elements individually. */
    AngleAxis();

    /** Construct elements individually. */
    AngleAxis(double x,double y,double z);

    /** Construct from an angle around an axis. */
    AngleAxis(const VW::Vector3D& axis,
	       const double& angle);

    /** Construct using an angle-axis vector. */
    AngleAxis(const VW::Vector3D& angleaxis);

    /** Construct from a rotation matrix. */
    AngleAxis(const VW::RotationMatrix& R);

    /** Construct from a rotation matrix. */
    AngleAxis(const VW::Quaternion& R);

    virtual ~AngleAxis() {}

    /** Set elements individually */
    void Set(double x,double y,double z);

    /** Set using a rotation axis and angle */
    void Set(const VW::Vector3D& axis,
	     const double& angle);

    /** Set using an angle-axis vector. */
    void Set(const VW::Vector3D& angleaxis);

    /** Set from a rotation matrix. */
    void Set(const VW::RotationMatrix& R);

    /** Set from a quaternion. */
    void Set(const VW::Quaternion& q);



 
    // Accessors (for L-value access see below)
    double GetX() const;
    double GetY() const;
    double GetZ() const;
    // L-value Accessors
    double& X();
    double& Y();
    double& Z();
    const double& X() const;
    const double& Y() const;
    const double& Z() const;

    void Get(double &x, double &y, double &z) const;

    VW::Vector3D        Axis() const;
    double              Angle() const;
    VW::Quaternion      Quaternion() const;
    VW::RotationMatrix  RotationMatrix() const;
    VW::RotationMatrix  RotationMatrixTranspose() const;
    VW::AngleAxis       Inverse() const;


    // Rotate a vector
    VW::Vector3D  Rotate(const VW::Vector3D& v) const;
 
    // Rotate a line
    VW::LineSeg3D Rotate(const VW::LineSeg3D& l) const;
 
    // Combine axials
    VW::AngleAxis operator*(const VW::AngleAxis& aa) const;

    // Scale the angle
    VW::AngleAxis operator*(const double scale) const;
    VW::AngleAxis operator/(const double scale) const;
    VW::AngleAxis& operator*=(const double scale);
    VW::AngleAxis& operator/=(const double scale);


    // Tests
    /** Is q1 identitical to this quaternion. */
    bool operator==(const VW::AngleAxis& aa) const;
    /** Is q1 different from this quaternion. */
    bool operator!=(const VW::AngleAxis& aa) const;



    // Streaming methods

    /** Write space separated data (order is x,y,z,r) */
    virtual 
    bool WriteASCII(std::ostream& stream) const;
    virtual 
    bool WriteXML(std::ostream& stream,
			   const std::string& name="") const;
    /** Write binary data (order is x,y,z,r) */
    virtual 
    bool WriteBinary(std::ostream& stream) const;

    /** Read from an ASCII input stream, from Roll, Pitch, Yaw (ZXY) Euler angles in 
     * degrees.
     */
    virtual 
    bool ReadASCII(std::istream& stream); 

    virtual 
    bool ReadXML(std::istream& stream, 
		 const std::string& name="");
    /** Read binary data (order is x,y,z,r) */
    virtual 
    bool ReadBinary(std::istream& stream);


  protected:
    double _x,_y,_z;
  };

}; // end namespace VW















// INLINERS
inline
VW::AngleAxis::AngleAxis()
 : _x(0), _y(0), _z(0)
{
}

// INLINERS
inline
VW::AngleAxis::AngleAxis(double x,double y,double z)
 : _x(x), _y(y), _z(z)
{
}

inline
VW::AngleAxis::AngleAxis(const VW::Vector3D& axis,
			   const double& angle)
{
  Set(axis,angle);
}

inline
VW::AngleAxis::AngleAxis(const VW::Vector3D& angleaxis)
{
  Set(angleaxis);
}

inline
VW::AngleAxis::AngleAxis(const VW::RotationMatrix& R)
{
  Set(R);
}

inline
VW::AngleAxis::AngleAxis(const VW::Quaternion& q)
{
  Set(q);
}


inline
void VW::AngleAxis::Set(double x,double y,double z)
{
  _x=x;
  _y=y;
  _z=z;
}

// Accessors
inline
double VW::AngleAxis::GetX() const
{
  return(_x);
}

inline
double VW::AngleAxis::GetY() const
{
  return(_y);
}

inline
double VW::AngleAxis::GetZ() const
{
  return(_z);
}


inline
void VW::AngleAxis::Get(double &x, double &y, double &z) const
{
  x = _x;
  y = _y;
  z = _z;
}

/** @returns a reference to the first imaginary part of the quaternion */
inline    double& 
VW::AngleAxis::X()
{
  return _x;
}

/** @returns a reference to the second imaginary part of the quaternion */
inline    double& 
VW::AngleAxis::Y()
{
  return _y;
}

/** @returns a reference to the third imaginary part of the quaternion */
inline    double& 
VW::AngleAxis::Z()
{
  return _z;
}

/** @returns a constant reference to the first imaginary part of the quaternion */
inline    const double& 
VW::AngleAxis::X() const
{
  return _x;
}

/** @returns a constant reference to the second imaginary part of the quaternion */
inline    const double& 
VW::AngleAxis::Y() const
{
  return _y;
}

/** @returns a constant reference to the third imaginary part of the quaternion */
inline    const double& 
VW::AngleAxis::Z() const
{
  return _z;
}


inline VW::AngleAxis 
VW::AngleAxis::operator*(const double scale) const
{
  VW::AngleAxis aa = *this;
  aa*=scale;
  return aa;
}

inline VW::AngleAxis 
VW::AngleAxis::operator/(const double scale) const
{
  VW::AngleAxis aa = *this;
  aa/=scale;
  return aa;
}

inline VW::AngleAxis& 
VW::AngleAxis::operator*=(const double scale)
{
  _x*=scale;
  _y*=scale;
  _z*=scale;
  return *this;
}

inline VW::AngleAxis& 
VW::AngleAxis::operator/=(const double scale)
{
  const double s2 = 1.0/scale;
  _x*=s2;
  _y*=s2;
  _z*=s2;
  return *this;
}

inline
VW::AngleAxis VW::AngleAxis::Inverse() const
{
  return( VW::AngleAxis(-_x, -_y, -_z) );
}




inline bool 
VW::AngleAxis::operator==(const VW::AngleAxis& aa) const
{
  return ((aa._x == _x) && (aa._y == _y) && (aa._z == _z));
}

inline bool 
VW::AngleAxis::operator!=(const VW::AngleAxis& aa) const
{
  return ((aa._x != _x) || (aa._y != _y) || (aa._z != _z));
}




// STREAMING STUFF //

inline
bool  VW::AngleAxis::WriteASCII(std::ostream& stream) const
{
  stream << _x << "\t" << _y << "\t" << _z;
  return(true);
}

inline
bool  VW::AngleAxis::WriteXML(std::ostream& stream, 
				     const std::string& name) const
{
  
  // Start tag
  if (name.size()>0)
    stream << "<AngleAxis NAME=\"" << name << "\"> ";
  else
    stream << "<AngleAxis> ";
  
  // Contents
  stream << "<X>" << _x << "</X> ";
  stream << "<Y>" << _y << "</Y> ";
  stream << "<Z>" << _z << "</Z> ";
  
  // End tag
  stream << "</AngleAxis>";
  
  return(true);
}

inline
bool VW::AngleAxis::WriteBinary(std::ostream& stream) const
{
  VW::WriteBinary(stream,_x);
  VW::WriteBinary(stream,_y);
  VW::WriteBinary(stream,_z);
  return(true);
}


inline
bool VW::AngleAxis::ReadASCII(std::istream& stream)
{
  stream >> _x >> _y >> _z; 
  return(true);
}

inline
bool VW::AngleAxis::ReadXML(std::istream& stream, 
				    const std::string& )
{
  std::string tagname;
  std::istringstream contents;
  if (VW::FindXMLBlock("AngleAxis",stream,tagname)) {
    if (VW::GetXMLBlock("X",stream,contents,tagname))
      contents >> _x;
    else {
      std::cerr << "VW::AngleAxis::ReadXML - error reading x-component.\n";
      return(false);
    }
    if (VW::GetXMLBlock("Y",stream,contents,tagname))
      contents >> _y;
    else {
      std::cerr << "VW::AngleAxis::ReadXML - error reading y-component.\n";
      return(false);
    }
    if (VW::GetXMLBlock("Z",stream,contents,tagname))
      contents >> _z;
    else {
      std::cerr << "VW::AngleAxis::ReadXML - error reading z-component.\n";
      return(false);
    }
  }else {
    return(false);
  }
  return(true);
}

inline
bool VW::AngleAxis::ReadBinary(std::istream& stream)
{
  VW::ReadBinary(stream,_x);
  VW::ReadBinary(stream,_y);
  VW::ReadBinary(stream,_z);
  return(true);
}



#endif
