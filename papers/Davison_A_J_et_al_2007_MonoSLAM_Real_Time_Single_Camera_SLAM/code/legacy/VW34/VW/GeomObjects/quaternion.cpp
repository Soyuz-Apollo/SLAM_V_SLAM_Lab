/** 
 * @file     quaternion.cpp
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

#include "angleaxis.h"
#include "quaternion.h"
#include "rotationmatrix.h"
#include "point3d.h"
#include "lineseg3d.h"
#include <math.h>

/**
 * @param axis  the axis (doesn't need to be a unit vector) around which to rotate
 * @param angle the rotation angle in radians
 */
void VW::Quaternion::Set(const VW::Vector3D& axis,
			   const double& angle)
{
  if (fabs(angle)<1e-8) {
      _x = 0.0;
      _y = 0.0;
      _z = 0.0;
      _r = 1.0;
      return;
  }

  double a = angle / 2.0;  // half angle
  double s = sin(a);
  double c = cos(a);

  // Make sure the axis is a unit vector
  double n=1.0/axis.Norm();

  _x = s * axis.GetX() * n;  // half angle multiplied with axis
  _y = s * axis.GetY() * n;  // half angle multiplied with axis
  _z = s * axis.GetZ() * n;  // half angle multiplied with axis
  _r = c;                    // real part is cosine of half angle
}

/**
 * The angle is recovered as \| angleaxis \| = angle and
 * the axis as axis = angleaxis / angle. 
 * @param angleaxis the 3D vector whose size gives the angle and direction 
 *                  gives the axis
 */
void VW::Quaternion::Set(const VW::AngleAxis& angleaxis)
{
  const double x = angleaxis.GetX();
  const double y = angleaxis.GetY();
  const double z = angleaxis.GetZ();
  const double angle = sqrt(x*x + y*y + z*z);
  if (angle > 0.0) {
    const double s=sin(angle/2.0)/angle;
    const double c=cos(angle/2.0);
    _x = s * x;
    _y = s * y;
    _z = s * z;
    _r = c;
  } else {
    _x = _y = _z = 0.0;
    _r = 1.0;
  }
}


/**
 * @param R the rotation matrix from which the quaternion will be extracted
 */
void VW::Quaternion::Set(const VW::RotationMatrix& R)
{
  double d0 = R[0][0], d1 = R[1][1], d2 = R[2][2];

  // The trace determines the method of decomposition
  double rr = 1.0 + d0 + d1 + d2;

  if (rr>0) {
      double s = 0.5 / sqrt(rr);
      _x = (R[2][1] - R[1][2]) * s;
      _y = (R[0][2] - R[2][0]) * s;
      _z = (R[1][0] - R[0][1]) * s;
      _r = 0.25 / s;
  } else {
    // Trace is less than zero, so need to determine which
    // major diagonal is largest
    if ((d0 > d1) && (d0 > d2)) {
      double s = 0.5 / sqrt(1 + d0 - d1 - d2);
      _x = 0.5 * s;
      _y = (R[0][1] + R[1][0]) * s;
      _z = (R[0][2] + R[2][0]) * s;
      _r = (R[1][2] + R[2][1]) * s;
    } else if (d1 > d2) {
       double s = 0.5 / sqrt(1 + d0 - d1 - d2);
      _x = (R[0][1] + R[1][0]) * s;
      _y = 0.5 * s;
      _z = (R[1][2] + R[2][1]) * s;
      _r = (R[0][2] + R[2][0]) * s;
    } else {
      double s = 0.5 / sqrt(1 + d0 - d1 - d2);
      _x = (R[0][2] + R[2][0]) * s;
      _y = (R[1][2] + R[2][1]) * s;
      _z = 0.5 * s;
      _r = (R[0][1] + R[1][0]) * s;
    }
  }
}

VW::RotationMatrix VW::Quaternion::RotationMatrix() const
{
  VW::RotationMatrix R;

  double x2 = _x * _x;
  double y2 = _y * _y;
  double z2 = _z * _z;
  double r2 = _r * _r;
  R[0][0] = r2 + x2 - y2 - z2;         // fill diagonal terms
  R[1][1] = r2 - x2 + y2 - z2;
  R[2][2] = r2 - x2 - y2 + z2;
  double xy = _x * _y;
  double yz = _y * _z;
  double zx = _z * _x;
  double rx = _r * _x;
  double ry = _r * _y;
  double rz = _r * _z;
  R[0][1] = 2 * (xy - rz);             // fill off diagonal terms
  R[0][2] = 2 * (zx + ry);
  R[1][0] = 2 * (xy + rz);
  R[1][2] = 2 * (yz - rx);
  R[2][0] = 2 * (zx - ry);
  R[2][1] = 2 * (yz + rx);

  return(R);
}

/**
 * @returns the rotation matrix corresponding to the inverse rotation by this
 *          quaternion.
 */
