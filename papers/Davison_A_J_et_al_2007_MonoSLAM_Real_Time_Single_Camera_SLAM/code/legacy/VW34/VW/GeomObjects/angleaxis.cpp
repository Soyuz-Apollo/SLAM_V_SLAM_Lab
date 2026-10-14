/** 
 * @file     angleaxis.cpp
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
void VW::AngleAxis::Set(const VW::Vector3D& axis,
			const double& angle)
{
  _x = angle * axis.GetX();
  _y = angle * axis.GetY();
  _z = angle * axis.GetZ();
}

/**
 * The angle is recovered as \| angleaxis \| = angle and
 * the axis as axis = angleaxis / angle. 
 * @param angleaxis the 3D vector whose size gives the angle and direction 
 *                  gives the axis
 */
void VW::AngleAxis::Set(const VW::Vector3D& angleaxis)
{
    _x = angleaxis.GetX();
    _y = angleaxis.GetY();
    _z = angleaxis.GetZ();
}


/**
 * @param R the rotation matrix from which the quaternion will be extracted
 */
void VW::AngleAxis::Set(const VW::RotationMatrix& R)
{
  /*  MATLAB code:
   * theta=acos((trace(R)-1)/2);
   * ax=(R-R');
   * a=[ax(3,2) ax(1,3) ax(2,1)]';
   * scale=sqrt(a'*a);
   * if (scale>0.00001)
   *    a=a/scale;
   *    a=a*theta;
   * else
   *    a=[0;0;0];
   * end
   */

  // The trace determines the angle
  const double diag = 0.5*(R[0][0] + R[1][1] + R[2][2] - 1.0);
  if (diag >= 1.0) {
    // zero angle
    _x = _y = _z = 0.0;
    return;
  } else {
    const double angle = acos(diag);
    _x = R[2][1] - R[1][2];
    _y = R[0][2] - R[2][0];
    _z = R[1][0] - R[0][1];
    const double axisnorm = sqrt(_x*_x + _y*_y + _z*_z);
    if (axisnorm<1e-8) {
      // Axis is undefined
      _x = _y = _z = 0.0;
      return;
    } else {
      // All OK, so scale it
      const double scale = angle/axisnorm;
      _x *= scale;
      _y *= scale;
      _z *= scale;
    }
  }
}

void VW::AngleAxis::Set(const VW::Quaternion& q)
{
  const double r = q.GetR();
  _x = q.GetX();
  _y = q.GetY();
  _z = q.GetZ();
  const double norm = sqrt(_x*_x + _y*_y + _z*_z);
  if (norm>0) {
    const double scaling = 2.0*atan2(norm,r) / norm;
    _x *= scaling;
    _y *= scaling;
    _z *= scaling;
  }
}

VW::Quaternion VW::AngleAxis::Quaternion() const
{
  const double angle = sqrt(_x*_x + _y*_y + _z*_z);
  if (angle < 1e-8) {
    return VW::Quaternion(0.0, 0.0, 0.0, 1.0);
  }
  const double s=sin(angle/2.0)/angle;
  const double c=cos(angle/2.0);
  return VW::Quaternion(s*_x,
			s*_y,
			s*_z,
			c);
}

VW::RotationMatrix VW::AngleAxis::RotationMatrix() const
{
  /*  MATLAB code:
   * if (a'*a < 1e-06)
   *   R=eye(3); % the 3x3 identity matrix
   * else
   *   theta=sqrt(a'*a);
   *   a=a/theta;
   *   ax=vec_to_ASmatrix(a);
   *   R=eye(3)+sin(theta)*ax+(1-cos(theta))*ax^2;
   * end  
   */

  // start with an identity matrix.
  VW::RotationMatrix R;
  const double angle = sqrt(_x*_x + _y*_y + _z*_z);

  // Identity is all that can be deduced from zero angle
  if (angle == 0)
    return R;

  const double c = cos(angle)-1.0;
  const double s = sin(angle);

  // normalize axis to a unit vector u
  const double norm = 1.0/angle;
  const double nx = _x*norm;
  const double ny = _y*norm;
  const double nz = _z*norm;
  
  // add (cos(angle)-1)*(1 - u u').
  R[0][0] += c* (nx*nx - 1.0);
  R[0][1] += c*  nx*ny;
  R[0][2] += c*  nx*nz;

  R[1][0] += c*  ny*nx;
  R[1][1] += c* (ny*ny - 1.0);
  R[1][2] += c*  ny*nz;

  R[2][0] += c*  nz*nx;
  R[2][1] += c*  nz*ny;
  R[2][2] += c* (nz*nz - 1.0);

  // add sin(angle) * [u]
  /*           */  R[0][1] -= s*nz; R[0][2] += s*ny;
  R[1][0] += s*nz; /*           */  R[1][2] -= s*nx;
  R[2][0] -= s*ny; R[2][1] += s*nx; /*           */

  return(R);
}

