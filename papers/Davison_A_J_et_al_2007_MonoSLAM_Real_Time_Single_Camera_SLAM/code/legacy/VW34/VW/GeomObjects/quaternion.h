/** 
 * @file     quaternion.h
 * @author   BJ Tordoff
 * @date     02/11/2002
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
 * @class    VW::Quaternion
 * @ingroup  GeometricObjects
 * @author   BJ Tordoff
 * @date     02/11/2002
 * @brief    Representation of a 3D quaternion.
 *
 * This quaternion class represents a 3D rotation as a complex vector
 * with three imaginary parts x,y,z and real part r: q = ix + jy + kz + r.
 * Whilst designed for simlarity to the (partially incorrect) VNL quaternion
 * class, it provides additional functions for rotating VW::Vector3D's and for
 * initialisation from and recovery of Euler angles.
 *
 * Note that unlike the VNL equivalent, it is guaranteed to be a unit vector.
 *
 * As an IOObject descendant it can be streamed as binary, ASCII or XML-style. 
 */



#ifndef _vw_quaternion_h_
#define _vw_quaternion_h_

// System
#include <iostream>
#include <cmath>

// VW includes
#include <VNL/vectorfixed.h>
#include <VW/IOBasics/ioobject.h>
#include <VW/IOBasics/miscioutils.h>
#include <VW/IOBasics/xmlstuff.h>

// Local includes


//////////////////////////
// Forward Declarations //
//////////////////////////
// namespace VNL {
//   template <unsigned int r, unsigned int c, class T> class MatrixFixed;
// }
namespace VW {
  class RotationMatrix;
  class AngleAxis;
  class Point3D; typedef Point3D Vector3D;
  class LineSeg3D;
}


/////////////////////////


namespace VW {

  class Quaternion : public IOObject {
  public:
    /** Construct elements individually. */
    Quaternion(double x=0,double y=0,double z=0,double r=1);

    /** Construct from an angle around an axis.
    @param axis  the axis (doesn't need to be a unit vector) around which to
    rotate
    @param angle the rotation angle in radians */
    Quaternion(const VW::Vector3D& axis,
	       const double& angle);

    /** Construct using an angle-axis vector. */
    Quaternion(const VW::AngleAxis& angleaxis);

    /** Construct from a rotation matrix. */
    Quaternion(const VW::RotationMatrix& R);

    virtual ~Quaternion() {}

    /** Set elements individually */
    void Set(double x,double y,double z,double r);

    /** Set using a rotation axis and angle */
    void Set(const VW::Vector3D& axis,
	     const double& angle);

    /** Set using an angle-axis vector. */
    void Set(const VW::AngleAxis& angleaxis);

    /** Set from a rotation matrix. */
    void Set(const VW::RotationMatrix& R);



    // Euler angles  ** Take care with the ordering **

    /** Create a quaternion from X,Y,Z rotations. Note that the rotations
     * are applied in order to an object, 
     * so q_xyz * X = R_z * R_y * R_x * X.
     */
    void SetFromXYZEuler(const double theta_x,
			 const double theta_y,
			 const double theta_z);


    /** Create a quaternion from roll, pitch, yaw. Note that the rotations
     * are applied in order to an object, 
     * so q_zxy * X = R_y * R_x * R_z * X.
     */
    void SetFromZXYEuler(const double theta_z,
			 const double theta_x,
			 const double theta_y);

    /** Create a quaternion from yaw, pitch, roll. Note that the rotations
     * are applied in order to an object, 
     * so q_zxy * X = R_z * R_x * R_y * X.
     */
    void SetFromYXZEuler(const double theta_y,
			 const double theta_x,
			 const double theta_z);


    /** Create a quaternion from Z,Y,X rotations. Note that the rotations
     * are applied in order to an object, 
     * so q_zyx * X = R_x * R_y * R_z * X.
     */
    void SetFromZYXEuler(const double theta_z,
			 const double theta_y,
			 const double theta_x);

