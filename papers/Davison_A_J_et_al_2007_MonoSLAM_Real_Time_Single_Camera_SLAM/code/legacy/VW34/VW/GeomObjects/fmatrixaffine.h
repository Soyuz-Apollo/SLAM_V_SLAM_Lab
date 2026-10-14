/** 
 * @file   fmatrixaffine.h
 * @author BJ Tordoff
 * @date   08/04/2005		   		
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
 * @class   VW::FMatrixAffine
 * @ingroup GeometricObjects
 * @author BJ Tordoff
 * @date   08/04/2005		   		
 * @brief   A class to represent an affine fundamental matrix.
 *
 * FMatrixAffine is a three-by-three degenerate matrix stored as the five non-zero
 * elements Fa = [ 0 0 a ]
 *               [ 0 0 b ]
 *               [ c d e ]
 *
 * It is automatically rank 2, so no extra constraints are required.
 * The class includes functions for calculating epipolar lines from points, 
 * and for calculating epipoles.
 */

#ifndef _fmatrixaffine_h_
#define _fmatrixaffine_h_

#include <iostream>
#include <VNL/matrix.h>
#include <VNL/matrixfixed.h>
#include <VW/MathBits/covariance3d.h>

// Local includes
#include "transform.h"
#include "point2d.h"
#include "line2d.h"

namespace VW {

  

  class FMatrixAffine {
  public:
    FMatrixAffine();
    FMatrixAffine(double a, double b, double c, double d, double e);
    FMatrixAffine(const VNL::MatrixFixed<3,3,double>& M);
    
    FMatrixAffine &operator= (const VNL::MatrixFixed<3,3,double>& M);
    FMatrixAffine &operator= (const VNL::Matrix<double>& M);
    
    void Set(double a, double b, double c, double d, double e);

    // Transferring a point to its epipolar line l = Fx
    VW::Line2D Transfer(const VW::Point2D& point) const;

    // Transferring a point to its epipolar line l = F'x
    VW::Line2D ReverseTransfer(const VW::Point2D& point) const;


    // Epipole directions (epipoles are at infinity by definition)
    void GetEpipoleDirections(VW::Vector2D& e1, VW::Vector2D& e2) const;

	 FMatrixAffine Transpose() const;

    // Get as a matrix
    VNL::MatrixFixed<3,3,double> AsMatrix() const;

    VNL::MatrixFixed<3,3,double> TransposeMatrix() const;

	 double GetA() const;
	 double GetB() const;
	 double GetC() const;
	 double GetD() const;
	 double GetE() const;


    std::ostream& operator<< (std::ostream& stream) const;

  protected: // internal data
	 double _a, _b, _c, _d, _e;

  };
  





class FMatrixAffineCovariance : public VNL::MatrixFixed<5,5,double> 
{
public:
  FMatrixAffineCovariance(double fill_with_value = 0.0);
  FMatrixAffineCovariance(const VNL::Matrix<double>& m);
  FMatrixAffineCovariance(const VNL::MatrixFixed<5,5,double>& m);
  FMatrixAffineCovariance& operator=(const VNL::MatrixFixed<5,5,double>& m);
  FMatrixAffineCovariance& operator=(const VNL::Matrix<double>& m);

  /** The uncertainty of the epipolar line in image 2 due to the covariance
   * of the FMatrix depends only on the point in image 1.
   * 22 flops 
   */
  VW::Covariance3D TransferUncertainty(const VW::Point2D& source_point) const;

  /** Ensure that this matrix is symmetric (20 flops) */
  void ForceSymmetry();

private:
};

  /** Transfer a point and its covariance using an uncertain estimate of
   * an FMatrix. The result is a (normalised) line and its covariance.
   */
  void TransferPoint(const VW::Point2D& source_point,
							const VW::FMatrixAffine& F,
							const VW::FMatrixAffineCovariance& FCovar,
							const float image_noise_variance,
							VW::Line2D& dest_line,
							VW::Covariance3D& dest_line_covar);




}; // end namespace VW;


		     






// Inline
inline VW::FMatrixAffine::FMatrixAffine()
  : _a(0.0), _b(0.0), _c(0.0), _d(0.0), _e(0.0)
{
}

inline VW::FMatrixAffine::FMatrixAffine(double a, double b, double c, double d, double e)
  : _a(a), _b(b), _c(c), _d(d), _e(e)
{
}

inline VW::FMatrixAffine::FMatrixAffine(const VNL::MatrixFixed<3,3,double>& M)
  : _a(M[0][2]), _b(M[1][2]), _c(M[2][0]), _d(M[2][1]), _e(M[2][2])
{
  assert((M[0][0]==0.0) && (M[0][1]==0.0) && (M[1][0]==0.0) && (M[1][1]==0.0));
}



inline VW::FMatrixAffine& 
VW::FMatrixAffine::operator=(const VNL::Matrix<double>& M)
{
  assert((M.Cols()==3) && (M.Rows()==3));
  assert((M[0][0]==0.0) && (M[0][1]==0.0) && (M[1][0]==0.0) && (M[1][1]==0.0));

  _a = M[0][2]; _b = M[1][2];
  _c = M[2][0]; _d = M[2][1];
  _e = M[2][2];

  return *this;
}
  
