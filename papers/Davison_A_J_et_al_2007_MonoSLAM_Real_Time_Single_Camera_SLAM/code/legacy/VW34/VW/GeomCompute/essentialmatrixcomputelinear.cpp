/** 
 * @file   essentialmatrixcomputelinear.cpp
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
#include <iostream>

#include <VNL/Algo/symmetriceigensystem.h>
#include <VNL/Algo/determinant.h>

#include <VW/GeomObjects/fmatrix.h>
#include <VW/GeomObjects/normalisation.h>
#include <VW/GeomObjects/point2d.h>

#include "essentialmatrixcomputelinear.h"
#include "essentialmatrixcompute.h" // General helper funcs

#define VERBOSE false

using namespace std;

  ///////////////////////////////////////////////////////////////////


using namespace VW::EssentialMatrixCompute;






bool VW::EssentialMatrixComputeLinear::
Compute(EssentialMatrix& fmatrix,
		 const std::vector<VW::Point2D>& x0,
		 const std::vector<VW::Point2D>& x1)
{
    // 4 steps:
    //   - Determine the optimal normalisation
    //   - Build the DLT matrix
    //   - Solve
    //   - Reconstruct E


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
    RecoverE(eig,n0,n1,fmatrix);
  else
    RecoverE(eig,fmatrix);

  return true; 
}

bool VW::EssentialMatrixComputeLinear::
Compute(VW::EssentialMatrix& result,
		  const std::vector<VW::Point2D>& x0,
		  const std::vector<VW::Point2D>& x1,
		  const std::vector<unsigned int>& which_features)
  {
    // 4 steps:
    //   - Determine the optimal normalisation
    //   - Build the DLT matrix
    //   - Solve
    //   - Reconstruct E


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
      RecoverE(eig,n0,n1,result);
    else
      RecoverE(eig,result);
    return true;
  }



bool VW::EssentialMatrixComputeLinear::
ComputeWeighted(EssentialMatrix& result,
					 const std::vector<VW::Point2D>& x0,
					 const std::vector<VW::Point2D>& x1,
					 const std::vector<double>& weights)
{
    // 4 steps:
    //   - Determine the optimal normalisation
    //   - Build the DLT matrix
    //   - Solve
    //   - Reconstruct E


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
    RecoverE(eig,n0,n1,result);
  else
    RecoverE(eig,result);

  return true; 

}
