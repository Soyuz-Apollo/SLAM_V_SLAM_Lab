/** 
 * @file   fmatrixcomputemlesac.cpp
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
#include <VW/GeomObjects/fmatrix.h>
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/line2d.h>
#include <VW/GeomObjects/normalisation.h>
#include <VW/MathBits/random.h>
#include <VW/MathBits/exp.h>
#include <VW/MathBits/pdfgaussian1d.h>
#include <VW/Utils/timer.h>

#include "sampling.h"
#include "mlesacfuncs.h"
#include "fmatrixcomputemlesac.h"
#include "fmatrixcompute7point.h"
#include "fmatrixcomputelinear.h"

#define OVERCONFIDENCE 10
#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE 0
static const double PI = 3.1415926535897932384626433832795; // 32 digits

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
static inline double MLESACGetInliersPoints(const std::vector<VW::Point2D>& x0, 
														  const std::vector<VW::Point2D>& x1, 
														  const VW::FMatrix& F, 
														  const VW::PDF::Gaussian1D& G,
														  const double U,
														  std::vector<double>& inlier_likelihood, 
														  std::vector<unsigned int>& inliers) 
{
  if (VERBOSE) std::cerr << "Search for inliers..." << std::endl;
  if (VERBOSE) std::cerr << "   Outlier likelihood = " << U << std::endl;
  const unsigned int num_features = x0.size();
  assert (inlier_likelihood.size() == num_features);
  
  // Score inliers
  double inlierscore = 0;
    for (unsigned int j = 0; j < num_features; j++) {
		const VW::Line2D epipolar_line = F.Transfer(x0[j]);
		const double error_squared = epipolar_line.GetDistance2(x1[j]);
		inlier_likelihood[j] = G.ApproxDensityForError2(error_squared);
    }
    
    // Find total cost of this model (MLE)
    double gamma = VW::MLESACFuncs::CalcMixingParameter(inlier_likelihood,
							U); // Same everywhere!
	 const double outlier_likelihood = (1-gamma) * U;
	 const double thresh = outlier_likelihood/10.0;
	 const double log_out = log(outlier_likelihood);

    for (unsigned int j = 0; j < num_features; j++) {
      double mle_this = (gamma * inlier_likelihood[j]);
      double mle_sum = mle_this + (1-gamma) * U;
    
		if (mle_this > thresh)
		  inlierscore -= log(mle_sum);
		else
		  inlierscore -= log_out;
    
      // Assign to inlier list if an inlier
      if (mle_this/mle_sum > 0.5) inliers.push_back (j);
      if (VERBOSE>1) std::cerr << j << ":\tIn: "<<inlier_likelihood[j]<<"\tOut: "<<U<<std::endl;
    }
  
    if (VERBOSE) std::cerr << "   Gamma = " << gamma << std::endl;
  
    if (VERBOSE>1) {
      std::cerr << "   found " << inliers.size() << " with score ";
      std::cerr << inlierscore <<"." << std::endl;
    }

    return inlierscore;

  }


static inline double 
RANSACGetInliersPoints(const std::vector<VW::Point2D>& x0, 
							  const std::vector<VW::Point2D>& x1, 
							  const VW::FMatrix& F, 
							  const double threshold,
							  std::vector<unsigned int>& inliers) 
{
  const double threshold_squared = threshold*threshold;
  const unsigned int num_features = x0.size();
  
  // Score inliers
  double inlierscore = 0;
  for (unsigned int j = 0; j < num_features; j++) {
	 const VW::Line2D epipolar_line = F.Transfer(x0[j]);
	 const double error_squared = epipolar_line.GetDistance2(x1[j]);
	 if (error_squared < threshold_squared) {
		inliers.push_back (j); 
		inlierscore++;
	 }
  }
  if (VERBOSE) {
	 std::cerr << "   found " << inliers.size() << "." << std::endl;
  }

  return inlierscore;  
}



static inline double 
MSACGetInliersPoints(const std::vector<VW::Point2D>& x0, 
							const std::vector<VW::Point2D>& x1, 
							const VW::FMatrix& F, 
							const double threshold,
							std::vector<double>& inlier_likelihood, 
							std::vector<unsigned int>& inliers) 
{
  const double threshold_squared = threshold*threshold;
  const unsigned int num_features = x0.size();
  assert (inlier_likelihood.size() == num_features);
  
  // Score inliers
  double inlierscore = 0;
  for (unsigned int j = 0; j < num_features; j++) {
	 const VW::Line2D epipolar_line = F.Transfer(x0[j]);
	 const double error_squared = epipolar_line.GetDistance2(x1[j]);
	 if (error_squared < threshold_squared) {
		inliers.push_back(j); 
		const double e = threshold - sqrt(error_squared);
		inlier_likelihood[j] = e*e;
	 } else {
		inlier_likelihood[j] = 0.0;
	 }
	 inlierscore += inlier_likelihood[j];
  }
  if (VERBOSE) {
	 std::cerr << "   found " << inliers.size() << "." << std::endl;
  }

  return inlierscore;  
}


static inline double 
RUNSACGetInliersPoints(const std::vector<VW::Point2D>& x0, 
							  const std::vector<VW::Point2D>& x1, 
							  const VW::FMatrix& F, 
							  const VW::FMatrixCovariance& Cf, 
							  const double image_noise,
							  const double search_area,
							  const std::vector<double>& prior,
							  std::vector<unsigned int>& inliers) 
{
  assert(x0.size() == x1.size());
  assert(prior.size() == x0.size());
  const unsigned int num_features = x0.size();

  const double sigma2 = (image_noise*image_noise);
  const double outlier_likelihood = 1.0 / search_area;
  const double in_prefix = 1.0 / sqrt(2.0*PI);
  const double log_out = log(outlier_likelihood);
  // Now evaluate inliers
  double log_lik = 0.0;

  // Covariance has three parts, due to noise in image 1, noise in image 2 and
  // noise in H. The image noise (and transferred) can be calculated as a 1-off
  const VW::Covariance3D Cx(sigma2, 0, 0, sigma2, 0, 0);
  const VW::Covariance3D Cxt = MapCovariance(F,Cx);
  for (unsigned int i=0;i<num_features;i++) {
      const VW::Point2D& p1 = x0[i];
      const VW::Point2D& p2 = x1[i];
    VW::Line2D l(F[0][0]*p1.GetX() + F[0][1]*p1.GetY() + F[0][2],
		 F[1][0]*p1.GetX() + F[1][1]*p1.GetY() + F[1][2],
		 F[2][0]*p1.GetX() + F[2][1]*p1.GetY() + F[2][2]);
    
    VW::Covariance3D Cl = Cxt + Cf.TransferUncertainty(p1);
    
    /** Normalise the line so that distances are correct */
    VW::NormaliseLine(l, Cl);
    
    
    // Resulting variance is V = l' C_x l + x' C_l x
    // where l is epipolar line with covariance C_l
    // and x is point in second image with covariance C_x
    // NB: If line is normalised and C_x is isotropic with variance sigma then
    //     l' C_x l = sigma^2
    const double v = VW::GetDistanceVariance(p2, l, Cl, sigma2);
    
    // Get error (Mahalanobis distance)
    const double distance = p2.GetX()*l[0] + p2.GetY()*l[1] + l[2];
    const double e2 = distance*distance/v;
    if (e2>9.0) {
      // Beyond 3 sigma, so outlier
      log_lik += log_out;
    } else {
      const double in_l = prior[i]*in_prefix*VW::PadeExp44(-0.5*e2) / sqrt(v);
      const double total_l = in_l + (1.0-prior[i])*outlier_likelihood;
      log_lik += log(total_l);
		if (in_l/total_l > 0.5) inliers.push_back (i);
    }
  }
  return -log_lik;
}

