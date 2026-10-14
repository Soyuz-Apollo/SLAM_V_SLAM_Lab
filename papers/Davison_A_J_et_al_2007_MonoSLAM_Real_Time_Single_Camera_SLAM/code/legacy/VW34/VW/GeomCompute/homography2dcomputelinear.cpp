/** 
 * @file   homography2d_computelinear.cpp
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


#ifndef _homography2dcomputelinear_cpp_
#define _homography2dcomputelinear_cpp_

#include "homography2dcomputelinear.h"
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/line2d.h>
#include <VW/GeomObjects/normalisation.h>
#include <VW/GeomObjects/homography2d.h>

#include <VNL/matrix.h>
#include <VNL/matrixfixed.h>
#include <VNL/Algo/symmetriceigensystem.h>


#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false

using namespace std;

namespace VW { namespace Homography2DCompute {


  ///////////////////////////////////////////////////////////////////

/** Unnormalise a 9-vector as though it were a 3x3 matrix 
 * ie. H = n1^-1*Hn*n0 */
  static inline VNL::VectorFixed<9,double> 
  UnnormaliseHVec(const VNL::VectorFixed<9,double>& v,
						const Normalisation& n0,
						const Normalisation& n1)
  {
    VNL::VectorFixed<9,double> result;
    double s0 = n0.GetScale(), s1 = 1.0/n1.GetScale();
    double x0 = -s0*n0.GetCentreX(), x1 = n1.GetCentreX();
    double y0 = -s0*n0.GetCentreY(), y1 = n1.GetCentreY();
    result[0] = s0*(s1*v[0]+x1*v[6]);
    result[1] = s0*(s1*v[1]+x1*v[7]);
    result[2] = x0*(s1*v[0]+x1*v[6])+y0*(s1*v[1]+x1*v[7])+(s1*v[2]+x1*v[8]);
    result[3] = s0*(s1*v[3]+y1*v[6]);
    result[4] = s0*(s1*v[4]+y1*v[7]);
    result[5] = x0*(s1*v[3]+y1*v[6])+y0*(s1*v[4]+y1*v[7])+(s1*v[5]+y1*v[8]);
    result[6] = s0*v[6];
    result[7] = s0*v[7];
    result[8] = x0*v[6]+y0*v[7]+v[8];
  
    return result;
  }

/** Unnormalise a homography
 * ie. H = n0^-1*Hn*n1 */
  static inline VW::Homography2D
  Unnormalise(const VW::Homography2D& H,
	      const Normalisation& n0,
	      const Normalisation& n1)
  {
//     std::cerr << "Hn=["<<H<<"]\n";
//     std::cerr << "N0=["<<n0.AsMatrix()<<"]\n";
//     std::cerr << "N1=["<<n1.AsMatrix()<<"]\n";
    VW::Homography2D result;
    double s0 = n0.GetScale(), s1 = 1.0/n1.GetScale();
    double x0 = -s0*n0.GetCentreX(), x1 = n1.GetCentreX();
    double y0 = -s0*n0.GetCentreY(), y1 = n1.GetCentreY();
    result[0][0] = s0*(s1*H[0][0]+x1*H[2][0]);
    result[0][1] = s0*(s1*H[0][1]+x1*H[2][1]);
    result[0][2] = x0*(s1*H[0][0]+x1*H[2][0])+y0*(s1*H[0][1]+x1*H[2][1])+(s1*H[0][2]+x1*H[2][2]);

    result[1][0] = s0*(s1*H[1][0]+y1*H[2][0]);
    result[1][1] = s0*(s1*H[1][1]+y1*H[2][1]);
    result[1][2] = x0*(s1*H[1][0]+y1*H[2][0])+y0*(s1*H[1][1]+y1*H[2][1])+(s1*H[1][2]+y1*H[2][2]);

    result[2][0] = s0*H[2][0];
    result[2][1] = s0*H[2][1];
    result[2][2] = x0*H[2][0]+y0*H[2][1]+H[2][2];
  
//     std::cerr << "H=["<<result<<"]\n";
    return result;
  }

