/** 
 * @file   fmaffinecompute.h
 * @author BJ Tordoff
 * @date   17/04/2005		   		
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
 * @namespace VW::FMatrixAffineCompute
 * @ingroup   GeometricAlgorithms
 * @author    BJ Tordoff
 * @date      17/04/2005
 * @brief     Assorted helper functions for computing affine Fundamental Matrices.
 */

#ifndef _fmaffinecompute_h_
#define _fmaffinecompute_h_

#include <VNL/vectorfixed.h>
#include <VNL/matrixfixed.h>
#include <VNL/Algo/symmetriceigensystem.h>

#include <VW/GeomObjects/normalisation.h>
#include <VW/GeomObjects/fmatrixaffine.h>


typedef VNL::VectorFixed<5,double> D5Vector;

namespace VW {
  namespace FMatrixAffineCompute {

	 /** Unnormalise an FMatrixAffine
	  * ie. F = n1'*Fn*n0 
	  * (hand optimised to 12 flops - matrix mult would be 90) */
	 VW::FMatrixAffine Unnormalise(const VW::FMatrixAffine& F,
											const Normalisation& n0,
											const Normalisation& n1);

	 /** Unnormalise a 5-vector which represents the non-zero elements of an affine fmatrix 
	  * ie. F = n1'*Fn*n0 */
	 D5Vector UnnormaliseFVec(const D5Vector& f,
									  const Normalisation& n0,
									  const Normalisation& n1);

	 /** Unnormalise a covariance */
	 VW::FMatrixAffineCovariance Unnormalise(const VW::FMatrixAffineCovariance& C,
														 const Normalisation& n0,
														 const Normalisation& n1);

	 /** Add a correspondence to a design matrix (as three outer products).
	  * Note that only the upper triangle of the symmetric matrix
	  * is filled in.
	  */
	 void AddToDesignMatrix(VNL::MatrixFixed<5,5,double>& D, 
									const VW::Point2D& x0, 
									const VW::Point2D& x1);

	 /** Add a weighted correspondence to a design matrix (as three outer 
	  * products). Note that only the upper triangle of the symmetric matrix
	  * is filled in.
	  */
	 void AddToDesignMatrix(VNL::MatrixFixed<5,5,double>& D, 
									const VW::Point2D& x0, 
									const VW::Point2D& x1,
									const double weight);

	 /** Retrieve from null vector when there is normalisation. */
	 void RecoverF(const VNL::SymmetricEigensystem<double>& eig,
						const Normalisation& n0,
						const Normalisation& n1,
						FMatrixAffine& result);
	 
	 /** Retrieve from null vector assuming no normalisation. */
	 void RecoverF(const VNL::SymmetricEigensystem<double>& eig,
						FMatrixAffine& result);


	 /** Copy the upper triangle of a matrix into the lower. */
	 void MakeSymmetric(VNL::MatrixFixed<5,5,double>& D);

  }; // End namespace FMatrixAffineCompute
}; // End namespace VW























// Inlines
 inline VW::FMatrixAffine VW::FMatrixAffineCompute::
Unnormalise(const VW::FMatrixAffine& F,
				 const VW::Normalisation& n0,
				 const VW::Normalisation& n1)
{
  const double s0 = n0.GetScale(), s1 = n1.GetScale();
  const double x0 = n0.GetCentreX(), x1 = n1.GetCentreX();
  const double y0 = n0.GetCentreY(), y1 = n1.GetCentreY();


  const double a = s1 * F.GetA();
  const double b = s1 * F.GetB();
  const double c = s0 * F.GetC();
  const double d = s0 * F.GetD();
  const double e = F.GetE() - x1*a - y1*b - x0*c - y0*d;

  return VW::FMatrixAffine(a,b,c,d,e);
}

inline D5Vector VW::FMatrixAffineCompute::
UnnormaliseFVec(const D5Vector& v,
					 const Normalisation& n0,
					 const Normalisation& n1)
{
  D5Vector result;
  const double s0 = n0.GetScale(), s1 = n1.GetScale();
  const double x0 = n0.GetCentreX(), x1 = n1.GetCentreX();
  const double y0 = n0.GetCentreY(), y1 = n1.GetCentreY();
  const double s01 = s0*s1;
	 
  result[0] = s1 *v[0];
  result[1] = s1 *v[1];
  result[2] = s0 *v[2];
  result[3] = s0 *v[3];
  result[4] = v[4] - x1*result[0] - y1*result[1] - x0*result[2] - y0*result[3];
	 
  return result;
}

