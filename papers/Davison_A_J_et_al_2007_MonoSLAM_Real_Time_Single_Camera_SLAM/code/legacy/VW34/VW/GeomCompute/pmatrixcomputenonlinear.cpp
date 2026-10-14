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



#include <VNL/leastsquaresfunction.h>
#include <VNL/Algo/svd.h>
#include <VNL/Algo/levenbergmarquardt.h>
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/point3d.h>
#include <VW/GeomObjects/rotationmatrix.h>
#include <VW/GeomObjects/projectionmatrix.h>
#include <VW/GeomObjects/calibrationmatrix.h>
#include <VW/GeomObjects/normalisation.h>
#include <VW/GeomObjects/normalisation3d.h>

#include "radialdistortion.h"

#include "pmatrixcomputenonlinear.h"

using namespace std;

#ifdef VERBOSE
#   undef VERBOSE
#endif
#define VERBOSE false

namespace VW { namespace PMatrixCompute {

  // Version without distortion
  void ParamVecToTransform(const VNL::Vector<double>& paramvec,
			   VW::ProjectionMatrix& P,
			const bool zero_skew=true,
			   const bool unity_aspect=false)
  {  
    // First, reform the projection matrix
	double a=paramvec[1];
	if (unity_aspect) a=1.0;
	double s=paramvec[2];
	if (zero_skew) s=0.0;
    const VW::CalibrationMatrix C(paramvec[0],
				  a,
				  s,
				  paramvec[3],
				  paramvec[4]);
    const VW::Vector3D         aa(paramvec[5],
				  paramvec[6],
				  paramvec[7]);
    const VW::RotationMatrix    R(aa);
    const VW::Vector3D          t(paramvec[8],
				  paramvec[9],
				  paramvec[10]);
    P.Compose(C,R,t);


  }
  // Version with distortion
  void ParamVecToTransform(const VNL::Vector<double>& paramvec,
			   VW::ProjectionMatrix& P,
			   double& kappa,
			   VW::Point2D& distortion_centre,
			const bool zero_skew=true,
			   const bool unity_aspect=false)
  {  
    // Get the projection matrix
    ParamVecToTransform(paramvec, P, zero_skew, unity_aspect);


    // And the distortion param
    kappa = paramvec[11];
    distortion_centre.Set(paramvec[12],paramvec[13]);

  }

  // Version without distortion
  void TransformToParamVec(const VW::ProjectionMatrix& P,
			   VNL::Vector<double>& paramvec,
			   const bool zero_skew=true,
			   const bool unity_aspect=false)
  {  
    // First, decompose the projection matrix
    VW::CalibrationMatrix C;
    VW::RotationMatrix    R;
    VW::Vector3D          t;
    P.Decompose(C,R,t);

    if (zero_skew) C.SetSkew(0.0);
    if (unity_aspect) C.SetAspectRatio(1.0);

    // Get the internal params
    paramvec[0] = C.GetFocalLength();
    paramvec[1] = C.GetAspectRatio();
    paramvec[2] = C.GetSkew();
    paramvec[3] = C.GetPrincipalPointX();
    paramvec[4] = C.GetPrincipalPointY();

    // Now the external
    VW::AngleAxis aa = R.AngleAxis();
    paramvec[5]  = aa.GetX();
    paramvec[6]  = aa.GetY();
    paramvec[7]  = aa.GetZ();
    paramvec[8]  = t.GetX();
    paramvec[9]  = t.GetY();
    paramvec[10] = t.GetZ();

  }
  // Version with distortion
  void TransformToParamVec(const VW::ProjectionMatrix& P,
			   const double& kappa,
			   const VW::Point2D& distortion_centre,
			   VNL::Vector<double>& paramvec,
			   const bool zero_skew=true,
			   const bool unity_aspect=false)
  {  
    // First get everything except the distortion
    TransformToParamVec(P,paramvec,zero_skew,unity_aspect);
    
    // And the distortion param
    paramvec[11] = kappa;
    paramvec[12] = distortion_centre.GetX();
    paramvec[13] = distortion_centre.GetY();

  }



