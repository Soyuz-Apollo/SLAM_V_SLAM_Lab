#ifndef _mlesacfuncs_h_
#define _mlesacfuncs_h_

#include <vector>
#include <VNL/vector.h>
#include <VNL/matrix.h>

namespace VW {

  /** Functions that are common to the MLESAC based computation routines */
  namespace MLESACFuncs {



    /** Calculate the mixture parameter for a Gaussian+Uniform mixture model using EM */
    inline double CalcMixingParameter (const std::vector<double>& inlier_likelihood,
				       const double outlier_likelihood)
    {
      return(0.5);


      // This looks a lot more convoluted than it is. We must evaulate
      //   z = gamma * inlier_likelihood / sum
      // where sum = (gamma * inlier_likelihood) + ((1-gamma) * outlier_likelihood)
      // then gamma = z / number_of_features
      
      static const double max_iters = 5;
      double gamma = 0.5;
      double old_gamma = 0;
      unsigned int num_iterations = 1;
      unsigned int num_features = static_cast<unsigned int>(inlier_likelihood.size());
      
      // If 3dp not reached in 5 iterations, gamma is probably going to be bunk
      while ((fabs (old_gamma - gamma) > 0.001) && (num_iterations <= max_iters)) {
	double sum = 0;
	for (unsigned int j = 0; j < num_features; j++) {
	  double in = gamma * inlier_likelihood[j];
	  double out = (1-gamma) * outlier_likelihood;
	  sum += in / (in+out);
	}
	old_gamma = gamma;
	gamma = sum / num_features;
	num_iterations++;
      }
      
      if (num_iterations > 5) gamma = 0.5;
      return gamma;    
    }
    

  }  // End namespace MLESACFuncs
}; // End namespace VW

#endif
