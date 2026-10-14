/** 
 * @file   fmatrixcompute7point.cpp
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
#include <VNL/Algo/rpolyroots.h>
#include <VNL/Algo/determinant.h>
#include <VNL/Algo/adjugate.h>
#include <VNL/Algo/svd.h>

#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/fmatrix.h>
#include <VW/GeomObjects/normalisation.h>

#include "fmatrixcompute7point.h"

using namespace std;
using namespace VNL;

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false


/** FMatrixCompute7Point::recover - Retrieve 1 or 3 FMatrix solutions from design matrix
 *
 * The algorithm for calculating the cubic polynomial alpha.F1 + (1-alpha).F2, and for solving
 * it and extracting the 1 or 3 real solutions for F, kindly provided by fsm.
 *
 * His comments are:
 * By definition, F * F^\adj = \det(F).
 *
 * So det(A + t B) = det(A) det(1 + t A^{-1} B)
 *                 = det(A) (1 + tr(t A^{-1} B)) + h.o.t.
 *                 = det(A) + t tr(det(A) A^{-1} B) + h.o.t.
 *                 = det(A) + t < A^\adj, B^t > + h.o.t.
 * and hence
 *   < A^\adj, A^t > = 3 det(A)
 *
 * ^\adj means the adjugate, which is defined by the first equation above.
 */
static inline
void SolveCubicForFs (const Matrix<double>& F1,
		      const Matrix<double>& F2,
		      vector<VW::FMatrix>& fmatrices)
{
  // coefficients of det( x F1 + y F2 ) = a x^3 + b x^2 y + c x y^2 + d y^3.
  Vector<double> cubic(4);
  cubic[0] = Determinant (F1);
  cubic[1] = DotProduct (Adjugate (F1), F2.Transpose());
  cubic[2] = DotProduct (F1.Transpose (), Adjugate (F2));
  cubic[3] = Determinant (F2);
  cubic.Normalize();

  // I don't understand about the flipping stuff, personally
  bool flipped = false;
  if (std::abs (cubic[0]) < std::abs (cubic[3])) {
    cubic.Flip();
    flipped = true;
  }
  
  VNL::RPolyRoots solve (cubic);
  for (int i = 0; i < 3; i++) {
    if (solve.Imag(i) == 0) {
      Matrix<double> Fi (flipped ?
		   (F1 + solve.Real(i) * F2) :
		   (F2 + solve.Real(i) * F1) );
      Fi /= Fi.FrobeniusNorm();
      fmatrices.push_back (VW::FMatrix (Fi));
    }
  }
}


/** Unnormalise an FMatrix
 * ie. F = n1'*Fn*n0 */
static inline VW::FMatrix
Unnormalise(const VW::FMatrix& F,
	    const VW::Normalisation& n0,
	    const VW::Normalisation& n1)
{
  VW::FMatrix result;
  const double s0 = n0.GetScale(), s1 = n1.GetScale();
  const double x0 = n0.GetCentreX(), x1 = n1.GetCentreX();
  const double y0 = n0.GetCentreY(), y1 = n1.GetCentreY();
  const double s01 = s0*s1;

  result[0][0] = s01*F[0][0];
  result[0][1] = s01*F[0][1];
  result[0][2] = s1 *F[0][2] - x0*result[0][0] - y0*result[0][1];

  result[1][0] = s01*F[1][0];
  result[1][1] = s01*F[1][1];
  result[1][2] = s1 *F[1][2] - x0*result[1][0] - y0*result[1][1];
 
  result[2][0] = s0 *F[2][0] - x1*result[0][0] - y1*result[1][0];
  result[2][1] = s0 *F[2][1] - x1*result[0][1] - y1*result[1][1];
  result[2][2] = F[2][2] - s1*x1*F[0][2] - s1*y1*F[1][2] - x0*result[2][0] - y0*result[2][1];

//   cerr << "Result=["<<result<<"]"<<endl;
//   cerr << "Check=["<<n1.AsMatrix().Transpose()*F*n0.AsMatrix()<<"]"<<endl;

  return result;
}


static inline
void RecoverFs(const VNL::SVD<double>& svd,
	       vector<VW::FMatrix>& fmatrices)
{
  assert (svd.V().Rows() == 9 && svd.V().Columns() == 9);

  // Get the 2D null-space of svd
  Vector<double> f1 (svd.V().GetColumn (7));
  Vector<double> f2 (svd.V().GetColumn (8));
  Matrix<double> F1 (f1.DataBlock(), 3, 3);
  Matrix<double> F2 (f2.DataBlock(), 3, 3);
  F1 /= F1.RMS ();
  F2 /= F2.RMS ();

  // Calculate and solve the cubic polynomial (fsm's stuff)
  fmatrices.clear ();
  SolveCubicForFs (F1, F2, fmatrices);
}