    /** Create a quaternion from Z,X,Z rotations. Note that the rotations
     * are applied in order to an object, 
     * so q_zxz * X = R_z2 * R_x * R_z1 * X.
     * The ZXZ form is common in physics-based problems.
     */
    void SetFromZXZEuler(const double theta_z1,
			 const double theta_x,
			 const double theta_z2);
 
    /** Set a quaternion from the values of a VNL 4-vector. The vector should
    have the R part <em>first</em>, as in many quaternion implementations.*/
    void SetRXYZ(const VNL::VectorFixed<4,double>& vRXYZ) ;


    // Accessors (for L-value access see below)
    double GetX() const;
    double GetY() const;
    double GetZ() const;
    double GetR() const;
    // L-value Accessors
    double& X();
    double& Y();
    double& Z();
    double& R();
    const double& X() const;
    const double& Y() const;
    const double& Z() const;
    const double& R() const;

    void Get(double &x, double &y, double &z, double &r) const;

    VW::Vector3D                 Axis() const;
    double                       Angle() const;
    VW::AngleAxis                AngleAxis() const;
    VW::RotationMatrix           RotationMatrix() const;
    VW::RotationMatrix           RotationMatrixTranspose() const;
    VW::Quaternion               Conjugate() const;
    VW::Quaternion               Inverse() const;

    // Get Euler angles
    void GetXYZEuler(double& theta_x,
		     double& theta_y,
		     double& theta_z) const;
    void GetZXYEuler(double& theta_z,
		     double& theta_x,
		     double& theta_y) const;
    void GetYXZEuler(double& theta_y,
		     double& theta_x,
		     double& theta_z) const;
    void GetZYXEuler(double& theta_z,
		     double& theta_y,
		     double& theta_x) const;
    void GetZXZEuler(double& theta_z1,
		     double& theta_x,
		     double& theta_z2) const;

    /** Get the quaternion as a VNL 4-vector. The R part is
    <em>first</em>, as in many quaternion implementations.*/
    VNL::VectorFixed<4,double> GetRXYZ() const;

    // Rotate a vector
    VW::Vector3D                 Rotate(const VW::Vector3D& v) const;
    VW::Vector3D                 operator*(const VW::Vector3D& v) const;

    // Rotate a line
    VW::LineSeg3D                 Rotate(const VW::LineSeg3D& l) const;
    VW::LineSeg3D                 operator*(const VW::LineSeg3D& l) const;

    // Combine quaternions
    VW::Quaternion               operator*(const VW::Quaternion& q) const;

    // Treat as complex
    VW::Vector3D                 Imaginary() const;
    double                       Real() const;





    // Tests
    /** Is q1 identitical to this quaternion. */
    bool operator==(const VW::Quaternion& q) const;
    /** Is q1 different from this quaternion. */
    bool operator!=(const VW::Quaternion& q) const;

    /** Make sure this is a unit vector.
     *  This is called automatically when constructing from
     *  four doubles.
     */
    void Normalise();



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
    double _x,_y,_z,_r;
  };

}; // end namespace VW















// INLINERS
inline
VW::Quaternion::Quaternion(double x,double y,double z,double r)
 : _x(x), _y(y), _z(z), _r(r)
{
  // Make sure that this is a unit vector
  Normalise();
}

inline
VW::Quaternion::Quaternion(const VW::Vector3D& axis,
			   const double& angle)
{
  Set(axis,angle);
}

inline
VW::Quaternion::Quaternion(const VW::AngleAxis& angleaxis)
{
  Set(angleaxis);
}

inline
VW::Quaternion::Quaternion(const VW::RotationMatrix& R)
{
  Set(R);
}


inline
void VW::Quaternion::Set(double x,double y,double z,double r)
{
  _x=x;
  _y=y;
  _z=z;
  _r=r;

  // Make sure that this is a unit vector
  Normalise();
}

// Accessors
inline
double VW::Quaternion::GetX() const
{
  return(_x);
}

inline
double VW::Quaternion::GetY() const
{
  return(_y);
}

inline
double VW::Quaternion::GetZ() const
{
  return(_z);
}

inline
double VW::Quaternion::GetR() const
{
  return(_r);
}

inline
void VW::Quaternion::Get(double &x, double &y, double &z, double &r) const
{
  x = _x;
  y = _y;
  z = _z;
  r = _r;
}

