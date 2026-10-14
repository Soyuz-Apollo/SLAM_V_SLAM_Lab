/** 
 * @file   fmatrix.h
 * @author JGH Knight
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
 * @class   VW::FMatrix
 * @ingroup GeometricObjects
 * @author  JGH Knight
 * @date    02/08/2002
 * @brief   A class to represent a fundamental matrix.
 *
 * FMatrix is a three-by-three degenerate matrix.  The class includes functions
 * for calculating epipolar lines from points, and for calculating epipoles
 */

#ifndef _fmatrix_h_
#define _fmatrix_h_

#include <iostream>
#include <VNL/matrixfixed.h>
#include <VW/MathBits/covariance3d.h>

// Local includes
#include "transform.h"
#include "point2d.h"
#include "line2d.h"
#include "lineseg2d.h"

namespace VW {

  

  class FMatrix : public Transform, public VNL::MatrixFixed<3,3,double>
  {
    // Some public constants
  public:
    static const unsigned int MIN_POINTSET_SIZE = 7;
    static const unsigned int MIN_LINESET_SIZE = 0;  // FMatrix cannot be calculated with lines
    static const unsigned int NUM_VIEWS = 2;
    //  static const char * const NAME = "FMatrix";
    static const char * const NAME;
    
    
    
  public:
    inline FMatrix () : VNL::MatrixFixed<3,3,double> (0.0) {};
    FMatrix (const VNL::MatrixFixed<3,3,double>& M) :
      VNL::MatrixFixed<3,3,double> (M) {};
    
    inline FMatrix &operator= (const VNL::MatrixFixed<3,3,double>& M);
    inline FMatrix &operator= (const VNL::Matrix<double>& M);
    
    // Transferring a point to its epipolar line l = Fx
    inline VW::Line2D Transfer(const VW::Point2D& point) const;

    // Transferring a point to its epipolar line l = F'x
    inline VW::Line2D ReverseTransfer(const VW::Point2D& point) const;


    template <class ElementType>
    inline void TransferPoint (const ElementType& point, VW::Line2D& eline) const;
    template <class ElementType>
    inline void ReverseTransferPoint (const ElementType& point, VW::Line2D& eline) const;
    // Epipoles
    void GetEpipoles (VW::Point2D& e1, VW::Point2D& e2) const;
    // Singularity Condition constraint.  Must be called explicitly (in case you don't want it)
    void Constrain ();
    // Get as a matrix
    VNL::MatrixFixed<3,3,double> AsMatrix () const 
      {
	return static_cast<VNL::MatrixFixed<3,3,double> > (*this);
      }
    
    template <class F>
    unsigned int GetMinFeatures (const F& f) 
      {
	assert (false);
	return 0;
      }

    unsigned int GetNumViews () 
      {
	return NUM_VIEWS;
      }

    std::ostream& operator<< (std::ostream& stream) const;
  };
  

class FMatrixCovariance : public VNL::MatrixFixed<9,9,double> 
{
public:
  FMatrixCovariance(double fill_with_value = 0.0);
  FMatrixCovariance(const VNL::Matrix<double>& m);
  FMatrixCovariance(const VNL::MatrixFixed<9,9,double>& m);
  FMatrixCovariance& operator=(const VNL::MatrixFixed<9,9,double>& m);
  FMatrixCovariance& operator=(const VNL::Matrix<double>& m);

  /** The uncertainty of the epipolar line in image 2 due to the covariance
   * of the FMatrix depends only on the point in image 1.
   * 72 flops 
   */
  VW::Covariance3D TransferUncertainty(const VW::Point2D& source_point) const;

  /** Ensure that this matrix is symmetric (72 flops) */
  void ForceSymmetry();

private:
};

  /** Transfer a point and its covariance using an uncertain estimate of
   * an FMatrix. The result is a (normalised) line and its covariance.
   */
  void TransferPoint(const VW::Point2D& source_point,
		     const VW::FMatrix& F,
		     const VW::FMatrixCovariance& FCovar,
		     const float image_noise_variance,
		     VW::Line2D& dest_line,
		     VW::Covariance3D& dest_line_covar);

  /** Find the variance of the distance between point and line given the line
   * uncertainty and image noise (in image 2). Note that the line and its
   * covariance must be normalised (using VW::NormaliseLine).
   */
  double GetDistanceVariance(const VW::Point2D& dest_point,
			     const VW::Line2D& epipolar_line,
			     const VW::Covariance3D& epipolar_line_covar,
			     const float image_noise_variance);

		     






// Inline

  inline FMatrix &FMatrix::operator=(const VNL::MatrixFixed<3,3,double>& M)
  {
	 data[0][0]=M(0,0);data[0][1]=M(0,1);data[0][2]=M(0,2);
	 data[1][0]=M(1,0);data[1][1]=M(1,1);data[1][2]=M(1,2);
	 data[2][0]=M(2,0);data[2][1]=M(2,1);data[2][2]=M(2,2);
	 return(*this);
  }

