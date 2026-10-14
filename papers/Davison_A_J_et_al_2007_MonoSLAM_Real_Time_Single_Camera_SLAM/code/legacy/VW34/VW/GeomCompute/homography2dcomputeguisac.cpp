/** 
 * @file   homography2dcomputeguisac.cpp
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


#ifndef _homography2dcomputeguisac_cpp_
#define _homography2dcomputeguisac_cpp_

#include "homography2dcomputeguisac.h"

#include <VNL/matrix.h>
#include <VNL/matrixfixed.h>
#include <VNL/vectorfixed.h>
#include <VNL/Algo/svd.h>
#include <VNL/vnlmath.h> // PI

#include <VW/GeomObjects/trajectorymap.h>
#include <VW/GeomObjects/homography2d.h>
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/line2d.h>
#include <VW/GeomObjects/normalisation.h>
#include <VW/MathBits/random.h>
#include <VW/MathBits/pdfgaussian2diso.h>

// Local
#include "sampling.h"
#include "mlesacfuncs.h"
#include "homography2dcomputelinear.h"

#ifdef VERBOSE
#   undef VERBOSE
#endif
#define VERBOSE 0

using namespace std;



/** Get the probability of the symmetric transfer error for one point */
static inline double 
GUISACCalcErrorProbabilityPoint(const VW::Point2D& x0,
					const VW::Point2D& x1,
					const VW::Homography2D& H,
					const VW::PDF::Gaussian2DIso& g)
{
  // Calculate putative point
  VW::Point2D transferred_point = H.TransferPoint (x0);
  
  // Is it within X & Y tolerance
  const double error_squared = (transferred_point - x1).NormSquared();
  const double prob = g.EvaluateDensityForError2(error_squared);
  if (VERBOSE) std::cerr << "Error^2 = " << error_squared << "  prob = " << prob << std::endl;
  
  // Use the error to estimate the probability assuming gaussian noise
  return(prob);
}



/** Find which point-pairs inlie this homography.
 * x0,x1 and H are passed as input, and the indices of inlying pairs
 * returned in inliers. The liklihood of inlying is also stored.
 * @param x0 features in image 0
 * @param x1 features in image 1
 * @param H the resulting homography (maps x0 to x1 as x1=H*x0)
 * @param G the (Gaussian) inlier PDF
 * @param U the (uniform) outlier PDF (constant)
 * @param inlier_likelihood the resulting probabilities that each feature 
 *        is an inlier.
 * @param inliers a list of indices to the inlying features
 * @returns the inlier score
 */
static inline double 
GUISACGetInliersPoints(const std::vector<VW::Point2D>& x0, 
		       const std::vector<VW::Point2D>& x1, 
		       const VW::Homography2D& H, 
		       const VW::PDF::Gaussian2DIso& G,
		       const double U,
		       const std::vector<double>& prior,
		       std::vector<double>& posterior) 
{
  assert(prior.size()==posterior.size());
  if (VERBOSE) std::cerr << "Search for inliers..." << std::endl;
  if (VERBOSE) std::cerr << "   Outlier likelihood = " << U << std::endl;
  const unsigned int num_features = x0.size();
  
  // Score inliers
  double inlierscore = 0;
  for (unsigned int j = 0; j < num_features; j++) {
    const double likelihood = GUISACCalcErrorProbabilityPoint(x0[j], x1[j], 
							H,G);
     const double mle_this = (prior[j] * likelihood);
     const double mle_sum = mle_this + (1-prior[j]) * U;
    
    inlierscore += -log(mle_sum);
    posterior[j] = mle_this/mle_sum;  
  }
  
  return inlierscore;

}



namespace VW { namespace Homography2DCompute {