/** Unnormalise a covariance */
  static inline VW::Homography2DCovariance 
  Unnormalise(const VW::Homography2DCovariance& C,
	      const Normalisation& n0,
	      const Normalisation& n1)
  {
    VW::Homography2DCovariance NC,NCN;
    double s0 = n0.GetScale(), s1 = 1.0/n1.GetScale();
    double x0 = -s0*n0.GetCentreX(), x1 = n1.GetCentreX();
    double y0 = -s0*n0.GetCentreY(), y1 = n1.GetCentreY();

    // Do a sparse multiplication, first NC = N*C
    for (unsigned int col=0;col<9;col++) {
      NC[0][col] = s0*(s1*C[0][col]+x1*C[6][col]);
      NC[1][col] = s0*(s1*C[1][col]+x1*C[7][col]);
      NC[2][col] = s1*(x0*C[0][col]+y0*C[1][col]+C[2][col]) + x1*(x0*C[6][col]+y0*C[7][col]+C[8][col]);

      NC[3][col] = s0*(s1*C[3][col]+y1*C[6][col]);
      NC[4][col] = s0*(s1*C[4][col]+y1*C[7][col]);
      NC[5][col] = s1*(x0*C[3][col]+y0*C[4][col]+C[5][col]) + y1*(x0*C[6][col]+y0*C[7][col]+C[8][col]);

      NC[6][col] = s0*C[6][col];
      NC[7][col] = s0*C[7][col];
      NC[8][col] = x0*C[6][col]+y0*C[7][col]+C[8][col];
    }

    // Now NCN = NC*N'
    for (unsigned int row=0;row<9;row++) {
      NCN[row][0] = s0*(s1*NC[row][0]+x1*NC[row][6]);
      NCN[row][1] = s0*(s1*NC[row][1]+x1*NC[row][7]);
      NCN[row][2] = s1*(x0*NC[row][0]+y0*NC[row][1]+NC[row][2]) + x1*(x0*NC[row][6]+y0*NC[row][7]+NC[row][8]);

      NCN[row][3] = s0*(s1*NC[row][3]+y1*NC[row][6]);
      NCN[row][4] = s0*(s1*NC[row][4]+y1*NC[row][7]);
      NCN[row][5] = s1*(x0*NC[row][3]+y0*NC[row][4]+NC[row][5]) + y1*(x0*NC[row][6]+y0*NC[row][7]+NC[row][8]);

      NCN[row][6] = s0*NC[row][6];
      NCN[row][7] = s0*NC[row][7];
      NCN[row][8] = x0*NC[row][6]+y0*NC[row][7]+NC[row][8];
    }

    return NCN;
  }