static inline
void RecoverFs(const VNL::SVD<double>& svd,
	       const VW::Normalisation& n0,
	       const VW::Normalisation& n1,
	       vector<VW::FMatrix>& fmatrices)
{
  assert (svd.V().Rows() == 9 && svd.V().Columns() == 9);

  // Get the 2D null-space of svd
  Vector<double> f1 (svd.V().GetColumn (7));
  Vector<double> f2 (svd.V().GetColumn (8));
  Matrix<double> F1 (f1.DataBlock(), 3, 3);
  Matrix<double> F2 (f2.DataBlock(), 3, 3);
  F1 /= F1.RMS ();
  F2 /= F2.RMS ();

  // Calculate and solve the cubic polynomial (fsm's stuff)
  fmatrices.clear ();
  SolveCubicForFs (F1, F2, fmatrices);

  for (unsigned int i=0;i<fmatrices.size();i++) { 
    fmatrices[i] = Unnormalise(fmatrices[i],n0,n1);
  }
}

typedef VNL::MatrixFixed<7,9,double> Matrix7x9;


void FillDesignMatrix(const VW::Point2D& x0,
		      const VW::Point2D& x1,
		      const unsigned int index,
		      Matrix7x9& D)
{
    const double fx = x0.GetX ();
    const double fy = x0.GetY ();;
    const double tx = x1.GetX ();
    const double ty = x1.GetY ();

    D[index][0] = tx*fx;
    D[index][1] = tx*fy;
    D[index][2] = tx;
    D[index][3] = ty*fx;
    D[index][4] = ty*fy;
    D[index][5] = ty;
    D[index][6] = fx;
    D[index][7] = fy;
    D[index][8] = 1.0;
}





void VW::FMatrixCompute::
Linear7Point(std::vector<FMatrix>& fmatrices,
	       const std::vector<VW::Point2D>& x0,
	       const std::vector<VW::Point2D>& x1,
	       const bool normalise,
	       const std::vector<unsigned int>& inlierlist)
{
  const std::string func_name("VW::FMatrixCompute::Linear7Point");
    // 4 steps:
    //   - Determine the optimal normalisation
    //   - Build the DLT matrix
    //   - Solve
    //   - Reconstruct Fs


    // * Step 1 - Build normalised list *

    // check sizes
    assert(x0.size() == x1.size());

    bool useinlierlist(inlierlist.size() > 0);

    // Take a pointer to one or other inlier list;
    std::vector<unsigned int> defaultinliers;
    const std::vector<unsigned int>* which_features;

    unsigned int num_points;
    if (useinlierlist) {
      // Use only those on the inlier list
      which_features = &inlierlist;
    } else {
      // If inlier list was not setup initially, fill it now
      for (unsigned int i=0;i<x0.size();i++) {
	defaultinliers.push_back(i);
      }
      which_features = &defaultinliers;

    }
    num_points = which_features->size();

    if (num_points != 7) {
      cerr << func_name << ": Error: can only accept 7 feature matches!" << endl;
      assert(num_points == 7);
    }

    Matrix7x9 D(0.0);

    // Code splits here, according to whether normalisation is used

    VW::Normalisation n0;
    VW::Normalisation n1;

    if (normalise) {
      // Get the normalisation
      n0.Init(x0,*which_features);
      n1.Init(x1,*which_features);

      // * Step 2 - Build DLT *

      // Loop through design matrix D filling with values
      vector<unsigned int>::const_iterator it = which_features->begin();
      vector<unsigned int>::const_iterator it_end = which_features->end();
      unsigned int index=0;
      for ( ; it != it_end ; it++,index) {
	VW::Point2D norm_point0 = n0.Normalise(x0[*it]);   
 	VW::Point2D norm_point1 = n1.Normalise(x1[*it]);   
	FillDesignMatrix(norm_point0,norm_point1,index,D);
      }
    } else {
      // * Step 2 - Build DLT *

      // Loop through design matrix D filling with values
      vector<unsigned int>::const_iterator it = which_features->begin();
      vector<unsigned int>::const_iterator it_end = which_features->end();
      unsigned int index=0;
      for ( ; it != it_end ; it++,index++) {
	FillDesignMatrix(x0[*it],x1[*it],index,D);
      }
    }

    // * Step 3 - Solve *
  if (VERBOSE) cout << func_name << ": Decomposing"  << endl;
  VNL::SVD<double> svd(D);

  if (VERBOSE) cout << func_name << ": Recovering FMatrix" << endl;
  // * Step 4 - Recover *
  if (normalise)
    RecoverFs(svd, n0, n1, fmatrices);
  else
    RecoverFs(svd, fmatrices);

  if (VERBOSE) {
    std::cout << func_name << ": FMatrices out:\n1:\n" 
	      << fmatrices[0] << std::endl;
    if (fmatrices.size () == 3) {
      std::cout << "2:\n" << fmatrices[1] << "\n3:\n" 
		<< fmatrices[2] << std::endl;
    }
  }

}
