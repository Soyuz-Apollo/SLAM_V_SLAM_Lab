/** 
 * @file   pmatrixcomputelinear.cpp
 * @author BJ Tordoff
 * @date   14/08/2003		   		
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


#include "pmatrixcomputelinear.h"


#include <VNL/Algo/svd.h>
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/projectionmatrix.h>
#include <VW/GeomObjects/normalisation.h>
#include <VW/GeomObjects/normalisation3d.h>

#ifdef VERBOSE
#   undef VERBOSE
#endif
#define VERBOSE false

static inline void 
RecoverP(const VNL::Vector<double>& p,
	 VW::ProjectionMatrix& result) 
{
const double scale = 1.0 / p.TwoNorm();
	result[0][0] = scale * p[0];
	result[0][1] = scale * p[1];
	result[0][2] = scale * p[2];
	result[0][3] = scale * p[3];
	result[1][0] = scale * p[4];
	result[1][1] = scale * p[5];
	result[1][2] = scale * p[6];
	result[1][3] = scale * p[7];
	result[2][0] = scale * p[8];
	result[2][1] = scale * p[9];
	result[2][2] = scale * p[10];
	result[2][3] = scale * p[11];
}

static inline void RecoverP(const VNL::Vector<double>& p,
		     const VW::Normalisation& n,
		     const VW::Normalisation3D& N,
	VW::ProjectionMatrix& result) 
{
const double scale = 1.0 / p.TwoNorm();
	result[0][0] = scale * p[0];
	result[0][1] = scale * p[1];
	result[0][2] = scale * p[2];
	result[0][3] = scale * p[3];
	result[1][0] = scale * p[4];
	result[1][1] = scale * p[5];
	result[1][2] = scale * p[6];
	result[1][3] = scale * p[7];
	result[2][0] = scale * p[8];
	result[2][1] = scale * p[9];
	result[2][2] = scale * p[10];
	result[2][3] = scale * p[11];

	result = n.InverseMatrix() * result.AsMatrix() * N.AsMatrix();
}

static inline void Build3MatrixRows(VNL::Matrix<double>& D, unsigned int row,
	const VW::Point2D& imagepoint,
	const VW::Point3D& worldpoint)
{
		D[row][0] =  0.0;
		D[row][1] =  0.0;
		D[row][2] =  0.0;
		D[row][3] =  0.0;
		D[row][4] = -worldpoint.GetX ();
		D[row][5] = -worldpoint.GetY ();
		D[row][6] = -worldpoint.GetZ ();
		D[row][7] = -1.0;
		D[row][8] =  worldpoint.GetX () * imagepoint.GetY ();
		D[row][9] =  worldpoint.GetY () * imagepoint.GetY ();
		D[row][10]=  worldpoint.GetZ () * imagepoint.GetY ();
		D[row][11]=  1.0                * imagepoint.GetY ();

		row++;

		D[row][0] =  worldpoint.GetX ();
		D[row][1] =  worldpoint.GetY ();
		D[row][2] =  worldpoint.GetZ ();
		D[row][3] =  1.0;
		D[row][4] =  0.0;
		D[row][5] =  0.0;
		D[row][6] =  0.0;
		D[row][7] =  0.0;
		D[row][8] = -worldpoint.GetX () * imagepoint.GetX ();
		D[row][9] = -worldpoint.GetY () * imagepoint.GetX ();
		D[row][10]= -worldpoint.GetZ () * imagepoint.GetX ();
		D[row][11]= -1.0                * imagepoint.GetX ();

		row++;

		D[row][0] = -worldpoint.GetX () * imagepoint.GetY ();
		D[row][1] = -worldpoint.GetY () * imagepoint.GetY ();
		D[row][2] = -worldpoint.GetZ () * imagepoint.GetY ();
		D[row][3] = -1.0                * imagepoint.GetY ();
		D[row][4] =  worldpoint.GetX () * imagepoint.GetX ();
		D[row][5] =  worldpoint.GetY () * imagepoint.GetX ();
		D[row][6] =  worldpoint.GetZ () * imagepoint.GetX ();
		D[row][7] =  1.0                * imagepoint.GetX ();
		D[row][8] =  0.0;
		D[row][9] =  0.0;
		D[row][10]=  0.0;
		D[row][11]=  0.0;
}




void VW::PMatrixCompute::
LinearFromPoints(VW::ProjectionMatrix& result,
		 const std::vector<VW::Point2D>& x,
		 const std::vector<VW::Point3D>& X,
		 const bool normalise,
		 const std::vector<unsigned int>& inlierlist)
{
  // 4 steps:
  //   - Determine the optimal normalisation
  //   - Build the DLT matrix
  //   - Solve
  //   - Reconstruct


  // * Step 1 - Build normalised list *
  if (VERBOSE) std::cout << "PMatrixCompute::LinearFromPoints: 1" 
			 << std::endl;
  bool useinlierlist(inlierlist.size() > 0);

// Take a pointer to one or other inlier list;
std::vector<unsigned int> defaultinliers;
const std::vector<unsigned int>* which_features;

  unsigned int num_points;
  if (useinlierlist) {
    // Use only those on the inlier list
    num_points = inlierlist.size ();
    which_features = &inlierlist;
  } else {
    num_points = x.size ();
    // If inlier list was not setup initially, fill it now
    for (unsigned int i=0;i<num_points;i++) {
defaultinliers.push_back(i);
}
which_features = &defaultinliers;

  }
  num_points = which_features->size();
  if (VERBOSE) std::cout << "PMatrixCompute::LinearFromPoints: Using " 
			 << num_points << " points" << std::endl;
  
  // Build the measurement matrix
  unsigned int maxrow = (num_points*3);
  VNL::Matrix < double >D (maxrow, 12);
  VNL::Vector < double >b (maxrow);
  
  
  // Code splits here according to normalisation
  VW::Normalisation n;
  VW::Normalisation3D N;
  
  if (normalise) {
    // Get the normalisation
    n.Init(x,*which_features);
    N.Init(X,*which_features);
    
    if (VERBOSE) {
      std::cout << "n="<<n.AsMatrix()<<std::endl;
      std::cout << "N="<<N.AsMatrix()<<std::endl;
    }

    VW::Point3D t = X[(*which_features)[0]];

    // * Step 2 - Build DLT (normalised) *
    if (VERBOSE) std::cout << "PMatrixCompute::LinearFromPoints: 2 (normalised)" << std::endl;
    
    // Loop through design matrix D filling with values
    
    /* NB we implicitly constraint pvec[11] to be unity,  
     * Then renormalize after  
     */
    std::vector<unsigned int>::const_iterator it = which_features->begin();
    for (unsigned int index = 0; index < maxrow; index+=3, it++)
      {
	if (VERBOSE) std::cout << "Build row "<<index<<std::endl;
	const VW::Point2D norm_image_point = n.Normalise(x[*it]);
	const VW::Point3D norm_world_point = N.Normalise(X[*it]);
	Build3MatrixRows(D,index,norm_image_point,norm_world_point);
	
	
      }
  } else {
    // Build with no normalisation
    // * Step 2 - Build DLT (unnormalised) *
    if (VERBOSE) std::cout << "PMatrixCompute::LinearFromPoints: 2 (unnormalised)" << std::endl;
    
    // Loop through design matrix D filling with values
    
    /* NB we implicitly constraint pvec[11] to be unity,  
     * Then renormalize after  
     */
    std::vector<unsigned int>::const_iterator it = which_features->begin();
    for (unsigned int index = 0; index < maxrow; index+=3, it++) {
      if (VERBOSE) std::cout << "Build row "<<index<<std::endl;
      const VW::Point2D image_point = x[*it];
      const VW::Point3D world_point = X[*it];
      Build3MatrixRows(D,index,image_point,world_point);  
    }
    
  }
  
  // * Step 3 - Solve *
if (VERBOSE) std::cout << "PMatrixCompute::LinearFromPoints: 3" << std::endl;
  VNL::SVD < double >svd (D);
  VNL::Vector < double >p = svd.Nullvector();

	
    if (VERBOSE) std::cerr <<" D=" << D << std::endl;
  

  // * Step 4 - Recover *
 if (VERBOSE) std::cout << "PMatrixCompute::LinearFromPoints: 4" << std::endl;
   if (normalise)
      RecoverP(p,n,N,result);
    else
      RecoverP(p,result);


}