/** @returns a reference to the first imaginary part of the quaternion */
inline    double& 
VW::Quaternion::X()
{
  return _x;
}

/** @returns a reference to the second imaginary part of the quaternion */
inline    double& 
VW::Quaternion::Y()
{
  return _y;
}

/** @returns a reference to the third imaginary part of the quaternion */
inline    double& 
VW::Quaternion::Z()
{
  return _z;
}

/** @returns a reference to the real part of the quaternion */
inline    double& 
VW::Quaternion::R()
{
  return _r;
}

/** @returns a constant reference to the first imaginary part of the quaternion */
inline    const double& 
VW::Quaternion::X() const
{
  return _x;
}

/** @returns a constant reference to the second imaginary part of the quaternion */
inline    const double& 
VW::Quaternion::Y() const
{
  return _y;
}

/** @returns a constant reference to the third imaginary part of the quaternion */
inline    const double& 
VW::Quaternion::Z() const
{
  return _z;
}

/** @returns a constant reference to the real part of the quaternion */
inline    const double& 
VW::Quaternion::R() const
{
  return _r;
}


/** @returns the real part (cos(theta/2)) of the quaternion */
inline
double VW::Quaternion::Real() const
{
  return(_r);
}


inline
VW::Quaternion VW::Quaternion::Conjugate() const
{
  return Quaternion(-_x, -_y, -_z, _r);
}

inline
VW::Quaternion VW::Quaternion::Inverse() const
{
  // Same as conjugate, unless not a unit vector, in which case
  // length is inverted
  double l = 1/(_x*_x + _y*_y + _z*_z + _r*_r);
  return( VW::Quaternion(-_x*l, -_y*l, -_z*l, _r*l) );
}

inline void
VW::Quaternion::Normalise()
{
  double n=1/sqrt(_x*_x + _y*_y + _z*_z + _r*_r);
  _x*=n;
  _y*=n;
  _z*=n;
  _r*=n;
}



// Euler conversions based on
// http://www.mathworks.com/access/helpdesk/help/toolbox/aeroblks/euleranglestoquaternions.html
// but note that Mathworks defines yaw=X and pitch=Y (we do the other way round), 
// and that they do not mention that the multiplication:
// (cx-isx)(cy-isy)(cz-isz) gives the conjugate of the desired quaternion!

inline
void VW::Quaternion::SetFromXYZEuler(const double theta_x,
				     const double theta_y,
				     const double theta_z)
{
  const double cx(cos(0.5*theta_x));
  const double cy(cos(0.5*theta_y));
  const double cz(cos(0.5*theta_z));
  const double sx(sin(0.5*theta_x));
  const double sy(sin(0.5*theta_y));
  const double sz(sin(0.5*theta_z));

  this->_r = cx*cy*cz + sx*sy*sz;
  this->_x = sx*cy*cz - cx*sy*sz;
  this->_y = cx*sy*cz + sx*cy*sz;
  this->_z = cx*cy*sz - sx*sy*cz;
}


inline
void VW::Quaternion::SetFromZXYEuler(const double theta_z,
				     const double theta_x,
				     const double theta_y)
{
  const double cx(cos(0.5*theta_x));
  const double cy(cos(0.5*theta_y));
  const double cz(cos(0.5*theta_z));
  const double sx(sin(0.5*theta_x));
  const double sy(sin(0.5*theta_y));
  const double sz(sin(0.5*theta_z));

  this->_r =  cx*cy*cz + sx*sy*sz;
  this->_x =  sx*cy*cz + cx*sy*sz;
  this->_y =  cx*sy*cz - sx*cy*sz;
  this->_z =  cx*cy*sz - sx*sy*cz;
}

inline
void VW::Quaternion::SetFromYXZEuler(const double theta_y,
				     const double theta_x,
				     const double theta_z)
{
  const double cx(cos(0.5*theta_x));
  const double cy(cos(0.5*theta_y));
  const double cz(cos(0.5*theta_z));
  const double sx(sin(0.5*theta_x));
  const double sy(sin(0.5*theta_y));
  const double sz(sin(0.5*theta_z));

  this->_r =  cx*cy*cz - sx*sy*sz;
  this->_x =  sx*cy*cz - cx*sy*sz;
  this->_y =  cx*sy*cz + sx*cy*sz;
  this->_z =  cx*cy*sz + sx*sy*cz;
}



