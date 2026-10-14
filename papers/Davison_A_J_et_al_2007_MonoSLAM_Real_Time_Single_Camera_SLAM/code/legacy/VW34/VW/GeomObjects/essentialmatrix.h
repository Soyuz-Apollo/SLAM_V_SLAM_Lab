/** 
 * @file   essentialmatrix.h
 * @author BJ Tordoff
 * @date   19/04/2005		   		
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
 * @class   VW::EssentialMatrix
 * @ingroup GeometricObjects
 * @author  BJ Tordoff
 * @date    19/04/2005		   		
 * @brief   A class to represent an essential matrix.
 *
 * EssentialMatrix is a three-by-three degenerate matrix.  
 * The class includes functions
 * for calculating epipolar lines from points, and enforcing constriants.
 */

#ifndef _essentialmatrix_h_
#define _essentialmatrix_h_

#include <iostream>
#include <VNL/matrixfixed.h>
#include <VW/MathBits/covariance3d.h>

// Local includes
#include "transform.h"
#include "point2d.h"
#include "line2d.h"
#include "lineseg2d.h"
#include "rotationmatrix.h"
#include "point3d.h"

namespace VW {

  

  class EssentialMatrix : public Transform, public VNL::MatrixFixed<3,3,double>
  {
  public:
    EssentialMatrix () : VNL::MatrixFixed<3,3,double> (0.0) {};
    EssentialMatrix (const VNL::MatrixFixed<3,3,double>& M) :
	VNL::MatrixFixed<3,3,double> (M) {};
    
    EssentialMatrix &operator= (const VNL::MatrixFixed<3,3,double>& M);
    EssentialMatrix &operator= (const VNL::Matrix<double>& M);
    
    // Transferring a point to its epipolar line l = Ex
    inline VW::Line2D Transfer(const VW::Point2D& point) const;

    // Transferring a point to its epipolar line l = E'x
    inline VW::Line2D ReverseTransfer(const VW::Point2D& point) const;


    // Epipoles
    void GetEpipoles(VW::Point2D& e1, VW::Point2D& e2) const;


    /** Enforce constraints. */
    void Constrain ();
    void ConstrainOld();

    /** Recovery the rotation and translation. */
    void Decompose(VW::RotationMatrix& R,
		   VW::Vector3D& t) const;

    // Get as a matrix
    VNL::MatrixFixed<3,3,double> AsMatrix () const 
      {
	return static_cast<VNL::MatrixFixed<3,3,double> > (*this);
      }
    
    std::ostream& operator<< (std::ostream& stream) const;
  };
  





// Inline

  inline EssentialMatrix &EssentialMatrix::operator=(const VNL::MatrixFixed<3,3,double>& M)
  {
	 data[0][0]=M(0,0);data[0][1]=M(0,1);data[0][2]=M(0,2);
	 data[1][0]=M(1,0);data[1][1]=M(1,1);data[1][2]=M(1,2);
	 data[2][0]=M(2,0);data[2][1]=M(2,1);data[2][2]=M(2,2);
	 return(*this);
  }

  inline EssentialMatrix &EssentialMatrix::operator=(const VNL::Matrix<double>& M)
  {
	 assert (M.Rows() == 3 && M.Columns() == 3);
	 
	 data[0][0]=M(0,0);data[0][1]=M(0,1);data[0][2]=M(0,2);
	 data[1][0]=M(1,0);data[1][1]=M(1,1);data[1][2]=M(1,2);
	 data[2][0]=M(2,0);data[2][1]=M(2,1);data[2][2]=M(2,2);
	 return(*this);
  }

inline VW::Line2D 
EssentialMatrix::Transfer(const VW::Point2D& point) const
{
  // Transfer it: l' = Fx
  const double a = data[0][0]*point.GetX()+data[0][1]*point.GetY()+data[0][2];
  const double b = data[1][0]*point.GetX()+data[1][1]*point.GetY()+data[1][2];
  const double c = data[2][0]*point.GetX()+data[2][1]*point.GetY()+data[2][2];

  return VW::Line2D(a,b,c);
}

inline VW::Line2D 
EssentialMatrix::ReverseTransfer(const VW::Point2D& point) const
{
  // Transfer it: l' = F'x
  const double a = data[0][0]*point.GetX()+data[1][0]*point.GetY()+data[2][0];
  const double b = data[0][1]*point.GetX()+data[1][1]*point.GetY()+data[2][1];
  const double c = data[0][2]*point.GetX()+data[1][2]*point.GetY()+data[2][2];

  return VW::Line2D(a,b,c);
}



}; // end namespace VW;

#endif // #ifndef _fmatrix_h_
