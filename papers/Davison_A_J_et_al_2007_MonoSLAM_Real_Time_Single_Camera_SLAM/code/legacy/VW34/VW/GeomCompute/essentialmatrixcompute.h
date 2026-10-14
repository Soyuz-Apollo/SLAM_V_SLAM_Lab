/** 
 * @file   essentialmatrixcompute.h
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
 * @namespace VW::EssentialMatrixCompute
 * @ingroup   GeometricAlgorithms
 * @author    BJ Tordoff
 * @date      19/04/2005
 * @brief     Assorted helper functions for computing Essential Matrices.
 */

#ifndef _ematrixcompute_h_
#define _ematrixcompute_h_

#include <VNL/vectorfixed.h>
#include <VNL/matrixfixed.h>
#include <VNL/Algo/symmetriceigensystem.h>

#include <VW/GeomObjects/normalisation.h>
#include <VW/GeomObjects/essentialmatrix.h>


typedef VNL::VectorFixed<9,double> D9Vector;

namespace VW {
  namespace EssentialMatrixCompute {
    
    /** Unnormalise an EssentialMatrix
     * ie. E = n1'*En*n0 
     * (hand optimised to 35 flops - matrix mult would be 90) */
    VW::EssentialMatrix Unnormalise(const VW::EssentialMatrix& E,
				    const Normalisation& n0,
				    const Normalisation& n1);
    
    /** Unnormalise a 9-vector as though it were a 3x3 matrix 
     * ie. E = n1'*En*n0
     * (hand optimised to 35 flops - matrix mult would be 90) */
    D9Vector UnnormaliseEVec(const D9Vector& f,
			     const Normalisation& n0,
			     const Normalisation& n1);

    /** Add a correspondence to a design matrix (as three outer products).
     * Note that only the upper triangle of the symmetric matrix
     * is filled in.
     */
    void AddToDesignMatrix(VNL::MatrixFixed<9,9,double>& D, 
			   const VW::Point2D& x0, 
			   const VW::Point2D& x1);

    /** Add a weighted correspondence to a design matrix (as three outer 
     * products). Note that only the upper triangle of the symmetric matrix
     * is filled in.
     */
    void AddToDesignMatrix(VNL::MatrixFixed<9,9,double>& D, 
			   const VW::Point2D& x0, 
			   const VW::Point2D& x1,
			   const double weight);

    /** Retrieve from null vector when there is normalisation. */
    void RecoverE(const VNL::SymmetricEigensystem<double>& eig,
		  const Normalisation& n0,
		  const Normalisation& n1,
		  EssentialMatrix& result);
	 
    /** Retrieve from null vector assuming no normalisation. */
    void RecoverE(const VNL::SymmetricEigensystem<double>& eig,
		  EssentialMatrix& result);


    /** Copy the upper triangle of a matrix into the lower. */
    void MakeSymmetric(VNL::MatrixFixed<9,9,double>& D);

  }; // End namespace EssentialMatrixCompute
}; // End namespace VW























// Inlines
inline VW::EssentialMatrix VW::EssentialMatrixCompute::
Unnormalise(const VW::EssentialMatrix& E,
	    const VW::Normalisation& n0,
	    const VW::Normalisation& n1)
{
  VW::EssentialMatrix result;
  const double s0 = n0.GetScale(), s1 = n1.GetScale();
  const double x0 = n0.GetCentreX(), x1 = n1.GetCentreX();
  const double y0 = n0.GetCentreY(), y1 = n1.GetCentreY();
  const double s01 = s0*s1;

  result[0][0] = s01*E[0][0];
  result[0][1] = s01*E[0][1];
  result[0][2] = s1 *E[0][2] - x0*result[0][0] - y0*result[0][1];

  result[1][0] = s01*E[1][0];
  result[1][1] = s01*E[1][1];
  result[1][2] = s1 *E[1][2] - x0*result[1][0] - y0*result[1][1];
 
  result[2][0] = s0 *E[2][0] - x1*result[0][0] - y1*result[1][0];
  result[2][1] = s0 *E[2][1] - x1*result[0][1] - y1*result[1][1];
  result[2][2] = E[2][2] - s1*x1*E[0][2] - s1*y1*E[1][2] - x0*result[2][0] - y0*result[2][1];

  return result;
}

