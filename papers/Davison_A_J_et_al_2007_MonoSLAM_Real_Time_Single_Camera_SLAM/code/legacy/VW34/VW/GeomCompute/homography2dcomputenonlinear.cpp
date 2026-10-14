/** 
 * @file   homography2dcomputenonlinear.cpp
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


#ifndef _homography2dcomputenonlinear_cpp_
#define _homography2dcomputenonlinear_cpp_

#include <VNL/matrix.h>
#include <VNL/matrixfixed.h>
#include <VNL/Algo/svd.h>
#include <VNL/Algo/levenbergmarquardt.h>
#include <VNL/leastsquaresfunction.h>
#include <VW/GeomObjects/trajectorymap.h>
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/line2d.h>
#include <VW/GeomObjects/homography2d.h>

#include "homography2dcomputenonlinear.h"
#include "computefuncs.h"

using namespace std;

namespace VW { namespace Homography2DCompute {

#define SAMPSON_ERROR

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
#if defined(SAMPSON_ERROR)
    VNL::LeastSquaresFunction (9, whichfeatures.size(), no_gradient),
#elif defined(SYM_TRANSFER_ERROR)
    VNL::LeastSquaresFunction (9, whichfeatures.size()*2, no_gradient),
#else
# error NO COST FUNCTION DEFINED!
#endif
    _features0(features0),
    _features1(features1),
    _whichfeatures(whichfeatures)
  {}
  
  void LeastSquaresFunctionPoints::F(const VNL::Vector<double>& x, VNL::Vector<double>& fx)
  {
    VW::Homography2D H;
    VW::ComputeFuncs::ConvertParamVectorToTransform(x,H);
    VW::Homography2D Hinv;
    Hinv = VNL::SVD<double>(H).Inverse ();
    
    VNL::Vector<double>::iterator fx_it = fx.begin ();
    vector<unsigned int>::const_iterator it = _whichfeatures.begin();
    vector<unsigned int>::const_iterator itend = _whichfeatures.end();
    for ( ; it != itend ; it++) {
      const VW::Point2D& orig_p1 = _features0[*it];
      const VW::Point2D& orig_p2 = _features1[*it];
      
# if defined(SAMPSON_ERROR)
      
      // Sampson error
      double x = orig_p1.GetX();
      double y = orig_p1.GetY();
      double xp = orig_p2.GetX();
      double yp = orig_p2.GetY();
      double h11 = H(0,0), h12 = H(0,1), h13 = H(0,2);
      double h21 = H(1,0), h22 = H(1,1), h23 = H(1,2);
      double h31 = H(2,0), h32 = H(2,1), h33 = H(2,2);
      
      VNL::VectorFixed<2,double> epsilon;
      epsilon[0] = -(x*h21 + y*h22 + h23) + yp*(x*h31 + y*h32 + h33);
      epsilon[1] =   x*h11 + y*h12 + h13  - xp*(x*h31 + y*h32 + h33);
      
      double J_data [8] = {-h21 + yp*h31, -h22 + yp*h32,
			   0.0, x*h31 + y*h32 + h33,
			   h11 - xp*h31, h21 - xp*h32,
			   -(x*h31 + y*h32 + h33), 0.0};
      VNL::Matrix<double> J (J_data, 2, 4);
      VNL::SVD<double> svdJJt (J * J.Transpose());

      *fx_it++ = sqrt (VNL::DotProduct (epsilon, svdJJt.Inverse() * epsilon));
      
# elif defined(SYM_TRANSFER_ERROR)
      
      // Symmetric transfer error 
      const VW::Point2D transferred_p2 = H.TransferPoint (orig_p1);
      const VW::Point2D transferred_p1 = Hinv.TransferPoint (orig_p2);
      
      *fx_it++ = orig_p1.GetDistance(transferred_p1);
      *fx_it++ = orig_p2.GetDistance(transferred_p2);
      
# else
#  error NO COST FUNCTION SELECTED
# endif
      
    }
  }


} } // End namespaces



using namespace VW::Homography2DCompute;







///////////////////////////////////////////////////////////////////


bool VW::Homography2DComputeNonLinear::
Compute( VW::Homography2D& result,
	 const std::vector<VW::Point2D>& x0,
	 const std::vector<VW::Point2D>& x1)
{
  // Make a default list of every feature
  const unsigned int num_features = x0.size();
  std::vector<unsigned int> whichfeatures;
  whichfeatures.reserve(num_features);
  for (unsigned int i=0;i<num_features;i++) whichfeatures.push_back(i);

  Compute(result, x0, x1, whichfeatures);

}


bool VW::Homography2DComputeNonLinear::
Compute( VW::Homography2D& result,
	 const std::vector<VW::Point2D>& x0,
	 const std::vector<VW::Point2D>& x1,
	 const std::vector<unsigned int>& whichfeatures)
{
  
  VW::Homography2DCompute::
    LeastSquaresFunctionPoints lsfunc(x0,x1,whichfeatures);


  VNL::LevenbergMarquardt minimiser(lsfunc);
  minimiser.SetFTolerance (_tolerance);
  minimiser.SetMaxFunctionEvals (_max_iters * lsfunc.GetNumberOfUnknowns());
  
  // Form the parameter vector from the initial transform
  VNL::Vector<double> x(9);
  VW::ComputeFuncs::ConvertTransformToParamVector(result,x);

  // Mimimise (spelt wrong in vnl...darn those americanisms!)
  bool _success = minimiser.Minimize (x);

  // Reform output transform
  if (_success) VW::ComputeFuncs::ConvertParamVectorToTransform(x,result);
  return(_success);
}
 
  
bool VW::Homography2DComputeNonLinear::
Compute( VW::Homography2D& result,
	 const std::vector<VW::Line2D>& l0,
	 const std::vector<VW::Line2D>& l1)
{
  // Make a default list of every feature
  const unsigned int num_features = l0.size();
  std::vector<unsigned int> whichfeatures;
  whichfeatures.reserve(num_features);
  for (unsigned int i=0;i<num_features;i++) whichfeatures.push_back(i);
  
  Compute(result, l0, l1, whichfeatures);

}

bool VW::Homography2DComputeNonLinear::
Compute( VW::Homography2D& result,
	 const std::vector<VW::Line2D>& l0,
	 const std::vector<VW::Line2D>& l1,
	 const std::vector<unsigned int>& whichfeatures)
{
  assert(false);
}
  
#endif // #ifndef _homography2dcomputenonlinear_cpp_