namespace VW { namespace FMatrixCompute {
  void MLESACCore(FMatrix& result,
						const std::vector<Point2D>& norm_x0,
						const std::vector<Point2D>& norm_x1,
						std::vector<unsigned int>& inliers,
						double desired_confidence,
						unsigned int max_iters,
						double image_noise,
						double search_area)
  {
	 const std::string func_name("VW::FMatrixCompute::MLESACFromPoints");
	 // Some constants
	 static const unsigned int sample_size = 8;
  const unsigned int num_features = norm_x0.size();
  
  // Make sure we're actually random
  SeedRand();
  
  // First, calculate the probability distributions (errors evaluated in image1)
  const PDF::Gaussian1D gaussian(0,image_noise*image_noise);
  const double uniform = 1.0/search_area;
  const double threshold = 3*image_noise;
  
  
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
	 // confidence setting (convert % to log)
   const double log_conf = log10(100.0-desired_confidence) - 2.0;
	
    // We use the standard RANSAC stopping condition, even though it is crap
    unsigned int this_max_iters = 1000000000;

	 double hyp_time = 0.0, covhyp_time = 0.0;
	 double ransac_eval_time = 0.0;
	 double msac_eval_time = 0.0;
	 double mlesac_eval_time = 0.0;
	 double runsac_eval_time = 0.0;
	 unsigned int num_hyps=0,num_evals=0;

	 VW::Timer t;

    if (VERBOSE) std::cerr << func_name << " - going to MLESAC from " << num_features << " features." << std::endl;


    // The core MLESAC loop
    bool finished = false;  // Don't use this yet but will
	 VW::FMatrixCovariance covar;
	 VW::FMatrixComputeLinear computelinear(false);
    for (unsigned int i = 0; i < this_max_iters && !finished; i++) {

      if (VERBOSE) std::cerr << func_name << " - iteration " << i << std::endl;

      // Pick new sample.
      std::vector<unsigned int> sample_indices;

      if (VERBOSE>1) std::cerr << "1" << std::endl;

      // Random selection from a list of size "num_features"
      ::VW::RandomSample (sample_size, num_features, sample_indices);

      if (VERBOSE>1) {
	std::cerr << "samples: ";
	for (unsigned int j=0;j<sample_size;j++) std::cerr << sample_indices[j] << " ";
	std::cerr << std::endl;
      }
      if (VERBOSE>1) std::cerr << "2" << std::endl;

      // Calculate transform from the sample features, 
      // without re-normalising
		VW::RandomSample (sample_size, num_features, sample_indices);
		computelinear.Compute(result,
									 norm_x0,norm_x1,
									 sample_indices);

      if (VERBOSE>1) std::cerr << "3" << std::endl;

      // In this case, since we might have more than one F, iterate over them

	// Get inlier likelihoods and calculate total inlier score
	inliers.clear();
	const double inlierscore = MLESACGetInliersPoints(norm_x0, norm_x1, 
																	  result,
																	  gaussian, uniform,
																	  inlier_likelihood, 
																	  inliers);

	if (VERBOSE>1) std::cerr << "4" << std::endl;
	
	// Adjust best inlier list
	if ((inlierscore < bestinlierscore) || (i == 0)) {
	  if (VERBOSE)  {
	    std::cout << func_name << " Current best transform has ";
	    std::cout << inliers.size() << " with score " << inlierscore << std::endl;
	  }
	  bestinlierscore = inlierscore;
	  best_inliers.clear ();
	  best_inliers = inliers;

	  double best_inlier_frac = double(inliers.size()) / double(num_features);
	  // 99% confidence condition
	  this_max_iters = static_cast<unsigned int>(log_conf / log10(1 - pow(best_inlier_frac,
									      int(sample_size))));
	  if ((max_iters>0) && (this_max_iters>max_iters)) {
	    this_max_iters = max_iters;
	  }

	  // 	  if (VERBOSE)
 std::cerr << "New max_iters = " << this_max_iters 
									 << " (already done "<<i<<")"<<std::endl;
      }
    }

    // Now use best inlier set to calculate the transform properly
    inliers = best_inliers;
    if (VERBOSE)  {
      std::cout << func_name << " Recalculating transform from ";
      std::cout << inliers.size () << " inliers" << std::endl;
    }
    computelinear.Compute(result,norm_x0,norm_x1,inliers);
  }
} } // End namespaces



bool VW::FMatrixComputeMLESAC::
Compute(VW::FMatrix& result,
	const std::vector<VW::Point2D>& x0,
	const std::vector<VW::Point2D>& x1,
	std::vector<unsigned int>& inliers)
{


    // Check the lists sizes are equal
    const unsigned int num_features = x0.size();
    if (x0.size() != x1.size()) {
      std::cerr << "VW::FMatrixCompute::MLESACFromPoints - feature lists are different sizes (" << x0.size() << " != " << x1.size() << ")" << std::endl;
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
    VW::FMatrixCompute::MLESACCore(result,
											  norm_x0, norm_x1,
											  inliers,
											  _desired_confidence,
											  _max_iters,
											  _image_noise*s,
											  _search_area*s*s);

    // Un-normalise the result
    result = n1.AsMatrix().Transpose() * result * n0.AsMatrix ();

    if (VERBOSE)  std::cout << "VW::Homography2DCompute::MLESACFromPoints: Done" << std::endl;
    
  }