inline
void VW::Quaternion::SetFromZYXEuler(const double theta_z,
				     const double theta_y,
				     const double theta_x)
{
  const double cx(cos(0.5*theta_x));
  const double cy(cos(0.5*theta_y));
  const double cz(cos(0.5*theta_z));
  const double sx(sin(0.5*theta_x));
  const double sy(sin(0.5*theta_y));
  const double sz(sin(0.5*theta_z));

  this->_r = cx*cy*cz - sx*sy*sz;
  this->_x = sx*cy*cz + cx*sy*sz;
  this->_y = cx*sy*cz - sx*cy*sz;
  this->_z = cx*cy*sz + sx*sy*cz;

}
 
inline
void VW::Quaternion::SetFromZXZEuler(const double theta_z1,
				     const double theta_x,
				     const double theta_z2)
{
	using namespace std; // for cos, sin
  const double cz1(cos(0.5*theta_z1));
  const double cx(cos(0.5*theta_x));
  const double cz2(cos(0.5*theta_z2));
  const double sz1(sin(0.5*theta_z1));
  const double sx(sin(0.5*theta_x));
  const double sz2(sin(0.5*theta_z2));

  this->_r = cz2*cx*cz1 - sz2*cx*sz1;
  this->_x = cz2*sx*cz1 + sz2*sx*sz1;
  this->_y = sz2*sx*cz1 - cz2*sx*sz1;
  this->_z = cz2*cx*sz1 + sz2*cx*cz1;
}

inline
void VW::Quaternion::SetRXYZ(const VNL::VectorFixed<4,double>& vRXYZ) 
{
  _r = vRXYZ[0];
  _x = vRXYZ[1];
  _y = vRXYZ[2];
  _z = vRXYZ[3];
}

inline
void VW::Quaternion::GetXYZEuler(double& theta_x,
				 double& theta_y,
				 double& theta_z) const
{
 	using namespace std; // for cos, sin
  theta_x = atan2( 2*(_y*_z + _r*_x),
		   _r*_r - _x*_x - _y*_y + _z*_z );
  
  theta_y = asin( -2*(_x*_z - _r*_y) );

  theta_z = atan2( 2*(_x*_y + _r*_z),
		   _r*_r + _x*_x - _y*_y - _z*_z );

}

// Based on
// http://www.mathworks.com/access/helpdesk/help/toolbox/aeroblks/quaternionstoeulerangles.html
inline 
void VW::Quaternion::GetZXYEuler(double& theta_z,
				 double& theta_x,
				 double& theta_y) const
{
  using namespace std; // for atan2, asin, etc

  theta_x =  asin(-2*(_y*_z - _r*_x) );

  theta_y =  atan2( 2*(_x*_z + _r*_y),
		   _r*_r - _x*_x - _y*_y + _z*_z );

  theta_z =  atan2( 2*(_x*_y + _r*_z),
		   _r*_r - _x*_x + _y*_y - _z*_z );
}

inline 
void VW::Quaternion::GetYXZEuler(double& theta_y,
				 double& theta_x,
				 double& theta_z) const
{
  using namespace std; // for atan2
  theta_x =  asin( 2*(_y*_z + _r*_x) );

  theta_y =  atan2(-2*(_x*_z - _r*_y),
		   _r*_r - _x*_x - _y*_y + _z*_z );

  theta_z =  atan2(-2*(_x*_y - _r*_z),
		   _r*_r - _x*_x + _y*_y - _z*_z );
}

inline
void VW::Quaternion::GetZYXEuler(double& theta_z,
				 double& theta_y,
				 double& theta_x) const
{
	using namespace std; // for cos, sin
   theta_x = atan2( -2*(_y*_z - _r*_x),
		   _r*_r - _x*_x - _y*_y + _z*_z );
  
  theta_y = asin( 2*(_x*_z + _r*_y) );

  theta_z = atan2( -2*(_x*_y - _r*_z),
		   _r*_r + _x*_x - _y*_y - _z*_z );
		   
}

