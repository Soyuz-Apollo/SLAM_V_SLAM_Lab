/** 
 * @file   projectionmatrix.h
 * @author BJ Tordoff
 * @date   14/08/2003		   		
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
 * @class   VW::ProjectionMatrix
 * @ingroup GeometricObjects
 * @author BJ Tordoff
 * @date   14/08/2003		   		
 * @brief   A class to represent a projection matrix.
 *
 * ProjectionMatrix is a three-by-four homogeneous matrix.  The class includes functions
 * for projecting points and line-segments, and in GeomCompute can be found functions for
 * decomposition and computation from matched points.
 */

#ifndef _projectionmatrix_h_
#define _projectionmatrix_h_

#include <iostream>
#include <VNL/matrixfixed.h>

// Local includes
#include "point2d.h"
#include "point3d.h"
#include "lineseg2d.h"
#include "lineseg3d.h"

namespace VW {

  class RotationMatrix;
  class CalibrationMatrix;

  class ProjectionMatrix : public VNL::MatrixFixed<3,4,double>
  {     
  public:
    ProjectionMatrix ();
    ProjectionMatrix (const VNL::MatrixFixed<3,4,double>& M);
    ProjectionMatrix (const VNL::Matrix<double>& M);
    ProjectionMatrix (const VW::CalibrationMatrix& K,
		      const VW::RotationMatrix& R,
		      const VW::Vector3D& t);
    
    inline ProjectionMatrix &operator= (const VNL::MatrixFixed<3,4,double>& m);
    inline ProjectionMatrix &operator= (const VNL::Matrix<double>& m);
    
    /** project a 3D point onto a 2D point. */
    VW::Point2D Project(const VW::Point3D& world_point) const;
 
     /** project a 3D line-segment onto a 2D line-segment. */
   VW::LineSeg2D Project(const VW::LineSeg3D& world_line) const;
 
				// Get as a matrix
    VNL::MatrixFixed<3,4,double> AsMatrix () const {
      return static_cast<VNL::MatrixFixed<3,4,double> > (*this);
    }
    
     std::ostream& operator<< (std::ostream& stream) const;

    void Compose(const VW::CalibrationMatrix& K,
		 const VW::RotationMatrix& R,
		 const VW::Vector3D& t);

    /** @returns true for success, false if something went wrong. */
    bool Decompose(VW::CalibrationMatrix& result_K,
		   VW::RotationMatrix& result_R,
		   VW::Vector3D& result_t) const;

  };
  


}; // end namespace VW;

// Inline

inline 
VW::ProjectionMatrix::ProjectionMatrix ()
  : VNL::MatrixFixed<3,4,double> (0.0) 
{
};

inline 
VW::ProjectionMatrix::ProjectionMatrix (const VNL::MatrixFixed<3,4,double>& M)
  : VNL::MatrixFixed<3,4,double> (M) 
{
};

inline 
VW::ProjectionMatrix::ProjectionMatrix (const VNL::Matrix<double>& M)
{
  assert (M.Rows() == 3 && M.Columns() == 4);

  data[0][0]=M(0,0);data[0][1]=M(0,1);data[0][2]=M(0,2);data[0][3]=M(0,3);
  data[1][0]=M(1,0);data[1][1]=M(1,1);data[1][2]=M(1,2);data[1][3]=M(1,3);
  data[2][0]=M(2,0);data[2][1]=M(2,1);data[2][2]=M(2,2);data[2][3]=M(2,3);
};

inline 
VW::ProjectionMatrix::ProjectionMatrix (const VW::CalibrationMatrix& K,
					const VW::RotationMatrix& R,
					const VW::Vector3D& t)
{
  Compose(K,R,t);
}


inline VW::Point2D 
VW::ProjectionMatrix::Project(const VW::Point3D& world_point) const
{
const double x = data[0][0]*world_point.GetX() + data[0][1]*world_point.GetY() + data[0][2]*world_point.GetZ() + data[0][3];
const double y = data[1][0]*world_point.GetX() + data[1][1]*world_point.GetY() + data[1][2]*world_point.GetZ() + data[1][3];
const double z = data[2][0]*world_point.GetX() + data[2][1]*world_point.GetY() + data[2][2]*world_point.GetZ() + data[2][3];
	return VW::Point2D(x/z, y/z);
}

inline VW::LineSeg2D 
VW::ProjectionMatrix::Project(const VW::LineSeg3D& world_line) const
{
	return VW::LineSeg2D(this->Project(world_line.GetStartPoint()),
											this->Project(world_line.GetEndPoint()));
}
 
inline VW::ProjectionMatrix &
VW::ProjectionMatrix::operator= (const VNL::MatrixFixed<3,4,double>& m)
{
  data[0][0]=m(0,0);data[0][1]=m(0,1);data[0][2]=m(0,2);data[0][3]=m(0,3);
  data[1][0]=m(1,0);data[1][1]=m(1,1);data[1][2]=m(1,2);data[1][3]=m(1,3);
  data[2][0]=m(2,0);data[2][1]=m(2,1);data[2][2]=m(2,2);data[2][3]=m(2,3);
  return(*this);
}

inline VW::ProjectionMatrix &
VW::ProjectionMatrix::operator= (const VNL::Matrix<double>& m)
{
  assert (m.Rows() == 3 && m.Columns() == 4);

  data[0][0]=m(0,0);data[0][1]=m(0,1);data[0][2]=m(0,2);data[0][3]=m(0,3);
  data[1][0]=m(1,0);data[1][1]=m(1,1);data[1][2]=m(1,2);data[1][3]=m(1,3);
  data[2][0]=m(2,0);data[2][1]=m(2,1);data[2][2]=m(2,2);data[2][3]=m(2,3);
  return(*this);
}


#endif // #ifndef _projectionmatrix_h_
