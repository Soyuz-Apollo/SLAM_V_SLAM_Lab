/** 
 * @file   fmatrixcomputenonlinear.cpp
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
#include <VNL/vector.h>
#include <VNL/matrix.h>
#include <VNL/Algo/svd.h>
#include <VNL/Algo/levenbergmarquardt.h>
#include <VNL/leastsquaresfunction.h>

#include <VW/GeomObjects/fmatrix.h>
#include <VW/GeomObjects/point2d.h>

#include "fmatrixcomputenonlinear.h"

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false

using namespace std;

namespace VW { namespace FMatrixCompute {

  VNL::Vector<double> GenerateParameterVector(const VW::FMatrix& fmatrix)
{
  VNL::Matrix<double> mat = fmatrix.AsMatrix();
  mat /= mat.FrobeniusNorm();

  // F = [t]x M where t = e' and M = [e']x F
  VNL::SVD<double> svdFt (mat.Transpose ());
  VNL::Vector<double> e2 = svdFt.Nullvector ();
  VNL::Vector<double> x (12);
  x[0]  = e2[0];
  x[1]  = e2[1];
  x[2]  = e2[2];
  x[3]  = -e2[2]*mat(1,0) + e2[1]*mat(2,0);
  x[4]  = -e2[2]*mat(1,1) + e2[1]*mat(2,1);
  x[5]  = -e2[2]*mat(1,2) + e2[1]*mat(2,2);
  x[6]  =  e2[2]*mat(0,0) - e2[0]*mat(2,0);
  x[7]  =  e2[2]*mat(0,1) - e2[0]*mat(2,1);
  x[8]  =  e2[2]*mat(0,2) - e2[0]*mat(2,2);
  x[9]  = -e2[1]*mat(0,0) + e2[0]*mat(1,0);
  x[10] = -e2[1]*mat(0,1) + e2[0]*mat(1,1);
  x[11] = -e2[1]*mat(0,2) + e2[0]*mat(1,2);
  
  return x;
}

VW::FMatrix ReconstructTransform(const VNL::Vector<double>& x)
{
  VW::FMatrix F;
  assert (x.size () == 12);

  // F = [t]x M where t = (x[0], x[1], x[2])', and M = mat(x[3] to x[11])
  F[0][0] = -x[2]*x[6] + x[1]*x[9];
  F[0][1] = -x[2]*x[7] + x[1]*x[10];
  F[0][2] = -x[2]*x[8] + x[1]*x[11];
  F[1][0] =  x[2]*x[3] - x[0]*x[9];
  F[1][1] =  x[2]*x[4] - x[0]*x[10];
  F[1][2] =  x[2]*x[5] - x[0]*x[11];
  F[2][0] = -x[1]*x[3] + x[0]*x[6];
  F[2][1] = -x[1]*x[4] + x[0]*x[7];
  F[2][2] = -x[1]*x[5] + x[0]*x[8];

  F = F / F.FrobeniusNorm ();
  return F;
}

  ///////////////////////////////////////////////////////////////////

  /** A small class that knows how to be minimised. */
  class LeastSquaresFunctionPoints : public VNL::LeastSquaresFunction {
  public:
  
    LeastSquaresFunctionPoints(const std::vector<VW::Point2D>& features0,
			       const std::vector<VW::Point2D>& features1,
			       const std::vector<unsigned int>& whichfeatures);
    void F(const VNL::Vector<double>& x, VNL::Vector<double>& fx);
    
    
  protected:
    //  Store pointers to data for later access
    const std::vector<VW::Point2D> &_features0, &_features1;
    const std::vector<unsigned int> &_whichfeatures;
    
  }; // End of minimisation class
  
  ///////////////////////////////////////////////////////////////////


  LeastSquaresFunctionPoints::LeastSquaresFunctionPoints(const std::vector<VW::Point2D>& features0,
							 const std::vector<VW::Point2D>& features1,
							 const std::vector<unsigned int>& whichfeatures)
    :
    VNL::LeastSquaresFunction (12, whichfeatures.size(), no_gradient),
    _features0(features0),
    _features1(features1),
    _whichfeatures(whichfeatures)
  {
  }
  
  void LeastSquaresFunctionPoints::F(const VNL::Vector<double>& x, VNL::Vector<double>& fx)
  {
	 static unsigned int iter=0;
    VW::FMatrix F = ReconstructTransform(x);
	 iter++;

    // We use Sampson distance (HZ 10.4.3-p270) rather than symmetric epipolar
    // distance, because it apparently gives slightly better results.
    VNL::Vector<double>::iterator fx_it = fx.begin();
    vector<unsigned int>::const_iterator it = _whichfeatures.begin();
    vector<unsigned int>::const_iterator itend = _whichfeatures.end();
	 double total_cost=0.0;
    for ( ; it != itend ; it++) {
      const VW::Point2D& p0 = _features0[*it];
      const VW::Point2D& p1 = _features1[*it];
      const VW::Line2D e0 = F.Transfer(p0);
      const VW::Line2D e1 = F.ReverseTransfer(p1);
      
      const double numerator = p1.GetX()*e0[0] + p1.GetY()*e0[1] + e0[2];
      const double denominator2 = e0[0]*e0[0] + e0[1]*e0[1] + e1[0]*e1[0] + e1[1]*e1[1];
		const double cost = numerator / sqrt(denominator2);
		*fx_it++ = cost;
		total_cost += (cost*cost);
    }
	 //cerr << "   cost="<<total_cost<<endl;
  }


} } // End namespaces


using namespace VW::FMatrixCompute;

///////////////////////////////////////////////////////////////////


bool VW::FMatrixComputeNonLinear::
Compute( VW::FMatrix& result,
			const std::vector<VW::Point2D>& x0,
			const std::vector<VW::Point2D>& x1)
{
  // Make a default list of every feature
  const unsigned int num_features = x0.size();
  std::vector<unsigned int> whichfeatures;
  whichfeatures.reserve(num_features);
  for (unsigned int i=0;i<num_features;i++) whichfeatures.push_back(i);

  Compute(result, x0, x1, whichfeatures);
  return true;
}


bool VW::FMatrixComputeNonLinear::
Compute( VW::FMatrix& result,
			const std::vector<VW::Point2D>& x0,
			const std::vector<VW::Point2D>& x1,
			const std::vector<unsigned int>& whichfeatures)
{
  
  LeastSquaresFunctionPoints lsfunc(x0,x1,whichfeatures);


  VNL::LevenbergMarquardt minimiser(lsfunc);
  minimiser.SetFTolerance (_tolerance);
  minimiser.SetMaxFunctionEvals (_max_iters * lsfunc.GetNumberOfUnknowns());
  
  // Form the parameter vector from the initial transform
  VNL::Vector<double> x = GenerateParameterVector(result);
  if (VERBOSE) cerr << "VW::FMatrixCompute::NonLinearFromPoints"<<endl;
  if (VERBOSE) cerr << "   Initial params = ["<<x<<"]"<<endl;


  // Mimimise (spelt wrong in vnl...darn those americanisms!)
  bool _success = minimiser.Minimize (x);

  if (VERBOSE) cerr << "   Final params = ["<<x<<"]"<<endl;
  // Reform output transform
  if (_success) result = ReconstructTransform(x);

  return(_success);
 
}
 
 