inline
void VW::Quaternion::GetZXZEuler(double& theta_z1,
				 double& theta_x,
				 double& theta_z2) const
{
	using namespace std; // for cos, sin
   theta_z2 = atan2 ( _x*_z + _r*_y , _r*_x - _y*_z );
  theta_x  = acos  ( _r*_r - _x*_x - _y*_y + _z*_z );
  theta_z1 = atan2 ( _x*_z - _r*_y , _y*_z + _r*_x );
}

inline
VNL::VectorFixed<4,double> VW::Quaternion::GetRXYZ() const
{
  VNL::VectorFixed<4,double> vRXYZ;
  vRXYZ[0] = _r;
  vRXYZ[1] = _x;
  vRXYZ[2] = _y;
  vRXYZ[3] = _z;
  
  return vRXYZ;
}

inline bool 
VW::Quaternion::operator==(const VW::Quaternion& q) const
{
  return ((q._x == _x) && (q._y == _y) && (q._z == _z) && (q._r == _r));
}

inline bool 
VW::Quaternion::operator!=(const VW::Quaternion& q) const
{
  return ((q._x != _x) || (q._y != _y) || (q._z != _z) || (q._r != _r));
}




// STREAMING STUFF //

inline
bool  VW::Quaternion::WriteASCII(std::ostream& stream) const
{
  stream << _x << "\t" << _y << "\t" << _z << "\t" << _r;
  return(true);
}

inline
bool  VW::Quaternion::WriteXML(std::ostream& stream, 
				     const std::string& name) const
{
  
  // Start tag
  if (name.size()>0)
    stream << "<Quaternion NAME=\"" << name << "\"> ";
  else
    stream << "<Quaternion> ";
  
  // Contents
  stream << "<X>" << _x << "</X> ";
  stream << "<Y>" << _y << "</Y> ";
  stream << "<Z>" << _z << "</Z> ";
  stream << "<R>" << _r << "</R> ";
  
  // End tag
  stream << "</Quaternion>";
  
  return(true);
}

inline
bool VW::Quaternion::WriteBinary(std::ostream& stream) const
{
  VW::WriteBinary(stream,_x);
  VW::WriteBinary(stream,_y);
  VW::WriteBinary(stream,_z);
  VW::WriteBinary(stream,_r);
  return(true);
}


inline
bool VW::Quaternion::ReadASCII(std::istream& stream)
{
  stream >> _x >> _y >> _z >> _r; 
  return(true);
}

inline
bool VW::Quaternion::ReadXML(std::istream& stream, 
				    const std::string&)
{
  std::string tagname;
  std::istringstream contents;
  if (VW::FindXMLBlock("Quaternion",stream,tagname)) {
    if (VW::GetXMLBlock("X",stream,contents,tagname))
      contents >> _x;
    else {
      std::cerr << "VW::Quaternion::ReadXML - error reading x-component.\n";
      return(false);
    }
    if (VW::GetXMLBlock("Y",stream,contents,tagname))
      contents >> _y;
    else {
      std::cerr << "VW::Quaternion::ReadXML - error reading y-component.\n";
      return(false);
    }
    if (VW::GetXMLBlock("Z",stream,contents,tagname))
      contents >> _z;
    else {
      std::cerr << "VW::Quaternion::ReadXML - error reading z-component.\n";
      return(false);
    }
    if (VW::GetXMLBlock("R",stream,contents,tagname))
      contents >> _r;
    else {
      std::cerr << "VW::Quaternion::ReadXML - error reading r-component.\n";
      return(false);
    } 
  }else {
    return(false);
  }
  return(true);
}

inline
bool VW::Quaternion::ReadBinary(std::istream& stream)
{
  VW::ReadBinary(stream,_x);
  VW::ReadBinary(stream,_y);
  VW::ReadBinary(stream,_z);
  VW::ReadBinary(stream,_r);
  return(true);
}



#endif