inline VW::FMatrixAffineCovariance VW::FMatrixAffineCompute::
Unnormalise(const VW::FMatrixAffineCovariance& C,
				const Normalisation& n0,
				const Normalisation& n1)
{
  const double s0 = n0.GetScale(), s1 = n1.GetScale();
  const double x0 = n0.GetCentreX(), x1 = n1.GetCentreX();
  const double y0 = n0.GetCentreY(), y1 = n1.GetCentreY();
  const double s01 = s0*s1;

  // Do a sparse multiplication, first NC = N*C
  // (12*5=60 flops)
  VNL::MatrixFixed<5,5,double> NC(0.0);
   for (unsigned int col=0;col<5;col++) {
	  NC[0][col] = s1*C[0][col];
	  NC[1][col] = s1*C[1][col];
	  NC[2][col] = s0*C[2][col];
	  NC[3][col] = s0*C[3][col];
	  NC[4][col] = C[4][col] - x1*NC[0][col] - y1*NC[1][col] - x0*NC[2][col] - y0*NC[3][col];
  }

  // Now the other half
  // (34*9=306 flops)
  VW::FMatrixAffineCovariance NCN;
  for (unsigned int row=0;row<9;row++) {
    NCN[row][0] = s1*NC[row][0];
    NCN[row][1] = s1*NC[row][1];
    NCN[row][2] = s0*NC[row][2];
    NCN[row][3] = s0*NC[row][3];
	 NCN[row][4] = NC[row][4] - x1*NCN[row][0] - y1*NCN[row][1] - x0*NCN[row][2] - y0*NCN[row][3];
  }
  
  std::cerr << "NCN=[" << NCN << "]\n";
  

  // This bit of code just checks that the hand-written stuff is correct
   VNL::MatrixFixed<5,5,double> N(0.0);
   N[0][0] = s1;
   N[1][1] = s1;

   N[2][2] = s0;
   N[3][3] = s0;

   N[4][0] = -s1*x1;
   N[4][1] = -s1*y1;
   N[4][2] = -s0*x0;
   N[4][3] = -s0*y0;
   N[4][4] = 1.0;

	std::cerr << "NCN2=[" << N*C*N.Transpose() << "]" << std::endl;

  return NCN;
}

inline void VW::FMatrixAffineCompute::
MakeSymmetric(VNL::MatrixFixed<5,5,double>& D)
{
  D[1][0] = D[0][1];
  
  D[2][0] = D[0][2];
  D[2][1] = D[1][2];
  
  D[3][0] = D[0][3];
  D[3][1] = D[1][3];
  D[3][2] = D[2][3];
  
  D[4][0] = D[0][4];
  D[4][1] = D[1][4];
  D[4][2] = D[2][4];
  D[4][3] = D[3][4];
}

inline void VW::FMatrixAffineCompute::
RecoverF(const VNL::SymmetricEigensystem<double>& eig,
			const VW::Normalisation& n0,
			const VW::Normalisation& n1,
			VW::FMatrixAffine& result)
{
  assert (eig.V.Rows() == 5 && eig.V.Columns() == 5);

  // Fill fundamental matrix and constrain (to rank 2), and unnormalise
  const VNL::VectorFixed<5,double> null =  eig.Nullvector();
  result.Set(null[0], null[1], null[2], null[3], null[4]);
  result = Unnormalise(result,n0,n1);
}

inline void VW::FMatrixAffineCompute::
RecoverF(const VNL::SymmetricEigensystem<double>& eig,
			VW::FMatrixAffine& result)
{
  assert (eig.V.Rows() == 5 && eig.V.Columns() == 5);

  // Fill fundamental matrix and constrain (to rank 2), and unnormalise
  const VNL::VectorFixed<5,double> null =  eig.Nullvector();
  result.Set(null[0], null[1], null[2], null[3], null[4]);
}

inline void VW::FMatrixAffineCompute::
AddToDesignMatrix(VNL::MatrixFixed<5,5,double>& D, 
						const VW::Point2D& x0, 
						const VW::Point2D& x1)
{
  const double x = x0.GetX();
  const double y = x0.GetY();
  const double tx = x1.GetX();
  const double ty = x1.GetY();
	 
  // Fill only above the diagonal
  D[0][0]+=tx*tx; D[0][1]+=tx*ty; D[0][2]+=tx*x; D[0][3]+=tx*y; D[0][4]+=tx;
                  D[1][1]+=ty*ty; D[1][2]+=ty*x; D[1][3]+=ty*y; D[1][4]+=ty;
                                  D[2][2]+= x*x; D[2][3]+= x*y; D[2][4]+= x;
                                                 D[3][3]+= y*y; D[3][4]+= y;
                                                                D[4][4]+= 1;
}


inline void VW::FMatrixAffineCompute::
AddToDesignMatrix(VNL::MatrixFixed<5,5,double>& D, 
						const VW::Point2D& x0, 
						const VW::Point2D& x1,
						const double weight)
  {
    const double x = weight*x0.GetX();
    const double y = weight*x0.GetY();
    const double tx = weight*x1.GetX();
    const double ty = weight*x1.GetY();
    const double w = weight;
    
    // Fill only above the diagonal
  D[0][0]+=tx*tx; D[0][1]+=tx*ty; D[0][2]+=tx*x; D[0][3]+=tx*y; D[0][4]+=tx*w;
                  D[1][1]+=ty*ty; D[1][2]+=ty*x; D[1][3]+=ty*y; D[1][4]+=ty*w;
                                  D[2][2]+= x*x; D[2][3]+= x*y; D[2][4]+= x*w;
                                                 D[3][3]+= y*y; D[3][4]+= y*w;
                                                                D[4][4]+= w*w;
}



#endif