/**
 * @returns the rotation matrix corresponding to the inverse rotation by this
 *          quaternion.
 */
VW::RotationMatrix VW::AngleAxis::RotationMatrixTranspose() const
{
  // start with an identity matrix.
  VW::RotationMatrix R;
  const double angle = sqrt(_x*_x + _y*_y + _z*_z);

  // Identity is all that can be deduced from zero angle
  if (angle == 0)
    return R;

  const double c = cos(angle)-1.0;
  const double s = sin(angle);

  // normalize axis to a unit vector u
  const double norm = 1.0/angle;
  const double nx = _x*norm;
  const double ny = _y*norm;
  const double nz = _z*norm;
 
  // add (cos(angle)-1)*(1 - u u').
  R[0][0] += c* (nx*nx - 1.0);
  R[0][1] += c*  nx*ny;
  R[0][2] += c*  nx*nz;

  R[1][0] += c*  ny*nx;
  R[1][1] += c* (ny*ny - 1.0);
  R[1][2] += c*  ny*nz;

  R[2][0] += c*  nz*nx;
  R[2][1] += c*  nz*ny;
  R[2][2] += c* (nz*nz - 1.0);

  // add sin(angle) * [u]
  /*           */  R[0][1] += s*nz; R[0][2] -= s*ny;
  R[1][0] -= s*nz; /*           */  R[1][2] += s*nx;
  R[2][0] += s*ny; R[2][1] -= s*nx; /*           */

  return(R);
}

/**
 * @returns the angle of rotation in radians.
 */
double VW::AngleAxis::Angle() const
{
  return (sqrt(_x*_x + _y*_y + _z*_z));
}


/** @returns the axis of rotation. */
VW::Vector3D VW::AngleAxis::Axis() const
{
  const double ang = sqrt(_x*_x + _y*_y + _z*_z);
  if (ang<1e-8) {
    return VW::Vector3D(0,0,0);
  }
  const double scale = 1.0/ang;
  return VW::Vector3D(_x*scale, _y*scale, _z*scale);
}

/** Rotate a vector.
 * @param v the vector to be rotated
 * @returns the resulting rotated vector
 */
VW::Vector3D VW::AngleAxis::Rotate(const VW::Vector3D& v) const
{
  const double angle = sqrt(_x*_x + _y*_y + _z*_z);
  if (angle<1e-8) {
    return v;
  }
  const double sn = sin(angle);
  const double cs = cos(angle);
  const VW::Vector3D axis(_x/angle, _y/angle, _z/angle);
  const VW::Vector3D n = VectorProduct(axis, v);
  const VW::Vector3D delta = sn * n + (cs-1.0) * VectorProduct(n, axis);
  return (v + delta);
}

/** Rotate a line segment.
 * @param l the line segment to be rotated
 * @returns the resulting rotated line segment
 */
VW::LineSeg3D 
VW::AngleAxis::Rotate(const VW::LineSeg3D& l) const
{
  // Simply rotate both lines
  const double angle = sqrt(_x*_x + _y*_y + _z*_z);
  if (angle<1e-8) {
    return l;
  }
  const double sn = sin(angle);
  const double cs = cos(angle);
  const VW::Vector3D axis(_x/angle, _y/angle, _z/angle);
  const VW::Vector3D ns = VectorProduct(axis, l.StartPoint());
  const VW::Vector3D ne = VectorProduct(axis, l.EndPoint());
  const VW::Vector3D deltas = sn * ns + (cs-1.0) * VectorProduct(ns, axis);
  const VW::Vector3D deltae = sn * ne + (cs-1.0) * VectorProduct(ne, axis);
  return VW::LineSeg3D(l.StartPoint() + deltas,
		       l.EndPoint()   + deltae);
}


/** Combine two rotations (this is VERY costly).
 * @param aa the inner rotation
 * @returns the resulting rotation (like this*q for matrices)
 */
VW::AngleAxis VW::AngleAxis::operator*(const VW::AngleAxis& aa) const
{
  // This is best done as quaternions
  VW::Quaternion q1(*this), q2(aa);
  return AngleAxis(q1*q2);
}