inline VNL::VectorFixed<9,double> VW::EssentialMatrixCompute::
UnnormaliseEVec(const VNL::VectorFixed<9,double>& v,
		const VW::Normalisation& n0,
		const VW::Normalisation& n1)
{
  VNL::VectorFixed<9,double> result;
  const double s0 = n0.GetScale(), s1 = n1.GetScale();
  const double x0 = n0.GetCentreX(), x1 = n1.GetCentreX();
  const double y0 = n0.GetCentreY(), y1 = n1.GetCentreY();
  const double s01 = s0*s1;
	 
  result[0] = s01*v[0];
  result[1] = s01*v[1];
  result[2] = s1 *v[2] - x0*result[0] - y0*result[1];
	 
  result[3] = s01*v[3];
  result[4] = s01*v[4];
  result[5] = s1 *v[5] - x0*result[3] - y0*result[4];
	 
  result[6] = s0 *v[6] - x1*result[0] - y1*result[3];
  result[7] = s0 *v[7] - x1*result[1] - y1*result[4];
  result[8] = v[8] - s1*x1*v[2] - s1*y1*v[5] - x0*result[6] - y0*result[7];
	 
  return result;
}


inline void VW::EssentialMatrixCompute::
MakeSymmetric(VNL::MatrixFixed<9,9,double>& D)
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
  
  D[5][0] = D[0][5];
  D[5][1] = D[1][5];
  D[5][2] = D[2][5];
  D[5][3] = D[3][5];
  D[5][4] = D[4][5];
  
  D[6][0] = D[0][6];
  D[6][1] = D[1][6];
  D[6][2] = D[2][6];
  D[6][3] = D[3][6];
  D[6][4] = D[4][6];
  D[6][5] = D[5][6];
  
  D[7][0] = D[0][7];
  D[7][1] = D[1][7];
  D[7][2] = D[2][7];
  D[7][3] = D[3][7];
  D[7][4] = D[4][7];
  D[7][5] = D[5][7];
  D[7][6] = D[6][7];
  
  D[8][0] = D[0][8];
  D[8][1] = D[1][8];
  D[8][2] = D[2][8];
  D[8][3] = D[3][8];
  D[8][4] = D[4][8];
  D[8][5] = D[5][8];
  D[8][6] = D[6][8];
  D[8][7] = D[7][8];
}

inline void VW::EssentialMatrixCompute::
RecoverE(const VNL::SymmetricEigensystem<double>& eig,
	 const VW::Normalisation& n0,
	 const VW::Normalisation& n1,
	 VW::EssentialMatrix& result)
{
  assert (eig.V.Rows() == 9 && eig.V.Columns() == 9);

  // Fill fundamental matrix and constrain (to rank 2), and unnormalise, and normalise
  // Note HZ recommends constraining before denormalisation (sec 10.2, p265)
  result = VNL::Matrix<double>(eig.Nullvector().DataBlock(), 3, 3);
  result.Constrain ();
  result = Unnormalise(result,n0,n1);
}

inline void VW::EssentialMatrixCompute::
RecoverE(const VNL::SymmetricEigensystem<double>& eig,
	 VW::EssentialMatrix& result)
{
  assert (eig.V.Rows() == 9 && eig.V.Columns() == 9);

  // Fill fundamental matrix and constrain (to rank 2)
  result = VNL::Matrix<double>(eig.Nullvector().DataBlock(), 3, 3);
  result.Constrain ();
}