  inline FMatrix &FMatrix::operator=(const VNL::Matrix<double>& M)
  {
	 assert (M.Rows() == 3 && M.Columns() == 3);
	 
	 data[0][0]=M(0,0);data[0][1]=M(0,1);data[0][2]=M(0,2);
	 data[1][0]=M(1,0);data[1][1]=M(1,1);data[1][2]=M(1,2);
	 data[2][0]=M(2,0);data[2][1]=M(2,1);data[2][2]=M(2,2);
	 return(*this);
  }

inline VW::Line2D 
FMatrix::Transfer(const VW::Point2D& point) const
{
  // Transfer it: l' = Fx
  const double a = data[0][0]*point.GetX()+data[0][1]*point.GetY()+data[0][2];
  const double b = data[1][0]*point.GetX()+data[1][1]*point.GetY()+data[1][2];
  const double c = data[2][0]*point.GetX()+data[2][1]*point.GetY()+data[2][2];

  return VW::Line2D(a,b,c);
}

inline VW::Line2D 
FMatrix::ReverseTransfer(const VW::Point2D& point) const
{
  // Transfer it: l' = F'x
  const double a = data[0][0]*point.GetX()+data[1][0]*point.GetY()+data[2][0];
  const double b = data[0][1]*point.GetX()+data[1][1]*point.GetY()+data[2][1];
  const double c = data[0][2]*point.GetX()+data[1][2]*point.GetY()+data[2][2];

  return VW::Line2D(a,b,c);
}


template <class ElementType> inline void 
FMatrix::TransferPoint(const ElementType& point,
		       VW::Line2D& eline) const
{
  // Test whether this feature can actually be cast to a Point2D
  assert(dynamic_cast<const VW::Point2D*>(&point));

  // Transfer it: l' = Fx
  VNL::Vector<double> x(3);
  x[0] = point.GetX();
  x[1] = point.GetY();
  x[2] = 1;
  eline = VW::Line2D (AsMatrix () * x);
}


template <class ElementType> inline void 
FMatrix::ReverseTransferPoint(const ElementType& point,
					      VW::Line2D& eline) const
{
  // Test whether this feature can actually be cast to a Point2D
  assert(dynamic_cast<const VW::Point2D*>(&point));

  // Transfer it: l = Ft x'
  VNL::VectorFixed<3,double> x;
  x[0] = point.GetX();
  x[1] = point.GetY();
  x[2] = 1;
  eline = VW::Line2D (AsMatrix().Transpose () * x);
}


  /*
	*   Now stuff for its covariance
	*/


inline 
FMatrixCovariance::FMatrixCovariance(double fill_with_value)
  : VNL::MatrixFixed<9,9,double>(fill_with_value)
{
}

  inline 
FMatrixCovariance::
FMatrixCovariance(const VNL::Matrix<double>& m)
  : VNL::MatrixFixed<9,9,double>(m)
{
  ForceSymmetry();
}

inline FMatrixCovariance::
FMatrixCovariance(const VNL::MatrixFixed<9,9,double>& m)
  : VNL::MatrixFixed<9,9,double>(m)
{
  ForceSymmetry();
}

inline VW::FMatrixCovariance&
FMatrixCovariance::operator=(const VNL::Matrix<double>& m)
{
  assert((m.Rows()==9) && m.Cols()==9);
  memcpy(this->DataBlock(), m.DataBlock(), 81*sizeof(data[0][0]));
  return(*this);
}

inline VW::FMatrixCovariance&
FMatrixCovariance::operator=(const VNL::MatrixFixed<9,9,double>& m)
{
  memcpy(this->DataBlock(), m.DataBlock(), 81*sizeof(data[0][0]));
  return(*this);
}

inline void FMatrixCovariance::
ForceSymmetry()
{
  for (unsigned int row=0;row<8;row++) {
    for (unsigned int col=row+1;col<9;col++) {
      const double av = 0.5*(data[row][col] + data[col][row]);
      data[row][col] = data[col][row] = av;
    }
  }
}

inline VW::Covariance3D
FMatrixCovariance::TransferUncertainty(const VW::Point2D& source_point) const
{
  const double x = source_point.GetX();
  const double y = source_point.GetY();
  const double xx = x*x; // 3 flops
  const double yy = y*y;
  const double xy = x*y;

  // Symmetric, so just do upper triangle first
  VW::Covariance3D result;
  result[0] =      data[0][0]*xx + data[1][1]*yy + data[2][2]
            + 2.0*(data[0][1]*xy + data[0][2]*x  + data[1][2]*y); // 11
  result[1] =      data[0][3]*xx + data[1][4]*yy + data[2][5]
                   + (data[0][4]+data[1][3])*xy 
                   + (data[0][5]+data[2][3])*x 
                   + (data[1][5]+data[2][4])*y; // 13
  result[2] =      data[0][6]*xx + data[1][7]*yy + data[2][8]
                   + (data[0][7]+data[1][6])*xy 
                   + (data[0][8]+data[2][6])*x 
                   + (data[1][8]+data[2][7])*y; // 13
  result[3] =      data[3][3]*xx + data[4][4]*yy + data[5][5]
            + 2.0*(data[3][4]*xy + data[3][5]*x  + data[4][5]*y); // 11
  result[4] =      data[3][6]*xx + data[4][7]*yy + data[5][8]
                   + (data[3][7]+data[4][6])*xy 
                   + (data[3][8]+data[5][6])*x
                   + (data[4][8]+data[5][7])*y; // 13
  result[5] =      data[6][6]*xx + data[7][7]*yy + data[8][8]
            + 2.0*(data[6][7]*xy + data[6][8]*x  + data[7][8]*y); // 11

  return result;
}

}; // end namespace VW;

#endif // #ifndef _fmatrix_h_