  ///////////////////////////////////////////////////////////////////

  /** A small class that knows how to be minimised. */
  class LSFuncPoints : public VNL::LeastSquaresFunction {
  public:
  
    LSFuncPoints(const std::vector<VW::Point2D>& x,
		 const std::vector<VW::Point3D>& X,
		 const std::vector<unsigned int>& whichfeatures,
		 const bool estimate_kappa=true,
		const bool zero_skew=true,
					       const bool unity_aspect=false);
    void F(const VNL::Vector<double>& x, VNL::Vector<double>& fx);
    
    
  protected:
    //  Store pointers to data for later access
    const std::vector<VW::Point2D> &_x;
    const std::vector<VW::Point3D> &_X;
    const std::vector<unsigned int> &_whichfeatures;
    const bool _estimate_kappa;
const bool _zero_skew, _unity_aspect;
    
  }; // End of minimisation class
}; // End of PMatrixCompute namespace
}; // End of VW namespace

VW::PMatrixCompute::LSFuncPoints::LSFuncPoints(const std::vector<VW::Point2D>& x,
					       const std::vector<VW::Point3D>& X,
					       const std::vector<unsigned int>& whichfeatures,
					       const bool estimate_kappa,
					       const bool zero_skew,
					       const bool unity_aspect)
    :
    VNL::LeastSquaresFunction (11 + 3*(estimate_kappa == true), 
			       int(whichfeatures.size()), 
			       no_gradient),
    _x(x),
    _X(X),
    _whichfeatures(whichfeatures),
    _estimate_kappa(estimate_kappa),
	_zero_skew(zero_skew),
	_unity_aspect(unity_aspect)
  {}

void 
VW::PMatrixCompute::LSFuncPoints::F(const VNL::Vector<double>& params,
				    VNL::Vector<double>& fx)
{
  // Two versions, depending on whether kappa is in use
  if (_estimate_kappa) {

    // Get projection from params
    VW::ProjectionMatrix P;
    double kappa = 0;
    VW::Point2D distortion_centre;
    VW::PMatrixCompute::ParamVecToTransform(params,P,kappa,distortion_centre,_zero_skew);
    // Go through each feature projecting.
    std::vector<unsigned int>::const_iterator i = _whichfeatures.begin();
    std::vector<unsigned int>::const_iterator e = _whichfeatures.end();
    double* fx_it = fx.begin();
    double cost=0.0;


    for ( ; i != e ; i++, fx_it++) {
      
      // use current projection matrix to predict the point position
      const VW::Point2D xproj = P.Project(_X[*i]);
      const VW::Point2D xdist 
	= VW::RadialDistortion::DistortPoint(kappa,
					     xproj,
					     distortion_centre);
      // Cost is distance between predicted and measured positions
      *fx_it = xdist.GetDistance(_x[*i]);
      cost += *fx_it;
    }
  } else {
    // Version without distortion is faster
    // Get projection from params
    VW::ProjectionMatrix P;
    VW::PMatrixCompute::ParamVecToTransform(params,P,_zero_skew);

    // Go through each feature projecting.
    std::vector<unsigned int>::const_iterator i = _whichfeatures.begin();
    std::vector<unsigned int>::const_iterator e = _whichfeatures.end();
    double* fx_it = fx.begin();
    for ( ; i != e ; i++, fx_it++) {

      // use current projection matrix to predict the point position
      const VW::Point2D xproj = P.Project(_X[*i]);

      // Cost is distance between predicted and measured positions
      *fx_it = xproj.GetDistance(_x[*i]);
    }
    
  }

}

  ///////////////////////////////////////////////////////////////////