inline void VW::EssentialMatrixCompute::
AddToDesignMatrix(VNL::MatrixFixed<9,9,double>& D, 
		  const VW::Point2D& x0, 
		  const VW::Point2D& x1)
{
  const double x = x0.GetX();
  const double y = x0.GetY();
  const double tx = x1.GetX();
  const double ty = x1.GetY();
	 
  // Fill only above the diagonal
	 
  { // 00 Block (upper triangle only)
    const double t = tx*tx;
    D[0][0] += t*x*x;  D[0][1] += t*x*y;  D[0][2] += t*x;
    D[1][1] += t*y*y;  D[1][2] += t*y;
    D[2][2] += t;
  }
  { // 01 Block (symmetric)
    const double t = tx*ty;
    D[0][3] += t*x*x;  D[0][4] += t*x*y;  D[0][5] += t*x;
    D[1][3] += t*x*y;  D[1][4] += t*y*y;  D[1][5] += t*y;
    D[2][3] += t*x;    D[2][4] += t*y;    D[2][5] += t;
  }
  { // 02 Block (symmetric)
    const double t = tx;
    D[0][6] += t*x*x;  D[0][7] += t*x*y;  D[0][8] += t*x;
    D[1][6] += t*x*y;  D[1][7] += t*y*y;  D[1][8] += t*y;
    D[2][6] += t*x;    D[2][7] += t*y;    D[2][8] += t;
  }
  { // 11 Block (upper triangle only)
    const double t = ty*ty;
    D[3][3] += t*x*x;  D[3][4] += t*x*y;  D[3][5] += t*x;
    D[4][4] += t*y*y;  D[4][5] += t*y;
    D[5][5] += t;
  }
  { // 12 Block (symmetric)
    const double t = ty;
    D[3][6] += t*x*x;  D[3][7] += t*x*y;  D[3][8] += t*x;
    D[4][6] += t*x*y;  D[4][7] += t*y*y;  D[4][8] += t*y;
    D[5][6] += t*x;    D[5][7] += t*y;    D[5][8] += t;
  }
  { // 22 Block (upper triangle only)
    const double t = 1.0;
    D[6][6] += t*x*x;  D[6][7] += t*x*y;  D[6][8] += t*x;
    D[7][7] += t*y*y;  D[7][8] += t*y;
    D[8][8] += t;
  }
}


inline void VW::EssentialMatrixCompute::
AddToDesignMatrix(VNL::MatrixFixed<9,9,double>& D, 
		  const VW::Point2D& x0, 
		  const VW::Point2D& x1,
		  const double weight)
{
  const double x = x0.GetX();
  const double y = x0.GetY();
  const double tx = x1.GetX();
  const double ty = x1.GetY();
  const double w2 = weight*weight;
    
  // Fill only above the diagonal
    
  { // 00 Block (upper triangle only)
    const double t = w2*tx*tx;
    D[0][0] += t*x*x;  D[0][1] += t*x*y;  D[0][2] += t*x;
    D[1][1] += t*y*y;  D[1][2] += t*y;
    D[2][2] += t;
  }
  { // 01 Block (symmetric)
    const double t = w2*tx*ty;
    D[0][3] += t*x*x;  D[0][4] += t*x*y;  D[0][5] += t*x;
    D[1][3] += t*x*y;  D[1][4] += t*y*y;  D[1][5] += t*y;
    D[2][3] += t*x;    D[2][4] += t*y;    D[2][5] += t;
  }
  { // 02 Block (symmetric)
    const double t = w2*tx;
    D[0][6] += t*x*x;  D[0][7] += t*x*y;  D[0][8] += t*x;
    D[1][6] += t*x*y;  D[1][7] += t*y*y;  D[1][8] += t*y;
    D[2][6] += t*x;    D[2][7] += t*y;    D[2][8] += t;
  }
  { // 11 Block (upper triangle only)
    const double t = w2*ty*ty;
    D[3][3] += t*x*x;  D[3][4] += t*x*y;  D[3][5] += t*x;
    D[4][4] += t*y*y;  D[4][5] += t*y;
    D[5][5] += t;
  }
  { // 12 Block (symmetric)
    const double t = w2*ty;
    D[3][6] += t*x*x;  D[3][7] += t*x*y;  D[3][8] += t*x;
    D[4][6] += t*x*y;  D[4][7] += t*y*y;  D[4][8] += t*y;
    D[5][6] += t*x;    D[5][7] += t*y;    D[5][8] += t;
  }
  { // 22 Block (upper triangle only)
    const double t = w2;
    D[6][6] += t*x*x;  D[6][7] += t*x*y;  D[6][8] += t*x;
    D[7][7] += t*y*y;  D[7][8] += t*y;
    D[8][8] += t;
  }
}

#endif