  void GUISACCore(VW::Homography2D& result,
						const std::vector<VW::Point2D>& norm_x0,
						const std::vector<VW::Point2D>& norm_x1,
						const std::vector<double>& prior,
						std::vector<double>& posterior,
						double desired_confidence,
						unsigned int max_iters,
						double image_noise,
						double search_area)
  {
    // Some constants
    static const unsigned int sample_size = 4;
    const unsigned int num_features = norm_x0.size();
    posterior.resize(num_features);
  
	 // Make sure we're actually random
	 VW::SeedRand();

	 // First, calculate the probability distributions (errors evaluated in image1) and accumulate the priors
    VNL::VectorFixed<2,double> mean;mean[0]=0;mean[1]=0;
    const VW::PDF::Gaussian2DIso gaussian(mean,image_noise*image_noise);
    const double uniform = 1.0/search_area;
    std::vector<double> cumulative_prior;
    VW::AccumulateWeights(prior,
								  cumulative_prior);

    // Set up for the loop
    //   - storage for the best result
    double bestinlierscore = 1e99;
    std::vector<double> this_posterior;
    this_posterior.resize (num_features);
	 // confidence setting (convert % to log)
	 const double log_conf = log10(100.0-desired_confidence) - 2.0;

    // We use the standard RANSAC stopping condition, even though it is crap
    unsigned int this_max_iters = 1000000000;
    if (max_iters>0) this_max_iters = max_iters;

    if (VERBOSE) std::cerr << "VW::Homography2DCompute::GUISACFromPoints - going to GUISAC from " << num_features << " features." << std::endl;


    // The core GUISAC loop
    bool finished = false;  // Don't use this yet but will
	 VW::Homography2DComputeLinear compute_linear(false);
    for (unsigned int i = 0; i < this_max_iters && !finished; i++) {

      if (VERBOSE) std::cerr << "VW::Homography2DCompute::GUISACFromPoints - iteration " << i << std::endl;

      // Pick new sample.
      std::vector<unsigned int> sample_indices;

      if (VERBOSE>1) std::cerr << "1" << std::endl;

      // Random selection from a list of size "num_features"
      VW::WeightedSample (sample_size, cumulative_prior, sample_indices);
      //VW::RandomSample (sample_size, num_features, sample_indices);

      if (VERBOSE>1) {
		  std::cerr << "samples: ";
		  for (unsigned int j=0;j<sample_size;j++) std::cerr << sample_indices[j] << " ";
		  std::cerr << std::endl;
      }
      if (VERBOSE>1) std::cerr << "2" << std::endl;

      // Calculate transform from the sample features, 
      // without re-normalising
      compute_linear.Compute(result,norm_x0,norm_x1,sample_indices);

      if (VERBOSE>1) std::cerr << "3" << std::endl;

      // Get inlier likelihoods and calculate total inlier score for this model
      std::vector<unsigned int> inliers;
      inliers.reserve(num_features);
      double inlierscore = GUISACGetInliersPoints (norm_x0, norm_x1, 
																	result,
																	gaussian, uniform,
																	prior, this_posterior);

      if (VERBOSE>1) std::cerr << "4" << std::endl;

      // Adjust best inlier list
      if (inlierscore < bestinlierscore || i == 0) {
		  bestinlierscore = inlierscore;
		  // Count inliers
		  posterior = this_posterior;
		  double expected_inliers = 0.0;
		  for (unsigned int f=0;f<num_features;f++) {
			 expected_inliers += posterior[f];
		  }
		  double best_inlier_frac = expected_inliers / double(num_features);
		  if (VERBOSE)  {
			 std::cout << "VW::Homography2DCompute::GUISACFromPoints: Current best transform has ";
			 std::cout << expected_inliers << " with score " << inlierscore << std::endl;
		  }
		  // 99% confidence condition
		  this_max_iters = static_cast<unsigned int>(log_conf / log10(1 - pow(best_inlier_frac,
																									 int(sample_size))));
		  if (VERBOSE) std::cerr << "New max_iters = " << this_max_iters 
										 << " (already done "<<i<<", max is "<<max_iters<<")"<<std::endl;
		  if ((max_iters>0) && (this_max_iters>max_iters)) {
			 this_max_iters = max_iters;
		  }
      }
    }
    // Now use best inlier set to calculate the transform properly
    if (VERBOSE)  {
		std::cout << "VW::Homography2DCompute::GUISACFromPoints: Recalculating transform" << endl; 
    }
    compute_linear.ComputeWeighted(result,norm_x0,norm_x1,posterior);
  }
}} // End namespaces








  ///////////////////////////////////////////////////////////////////



bool VW::Homography2DComputeGUISAC::
Compute(VW::Homography2D& result,
	const std::vector<VW::Point2D>& x0,
	const std::vector<VW::Point2D>& x1,
	const std::vector<double>& prior,
	std::vector<double>& posterior)
{

    // Check the lists sizes are equal
    const unsigned int num_features = x0.size();
    if (x0.size() != x1.size()) {
      std::cerr << "VW::Homography2DCompute::GUISACFromPoints - feature lists are different sizes (" << x0.size() << " != " << x1.size() << ")" << std::endl;
      exit(0);
    }

    if ((_desired_confidence<0.0) 
	|| (_desired_confidence>=100.0)) {
    std::cerr << "VW::Homography2DCompute::GUISACFromPoints - confidence must be in range 0<c<100%" << std::endl;
    exit(0);
  }

    // Do the normalisation as a one-off
    VW::Normalisation n0(x0);
    VW::Normalisation n1(x1);
    std::vector<VW::Point2D> norm_x0,norm_x1;
    for (unsigned int i=0;i<num_features;i++) {
      norm_x0.push_back(n0.Normalise(x0[i]));
      norm_x1.push_back(n1.Normalise(x1[i]));
    }
    double s = n1.GetScale();

    // The main bit just acts on the points.
    VW::Homography2DCompute::GUISACCore(result,
					norm_x0, norm_x1,
					prior,posterior,
					_desired_confidence,
					_max_iters,
					_image_noise*s,
					_search_area*s*s);


    // Un-normalise the result
    result = n1.InverseMatrix () * result * n0.AsMatrix ();

    if (VERBOSE)  std::cout << "VW::Homography2DCompute::GUISACFromPoints: Done" << std::endl;
    
  }






#endif // #ifndef _homography2dcomputeguisac_cpp_
