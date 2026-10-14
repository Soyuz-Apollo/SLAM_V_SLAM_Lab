/** 
 * @file   homography2dcomputegunsac.cpp
 * @author JGH Knight, BJ Tordoff
 * @date   28/02/2005		   		
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


#include "homography2dcomputegunsac.h"

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
#include <VW/MathBits/covariance2d.h>
#include <VW/MathBits/covariance3d.h>

// Local
#include "sampling.h"
#include "mlesacfuncs.h"
#include "homography2dcomputelinear.h"

#ifdef VERBOSE
#   undef VERBOSE
#endif
#define VERBOSE 0

using namespace std;






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
GUNSACGetInliersPoints(const std::vector<VW::Point2D>& x0, 
		       const std::vector<VW::Point2D>& x1, 
		       const VW::Homography2D& H, 
		       const VW::Homography2DCovariance& Ch, 
		       const double image_noise,
		       const double search_area,
		       const std::vector<double>& prior,
		       std::vector<double>& posterior)
{
  assert(x0.size() == x1.size());
  assert(prior.size() == x0.size());
  const unsigned int num_features = x0.size();

  const double sigma2 = (image_noise*image_noise);
  const double outlier_likelihood = 1.0 / search_area;
  const double in_prefix = 1.0 / (2.0*M_PI);
  const double log_out = log(outlier_likelihood);
  // Now evaluate inliers
  double log_lik = 0.0;
  posterior.resize(num_features);

  // Covariance has three parts, due to noise in image 1, noise in image 2 and
  // noise in H. The image noise (and transferred) can be calculated as a 1-off
  const VW::Covariance3D Cx(sigma2, 0, 0, sigma2, 0, 0);
  const VW::Covariance3D Cxt = MapCovariance(H,Cx);
  for (unsigned int i=0;i<num_features;i++) {
      const VW::Point2D& p1 = x0[i];
      const VW::Point2D& p2 = x1[i];
      VNL::VectorFixed<3,double> tp;
      tp[0] = H[0][0]*p1.GetX() + H[0][1]*p1.GetY() + H[0][2];
      tp[1] = H[1][0]*p1.GetX() + H[1][1]*p1.GetY() + H[1][2];
      tp[2] = H[2][0]*p1.GetX() + H[2][1]*p1.GetY() + H[2][2];
      
      const VW::Covariance3D C = Cxt;// + Ch.ProjectAroundPoint(tp);
      
      // Now project around the homg point
      VW::Point2D dest_point(tp[0]/tp[2], tp[1]/tp[2]);
      VW::Covariance2D dest_covar = C.LineariseAroundHomgPoint(tp[0],
							       tp[1],
							       tp[2]);
      // Finally, add image noise in second image
      dest_covar[0] += sigma2;
      dest_covar[2] += sigma2;
      
      // Get error
      const double e2 = dest_covar.GetMahalanobisDistance(dest_point.GetX()-p2.GetX(),
							  dest_point.GetY()-p2.GetY());
      if (e2>9.0) {
	// Beyond 3 sigma, so outlier
	log_lik += log_out;
	posterior[i]=0.0;
      } else {
	const double det = dest_covar.Determinant();
	if (det<=0.0) {
	  // Not pos-def
	  cerr << "Not pos-def C=[" << dest_covar.AsMatrix() << "]" << endl;
	  log_lik += log_out;
	  posterior[i]=0.0;
	} else {
	  const double in_l = prior[i]*in_prefix*exp(-0.5*e2) / sqrt(dest_covar.Determinant());
	  const double total_l = in_l + (1.0-prior[i])*outlier_likelihood;
	  log_lik += log(total_l);
	  posterior[i]=in_l / total_l;
	}
      }
  }
  return -log_lik;
}



namespace VW { namespace Homography2DCompute {

  void GUNSACCore(VW::Homography2D& result,
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

    if (VERBOSE) std::cerr << "VW::Homography2DCompute::GUNSACFromPoints - going to GUNSAC from " << num_features << " features." << std::endl;


    // The core GUNSAC loop
    bool finished = false;  // Don't use this yet but will
    VW::Homography2DCovariance result_covar;
	 VW::Homography2DComputeLinear compute_linear(false);
    for (unsigned int i = 0; i < this_max_iters && !finished; i++) {

      if (VERBOSE) std::cerr << "VW::Homography2DCompute::GUNSACFromPoints - iteration " << i << std::endl;

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
      compute_linear.Compute(result,result_covar,
									  norm_x0,norm_x1,
									  image_noise, image_noise,
									  sample_indices);

      if (VERBOSE>1) std::cerr << "3" << std::endl;

      // Get inlier likelihoods and calculate total inlier score for this model
      double inlierscore = GUNSACGetInliersPoints (norm_x0, norm_x1, 
						   result, result_covar,
						   image_noise, search_area,
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
	  std::cout << "VW::Homography2DCompute::GUNSACFromPoints: Current best transform has ";
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
    std::cout << "VW::Homography2DCompute::GUNSACFromPoints: Recalculating transform" << endl; 
    }
    compute_linear.ComputeWeighted(result,norm_x0,norm_x1,posterior);
  }
}} // End namespaces








  ///////////////////////////////////////////////////////////////////



bool VW::Homography2DComputeGUNSAC::
Compute(VW::Homography2D& result,
	const std::vector<VW::Point2D>& x0,
	const std::vector<VW::Point2D>& x1,
	const std::vector<double>& prior,
	std::vector<double>& posterior)
{

    // Check the lists sizes are equal
    const unsigned int num_features = x0.size();
    if (x0.size() != x1.size()) {
      std::cerr << "VW::Homography2DCompute::GUNSACFromPoints - feature lists are different sizes (" << x0.size() << " != " << x1.size() << ")" << std::endl;
      exit(0);
    }

    if ((_desired_confidence<0.0) 
	|| (_desired_confidence>=100.0)) {
    std::cerr << "VW::Homography2DCompute::GUNSACFromPoints - confidence must be in range 0<c<100%" << std::endl;
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
    VW::Homography2DCompute::GUNSACCore(result,
					norm_x0, norm_x1,
					prior,posterior,
					_desired_confidence,
					_max_iters,
					_image_noise*s,
					_search_area*s*s);


    // Un-normalise the result
    result = n1.InverseMatrix () * result * n0.AsMatrix ();

    if (VERBOSE)  std::cout << "VW::Homography2DCompute::GUNSACFromPoints: Done" << std::endl;
    
  }