// First, some utility functions
/** Retrieve from null vector when there is normalisation. */
static inline void 
RecoverH(const VNL::SymmetricEigensystem<double>& eig,
	 const Normalisation& n0,
	 const Normalisation& n1,
	 Homography2D& homography)
{
  assert (eig.V.Rows() == 9 && eig.V.Columns() == 9);

  // Fill homography and unnormalise, and normalise
  VNL::VectorFixed<9,double> v(UnnormaliseHVec(eig.Nullvector(),n0,n1));
  homography.CopyIn(v.DataBlock());
}

  /** Retrieve from null vector assuming no normalisation. */
  static inline void 
  RecoverH(const VNL::SymmetricEigensystem<double>& eig,
	   Homography2D& homography)
  {
    assert (eig.V.Rows() == 9 && eig.V.Columns() == 9);

    // Fill homography
    homography.CopyIn(eig.Nullvector().DataBlock());
  }


  static inline void 
  Add3MatrixRows(VNL::MatrixFixed<9,9,double>& D, 
		 const VW::Point2D& x0, 
		 const VW::Point2D& x1)
  {
    // Add outer products (top triangle only)

    const double x = x0.GetX();
    const double y = x0.GetY();
    const double tx = x1.GetX();
    const double ty = x1.GetY();

    // First diagonal blocks

    { // 00 Block (upper triangle only)
      const double t = (1.0 + ty*ty);
      D[0][0] += t*x*x;  D[0][1] += t*x*y;  D[0][2] += t*x;
                         D[1][1] += t*y*y;  D[1][2] += t*y;
                                            D[2][2] += t;
    }
    { // 11 Block (upper triangle only)
      const double t = (1.0 + tx*tx);
      D[3][3] += t*x*x;  D[3][4] += t*x*y;  D[3][5] += t*x;
                         D[4][4] += t*y*y;  D[4][5] += t*y;
                                            D[5][5] += t;
    }
    { // 22 Block (upper triangle only)
      const double t = (tx*tx + ty*ty);
      D[6][6] += t*x*x;  D[6][7] += t*x*y;  D[6][8] += t*x;
                         D[7][7] += t*y*y;  D[7][8] += t*y;
                                            D[8][8] += t;
    }

    // Now the off-diagonal blocks
    { // 01 Block (symmetric)
      const double t = -tx*ty;
      D[0][3] += t*x*x;  D[0][4] += t*x*y;  D[0][5] += t*x;
      D[1][3] += t*x*y;  D[1][4] += t*y*y;  D[1][5] += t*y;
      D[2][3] += t*x;    D[2][4] += t*y;    D[2][5] += t;
    }
    { // 02 Block (symmetric)
      const double t = -tx;
      D[0][6] += t*x*x;  D[0][7] += t*x*y;  D[0][8] += t*x;
      D[1][6] += t*x*y;  D[1][7] += t*y*y;  D[1][8] += t*y;
      D[2][6] += t*x;    D[2][7] += t*y;    D[2][8] += t;
    }
    { // 12 Block (symmetric)
      const double t = -ty;
      D[3][6] += t*x*x;  D[3][7] += t*x*y;  D[3][8] += t*x;
      D[4][6] += t*x*y;  D[4][7] += t*y*y;  D[4][8] += t*y;
      D[5][6] += t*x;    D[5][7] += t*y;    D[5][8] += t;
    }


  }

  static inline void 
  Add3WeightedMatrixRows(VNL::MatrixFixed<9,9,double>& D, 
			 const VW::Point2D& x0, 
			 const VW::Point2D& x1,
			 const double weight)
  {
    // Add outer products (top triangle only)

    const double x = x0.GetX();
    const double y = x0.GetY();
    const double tx = x1.GetX();
    const double ty = x1.GetY();
    const double w2 = weight*weight;

    // First diagonal blocks

    { // 00 Block (upper triangle only)
      const double t = w2*(1.0 + ty*ty);
      D[0][0] += t*x*x;  D[0][1] += t*x*y;  D[0][2] += t*x;
                         D[1][1] += t*y*y;  D[1][2] += t*y;
                                            D[2][2] += t;
    }
    { // 11 Block (upper triangle only)
      const double t = w2*(1.0 + tx*tx);
      D[3][3] += t*x*x;  D[3][4] += t*x*y;  D[3][5] += t*x;
                         D[4][4] += t*y*y;  D[4][5] += t*y;
                                            D[5][5] += t;
    }
    { // 22 Block (upper triangle only)
      const double t = w2*(tx*tx + ty*ty);
      D[6][6] += t*x*x;  D[6][7] += t*x*y;  D[6][8] += t*x;
                         D[7][7] += t*y*y;  D[7][8] += t*y;
                                            D[8][8] += t;
    }

    // Now the off-diagonal blocks
    { // 01 Block (symmetric)
      const double t = -w2*tx*ty;
      D[0][3] += t*x*x;  D[0][4] += t*x*y;  D[0][5] += t*x;
      D[1][3] += t*x*y;  D[1][4] += t*y*y;  D[1][5] += t*y;
      D[2][3] += t*x;    D[2][4] += t*y;    D[2][5] += t;
    }
    { // 02 Block (symmetric)
      const double t = -w2*tx;
      D[0][6] += t*x*x;  D[0][7] += t*x*y;  D[0][8] += t*x;
      D[1][6] += t*x*y;  D[1][7] += t*y*y;  D[1][8] += t*y;
      D[2][6] += t*x;    D[2][7] += t*y;    D[2][8] += t;
    }
    { // 12 Block (symmetric)
      const double t = -w2*ty;
      D[3][6] += t*x*x;  D[3][7] += t*x*y;  D[3][8] += t*x;
      D[4][6] += t*x*y;  D[4][7] += t*y*y;  D[4][8] += t*y;
      D[5][6] += t*x;    D[5][7] += t*y;    D[5][8] += t;
    }


  }

  static inline void 
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


  // J=-sum u_i*u_i'/lamba_i where i=2...8: the non-zero eigenvalues/vectors.
  static inline void 
  GetJ(const VNL::SymmetricEigensystem<double>& eig,
       Homography2DCovariance& J)
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
  AddToCa(Homography2DCovariance& Ca, 
	  const VW::Homography2D& H, 
	  const VW::Point2D& x0, 
	  const VW::Point2D& x1,
	  double sigma0, double sigma1)
  {
    const double s0_2 = sigma0*sigma0;
    const double s1_2 = sigma1*sigma1;
    const double x = x0.GetX(), y = x0.GetY();
    const double X = x1.GetX(), Y = x1.GetY();
    const double x2=x*x, y2=y*y, xy=x*y;
    const double b14 = Y*H[0][0] - X*H[1][0];
    const double b17 =   H[0][0] - X*H[2][0];
    const double b28 =   H[0][1] - X*H[2][1];
    const double b47 =   H[1][0] - Y*H[2][0];
    const double b52 = X*H[1][1] - Y*H[0][1];
    const double b58 =   H[1][1] - Y*H[2][1];
    const double b123 = x*H[0][0] + y*H[0][1] + H[0][2];
    const double b456 = x*H[1][0] + y*H[1][1] + H[1][2];
    const double b789 = x*H[2][0] + y*H[2][1] + H[2][2];

    const double lambda_xx = s0_2*(b47*b47 + b58*b58) + s1_2*b789*b789;
    const double lambda_yy = s0_2*(b28*b28 + b17*b17) + s1_2*b789*b789;
    const double lambda_zz = s0_2*(b14*b14 + b52*b52) + s1_2*(b123*b123+b456*b456);
    const double lambda_xy = -s0_2*(b17*b47 + b28*b58);
    const double lambda_xz = -s0_2*(b52*b58 - b14*b47) - s1_2*(b123*b789);
    const double lambda_yz = -s0_2*(b14*b17 - b52*b28) - s1_2*(b456*b789);

    // Ca is symmetric and each 3x3 block is also symmetric

    // 0-0 block
    { const double l = lambda_yy - 2*Y*lambda_yz + Y*Y*lambda_zz;
      Ca[0][0] += l*x2;
      Ca[0][1] += l*xy;
      Ca[0][2] += l*x;
      Ca[1][1] += l*y2;
      Ca[1][2] += l*y;
      Ca[2][2] += l;
    }

    // 0-1 block
    { const double l = Y*lambda_xz + X*lambda_yz - X*Y*lambda_zz - lambda_xy;
      Ca[0][3] += l*x2;
      Ca[0][4] += l*xy;
      Ca[0][5] += l*x;
      Ca[1][4] += l*y2;
      Ca[1][5] += l*y;
      Ca[2][5] += l;
    }

    // 0-2 block
     { const double l = Y*lambda_xy - X*lambda_yy + X*Y*lambda_yz - Y*Y*lambda_xz;
      Ca[0][6] += l*x2;
      Ca[0][7] += l*xy;
      Ca[0][8] += l*x;
      Ca[1][7] += l*y2;
      Ca[1][8] += l*y;
      Ca[2][8] += l;
    }

     // 1-1 block
     { const double l = lambda_xx - 2*X*lambda_xz + X*X*lambda_zz;
      Ca[3][3] += l*x2;
      Ca[3][4] += l*xy;
      Ca[3][5] += l*x;
      Ca[4][4] += l*y2;
      Ca[4][5] += l*y;
      Ca[5][5] += l;
    }

     // 1-2 block
     { const double l = X*lambda_xy + X*Y*lambda_xz - Y*lambda_xx - X*X*lambda_yz;
      Ca[3][6] += l*x2;
      Ca[3][7] += l*xy;
      Ca[3][8] += l*x;
      Ca[4][7] += l*y2;
      Ca[4][8] += l*y;
      Ca[5][8] += l;
    }

     // 2-2 block
     { const double l = Y*Y*lambda_xx - 2*X*Y*lambda_xy + X*X*lambda_yy;
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


  ///////////////////////////////////////////////////////////////////


}; }; //  End namespaces

