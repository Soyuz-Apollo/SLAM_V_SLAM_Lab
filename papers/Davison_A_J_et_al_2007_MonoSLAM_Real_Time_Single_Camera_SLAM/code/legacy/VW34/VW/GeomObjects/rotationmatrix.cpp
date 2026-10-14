/** 
 * @file     rotationmatrix.cc
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


// System includes
#include <cmath>
#include <VNL/Algo/svd.h>
#include <VNL/Algo/determinant.h>

// Local includes
#include "rotationmatrix.h"
#include "quaternion.h"
#include "point3d.h"

using namespace std;
using namespace VW;


void
VW::RotationMatrix::Set(const VW::Quaternion& q)
{
  *this = q.RotationMatrix();
}

void
VW::RotationMatrix::Set(const VW::Vector3D& angleaxis)
{
  double angle=angleaxis.Norm();
  Set(angleaxis,angle);
}


/** Note that the axis doesn't have to be a unit vector.
 * @param axis  the axis around which to rotate
 * @param angle the angle of rotation in radians
 */
void VW::RotationMatrix::Set(const VW::Vector3D& axis,
			     const double& angle)
{
  // start with an identity matrix.
  this->SetIdentity();

  // Identity is all that can be deduced from zero angle
  if (angle == 0)
    return;

  // normalize to a unit vector u
  double u[3],norm;
  norm=1.0/axis.Norm();
  u[0]=axis.GetX()*norm;
  u[1]=axis.GetY()*norm;
  u[2]=axis.GetZ()*norm;
  
  // add (cos(angle)-1)*(1 - u u').
  double cos_angle = cos(angle);
  for (unsigned i=0; i<3; ++i)
    for (unsigned j=0; j<3; ++j)
      this->data[i][j] += (cos_angle-1) * ((i==j ? 1:0) - u[i]*u[j]);

  // add sin(angle) * [u]
  double sin_angle = sin(angle);
  /* */                         data[0][1] -= sin_angle*u[2]; data[0][2] += sin_angle*u[1];
  data[1][0] += sin_angle*u[2]; /* */                         data[1][2] -= sin_angle*u[0];
  data[2][0] -= sin_angle*u[1]; data[2][1] += sin_angle*u[0]; /* */

}

/**
 * Note that this can be a costly operation, as the matrix is
 * checked for orthonormality using the SVD. If the matrix also
 * includes a reflection (ie. has negative determinant) then
 * it is impossible to tell which row or col needs inverting
 * and the function will abort.
 * @param M the matrix to convert to a rotation matrix
 */
void VW::RotationMatrix::Set(const VNL::MatrixFixed<3,3,double>& M)
{
  // If M is even close to being a rotation it should have
  // determinant close to 1. Negative determinant means a
  // row or col is inverted
  double d=VNL::Determinant(M);

  if (d<0) {
    std::cerr << "VW::RotationMatrix::Set(MatrixFixed<3,3>& M): Error - determinant of M is negative (ie. one or more rows/cols are inverted). Impossible to tell which one, so aborting." << std::endl;
    exit(0);
  }

  // Doing SVD and setting all singular values to 1 gives the
  // rotation matrix closest to M in the Mahalanobis distance sense.
  VNL::SVD<double> svd(M);

  *this = svd.U() * svd.V().Transpose();

}

void VW::RotationMatrix::SetIdentity()
{
  data[0][0] = 1.0;   data[0][1] = 0.0;   data[0][2] = 0.0; 
  data[1][0] = 0.0;   data[1][1] = 1.0;   data[1][2] = 0.0; 
  data[2][0] = 0.0;   data[2][1] = 0.0;   data[2][2] = 1.0; 
}

void VW::RotationMatrix::SetFromXYZEuler(const double theta_x,
					 const double theta_y,
					 const double theta_z)
{
  const double sx = sin(theta_x), cx = cos(theta_x);
  const double sy = sin(theta_y), cy = cos(theta_y);
  const double sz = sin(theta_z), cz = cos(theta_z);
  data[0][0] =            cy*cz;   
  data[0][1] = -cx*sz +sx*sy*cz;   
  data[0][2] =  sx*sz +cx*sy*cz; 

  data[1][0] =            cy*sz;   
  data[1][1] =  cx*cz +sx*sy*sz;   
  data[1][2] = -sx*cz +cx*sy*sz;
 
  data[2][0] =   -sy;   
  data[2][1] = sx*cy;   
  data[2][2] = cx*cy; 
}