inline VW::FMatrixAffine& 
VW::FMatrixAffine::operator=(const VNL::MatrixFixed<3,3,double>& M)
{
  assert((M[0][0]==0.0) && (M[0][1]==0.0) && (M[1][0]==0.0) && (M[1][1]==0.0));

  _a = M[0][2]; _b = M[1][2];
  _c = M[2][0]; _d = M[2][1];
  _e = M[2][2];

  return *this;
}
  
inline void 
VW::FMatrixAffine::Set(double a, double b, double c, double d, double e)
{
  _a = a; _b = b;
  _c = c; _d = d;
  _e = e;
}

inline void 
VW::FMatrixAffine::GetEpipoleDirections(VW::Vector2D& e1, VW::Vector2D& e2) const
{
  e1.Set(_d, -_c);
  e2.Set(_b, -_a);
}

inline VW::FMatrixAffine 
VW::FMatrixAffine::Transpose() const
{
  return VW::FMatrixAffine(_c,_d,_a,_b,_e);
}

inline VNL::MatrixFixed<3,3,double> 
  VW::FMatrixAffine::AsMatrix() const
{
  VNL::MatrixFixed<3,3,double> M(0.0);
  M[0][2]=_a;
  M[1][2]=_b;
  M[2][0]=_c;
  M[2][1]=_d;
  M[2][2]=_e;
  return M;
}

inline VNL::MatrixFixed<3,3,double> 
VW::FMatrixAffine::TransposeMatrix() const
{
  VNL::MatrixFixed<3,3,double> M(0.0);
  M[0][2]=_c;
  M[1][2]=_d;
  M[2][0]=_a;
  M[2][1]=_b;
  M[2][2]=_e;
  return M;
}

inline double 
VW::FMatrixAffine::GetA() const
{
  return _a;
}

inline double 
VW::FMatrixAffine::GetB() const
{
  return _b;
}

inline double 
VW::FMatrixAffine::GetC() const
{
  return _c;
}

inline double 
VW::FMatrixAffine::GetD() const
{
  return _d;
}

inline double 
VW::FMatrixAffine::GetE() const
{
  return _e;
}

inline std::ostream& 
VW::FMatrixAffine::operator<< (std::ostream& stream) const
{
  stream << "0 0 " << _a << std::endl;
  stream << "0 0 " << _b << std::endl;
  stream << _c << " " << _d << " " << _e << std::endl;
  return stream;
}



/*
 *   Now stuff for its covariance
 */


inline 
VW::FMatrixAffineCovariance::FMatrixAffineCovariance(double fill_with_value)
  : VNL::MatrixFixed<5,5,double>(fill_with_value)
{
}

inline 
VW::FMatrixAffineCovariance::
FMatrixAffineCovariance(const VNL::Matrix<double>& m)
  : VNL::MatrixFixed<5,5,double>(m)
{
  ForceSymmetry();
}

inline VW::FMatrixAffineCovariance::
FMatrixAffineCovariance(const VNL::MatrixFixed<5,5,double>& m)
  : VNL::MatrixFixed<5,5,double>(m)
{
  ForceSymmetry();
}

inline VW::FMatrixAffineCovariance&
VW::FMatrixAffineCovariance::operator=(const VNL::Matrix<double>& m)
{
  assert((m.Rows()==5) && m.Cols()==5);
  memcpy(this->DataBlock(), m.DataBlock(), 25*sizeof(data[0][0]));
  return(*this);
}

inline VW::FMatrixAffineCovariance&
VW::FMatrixAffineCovariance::operator=(const VNL::MatrixFixed<5,5,double>& m)
{
  memcpy(this->DataBlock(), m.DataBlock(), 25*sizeof(data[0][0]));
  return(*this);
}

inline void VW::FMatrixAffineCovariance::ForceSymmetry()
{
  for (unsigned int row=0;row<4;row++) {
    for (unsigned int col=row+1;col<5;col++) {
      const double av = 0.5*(data[row][col] + data[col][row]);
      data[row][col] = data[col][row] = av;
    }
  }
}

inline VW::Covariance3D
VW::FMatrixAffineCovariance::TransferUncertainty(const VW::Point2D& source_point) const
{
  const double x = source_point.GetX();
  const double y = source_point.GetY();
  const double xx = x*x; // 3 flops
  const double yy = y*y;
  const double xy = x*y;

  // Symmetric, so just do upper triangle first (19 flops)
  VW::Covariance3D result;
  result[0] =      data[0][0];
  result[1] =      data[0][1];
  result[2] =      data[0][2]*x + data[0][3]*y + data[0][4]; // 4
  result[3] =      data[1][1];
  result[4] =      data[1][2]*x + data[1][3]*y + data[1][4]; // 4
  result[5] =      data[2][2]*xx + data[3][3]*yy + data[4][4]
            + 2.0*(data[2][3]*xy + data[2][4]*x  + data[3][4]*y); // 11

  return result;
}

#endif // #ifndef _fmatrixaffine_h_
