/** 
 * @file   fmatrixcomputelinear.cpp
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
#include <iostream>

#include <VNL/Algo/symmetriceigensystem.h>
#include <VNL/Algo/determinant.h>

#include <VW/GeomObjects/fmatrix.h>
#include <VW/GeomObjects/normalisation.h>
#include <VW/GeomObjects/point2d.h>

#include "fmatrixcomputelinear.h"
#include "fmatrixcompute.h" // General helper funcs

#define VERBOSE false

using namespace std;

  ///////////////////////////////////////////////////////////////////


using namespace VW::FMatrixCompute;







  // J=-sum u_i*u_i'/lamba_i where i=2...8: the non-zero eigenvalues/vectors.
  static inline void 
  GetJ(const VNL::SymmetricEigensystem<double>& eig,
       VW::FMatrixCovariance& J)
{
  J.Fill(0.0);
  // Build only the upper triangle
  for (unsigned int i=1;i<9;i++) {
    const double scale = 1.0/eig.GetEigenvalue(i);
    const VNL::VectorFixed<9,double> v = eig.GetEigenvector(i);
    for (unsigned int row=0;row<9;row++) {
      double* J_ptr = J[row]+row;
      for (unsigned int col=row;col<9;col++,J_ptr++) {
	(*J_ptr) -= scale*v[row]*v[col];
      }
    }
  }
  MakeSymmetric(J);
}


  static inline void 
  AddToCa(VW::FMatrixCovariance& Ca, 
	  const VW::FMatrix& F, 
	  const VW::Point2D& x0, 
	  const VW::Point2D& x1,
	  double sigma0, double sigma1)
  {
    const double s0_2 = sigma0*sigma0;
    const double s1_2 = sigma1*sigma1;
    const double x = x0.GetX(), y = x0.GetY();
    const double X = x1.GetX(), Y = x1.GetY();
    const double x2=x*x, y2=y*y, xy=x*y;

	 const double fx = x*F[0][0] + y*F[0][1] + F[0][2];
	 const double fy = y*F[1][0] + y*F[1][1] + F[1][2];
	 const double fX = X*F[0][0] + Y*F[1][0] + F[2][0];
	 const double fY = X*F[0][1] + Y*F[1][1] + F[2][1];

    const double lambda = s0_2*(fX*fX + fY*fY) + s1_2*(fx*fx + fy*fy);
    // Ca is symmetric and each 3x3 block is also symmetric

    // 0-0 block
    { const double l = lambda * X * X;
      Ca[0][0] += l*x2;
      Ca[0][1] += l*xy;
      Ca[0][2] += l*x;
      Ca[1][1] += l*y2;
      Ca[1][2] += l*y;
      Ca[2][2] += l;
    }

    // 0-1 block
    { const double l = lambda * X * Y;
      Ca[0][3] += l*x2;
      Ca[0][4] += l*xy;
      Ca[0][5] += l*x;
      Ca[1][4] += l*y2;
      Ca[1][5] += l*y;
      Ca[2][5] += l;
    }

    // 0-2 block
     { const double l = lambda * X;
      Ca[0][6] += l*x2;
      Ca[0][7] += l*xy;
      Ca[0][8] += l*x;
      Ca[1][7] += l*y2;
      Ca[1][8] += l*y;
      Ca[2][8] += l;
    }

     // 1-1 block
     { const double l = lambda * Y * Y;
      Ca[3][3] += l*x2;
      Ca[3][4] += l*xy;
      Ca[3][5] += l*x;
      Ca[4][4] += l*y2;
      Ca[4][5] += l*y;
      Ca[5][5] += l;
    }

     // 1-2 block
     { const double l = lambda * Y;
      Ca[3][6] += l*x2;
      Ca[3][7] += l*xy;
      Ca[3][8] += l*x;
      Ca[4][7] += l*y2;
      Ca[4][8] += l*y;
      Ca[5][8] += l;
    }

     // 2-2 block
     { const double l = lambda;
      Ca[6][6] += l*x2;
      Ca[6][7] += l*xy;
      Ca[6][8] += l*x;
      Ca[7][7] += l*y2;
      Ca[7][8] += l*y;
      Ca[8][8] += l;
    }

    // Aply off-diagonal block symmetry
    // 0-1 block
    Ca[1][3] = Ca[0][4];
    Ca[2][3] = Ca[0][5];
    Ca[2][4] = Ca[1][5];

    // 0-2 block
    Ca[1][6] = Ca[0][7];
    Ca[2][6] = Ca[0][8];
    Ca[2][7] = Ca[1][8];

    // 1-2 block
    Ca[4][6] = Ca[3][7];
    Ca[5][6] = Ca[3][8];
    Ca[5][7] = Ca[4][8];
  }




bool VW::FMatrixComputeLinear::
Compute(FMatrix& fmatrix,
		 const std::vector<VW::Point2D>& x0,
		 const std::vector<VW::Point2D>& x1)
{
    // 4 steps:
    //   - Determine the optimal normalisation
    //   - Build the DLT matrix
    //   - Solve
    //   - Reconstruct F


    // * Step 1 - Build normalised list *

    // check sizes
    assert(x0.size() == x1.size());
    assert(x0.size() >= 8);

    const unsigned int num_points = x0.size();

    // For efficiency we calculate D'D
    VNL::MatrixFixed<9,9,double> D(0.0);

    if (VERBOSE) {
      std::cerr << "Building measurement matrix using "<< num_points;
      std::cerr <<" features" << std::endl;
    }

    // Code splits here, according to whether normalisation is used

    VW::Normalisation n0;
    VW::Normalisation n1;

  if (_normalise) {
    // Get the normalisation
    n0.Init(x0);
    n1.Init(x1);

    // * Step 2 - Build DLT *
    // Loop through design matrix D filling with values
    vector<VW::Point2D>::const_iterator it0 = x0.begin(), it1 = x1.begin();
    vector<VW::Point2D>::const_iterator it0_end = x0.end();
    for ( ; it0 != it0_end ; it0++, it1++) {
      VW::Point2D norm_point0 = n0.Normalise(*it0);   
      VW::Point2D norm_point1 = n1.Normalise(*it1);   
      AddToDesignMatrix(D,norm_point0,norm_point1);
    }
  } else {
    // * Step 2 - Build DLT *
    // Loop through design matrix D filling with values
    vector<VW::Point2D>::const_iterator it0 = x0.begin(), it1 = x1.begin();
    vector<VW::Point2D>::const_iterator it0_end = x0.end();
    for ( ; it0 != it0_end ; it0++, it1++) {
      AddToDesignMatrix(D,*it0,*it1);
    }
  }
  MakeSymmetric(D);

  // * Step 3 - Solve *
  VNL::SymmetricEigensystem<double> eig(D);
  
  // * Step 4 - Recover *
  if (_normalise)
    RecoverF(eig,n0,n1,fmatrix);
  else
    RecoverF(eig,fmatrix);

  return true; 
}

bool VW::FMatrixComputeLinear::
Compute(VW::FMatrix& result,
		  const std::vector<VW::Point2D>& x0,
		  const std::vector<VW::Point2D>& x1,
		  const std::vector<unsigned int>& which_features)
  {
    // 4 steps:
    //   - Determine the optimal normalisation
    //   - Build the DLT matrix
    //   - Solve
    //   - Reconstruct H


    // * Step 1 - Build normalised list *

    // check sizes
    assert(x0.size() == x1.size());
	 assert(x0.size() >= 4);
    const unsigned int num_points = which_features.size();
 

    // For efficiency we calculate D'D, so always 9x9
    VNL::MatrixFixed<9,9,double> D(0.0);

    // Code splits here, according to whether normalisation is used
    VW::Normalisation n0;
    VW::Normalisation n1;

    if (_normalise) {
      // Get the normalisation
      n0.Init(x0,which_features);
      n1.Init(x1,which_features);

      // * Step 2 - Build DLT *
      // Loop through design matrix D filling with values
      vector<unsigned int>::const_iterator it = which_features.begin();
      vector<unsigned int>::const_iterator it_end = which_features.end();
      for ( ; it != it_end ; it++) {
		  VW::Point2D norm_point0 = n0.Normalise(x0[*it]);   
		  VW::Point2D norm_point1 = n1.Normalise(x1[*it]);   
		  AddToDesignMatrix(D,norm_point0,norm_point1);
      }
    } else {
      // * Step 2 - Build DLT *
      // Loop through design matrix D filling with values
      vector<unsigned int>::const_iterator it = which_features.begin();
      vector<unsigned int>::const_iterator it_end = which_features.end();
      for ( ; it != it_end ; it++) {
		  AddToDesignMatrix(D,x0[*it],x1[*it]);
		  
      }
    }
    MakeSymmetric(D);
	 
    // * Step 3 - Solve *
    VNL::SymmetricEigensystem<double> eig(D);

  

    // * Step 4 - Recover *
    if (_normalise)
      RecoverF(eig,n0,n1,result);
    else
      RecoverF(eig,result);
    return true;
  }


bool VW::FMatrixComputeLinear::
Compute(FMatrix& result,
		  VW::FMatrixCovariance& covar,
		  const std::vector<VW::Point2D>& x0,
		  const std::vector<VW::Point2D>& x1,
		  const double noise0,
		  const double noise1,
		  const std::vector<unsigned int>& which_features)
{
  // 4 steps:
  //   - Determine the optimal normalisation
  //   - Build the DLT matrix
  //   - Solve
  //   - Reconstruct F


  // * Step 1 - Build normalised list *

  // check sizes
  assert(x0.size() == x1.size());
  assert(x0.size() >= 8);

  // For efficiency we calculate D'D
  VNL::MatrixFixed<9,9,double> D(0.0);

  // Code splits here, according to whether normalisation is used

  VW::Normalisation n0;
  VW::Normalisation n1;

  if (_normalise) {
	 // Get the normalisation
	 n0.Init(x0,which_features);
	 n1.Init(x1,which_features);

	 // * Step 2 - Build DLT *
	 // Loop through design matrix D filling with values
	 vector<unsigned int>::const_iterator it = which_features.begin();
	 vector<unsigned int>::const_iterator it_end = which_features.end();
	 for ( ; it != it_end ; it++) {
		VW::Point2D norm_point0 = n0.Normalise(x0[*it]);   
		VW::Point2D norm_point1 = n1.Normalise(x1[*it]);   
		AddToDesignMatrix(D,norm_point0,norm_point1);
	 }
  } else {
	 // * Step 2 - Build DLT *
	 // Loop through design matrix D filling with values
	 vector<unsigned int>::const_iterator it = which_features.begin();
	 vector<unsigned int>::const_iterator it_end = which_features.end();
	 for ( ; it != it_end ; it++) {
		AddToDesignMatrix(D,x0[*it],x1[*it]);
		  
	 }
  }

  // * Step 3 - Solve *
  VNL::SymmetricEigensystem<double> eig(D);

  
  
  // * Step 4 - Recover (F+covar) *
  if (VERBOSE) {
	 std::cerr << "Recover F plus covaraince" << std::endl;
  }
  VW::FMatrixCovariance J,Ca(0.0);
  if (_normalise) {
	 const double norm_noise0 = noise0*n0.GetScale();
	 const double norm_noise1 = noise1*n1.GetScale();
	 RecoverF(eig,result);
	 GetJ(eig,J);
	 vector<unsigned int>::const_iterator it = which_features.begin();
	 vector<unsigned int>::const_iterator it_end = which_features.end();
	 for ( ; it != it_end ; it++) {
		VW::Point2D norm_point0 = n0.Normalise(x0[*it]);   
		VW::Point2D norm_point1 = n1.Normalise(x1[*it]);   
		AddToCa(Ca, result, norm_point0, norm_point1,
				  norm_noise0, norm_noise1);
	 }
	 MakeSymmetric(Ca);

	 covar = J*Ca*J.Transpose();

	 // Now unnormalise
	 result = Unnormalise(result,n0,n1);
	 cerr << "C=["<<covar<<"]"<<endl;
	 covar = Unnormalise(covar,n0,n1);
	 cerr << "C=["<<covar<<"]"<<endl;

  } else {
	 RecoverF(eig,result);
	 GetJ(eig,J);
	 vector<unsigned int>::const_iterator it = which_features.begin();
	 vector<unsigned int>::const_iterator it_end = which_features.end();
	 for ( ; it != it_end ; it++) {
		AddToCa(Ca,result, x0[*it], x1[*it],
				  noise0, noise1);
	 }
	 MakeSymmetric(Ca);
	 covar = J*Ca*J.Transpose();
  }

  return true;
}

bool VW::FMatrixComputeLinear::
Compute(FMatrix& result,
		  VW::FMatrixCovariance& covar,
		  const std::vector<VW::Point2D>& x0,
		  const std::vector<VW::Point2D>& x1,
		  const double noise0,
		  const double noise1)
{
    // 4 steps:
    //   - Determine the optimal normalisation
    //   - Build the DLT matrix
    //   - Solve
    //   - Reconstruct F


    // * Step 1 - Build normalised list *

    // check sizes
    assert(x0.size() == x1.size());
    assert(x0.size() >= 8);

    const unsigned int num_points = x0.size();

    // For efficiency we calculate D'D
    VNL::MatrixFixed<9,9,double> D(0.0);

    if (VERBOSE) {
      std::cerr << "Building measurement matrix using "<< num_points;
      std::cerr <<" features" << std::endl;
    }

    // Code splits here, according to whether normalisation is used

    VW::Normalisation n0;
    VW::Normalisation n1;

  if (_normalise) {
    // Get the normalisation
    n0.Init(x0);
    n1.Init(x1);

    // * Step 2 - Build DLT *
    // Loop through design matrix D filling with values
    vector<VW::Point2D>::const_iterator it0 = x0.begin(), it1 = x1.begin();
    vector<VW::Point2D>::const_iterator it0_end = x0.end();
    for ( ; it0 != it0_end ; it0++, it1++) {
      VW::Point2D norm_point0 = n0.Normalise(*it0);   
      VW::Point2D norm_point1 = n1.Normalise(*it1);   
      AddToDesignMatrix(D,norm_point0,norm_point1);
    }
  } else {
    // * Step 2 - Build DLT *
    // Loop through design matrix D filling with values
    vector<VW::Point2D>::const_iterator it0 = x0.begin(), it1 = x1.begin();
    vector<VW::Point2D>::const_iterator it0_end = x0.end();
    for ( ; it0 != it0_end ; it0++, it1++) {
      AddToDesignMatrix(D,*it0,*it1);
    }
  }
  MakeSymmetric(D);

  // * Step 3 - Solve *
  VNL::SymmetricEigensystem<double> eig(D);
  
  // * Step 4 - Recover (F+covar) *
  if (VERBOSE) {
	 std::cerr << "Recover F plus covaraince" << std::endl;
  }
  VW::FMatrixCovariance J,Ca(0.0);
  if (_normalise) {
	 const double norm_noise0 = noise0*n0.GetScale();
	 const double norm_noise1 = noise1*n1.GetScale();
	 RecoverF(eig,result);
	 GetJ(eig,J);
   vector<VW::Point2D>::const_iterator it0 = x0.begin(), it1 = x1.begin();
    vector<VW::Point2D>::const_iterator it0_end = x0.end();
	 for ( ; it0 != it0_end ; it0++, it1++) {
		VW::Point2D norm_point0 = n0.Normalise(*it0);   
		VW::Point2D norm_point1 = n1.Normalise(*it1);   
		AddToCa(Ca, result, norm_point0, norm_point1,
				  norm_noise0, norm_noise1);
	 }
	 MakeSymmetric(Ca);

	 covar = J*Ca*J.Transpose();

	 // Now unnormalise
	 result = Unnormalise(result,n0,n1);
	 cerr << "C=["<<covar<<"]"<<endl;
	 covar = Unnormalise(covar,n0,n1);
	 cerr << "C=["<<covar<<"]"<<endl;

  } else {
	 RecoverF(eig,result);
	 GetJ(eig,J);
	 vector<VW::Point2D>::const_iterator it0 = x0.begin(), it1 = x1.begin();
	 vector<VW::Point2D>::const_iterator it0_end = x0.end();
	 for ( ; it0 != it0_end ; it0++, it1++) {
		AddToCa(Ca,result, *it0, *it1, noise0, noise1);
	 }
	 MakeSymmetric(Ca);
	 covar = J*Ca*J.Transpose();
  }
  return true; 
}



bool VW::FMatrixComputeLinear::
ComputeWeighted(FMatrix& result,
					 const std::vector<VW::Point2D>& x0,
					 const std::vector<VW::Point2D>& x1,
					 const std::vector<double>& weights)
{
    // 4 steps:
    //   - Determine the optimal normalisation
    //   - Build the DLT matrix
    //   - Solve
    //   - Reconstruct F


    // * Step 1 - Build normalised list *

    // check sizes
    assert(x0.size() == x1.size());
    assert(weights.size() == x0.size());
    assert(x0.size() >= 8);
     const unsigned int num_points = x0.size();

    // For efficiency we calculate D'D
    VNL::MatrixFixed<9,9,double> D(0.0);

    if (VERBOSE) {
      std::cerr << "Building measurement matrix using "<< num_points;
      std::cerr <<" features" << std::endl;
    }

    // Code splits here, according to whether normalisation is used

    VW::Normalisation n0;
    VW::Normalisation n1;

  if (_normalise) {
    // Get the normalisation
    n0.Init(x0);
    n1.Init(x1);

    // * Step 2 - Build DLT *
    // Loop through design matrix D filling with values
	 for (unsigned int i=0;i<num_points;i++) {
		VW::Point2D norm_point0 = n0.Normalise(x0[i]);   
		VW::Point2D norm_point1 = n1.Normalise(x1[i]);   
		AddToDesignMatrix(D,norm_point0,norm_point1,weights[i]);
	 }
  } else {
	 // * Step 2 - Build DLT *
	 
	 // Loop through design matrix D filling with values
	 for (unsigned int i=0;i<num_points;i++) {
		AddToDesignMatrix(D,x0[i],x1[i],weights[i]);
	 }
  }
  MakeSymmetric(D);

  // * Step 3 - Solve *
  VNL::SymmetricEigensystem<double> eig(D);
  
  // * Step 4 - Recover *
  if (_normalise)
    RecoverF(eig,n0,n1,result);
  else
    RecoverF(eig,result);

  return true; 

}