void VW::RotationMatrix::SetFromZXYEuler(const double theta_z,
					 const double theta_x,
					 const double theta_y)
{
  const double sx = sin(theta_x), cx = cos(theta_x);
  const double sy = sin(theta_y), cy = cos(theta_y);
  const double sz = sin(theta_z), cz = cos(theta_z);
  data[0][0] =  cy*cz +sx*sy*sz;   
  data[0][1] = -cy*sz +sx*sy*cz;   
  data[0][2] =  cx*sy; 

  data[1][0] =  cx*sz;   
  data[1][1] =  cx*cz;   
  data[1][2] = -sx;
 
  data[2][0] = -sy*cz +sx*cy*sz;   
  data[2][1] =  sy*sz +sx*cy*cz;   
  data[2][2] =  cx*cy; 
}

void VW::RotationMatrix::SetFromYXZEuler(const double theta_y,
					 const double theta_x,
					 const double theta_z)
{
  const double sx = sin(theta_x), cx = cos(theta_x);
  const double sy = sin(theta_y), cy = cos(theta_y);
  const double sz = sin(theta_z), cz = cos(theta_z);
  data[0][0] =  cy*cz -sx*sy*sz;   
  data[0][1] = -cx*sz;   
  data[0][2] =  sy*cz +sx*cy*sz;   

  data[1][0] =  cy*sz +sx*sy*cz;   
  data[1][1] =  cx*cz;   
  data[1][2] =  sy*sz -sx*cy*cz;   
 
  data[2][0] = -cx*sy; 
  data[2][1] =  sx;
  data[2][2] =  cx*cy; 
}


void VW::RotationMatrix::SetFromZYXEuler(const double theta_z,
					 const double theta_y,
					 const double theta_x)
{
  const double sx = sin(theta_x), cx = cos(theta_x);
  const double sy = sin(theta_y), cy = cos(theta_y);
  const double sz = sin(theta_z), cz = cos(theta_z);
  data[0][0] =  cy*cz;   
  data[0][1] = -cy*sz;   
  data[0][2] =  sy;   

  data[1][0] =  cx*sz +sx*sy*cz;   
  data[1][1] =  cx*cz -sx*sy*sz;   
  data[1][2] =        -sx*cy;   
 
  data[2][0] =  sx*sz -cx*sy*cz; 
  data[2][1] =  sx*cz +cx*sy*sz;
  data[2][2] =         cx*cy; 
}

void VW::RotationMatrix::SetFromZXZEuler(const double theta_z1,
					 const double theta_x,
					 const double theta_z2)
{
  const double sz1 = sin(theta_z1), cz1 = cos(theta_z1);
  const double sx  = sin(theta_x),  cx  = cos(theta_x);
  const double sz2 = sin(theta_z2), cz2 = cos(theta_z2);
  data[0][0] =  cz1*cz2 -cx*sz1*sz2;   
  data[0][1] = -sz1*cz2 -cx*cz1*sz2;   
  data[0][2] =  sx*sz2; 

  data[1][0] =  cz1*sz2 +cx*sz1*cz2;   
  data[1][1] = -sz1*sz2 +cx*cz1*cz2;   
  data[1][2] = -sx*cz2;
 
  data[2][0] =  sx*sz1;   
  data[2][1] =  sx*cz1;   
  data[2][2] =  cx; 
}

/**
 * Repeatedly multiplying rotations together can lead to round-off error
 * which causes the rotation to become invalid. This function uses the
 * SVD to force orthonormality, giving the closest valid rotation matrix
 * in a Mahalanobis distance sense. Call it every now and then if you're 
 * worried about errors accumulating.
 */
void VW::RotationMatrix::Normalise()
{
 // Doing SVD and setting all singular values to 1 gives the
  // rotation matrix closest to M in the Mahalanobis distance sense.
  VNL::SVD<double> svd(*this);

    // Only update if the SVD was successful (sometimes fails to converge
    // if already within precision of solution)
    if (!svd.Valid()) {
        cerr << "VW::RotationMatrix::Normalise(): SVD failed." << endl;
    } else {
      *this = svd.U() * svd.V().Transpose();
    }
}

