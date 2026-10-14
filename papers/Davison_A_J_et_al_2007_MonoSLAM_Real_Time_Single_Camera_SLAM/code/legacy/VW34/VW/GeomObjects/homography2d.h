/** 
 * @file   homography2d.h
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
 * @class   VW::Homography2D
 * @ingroup GeometricObjects
 * @author  JGH Knight
 * @date    02/08/02
 * @brief   A class to represent a planar homography.
 *
 * The homography class is simply a 3x3 matrix which has the additional
 * functionality required to transfer Point2D's and Line2D's between images.
 * For computation routines, see VW::Homography2DCompute.
 */


#ifndef _homography_h_
#define _homography_h_
/* Homography2D
 *
 * Image to image 3x3 homography
 */

#include <iostream>

#include <VNL/matrixfixed.h>
#include <VNL/vectorfixed.h>
#include <VW/MathBits/covariance3d.h>
//#include <VNL/matrixtranspose.h>
#include "transform.h"
#include "point2d.h"
#include "line2d.h"
#include "lineseg2d.h"

namespace VW {

class Homography2D : public Transform, public VNL::MatrixFixed<3,3,double> {
 public:
  static const unsigned int MIN_POINTSET_SIZE = 4;



  static const unsigned int MIN_LINESET_SIZE = 4;
  static const unsigned int NUM_VIEWS = 2;
  static const char *const NAME;

  Homography2D () {SetIdentity();}
  Homography2D (const VNL::MatrixFixed<3,3,double>& M) :
    VNL::MatrixFixed<3,3,double> (M) {};

  inline VW::Homography2D &operator= (const VNL::MatrixFixed<3,3,double> m);
  inline VW::Homography2D &operator= (const VNL::Matrix<double> m);


  Homography2D Inverse() const;

  /** Return the squared transfer error e^2 = |dest_point - H*source_point|^2 
   */
  inline double GetTransferError2(const VW::Point2D& source_point,
				  const VW::Point2D& dest_point) const;
				 

  /** Transfer a 2d point as \f$x_1 = H x_0 \f$
   * The homogeneous part is assumed unity. The template allows 
   * stuff derrived from Point2D to also be transferred using the 
   * same code (ie. PlesseyCorners).
   */
  template <class ElementType>
  ElementType TransferPoint(const ElementType& feature) const;

  /** Transfer a 2d line segment (by transferring the two endpoints)
   * The template allows 
   * stuff derrived from LineSeg2D to also be transferred using the 
   * same code.
   */
  template <class ElementType>
  ElementType TransferLineSeg(const ElementType& feature) const;

  /** Transfer a 2d line as \f$l_1 = H^T l_0 \f$
   * The template allows 
   * stuff derrived from Line2D to also be transferred using the 
   * same code.
   */
  template <class ElementType>
  ElementType TransferLine(const ElementType& feature) const;


  // Get as a matrix
  VNL::MatrixFixed<3,3,double> AsMatrix () const {
    return static_cast<VNL::MatrixFixed<3,3,double> > (*this);
  }

  template <class F>
    unsigned int GetMinFeatures (const F& f) 
    {
      assert (false);
      return 0;
    }
  unsigned int GetNumViews () {return NUM_VIEWS;}
  std::ostream& operator<< (std::ostream& stream) const;
};



class Homography2DCovariance : public VNL::MatrixFixed<9,9,double> 
{
public:
  Homography2DCovariance(double fill_with_value = 0.0);
  Homography2DCovariance(const VNL::Matrix<double>& m);
  Homography2DCovariance(const VNL::MatrixFixed<9,9,double>& m);
  Homography2DCovariance& operator=(const VNL::MatrixFixed<9,9,double>& m);
  Homography2DCovariance& operator=(const VNL::Matrix<double>& m);

  /** The uncertainty of the point in image 2 due to the covariance
   * of the homography depends only on the point in image 1.
   * 72 flops 
   */
  VW::Covariance3D TransferUncertainty(const VW::Point2D& source_point) const;

  /** Ensure that this matrix is symmetric */
  void ForceSymmetry();

private:
};

  class Covariance2D;