inline bool
VW::PMatrixCompute::NonLinearFromPoints(VW::ProjectionMatrix& P,
					double& kappa,
				VW::Point2D& distortion_centre,
					const std::vector<VW::Point2D>& x,
					const std::vector<VW::Point3D>& X,
					const bool zero_skew,
					const bool unity_aspect,
					double error_tolerance,
					unsigned int max_iters,
					const std::vector<unsigned int>& whichfeatures)
{
  const std::vector<unsigned int>* whichfeatures_ptr = &whichfeatures;

  // If no inliers were specified, use all of them
  std::vector<unsigned int> default_whichfeatures;
  if (whichfeatures.size()==0) {
    for (unsigned int i=0;i<x.size();i++) {
      default_whichfeatures.push_back(i);
    }
    whichfeatures_ptr = &default_whichfeatures;
  }

  std::vector<VW::Point2D> normx(x);
  std::vector<VW::Point3D> normX(X);
  VW::Normalisation n(normx,*whichfeatures_ptr);
  VW::Normalisation3D N(normX,*whichfeatures_ptr);

  n.Normalise(normx);
  N.Normalise(normX);

  VW::Point2D norm_distortion_centre = n.Normalise(distortion_centre);
  VW::ProjectionMatrix normP = n.AsMatrix() * P.AsMatrix() * N.InverseMatrix();
  double normkappa = kappa / (n.GetScale() * n.GetScale());


  VW::PMatrixCompute::LSFuncPoints lsfunc(normx,normX,*whichfeatures_ptr,true,zero_skew,
					  unity_aspect);


  VNL::LevenbergMarquardt minimiser(lsfunc);
  minimiser.SetFTolerance (error_tolerance);
  minimiser.SetMaxFunctionEvals (max_iters * lsfunc.GetNumberOfUnknowns());
  
  // Form the parameter vector from the initial transform
  VNL::Vector<double> params(14);


  TransformToParamVec(normP,
		      normkappa,
			norm_distortion_centre,
		      params,
			zero_skew);
  // Mimimise (spelt wrong in vnl...darn those americanisms!)
  bool _success = minimiser.Minimize (params);

  // Reform output transform
     ParamVecToTransform(params, normP, normkappa, norm_distortion_centre,zero_skew,
			 unity_aspect);
     P = n.InverseMatrix() * normP.AsMatrix() * N.AsMatrix();
     kappa = normkappa * (n.GetScale() * n.GetScale());
     distortion_centre = n.UnNormalise(norm_distortion_centre);

  return(_success);
}

inline bool
VW::PMatrixCompute::NonLinearFromPoints(VW::ProjectionMatrix& P,
					const std::vector<VW::Point2D>& x,
					const std::vector<VW::Point3D>& X,
					const bool zero_skew,
					const bool unity_aspect,
					double error_tolerance,
					unsigned int max_iters,
					const std::vector<unsigned int>& whichfeatures)
{
  const std::vector<unsigned int>* whichfeatures_ptr = &whichfeatures;

  // If no inliers were specified, use all of them
  std::vector<unsigned int> default_whichfeatures;
  if (whichfeatures.size()==0) {
    for (unsigned int i=0;i<x.size();i++) {
      default_whichfeatures.push_back(i);
    }
    whichfeatures_ptr = &default_whichfeatures;
  }

  VW::PMatrixCompute::LSFuncPoints lsfunc(x,X,*whichfeatures_ptr,false,
					  zero_skew,unity_aspect);


  VNL::LevenbergMarquardt minimiser(lsfunc);
  minimiser.SetFTolerance (error_tolerance);
  minimiser.SetMaxFunctionEvals (max_iters * lsfunc.GetNumberOfUnknowns());
  
  // Form the parameter vector from the initial transform
  VNL::Vector<double> params(11);
  TransformToParamVec(P,
			 params,
		      zero_skew,
		      unity_aspect);

  // Mimimise (spelt wrong in vnl...darn those americanisms!)
  bool _success = minimiser.Minimize (params);

  // Reform output transform
  if (_success) {
    VW::Point2D distortion_centre;
    ParamVecToTransform(params, P, zero_skew,
			unity_aspect);
  }
  return(_success);
}

