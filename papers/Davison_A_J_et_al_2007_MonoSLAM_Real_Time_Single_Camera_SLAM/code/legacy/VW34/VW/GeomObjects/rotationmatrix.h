/** 
 * @file     rotationmatrix.h
 * @author   BJ Tordoff
 * @date     15/02/2003
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
 * @class   VW::RotationMatrix
 * @ingroup GeometricObjects
 * @date    15/02/2003
 * @author  BJ Tordoff
 * @brief A class to store (3D) rotation matrices.
 *
 * Whilst this is mostly just and ordinary VW::MatrixFixed, it has some added functions
 * for enforcing ortho-normality and for creating from various other rotational
 * representations.
 */

#ifndef _rotationmatrix_h_
#define _rotationmatrix_h_

// System
#include <string>
#include <iostream>

// VW
#include <VNL/matrixfixed.h>

// Local
#include "quaternion.h"
#include "angleaxis.h"

namespace VW {

  // Forward declare
  class Point3D; typedef Point3D Vector3D;



  class RotationMatrix : public VNL::MatrixFixed<3,3,double> {

  public:
    
    /** Construct and set to identity. */
    RotationMatrix();

    /** Construct from another rotation matrix. */
    RotationMatrix(const RotationMatrix& R);

    /** Construct from an angle around an axis. */
    RotationMatrix(const VW::Vector3D& axis,
		   const double& angle);

    /** Construct using an angle-axis vector. */
    RotationMatrix(const VW::AngleAxis& angleaxis);

    /** Construct from a quaternion. */
    RotationMatrix(const VW::Quaternion& q);

    /** Set from another matrix. */
    RotationMatrix(const VNL::MatrixFixed<3,3,double>& M);



    /** Set from an angle around an axis. */
    void Set(const VW::Vector3D& axis,
		   const double& angle);

    /** Set from an angle-axis vector. */
    void Set(const VW::Vector3D& angleaxis);

    /** Set from a quaternion. */
    void Set(const VW::Quaternion& q);

    /** Set from another matrix. */
    void Set(const VNL::MatrixFixed<3,3,double>& M);

    /** Set to the identity matrix. */
    void SetIdentity();

    // Euler angles  ** Take care with the ordering **

    /** Create a RotationMatrix from X,Y,Z rotations. Note that the rotations
     * are applied in order to an object, 
     * so R_xyz * X = R_z * R_y * R_x * X.
     */
    void SetFromXYZEuler(const double theta_x,
			 const double theta_y,
			 const double theta_z);


    /** Create a RotationMatrix from roll, pitch, yaw. Note that the rotations
     * are applied in order to an object, 
     * so R_zxy * X = R_y * R_x * R_z * X.
     */
    void SetFromZXYEuler(const double theta_z,
			 const double theta_x,
			 const double theta_y);

    /** Create a RotationMatrix from yaw, pitch, roll. Note that the rotations
     * are applied in order to an object, 
     * so R_yxz * X = R_z * R_x * R_y * X.
     */
    void SetFromYXZEuler(const double theta_y,
			 const double theta_x,
			 const double theta_z);


    /** Create a RotationMatrix from Z,Y,X rotations. Note that the rotations
     * are applied in order to an object, 
     * so q_zyx * X = R_x * R_y * R_z * X.
     */
    void SetFromZYXEuler(const double theta_z,
			 const double theta_y,
			 const double theta_x);

    /** Create a RotationMatrix from Z,X,Z rotations. Note that the rotations
     * are applied in order to an object, 
     * so R_zxz * X = R_z2 * R_x * R_z1 * X.
     * The ZXZ form is common in physics-based problems.
     */
    void SetFromZXZEuler(const double theta_z1,
			 const double theta_x,
			 const double theta_z2);
 


    /** Return the quaternion representation. */
    VW::Quaternion Quaternion() const;
    
    /** Return the angle-axis vector.
     * Note that this is pretty expensive, so convert
     * to a quaternion if you plan to do this a lot. 
     */
    VW::AngleAxis AngleAxis() const;
    
    /** Return the angle of rotation (radians).
     * Note that this is pretty expensive, so convert
     * to a quaternion if you plan to do this a lot. 
     */
    double Angle() const;
    
    /** Return the axis of rotation. 
     * Note that this is pretty expensive, so convert
     * to a quaternion if you plan to do this a lot. 
     */
    VW::Vector3D Axis() const;
    
    /** Note that the inverse is the transpose, and both are valid rotations too. */
    VW::RotationMatrix Inverse() const;

    /** Note that the transpose is also a valid rotation. */
    VW::RotationMatrix Transpose() const;

    /** Force orthonormality using SVD. */
    void Normalise();

    /** Rotate a 3D vector around the origin. */
    VW::Vector3D Rotate(const VW::Vector3D& v) const;
    /** Rotate a 3D vector back around the origin (so that InverseRotate(Rotate(v))=v). */
   VW::Vector3D InverseRotate(const VW::Vector3D& v) const;


    // private: specifier removed by ajd: meant that you couldn't
    // do things like RotationMatrix A = B.Transpose()
    /** Set from a vnl matrix. */
    RotationMatrix(const VNL::Matrix<double>& M);

    bool WriteBinary(std::ostream& stream) const;
    bool WriteBinaryFile(const std::string& filename) const;
  };


}; // End namespace VW





















// Some inlines
inline
VW::RotationMatrix::RotationMatrix()
{
  this->SetIdentity();
}

inline
VW::RotationMatrix::RotationMatrix(const VW::RotationMatrix& R)
  : VNL::MatrixFixed<3,3,double>(R)
{
}

inline
VW::RotationMatrix::RotationMatrix(const VW::Vector3D& axis,
				   const double& angle)
{
  Set(axis,angle);
}

inline
VW::RotationMatrix::RotationMatrix(const VW::AngleAxis& angleaxis)
{
  Set(angleaxis);
}

inline
VW::RotationMatrix::RotationMatrix(const VW::Quaternion& q)
{
  Set(q);
}

/**
 * Note that this can be a costly operation, as the matrix is
 * checked for orthonormality using the SVD. If the matrix also
 * includes a reflection (ie. has negative determinant) then
 * it is impossible to tell which row or col needs inverting
 * and the function will abort.
 * @param M the matrix to convert to a rotation matrix
 */
inline
VW::RotationMatrix::RotationMatrix(const VNL::MatrixFixed<3,3,double>& M)
{
  Set(M);
}


inline
VW::RotationMatrix::RotationMatrix(const VNL::Matrix<double>& M)
  : VNL::MatrixFixed<3,3,double>(M)
{
}

inline VW::Quaternion 
VW::RotationMatrix::Quaternion() const
{
  return (VW::Quaternion(*this));
}
 
inline VW::AngleAxis
VW::RotationMatrix::AngleAxis() const
{
  return (VW::AngleAxis(*this));
}
 
inline VW::RotationMatrix 
VW::RotationMatrix::Inverse() const
{
  return Transpose();
}


inline VW::RotationMatrix 
VW::RotationMatrix::Transpose() const
{
  VW::RotationMatrix R;
  R[0][0] = data[0][0]; R[0][1] = data[1][0]; R[0][2] = data[2][0]; 
  R[1][0] = data[0][1]; R[1][1] = data[1][1]; R[1][2] = data[2][1]; 
  R[2][0] = data[0][2]; R[2][1] = data[1][2]; R[2][2] = data[2][2]; 

  return R;
}

#endif // _rotationmatrix_h_