  /** Transfer a point and both its uncertainty and that of the
   * tranform (note that the total uncertainty requires you to add
   * any noise in image 2).
   */
  void TransferPoint(const VW::Point2D& source_point,
		     const VW::Homography2D& H,
		     const VW::Homography2DCovariance& HCovar,
		     const float image_noise_variance,
		     VW::Point2D& dest_point,
		     VW::Covariance2D& dest_covar);








// Inline
inline double Homography2D::
GetTransferError2(const VW::Point2D& source_point,
		 const VW::Point2D& dest_point) const
{
  const double x = source_point.GetX(), y = source_point.GetY();
  const double tz = 1.0 / (data[2][0]*x + data[2][1]*y + data[2][2]);
  const double ex = dest_point.GetX() - tz*(data[0][0]*x + data[0][1]*y + data[0][2]); 
  const double ey = dest_point.GetY() - tz*(data[1][0]*x + data[1][1]*y + data[1][2]); 

  return ex*ex + ey*ey;
}

template <class ElementType> inline ElementType 
Homography2D::TransferPoint(const ElementType& feature) const
{
  // Test whether this feature can actually be cast to a Point2D
  assert(dynamic_cast<const VW::Point2D*>(&feature));

  ElementType result = feature;

  // Transfer it: x2 = H * x1
  const double x = feature.GetX();
  const double y = feature.GetY();

  const double newx = data[0][0]*x + data[0][1]*y + data[0][2];
  const double newy = data[1][0]*x + data[1][1]*y + data[1][2];
  const double newz = data[2][0]*x + data[2][1]*y + data[2][2];

  result.SetX(newx/newz);
  result.SetY(newy/newz);

  return (result);
}

template <class ElementType> inline ElementType 
Homography2D::TransferLineSeg(const ElementType& feature) const
{
  // Test whether this feature can actually be cast to a LineSeg2D
  assert(dynamic_cast<const VW::LineSeg2D*>(&feature));

  VW::Point2D orig0,orig1;
  VW::Point2D trans0,trans1;

  feature.GetEndPoints(orig0, orig1);
  trans0 = TransferLineSeg(orig0);
  trans1 = TransferLineSeg(orig1);

  return (VW::LineSeg2D(trans0,trans1));
}

template <class ElementType> inline ElementType 
Homography2D::TransferLine(const ElementType& feature) const
{
  // Test whether this feature can actually be cast to a Point2D
  assert(dynamic_cast<const VW::Line2D*>(&feature));

  VNL::MatrixFixed<3,3,double> transpose = (static_cast<VNL::Matrix<double> >(*this)).Transpose();

  // Transfer it: l2 = H' * l1
  return (VW::Line2D(transpose * feature));
}


// Specialisations of GetMinFeatures<F>() for different features
// Note the parameter for GetMinFeatures<F>() is a dummy argument
// So that other templated functions will compile (because
// GetMinFeatures() can then be called without template parameter
// because it is determined by the type of its function parameter).

template <> inline unsigned int 
Homography2D::GetMinFeatures<VW::Point2D> (const VW::Point2D&)
{
  return MIN_POINTSET_SIZE;
}

template <> inline unsigned int 
Homography2D::GetMinFeatures<VW::LineSeg2D> (const VW::LineSeg2D&)
{
  return MIN_LINESET_SIZE;
}

// ...perhaps there will be others for mixtures of lines and points...

// Display
inline std::ostream& 
Homography2D::operator<< (std::ostream& stream) const
{
  return stream << static_cast<const VNL::MatrixFixed<3,3,double>&> (*this);
}

inline VW::Homography2D&
Homography2D::operator= (const VNL::MatrixFixed<3,3,double> m)
{
  data[0][0]=m(0,0);data[0][1]=m(0,1);data[0][2]=m(0,2);
  data[1][0]=m(1,0);data[1][1]=m(1,1);data[1][2]=m(1,2);
  data[2][0]=m(2,0);data[2][1]=m(2,1);data[2][2]=m(2,2);
  return(*this);
}


inline VW::Homography2D &
Homography2D::operator= (const VNL::Matrix<double> m)
{
  assert (m.Rows() == 3 && m.Columns() == 3);

  data[0][0]=m(0,0);data[0][1]=m(0,1);data[0][2]=m(0,2);
  data[1][0]=m(1,0);data[1][1]=m(1,1);data[1][2]=m(1,2);
  data[2][0]=m(2,0);data[2][1]=m(2,1);data[2][2]=m(2,2);
  return(*this);
}

inline 
Homography2DCovariance::Homography2DCovariance(double fill_with_value)
  : VNL::MatrixFixed<9,9,double>(fill_with_value)
{
}

  inline 
Homography2DCovariance::
Homography2DCovariance(const VNL::Matrix<double>& m)
  : VNL::MatrixFixed<9,9,double>(m)
{
  ForceSymmetry();
}

inline Homography2DCovariance::
Homography2DCovariance(const VNL::MatrixFixed<9,9,double>& m)
  : VNL::MatrixFixed<9,9,double>(m)
{
  ForceSymmetry();
}

inline VW::Homography2DCovariance&
Homography2DCovariance::operator=(const VNL::Matrix<double>& m)
{
  assert((m.Rows()==9) && m.Cols()==9);
  memcpy(this->DataBlock(), m.DataBlock(), 81*sizeof(data[0][0]));
  return(*this);
}

inline VW::Homography2DCovariance&
Homography2DCovariance::operator=(const VNL::MatrixFixed<9,9,double>& m)
{
  memcpy(this->DataBlock(), m.DataBlock(), 81*sizeof(data[0][0]));
  return(*this);
}

inline void Homography2DCovariance::
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
Homography2DCovariance::TransferUncertainty(const VW::Point2D& source_point) const
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

#endif
