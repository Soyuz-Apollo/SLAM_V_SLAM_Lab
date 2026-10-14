/** 
 * @file     essentialmatrixcomputemlesac.cpp
 * @author   BJ Tordoff
 * @date     20/04/2005		   		
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
#include <VW/GeomObjects/essentialmatrix.h>
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/line2d.h>
#include <VW/GeomObjects/normalisation.h>
#include <VW/MathBits/random.h>
#include <VW/MathBits/exp.h>
#include <VW/MathBits/pdfgaussian1d.h>
#include <VW/Utils/timer.h>

#include "sampling.h"
#include "mlesacfuncs.h"
#include "essentialmatrixcomputemlesac.h"
#include "essentialmatrixcomputelinear.h"
#include "essentialmatrixcompute5point.h"

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE 0
static const double PI = 3.1415926535897932384626433832795; // 32 digits

using namespace std;


namespace VW { namespace EssentialMatrixCompute {

  static inline double MLESACGetInliersPoints(const std::vector<VW::Point2D>& x0, 
															 const std::vector<VW::Point2D>& x1, 
															 const VW::EssentialMatrix& E, 
															 const VW::PDF::Gaussian1D& G,
															 const double U,
															 std::vector<double>& inlier_likelihood, 
															 std::vector<unsigned int>& inliers) 
  {
	 if (VERBOSE) std::cerr << "Search for inliers to essential matrix..." << std::endl;
	 if (VERBOSE) std::cerr << "   E = [" << E << "]" << std::endl;
	 if (VERBOSE) std::cerr << "   Outlier likelihood = " << U << std::endl;
	 const unsigned int num_features = x0.size();
	 assert (inlier_likelihood.size() == num_features);
  
  // Score inliers
	 double inlierscore = 0;
	 for (unsigned int j = 0; j < num_features; j++) {
		const VW::Line2D epipolar_line = E.Transfer(x0[j]);
		const double error_squared = epipolar_line.GetDistance2(x1[j]);
		inlier_likelihood[j] = G.ApproxDensityForError2(error_squared);
    }
    
    // Find total cost of this model (MLE)
    double gamma = VW::MLESACFuncs::CalcMixingParameter(inlier_likelihood,
																		  U); // Same everywhere!
	 
  
    if (VERBOSE) std::cerr << "   Gamma = " << gamma << std::endl;

	 const double outlier_likelihood = (1-gamma) * U;
	 const double thresh = outlier_likelihood/10.0;
	 const double log_out = log(outlier_likelihood);

    for (unsigned int j = 0; j < num_features; j++) {
      double mle_this = (gamma * inlier_likelihood[j]);
      double mle_sum = mle_this + outlier_likelihood;
    
		if (mle_this > thresh)
		  inlierscore -= log(mle_sum);
		else
		  inlierscore -= log_out;
    
      // Assign to inlier list if an inlier
      if (mle_this/mle_sum > 0.5) inliers.push_back (j);
      if (VERBOSE>1) std::cerr << j << ":\tIn: "<<inlier_likelihood[j]<<"\tOut: "<<U<<std::endl;
    }
  
    if (VERBOSE>1) {
      std::cerr << "   found " << inliers.size() << " with score ";
      std::cerr << inlierscore <<"." << std::endl;
    }

    return inlierscore;

  }




  static inline void MLESACCore(EssentialMatrix& result,
										  const std::vector<Point2D>& norm_x0,
										  const std::vector<Point2D>& norm_x1,
										  std::vector<unsigned int>& inliers,
										  double desired_confidence,
										  unsigned int max_iters,
										  double image_noise,
										  double search_area)
  {
	 const std::string func_name("VW::EssentialMatrixCompute::MLESAC");
	 // Some constants
	 static const unsigned int sample_size = 5;
	 const unsigned int num_features = norm_x0.size();
  
  // First, calculate the probability distributions (errors evaluated in image1)
  const PDF::Gaussian1D gaussian(0,image_noise*image_noise);
  const double uniform = 1.0/search_area;
  
  
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
  
  if (VERBOSE) std::cerr << func_name << " - going to MLESAC from " << num_features << " features." << std::endl;


  // The core MLESAC loop
  bool finished = false;  // Don't use this yet but will
  VW::EssentialMatrixCompute5Point compute5point(false);
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

	 std::vector<VW::EssentialMatrix> Es;
	 Es.reserve(10);
	 compute5point.Compute(Es,
								  norm_x0,norm_x1,
								  sample_indices);
	 
	 if (VERBOSE>1) std::cerr << "3" << std::endl;
	 if (VERBOSE>1) std::cerr << "Got " << Es.size() << " E matrices." << std::endl;
	 
	 // In this case, since we might have more than one E, iterate over them
	 for (unsigned int e=0;e<Es.size();e++) {
		// Get inlier likelihoods and calculate total inlier score
		inliers.clear();
		const double inlierscore = MLESACGetInliersPoints(norm_x0, norm_x1, 
																		  Es[e],
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
		  best_inliers = inliers;
		
		  double best_inlier_frac = double(inliers.size()) / double(num_features);
		  // 99% confidence condition
		  this_max_iters = static_cast<unsigned int>(log_conf / log10(1 - pow(best_inlier_frac,
																									 int(sample_size))));
		  if ((max_iters>0) && (this_max_iters>max_iters)) {
			 this_max_iters = max_iters;
		  }
		
		  //		  if (VERBOSE)
			 std::cerr << "New max_iters = " << this_max_iters 
						  << " (already done "<<i<<")"<<std::endl;
		} // End if best
	 } // End for Es
  } // End main loop
  
  // Now use best inlier set to calculate the transform properly
  inliers = best_inliers;
  if (VERBOSE)  {
	 std::cout << func_name << " Recalculating transform from ";
	 std::cout << inliers.size () << " inliers" << std::endl;
  }

  VW::EssentialMatrixComputeLinear computelinear(false);
  computelinear.Compute(result,norm_x0,norm_x1,inliers);
  }
} } // End namespaces








bool VW::EssentialMatrixComputeMLESAC::
Compute(VW::EssentialMatrix& result,
		  const std::vector<VW::Point2D>& x0,
		  const std::vector<VW::Point2D>& x1,
		  std::vector<unsigned int>& inliers)
{

  
  // Check the lists sizes are equal
  const unsigned int num_features = x0.size();
  if (x0.size() != x1.size()) {
	 std::cerr << "VW::EssentialMatrixComputeMLESAC - feature lists are different sizes (" << x0.size() << " != " << x1.size() << ")" << std::endl;
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
  VW::EssentialMatrixCompute::MLESACCore(result,
													  norm_x0, norm_x1,
													  inliers,
													  _desired_confidence,
													  _max_iters,
													  _image_noise*s,
													  _search_area*s*s);
  
  // Un-normalise the result
  result = n1.AsMatrix().Transpose() * result * n0.AsMatrix ();
  
  if (VERBOSE)  std::cout << "VW::EssentialMatrixComputeMLESAC: Done" << std::endl;
  
  }
