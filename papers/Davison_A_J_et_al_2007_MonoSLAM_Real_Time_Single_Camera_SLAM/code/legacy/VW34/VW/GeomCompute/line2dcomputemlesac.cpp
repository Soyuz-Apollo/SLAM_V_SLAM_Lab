
#include <VNL/matrix.h>
#include <VNL/matrixfixed.h>
#include <VNL/vectorfixed.h>
#include <VNL/Algo/svd.h>
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/normalisation.h>
#include <VW/MathBits/random.h>
#include <VW/MathBits/pdfgaussian1d.h>

// Local
#include "sampling.h"
#include "mlesacfuncs.h"
#include "line2dcomputelinear.h"
#include "line2dcompute.h"

#include "line2dcomputemlesac.h"

#ifdef VERBOSE
#   undef VERBOSE
#endif
#define VERBOSE 0

///////////////////////////////////////////////////////////////////








namespace VW { namespace Line2DCompute {
static inline 
double GetInliersPoints (const std::vector<VW::Point2D>& x, 
			 const VW::Line2D& l, 
			 const VW::PDF::Gaussian1D& G,
			 const double U,
			 std::vector<double>& inlier_likelihood, 
			 std::vector<unsigned int>& inliers) 
{
  if (VERBOSE) std::cerr << "Search for inliers..." << std::endl;
  if (VERBOSE) std::cerr << "   Outlier likelihood = " << U << std::endl;
  const unsigned int num_features = x.size();
  assert (inlier_likelihood.size() == num_features);

  // Score inliers
  double inlierscore = 0;
  for (unsigned int j = 0; j < num_features; j++) {
     inlier_likelihood[j] = CalcErrorProbabilityPoint(x[j],l,G);
  }

  // Find total cost of this model (MLE)
  double gamma = VW::MLESACFuncs::CalcMixingParameter (inlier_likelihood,
						   U); // Same everywhere!
  for (unsigned int j = 0; j < num_features; j++) {
    double mle_this = (gamma * inlier_likelihood[j]);
    double mle_sum = mle_this + (1-gamma) * U;
    
    inlierscore += -log(mle_sum);
    
    // Assign to inlier list if an inlier
    if (mle_this/mle_sum > 0.5) inliers.push_back (j);
	if (VERBOSE) std::cerr << j << ":\tIn: "<<inlier_likelihood[j]<<"\tOut: "<<U<<std::endl;
  }
  
  if (VERBOSE) std::cerr << "   Gamma = " << gamma << std::endl;
  
  if (VERBOSE) {
    std::cerr << "   found " << inliers.size() << " with score ";
    std::cerr << inlierscore <<"." << std::endl;
  }

  return inlierscore;

  }


} } // End namespaces



///////////////////////////////////////////////////////////////////



void  VW::Line2DCompute::MLESACFromPoints(VW::Line2D& result,
								  const std::vector<VW::Point2D>& x,
								  std::vector<unsigned int>& inliers,
								  unsigned int max_iterations,
								  double image_noise,
								  double search_distance )
{
  // Some constants
  static const unsigned int sample_size = 2;


  const unsigned int num_features = x.size();

  // Make sure we're actually random
  VW::SeedRand();

// Do the normalisation as a one-off
VW::Normalisation n(x);
std::vector<VW::Point2D> norm_x;
for (unsigned int i=0;i<num_features;i++) {
  norm_x.push_back(n.Normalise(x[i]));
}

  // First, calculate the probability distributions
	double s = n.GetScale();
  VW::PDF::Gaussian1D gaussian(0,image_noise*image_noise*s*s);
  double uniform = 1/(search_distance*s);
  

  // Set up for the loop
  //   - wipe the inlier list
  inliers.clear ();
  inliers.reserve (num_features);
  //   - create a likelihood array
  std::vector<double> inlier_likelihood (num_features);
  //   - storage for the best result
  double bestinlierscore = 1e99;
  std::vector<unsigned int> best_inliers;
  best_inliers.reserve (num_features);


  // We use the standard RANSAC stopping condition, even though it is crap
  unsigned int this_max_iters = max_iterations;

  if (VERBOSE) std::cerr << "VW::Line2DCompute::MLESACFromPoints - going to MLESAC from " << num_features << " features." << std::endl;


  // The core MLESAC loop
  bool finished = false;  // Don't use this yet but will
  for (unsigned int i = 0; i < this_max_iters && !finished; i++) {

  if (VERBOSE) std::cerr << "VW::Line2DCompute::MLESACFromPoints - iteration " << i << std::endl;

    // Pick new sample.
    std::vector<unsigned int> sample_indices;

  if (VERBOSE>1) std::cerr << "1" << std::endl;

    // Random selection from a list of size "num_features"
    VW::RandomSample (sample_size, num_features, sample_indices);

   if (VERBOSE>1) {
	std::cerr << "samples: ";
	for (unsigned int j=0;j<sample_size;j++) std::cerr << sample_indices[j] << " ";
	std::cerr << std::endl;
   }
   if (VERBOSE>1) std::cerr << "2" << std::endl;

     // Calculate transform from the sample features, 
// without re-normalising
      VW::Line2DCompute::LinearFromPoints(result,norm_x,
	false,sample_indices);

   if (VERBOSE>1) std::cerr << "3" << std::endl;

      // Get inlier likelihoods and calculate total inlier score for this model
      inliers.clear();
      double inlierscore = GetInliersPoints (norm_x, 
					result,
					gaussian, uniform,
					inlier_likelihood, inliers);

   if (VERBOSE>1) std::cerr << "4" << std::endl;

      // Adjust best inlier list
      if (inlierscore < bestinlierscore || i == 0) {
	if (VERBOSE)  {
	  std::cout << "VW::Line2DCompute::MLESACFromPoints: Current best transform has ";
	  std::cout << inliers.size() << " with score " << inlierscore << std::endl;
	}
	bestinlierscore = inlierscore;
	best_inliers.clear ();
	best_inliers = inliers;
	double best_inlier_frac = double(inliers.size()) / double(num_features);
	// 99% confidence condition
	this_max_iters = static_cast<unsigned int>(-2 / log10(1 - pow(best_inlier_frac,
								      int(sample_size))));
	if (this_max_iters > max_iterations) this_max_iters = max_iterations;
	else std::cerr << "New max_iters = " << this_max_iters << " (already done "<<i<<")"<<std::endl;
      }
    }
    // Now use best inlier set to calculate the transform properly
    inliers = best_inliers;
    if (VERBOSE)  {
      std::cout << "VW::Line2DCompute::MLESACFromPoints: Recalculating transform from ";
      std::cout << inliers.size () << " inliers" << std::endl;
    }
    VW::Line2DCompute::LinearFromPoints(result,norm_x,
						false,inliers);

	result = n.UnNormalise(result,VW::Line2D());

    if (VERBOSE)  std::cout << "VW::Line2DCompute::MLESACFromPoints: Done" << std::endl;
    
  }
				     