using namespace VW::Homography2DCompute;






bool VW::Homography2DComputeLinear::
Compute(VW::Homography2D& result,
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
  const unsigned int num_points = x0.size();

  // For efficiency we calculate D'D, so always 9x9
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
      Add3MatrixRows(D,norm_point0,norm_point1);
    }
  } else {
    // * Step 2 - Build DLT *
    // Loop through design matrix D filling with values
    vector<VW::Point2D>::const_iterator it0 = x0.begin(), it1 = x1.begin();
    vector<VW::Point2D>::const_iterator it0_end = x0.end();
    for ( ; it0 != it0_end ; it0++, it1++) {
      Add3MatrixRows(D,*it0,*it1);
    }
  }
  MakeSymmetric(D);

  // * Step 3 - Solve *
  VNL::SymmetricEigensystem<double> eig(D);
  
  // * Step 4 - Recover *
  if (_normalise)
    RecoverH(eig,n0,n1,result);
  else
    RecoverH(eig,result);

  return true;
}

bool VW::Homography2DComputeLinear::
Compute(VW::Homography2D& result,
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
	Add3MatrixRows(D,norm_point0,norm_point1);
      }
    } else {
      // * Step 2 - Build DLT *
      // Loop through design matrix D filling with values
      vector<unsigned int>::const_iterator it = which_features.begin();
      vector<unsigned int>::const_iterator it_end = which_features.end();
      for ( ; it != it_end ; it++) {
	Add3MatrixRows(D,x0[*it],x1[*it]);

      }
    }
    MakeSymmetric(D);

    // * Step 3 - Solve *
    VNL::SymmetricEigensystem<double> eig(D);

  

    // * Step 4 - Recover *
    if (_normalise)
      RecoverH(eig,n0,n1,result);
    else
      RecoverH(eig,result);
    return true;
  }

