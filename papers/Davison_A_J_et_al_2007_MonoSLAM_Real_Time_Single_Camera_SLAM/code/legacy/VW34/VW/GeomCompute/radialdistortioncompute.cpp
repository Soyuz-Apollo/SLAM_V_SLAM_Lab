
#include <VNL/matrix.h>
#include <VNL/matrixfixed.h>
#include <VNL/Algo/svd.h>
#include <VNL/Algo/levenbergmarquardt.h>
#include <VNL/leastsquaresfunction.h>
#include <VW/GeomObjects/operations.h> // For GetDistance(line,point)
#include <VW/GeomCompute/line2dcomputelinear.h>

// Local stuff
#include "computefuncs.h"
#include "radialdistortion.h"
#include "radialdistortioncompute.h"

#ifdef VERBOSE
#   undef VERBOSE
#endif
#define VERBOSE false

namespace VW { 
  namespace RadialDistortionCompute {


  /** A small class that knows how to be minimised. */
  class LeastSquaresFunction : public VNL::LeastSquaresFunction {
  public:
  
    LeastSquaresFunction(const std::vector<std::vector<Point2D> >& line_points,
			 const std::vector<unsigned int>& whichfeatures,
			 const Point2D& distortion_centre);
    void F(const VNL::Vector<double>& x, VNL::Vector<double>& fx);
    
    
  protected:
    //  Store pointers to data for later access
    const std::vector<std::vector<Point2D> > &_features;
    const std::vector<unsigned int> &_whichfeatures;
	const ::VW::Point2D _distortioncentre;
    
  }; // End of minimisation class
  

    LeastSquaresFunction::LeastSquaresFunction(const std::vector<std::vector<Point2D> >& features,
					       const std::vector<unsigned int>& whichfeatures,
						   const ::VW::Point2D& distortion_centre)
    :
      VNL::LeastSquaresFunction (1, int(whichfeatures.size()), no_gradient),
      _features(features),
      _whichfeatures(whichfeatures),
      _distortioncentre(distortion_centre)
    {}
  
    void LeastSquaresFunction::F(const VNL::Vector<double>& x, 
				 VNL::Vector<double>& fx)
  {
    VNL::Vector<double>::iterator fx_it = fx.begin ();
    std::vector<unsigned int>::const_iterator it = _whichfeatures.begin();
    std::vector<unsigned int>::const_iterator itend = _whichfeatures.end();

    for (;
	 it != itend; 
	 it++,fx_it++) {
      *fx_it = 0;
      const std::vector<VW::Point2D>& points_for_line = _features[*it];

      std::vector<VW::Point2D> undist_points;

      unsigned int n = static_cast<unsigned int>(points_for_line.size());

      for (unsigned int i=0;i<n;i++) {
	undist_points.push_back(VW::RadialDistortion::UndistortPoint(x[0],
								     points_for_line[i],
								     _distortioncentre));
      }

      // Fit a new line
      Line2D l;
      Line2DCompute::LinearFromPoints(l,undist_points);

      for (unsigned int i=0;i<n;i++) {
	double d = ::VW::GetDistance(undist_points[i],l);
	*fx_it += (d*d);
      }


    }
  }


    bool NonLinearFromLines(double& kappa,
			    const std::vector<std::vector<VW::Point2D> >& linepoints,
			    const VW::Point2D& distortion_centre,
			    double error_tolerance,
			    unsigned int max_iters,
			     const std::vector<unsigned int>& whichfeatures)
    {
      const std::vector<unsigned int>* whichfeatures_ptr = &whichfeatures;
      
      // If no inliers were specified, use all of them
      std::vector<unsigned int> default_whichfeatures;
      if (whichfeatures.size()==0) {
	for (unsigned int i=0;i<linepoints.size();i++) default_whichfeatures.push_back(i);
	whichfeatures_ptr = &default_whichfeatures;
      }
      
      if (VERBOSE) std::cout << "RadialDistortionCompute::NonLinearFromLines: Compute from " << int(whichfeatures_ptr->size()) << " point-sets." << std::endl;

      LeastSquaresFunction lsfunc(linepoints,*whichfeatures_ptr, distortion_centre);
      
      
      VNL::LevenbergMarquardt minimiser(lsfunc);
      minimiser.SetFTolerance (error_tolerance);
      minimiser.SetMaxFunctionEvals (max_iters * lsfunc.GetNumberOfUnknowns());
      
      // Form the parameter vector from the initial transform
      VNL::Vector<double> x(1);
      x[0] = kappa;
      
      // Mimimise (spelt wrong in vnl...darn those americanisms!)
      bool _success = minimiser.Minimize (x);

      // Reform output transform
      if (_success) kappa = x[0];
      
      return(_success);
    }
    
  } // end namespace RadialDistortionCompute
} // End namespace VW