VW::RotationMatrix VW::Quaternion::RotationMatrixTranspose() const
{
  VW::RotationMatrix R;

  double x2 = _x * _x;
  double y2 = _y * _y;
  double z2 = _z * _z;
  double r2 = _r * _r;
  R[0][0] = r2 + x2 - y2 - z2;         // fill diagonal terms
  R[1][1] = r2 - x2 + y2 - z2;
  R[2][2] = r2 - x2 - y2 + z2;
  double xy = _x * _y;
  double yz = _y * _z;
  double zx = _z * _x;
  double rx = _r * _x;
  double ry = _r * _y;
  double rz = _r * _z;
  R[0][1] = 2 * (xy + rz);             // fill off diagonal terms
  R[0][2] = 2 * (zx - ry);
  R[1][2] = 2 * (yz + rx);
  R[1][0] = 2 * (xy - rz);
  R[2][0] = 2 * (zx + ry);
  R[2][1] = 2 * (yz - rx);

  return(R);
}

/**
 * @returns the angle of rotation in radians.
 */
double VW::Quaternion::Angle() const
{
  return (2.0 *
	  atan2 (sqrt(_x*_x + _y*_y + _z*_z), _r)
	  );
}

/** @returns a vector whose magnitude gives the angle of rotation and direction 
 *           gives the rotation axis 
 */
VW::AngleAxis VW::Quaternion::AngleAxis() const
{
  const double a = Angle();
  if (a>0.0) {
    const double scaling = a/sin(a/2.0);
    return VW::AngleAxis(_x*scaling, _y*scaling, _z*scaling);
  }
  return VW::AngleAxis(0,0,0);
}


/** @returns the axis of rotation. */
VW::Vector3D VW::Quaternion::Axis() const
{
  VW::Vector3D _i = Imaginary();
  double l = _i.Norm();
  if (l<1e-8) {
    _i.Set(0.0, 0.0, 0.0);
  }
  else {
    _i /= l;
  }
  return _i;
}

/** @returns the imaginary part (sin(theta/2)*(x,y,z)) of the quaternion */
VW::Vector3D VW::Quaternion::Imaginary() const
{
  return( VW::Vector3D(_x,_y,_z) );
}

/** 
 * @param v the vector to be rotated
 * @returns the resulting rotated vector 
 */
VW::Vector3D VW::Quaternion::operator*(const VW::Vector3D& v) const
{
  return (this->Rotate(v));
}

/** Rotate a vector.
 * @param v the vector to be rotated
 * @returns the resulting rotated vector
 */
VW::Vector3D VW::Quaternion::Rotate(const VW::Vector3D& v) const
{
  const VW::Vector3D _i = this->Imaginary();
  const VW::Vector3D _n = VectorProduct(_i, v);
  const VW::Vector3D delta = _r * _n - VectorProduct(_n, _i);
  return (v + 2.0 * delta);
}

/** Rotate a line segment.
 * @param l the line segment to be rotated
 * @returns the resulting rotated line segment
 */
VW::LineSeg3D 
VW::Quaternion::Rotate(const VW::LineSeg3D& l) const
{
  // Simply rotate both lines
  const VW::Vector3D i = this->Imaginary();
  const VW::Vector3D ns = VectorProduct(i, l.StartPoint());
  const VW::Vector3D ne = VectorProduct(i, l.EndPoint());
  const VW::Vector3D deltas = _r * ns - VectorProduct(ns, i);
  const VW::Vector3D deltae = _r * ne - VectorProduct(ne, i);
  const VW::LineSeg3D result(l.StartPoint() + 2.0 * deltas,
			     l.EndPoint()   + 2.0 * deltae);
  return (result);
}

/** Rotate a line segment.
 * @param l the line segment to be rotated
 * @returns the resulting rotated line segment
 */
VW::LineSeg3D 
VW::Quaternion::operator*(const VW::LineSeg3D& l) const
{
  return (this->Rotate(l));
}

/** Combine two rotations.
 * @param q the inner rotation
 * @returns the resulting rotation (like this*q for matrices)
 */
VW::Quaternion VW::Quaternion::operator*(const VW::Quaternion& q) const
{
  // real and img parts of args
  const double r1 = this->_r;                  
  const double r2 = q._r;
  const VW::Vector3D i1 = this->Imaginary();
  const VW::Vector3D i2 = q.Imaginary();

  // Real is product of real, and dot-product of imag
  double real_v = (r1 * r2) - (i1 * i2);

  // Imag is cross product of imag + real*imag for each
  VW::Vector3D img = VectorProduct(i1, i2);
  img += (i2 * r1) + (i1 * r2);

  // Finally, build the result
  VW::Quaternion prod(img.GetX(), img.GetY(), img.GetZ(), real_v);
  return prod;
}