bool VW::Homography2DComputeLinear::
ComputeWeighted(VW::Homography2D& result,
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
    assert(weights.size() == x0.size());

    const unsigned int num_points = x0.size();

    // For efficiency we calculate D'D, so always 9x9
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
	Add3WeightedMatrixRows(D,norm_point0,norm_point1,weights[i]);
      }
    } else {
      // * Step 2 - Build DLT *

      // Loop through design matrix D filling with values
      for (unsigned int i=0;i<num_points;i++) {
	Add3WeightedMatrixRows(D,x0[i],x1[i],weights[i]);
      }
    }
    MakeSymmetric(D);

    // * Step 3 - Solve *
    VNL::SymmetricEigensystem<double> eig(D);

  

    // * Step 4 - Recover *
    if (_normalise)
      RecoverH(eig,n0,n1,result);
    else
      RecoverH(eig,result);

    return true;

  }
				     
bool VW::Homography2DComputeLinear::
Compute(VW::Homography2D& result,
	     VW::Homography2DCovariance& covar,
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
  
  // For efficiency we calculate D'D, so always 9x9
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
      Add3MatrixRows(D,norm_point0,norm_point1);
    }
  } else {
    // * Step 2 - Build DLT *
    // Loop through design matrix D filling with values
     vector<VW::Point2D>::const_iterator it0 = x0.begin(), it1 = x1.begin();
    vector<VW::Point2D>::const_iterator it0_end = x0.end();
    for ( ; it0 != it0_end ; it0++, it1++) {
      Add3MatrixRows(D,*it0,*it1);
    }
  }
  MakeSymmetric(D);

  // * Step 3 - Solve *
  VNL::SymmetricEigensystem<double> eig(D);
  
  // * Step 4 - Recover (H+covar) *
  VW::Homography2DCovariance J,Ca(0.0);
  if (_normalise) {
    const double norm_noise0 = noise0*n0.GetScale();
    const double norm_noise1 = noise1*n1.GetScale();
    RecoverH(eig,result);
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
    RecoverH(eig,result);
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
   
bool VW::Homography2DComputeLinear::
Compute(VW::Homography2D& result,
	     VW::Homography2DCovariance& covar,
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

    // For efficiency we calculate D'D, so always 9x9
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
      n0.Init(x0,which_features);
      n1.Init(x1,which_features);


      // * Step 2 - Build DLT *

      // Loop through design matrix D filling with values
      vector<unsigned int>::const_iterator it = which_features.begin();
      vector<unsigned int>::const_iterator it_end = which_features.end();
      for ( ; it != it_end ; it++) {
	VW::Point2D norm_point0 = n0.Normalise(x0[*it]);   
 	VW::Point2D norm_point1 = n1.Normalise(x1[*it]);   
	Add3MatrixRows(D,norm_point0,norm_point1);
      }
    } else {
      // * Step 2 - Build DLT *

      // Loop through design matrix D filling with values
      vector<unsigned int>::const_iterator it = which_features.begin();
      vector<unsigned int>::const_iterator it_end = which_features.end();
      for ( ; it != it_end ; it++) {
	Add3MatrixRows(D,x0[*it],x1[*it]);

      }
    }
    MakeSymmetric(D);

    // * Step 3 - Solve *
    VNL::SymmetricEigensystem<double> eig(D);

 

    // * Step 4 - Recover (H+covar) *
    VW::Homography2DCovariance J,Ca(0.0);
    if (_normalise) {
      const double norm_noise0 = noise0*n0.GetScale();
      const double norm_noise1 = noise1*n1.GetScale();
      RecoverH(eig,result);
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
      RecoverH(eig,result);
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

bool VW::Homography2DComputeLinear::
Compute(VW::Homography2D& result,
	const std::vector<VW::Line2D>& l0,
	const std::vector<VW::Line2D>& l1)
{
  assert(false);
  return false;
}


#endif // #ifndef _homography2d_computelinear_cpp_