double VW::RotationMatrix::Angle() const
{
  double angle = 0;

  double d0 = data[0][0], d1 = data[1][1], d2 = data[2][2];

  // The trace determines the method of decomposition
  double rr = 1.0 + d0 + d1 + d2;

  if (rr>0) {
     angle = acos ( sqrt(rr) );
  } else {
    // Trace is less than zero, so need to determine which
    // major diagonal is largest
    if ((d0 > d1) && (d0 > d2)) {
      angle = 2 * acos ( 0.5 * (data[1][2] + data[2][1]) / sqrt(1 + d0 - d1 - d2));
    } else if (d1 > d2) {
      angle = 2 * acos ( 0.5 * (data[0][2] + data[2][0]) / sqrt(1 + d0 - d1 - d2));
    } else {
      angle = 2 * acos ( 0.5 * (data[0][1] + data[1][0]) / sqrt(1 + d0 - d1 - d2) );
    }
  }
  return angle;  
}

VW::Vector3D VW::RotationMatrix::Axis() const
{
  VW::Vector3D axis(0,0,0);

  double d0 = data[0][0], d1 = data[1][1], d2 = data[2][2];

  // The trace determines the method of decomposition
  double rr = 1.0 + d0 + d1 + d2;

  if (rr>0) {
    axis.SetX( data[2][1] - data[1][2] );
    axis.SetY( data[0][2] - data[2][0] );
    axis.SetZ( data[1][0] - data[0][1] );
  } else {
    // Trace is less than zero, so need to determine which
    // major diagonal is largest
    if ((d0 > d1) && (d0 > d2)) {
      axis.SetX( 0.5 );
      axis.SetY( data[0][1] + data[1][0] );
      axis.SetZ( data[0][2] + data[2][0] );
    } else if (d1 > d2) {
      axis.SetX( data[0][1] + data[1][0] );
      axis.SetY( 0.5 );
      axis.SetZ( data[1][2] + data[2][1] );
    } else {
      axis.SetX( data[0][2] + data[2][0] );
      axis.SetY( data[1][2] + data[2][1] );
      axis.SetZ( 0.5 );
    }
  }
  axis.Normalise();

  return (axis);
}

 
VW::Vector3D 
VW::RotationMatrix::Rotate(const VW::Vector3D& v) const
{
  return 
    VW::Vector3D(data[0][0]*v.GetX()+data[0][1]*v.GetY()+data[0][2]*v.GetZ(),
		 data[1][0]*v.GetX()+data[1][1]*v.GetY()+data[1][2]*v.GetZ(),
		 data[2][0]*v.GetX()+data[2][1]*v.GetY()+data[2][2]*v.GetZ());
		      
}

VW::Vector3D 
VW::RotationMatrix::InverseRotate(const VW::Vector3D& v) const
{
  return 
    VW::Vector3D(data[0][0]*v.GetX()+data[1][0]*v.GetY()+data[2][0]*v.GetZ(),
		 data[0][1]*v.GetX()+data[1][1]*v.GetY()+data[2][1]*v.GetZ(),
		 data[0][2]*v.GetX()+data[1][2]*v.GetY()+data[2][2]*v.GetZ());
		      
}

bool VW::RotationMatrix::WriteBinaryFile(const std::string& filename) const
{
    std::ofstream file(filename.c_str());
    if (!file) {
        return false;
    } else {
        WriteBinary(file);
        file.close();
    }
    return true;
}


bool VW::RotationMatrix::WriteBinary(std::ostream& stream) const
{
    // Faster to write the block at once?
        VW::WriteBinary(stream,data[0][0]);
        VW::WriteBinary(stream,data[0][1]);
        VW::WriteBinary(stream,data[0][2]);
        VW::WriteBinary(stream,data[1][0]);
        VW::WriteBinary(stream,data[1][1]);
        VW::WriteBinary(stream,data[1][2]);
        VW::WriteBinary(stream,data[2][0]);
        VW::WriteBinary(stream,data[2][1]);
        VW::WriteBinary(stream,data[2][2]);
    return true;
}
