/** 
 * @file   fmaffinecomputelinear.cpp
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

#include "fmaffinecomputelinear.h"


#include <iostream>

#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/fmatrixaffine.h>
#include <VW/GeomObjects/normalisation.h>

#include <VNL/Algo/symmetriceigensystem.h>

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false

using namespace std;

#include "fmaffinecompute.h"
using namespace VW::FMatrixAffineCompute;





// J=-sum u_i*u_i'/lamba_i where i=2...8: the non-zero eigenvalues/vectors.
static inline void 
GetJ(const VNL::SymmetricEigensystem<double>& eig,
	  VW::FMatrixAffineCovariance& J)
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
AddToCa(VW::FMatrixAffineCovariance& Ca, 
		  const VW::FMatrixAffine& F, 
		  const VW::Point2D& x0, 
		  const VW::Point2D& x1,
		  double sigma0, double sigma1)
{
  const double s0_2 = sigma0*sigma0;
  const double s1_2 = sigma1*sigma1;
  const double x = x0.GetX(), y = x0.GetY();
  const double X = x1.GetX(), Y = x1.GetY();
  
  const double fx = F.GetA();
  const double fy = F.GetB();
  const double fX = F.GetC();
  const double fY = F.GetD();
  
  const double lambda = s0_2*(fX*fX + fY*fY) + s1_2*(fx*fx + fy*fy);
  // Ca is symmetric, so just fill above
  Ca[0][0] += lambda*X*X;
  Ca[0][1] += lambda*X*Y;
  Ca[0][2] += lambda*X*x;
  Ca[0][3] += lambda*X*y;
  Ca[0][4] += lambda*X;

  Ca[1][1] += lambda*Y*Y;
  Ca[1][2] += lambda*Y*x;
  Ca[1][3] += lambda*Y*y;
  Ca[1][4] += lambda*Y;

  Ca[2][2] += lambda*x*x;
  Ca[2][3] += lambda*x*y;
  Ca[2][4] += lambda*x;

  Ca[3][3] += lambda*y*y;
  Ca[3][4] += lambda*y;

  Ca[4][4] += lambda;
}












//*********** Now the class ************//

bool VW::FMatrixAffineComputeLinear::
Compute(VW::FMatrixAffine& result,
		  const std::vector<VW::Point2D>& x0,
		  const std::vector<VW::Point2D>& x1)
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
  const unsigned int num_points = x0.size();

  // For efficiency we calculate D'D, so always 5x5
  VNL::MatrixFixed<5,5,double> D(0.0);

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
    RecoverF(eig,n0,n1,result);
  else
    RecoverF(eig,result);

  return true;
}

bool VW::FMatrixAffineComputeLinear::
Compute(VW::FMatrixAffine& result,
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
 

    // For efficiency we calculate D'D, so always 5x5
    VNL::MatrixFixed<5,5,double> D(0.0);

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

bool VW::FMatrixAffineComputeLinear::
ComputeWeighted(VW::FMatrixAffine& result,
					 const std::vector<VW::Point2D>& x0,
					 const std::vector<VW::Point2D>& x1,
					 const std::vector<double>& weights)
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
  assert(weights.size() == x0.size());
  
  const unsigned int num_points = x0.size();
  
  // For efficiency we calculate D'D, so always 5x5
  VNL::MatrixFixed<5,5,double> D(0.0);

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


bool VW::FMatrixAffineComputeLinear::
Compute(VW::FMatrixAffine& result,
	     VW::FMatrixAffineCovariance& covar,
	     const std::vector<VW::Point2D>& x0,
	     const std::vector<VW::Point2D>& x1,
	     const double noise0,
	     const double noise1)
{
  // 5 steps:
  //   - Determine the optimal normalisation
  //   - Build the DLT matrix
  //   - Solve
  //   - Get covariance
  //   - Reconstruct H
  
  
  // * Step 1 - Build normalised list *
  
  // check sizes
  assert(x0.size() == x1.size());
  const unsigned int num_points = x0.size();
  
  // For efficiency we calculate D'D, so always 5x5
  VNL::MatrixFixed<5,5,double> D(0.0);
  
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
  
  // * Step 4 - Recover (H+covar) *
  VW::FMatrixAffineCovariance J,Ca(0.0);
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
      AddToCa(Ca,result, norm_point0, norm_point1,
	      norm_noise0, norm_noise1);
    }
    MakeSymmetric(Ca);
    covar = J*Ca*J.Transpose();
    
    // Now unnormalise
    result = Unnormalise(result,n0,n1);
    covar = Unnormalise(covar,n0,n1);
    
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
   
bool VW::FMatrixAffineComputeLinear::
Compute(VW::FMatrixAffine& result,
	     VW::FMatrixAffineCovariance& covar,
	     const std::vector<VW::Point2D>& x0,
	     const std::vector<VW::Point2D>& x1,
	     const double noise0,
	     const double noise1,
	     const std::vector<unsigned int>& which_features)
{
    // 5 steps:
    //   - Determine the optimal normalisation
    //   - Build the DLT matrix
    //   - Solve
    //   - Get covariance
    //   - Reconstruct H


    // * Step 1 - Build normalised list *

    // check sizes
    assert(x0.size() == x1.size());
    const unsigned int num_points = which_features.size();

    // For efficiency we calculate D'D, so always 5x5
    VNL::MatrixFixed<5,5,double> D(0.0);

    if (VERBOSE) {
      std::cerr << "Building measurement matrix using "<< num_points;
      std::cerr <<" features" << std::endl;
    }

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

 

    // * Step 4 - Recover (H+covar) *
    VW::FMatrixAffineCovariance J,Ca(0.0);
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
		  AddToCa(Ca,result, norm_point0, norm_point1,
					 norm_noise0, norm_noise1);
      }
      MakeSymmetric(Ca);
      covar = J*Ca*J.Transpose();

      // Now unnormalise
      result = Unnormalise(result,n0,n1);
      covar = Unnormalise(covar,n0,n1);

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
