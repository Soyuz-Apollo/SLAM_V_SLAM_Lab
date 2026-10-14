/** 
 * @file   essentialmatrixcomputepreemptive.cpp
 * @author BJ Tordoff
 * @date   19/04/2005		   		
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
#include <iostream>
#include <algorithm> // for std::sort

#include <VW/GeomObjects/essentialmatrix.h>
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/line2d.h>
#include <VW/GeomObjects/normalisation.h>

#include <VW/MathBits/random.h>
#include <VW/MathBits/pdfgaussian1d.h>

#include <VW/Utils/timer.h>

#include "sampling.h"
#include "essentialmatrixcomputepreemptive.h"
#include "essentialmatrixcompute5point.h"
#include "essentialmatrixcomputelinear.h"
#include "essentialmatrixcompute.h"

using namespace std;
using namespace VW::EssentialMatrixCompute; // For utility funcs

namespace VW {

class EssHyp {
public:
  EssHyp() : score(0.0) {}
  EssHyp(const VW::EssentialMatrix& e, double s) : E(e), score(s) {}

  // Some stuff for sorting
  bool operator<(const EssHyp& E2) const {return (score < E2.score);}
  bool operator>(const EssHyp& E2) const {return (score > E2.score);}
  bool operator==(const EssHyp& E2) const {return (score == E2.score);}

public: // data
  VW::EssentialMatrix E;
  double score;

};

}; // End namespace VW;

bool VW::EssentialMatrixComputePreemptive::
Compute(VW::EssentialMatrix& E,
		  const std::vector<VW::Point2D>& x0,
		  const std::vector<VW::Point2D>& x1,
		  std::vector<double>& posteriors)
{
  VW::Timer t;t.Start();

  // Some constants
  static const unsigned int sample_size = 5;
  const unsigned int num_features = x0.size();

  const double max_hyp_time = 0.666*_max_time;

  // Normalise the features
  VW::Normalisation n0(x0);
  VW::Normalisation n1(x1);
  std::vector<VW::Point2D> norm_x0,norm_x1;
  for (unsigned int i=0;i<num_features;i++) {
	 norm_x0.push_back(n0.Normalise(x0[i]));
	 norm_x1.push_back(n1.Normalise(x1[i]));
  }
  const double norm_image_noise = n1.GetScale()*_image_noise;
  const double norm_search_area = n1.GetScale()*n1.GetScale()*_search_area;

  // Generate the hypotheses
  std::vector<EssHyp> hypotheses;
  hypotheses.reserve(_num_hyp);
  VW::EssentialMatrixCompute5Point compute5point(false);
  for (unsigned int i=0;i<_num_hyp;i++) {
	 std::vector<unsigned int> sample_indices;
	 VW::RandomSample (sample_size, num_features, sample_indices);
	 std::vector<VW::EssentialMatrix> Es;
	 compute5point.Compute(Es,
								  norm_x0,norm_x1,
								  sample_indices);		
	 for (unsigned int e=0;e<Es.size();e++) hypotheses.push_back(EssHyp(Es[e],0));
	 if ((max_hyp_time>=0.0) && (t.GetAsMilliseconds() > max_hyp_time)) {
		break;
	 }
  }
  const unsigned int n_h = hypotheses.size();

  // Now get a randomised order for testing the features
  std::vector<unsigned int> feature_index;
  CreateRandomisedList(feature_index, num_features);
  
  // Work out the truncation step (linear removal initially)
  const unsigned int step_size = 2*sample_size;
  const unsigned int num_steps = num_features / step_size;
  const unsigned int num_to_remove = n_h / (num_steps+1);
  
  // Some constants for the loop
  const VW::PDF::Gaussian1D G(0.0, norm_image_noise*norm_image_noise);
  const double U = 1/norm_search_area;

  unsigned int next_truncation_iter = step_size;
  unsigned int iter=0;
  for (;iter<num_features;iter++) {

	 if ((_max_time>=0.0) && (t.GetAsMilliseconds() > _max_time)) {
		break;
	 }

	 // Check if it's time to truncate
	 if (iter == next_truncation_iter) {
		next_truncation_iter += step_size;
		std::sort(hypotheses.begin(), hypotheses.end());
		hypotheses.erase(hypotheses.end()-num_to_remove, hypotheses.end());
	 }

	 const VW::Point2D nx0 = norm_x0[feature_index[iter]];
	 const VW::Point2D nx1 = norm_x1[feature_index[iter]];
	 const double out_like = U;
	 const double log_out = log(out_like);

	 // Scan down the hypotheses, evaluating the likelihood
	 for (unsigned int j=0;j<hypotheses.size();j++) {
		const VW::Line2D epipolar_line = hypotheses[j].E.Transfer(nx0);
	 
		// Is it within X & Y tolerance
		const double error_squared = epipolar_line.GetDistance2(nx1);
		const double in_like = G.ApproxDensityForError2(error_squared);

		if (in_like < (10.0 * out_like)) {
		  hypotheses[j].score -= log_out;
		} else {
		  hypotheses[j].score -= log(in_like + out_like);
		}
	 }
  }
  //cerr << "Stopped after " << iter << " features" << endl;


  // The 1st ranked hypothesis is the solution and is used to asses inliers (assume prior=0.5)
  double best_score = hypotheses[0].score;
  E = hypotheses[0].E;
  for (unsigned int i=1;i<hypotheses.size();i++) {
	 if (hypotheses[i].score < best_score) {
		best_score = hypotheses[i].score;
		E = hypotheses[i].E;
	 }
  }
  posteriors.resize(num_features);
  const double out_like = U; // prior=0.5
  for (unsigned int i=0;i<num_features;i++) {
	 const VW::Point2D nx0 = norm_x0[i];
	 const VW::Point2D nx1 = norm_x1[i];

	 const VW::Line2D epipolar_line = E.Transfer(nx0);
	 const double error_squared = epipolar_line.GetDistance2(nx1);
	 const double in_like = G.ApproxDensityForError2(error_squared);
	 posteriors[i] = in_like / (in_like + out_like);
  }
  
  // Recalculate from full dataset using posteriors
  VW::EssentialMatrixComputeLinear computelinear(false);
  computelinear.ComputeWeighted(E,norm_x0,norm_x1,posteriors);

  Unnormalise(E,n0,n1);

  return true;

};
bool VW::EssentialMatrixComputePreemptive::Compute(VW::EssentialMatrix& E,
																 const std::vector<VW::Point2D>& x0,
																 const std::vector<VW::Point2D>& x1,
																 const std::vector<double>& priors,
																 std::vector<double>& posteriors)
{
  VW::Timer t;t.Start();

  // Some constants
  static const unsigned int sample_size = 5;
  const unsigned int num_features = x0.size();
  const double max_hyp_time = 0.666*_max_time;

  // Normalise the features
  VW::Normalisation n0(x0);
  VW::Normalisation n1(x1);
  std::vector<VW::Point2D> norm_x0,norm_x1;
  for (unsigned int i=0;i<num_features;i++) {
	 norm_x0.push_back(n0.Normalise(x0[i]));
	 norm_x1.push_back(n1.Normalise(x1[i]));
  }
  const double norm_image_noise = n1.GetScale()*_image_noise;
  const double norm_search_area = n1.GetScale()*n1.GetScale()*_search_area;

  // Generate the hypotheses
  std::vector<EssHyp> hypotheses;
  hypotheses.reserve(_num_hyp);
  VW::EssentialMatrixCompute5Point compute5point(false);
  std::vector<double> cumulative_prior;
  VW::AccumulateWeights(priors, cumulative_prior);
  

  for (unsigned int i=0;i<_num_hyp;i++) {
	 std::vector<unsigned int> sample_indices;
	 VW::WeightedSample (sample_size, cumulative_prior, sample_indices);
	 std::vector<VW::EssentialMatrix> Es;
	 compute5point.Compute(Es,
								  norm_x0,norm_x1,
								  sample_indices);		
	 for (unsigned int e=0;e<Es.size();e++) hypotheses.push_back(EssHyp(Es[e],0));
	 if ((max_hyp_time>=0.0) && (t.GetAsMilliseconds() > max_hyp_time)) {
		break;
	 }
  }
  const unsigned int n_h = hypotheses.size();
  //  cerr << "   generated " << hypotheses.size() << " hypotheses." << endl;
  
  // Now get a randomised order for testing the features
  std::vector<unsigned int> feature_index;
  CreateRandomisedList(feature_index, num_features);

  // Work out the truncation step (linear removal initially)
  const unsigned int step_size = 2*sample_size;
  const unsigned int num_steps = num_features / step_size;
  const unsigned int num_to_remove = n_h / (num_steps+1);
  
  // Some constants for the loop
  const VW::PDF::Gaussian1D G(0.0, norm_image_noise*norm_image_noise);
  const double U = 1/norm_search_area;

  unsigned int next_truncation_iter = step_size;
  unsigned int iter=0;
  for (  ; iter<num_features ; iter++) {

	 if ((_max_time>=0.0) && (t.GetAsMilliseconds() > _max_time)) {
		break;
	 }

	 // Check if it's time to truncate
	 if (iter == next_truncation_iter) {
		//		cerr << "Truncate (currently " << hypotheses.size() << ")" << endl;
		next_truncation_iter += step_size;

		std::sort(hypotheses.begin(), hypotheses.end());
		hypotheses.erase(hypotheses.end()-num_to_remove, hypotheses.end());
	 }

	 const VW::Point2D nx0 = norm_x0[feature_index[iter]];
	 const VW::Point2D nx1 = norm_x1[feature_index[iter]];
	 const double prior = priors[feature_index[iter]];
	 const double out_like = (1.0 - prior) * U;
	 const double log_out = log(out_like);

	 // Scan down the hypotheses, evaluating the likelihood
	 for (unsigned int j=0;j<hypotheses.size();j++) {
		const VW::Line2D epipolar_line = hypotheses[j].E.Transfer(nx0);
	 
		// Is it within X & Y tolerance
		const double error_squared = epipolar_line.GetDistance2(nx1);
		const double in_like = prior * G.ApproxDensityForError2(error_squared);

		if (in_like < (10.0 * out_like)) {
		  hypotheses[j].score -= log_out;
		} else {
		  hypotheses[j].score -= log(in_like + out_like);
		}
	 }
  }
  //cerr << "Stopped after " << iter << " features" << endl;

  // The 1st ranked hypothesis is the solution and is used to asses inliers
  double best_score = hypotheses[0].score;
  E = hypotheses[0].E;
  for (unsigned int i=1;i<hypotheses.size();i++) {
	 if (hypotheses[i].score < best_score) {
		best_score = hypotheses[i].score;
		E = hypotheses[i].E;
	 }
  }
  posteriors.resize(priors.size());
  for (unsigned int i=0;i<num_features;i++) {
	 const VW::Point2D nx0 = norm_x0[i];
	 const VW::Point2D nx1 = norm_x1[i];
	 const double prior = priors[i];
	 const double out_like = (1.0 - prior) * U;

	 const VW::Line2D epipolar_line = E.Transfer(nx0);
	 const double error_squared = epipolar_line.GetDistance2(nx1);
	 const double in_like = prior * G.ApproxDensityForError2(error_squared);
	 posteriors[i] = in_like / (in_like + out_like);
  }

  // Recalculate from full dataset using posteriors
  VW::EssentialMatrixComputeLinear computelinear(false);
  computelinear.ComputeWeighted(E,norm_x0,norm_x1,posteriors);

  Unnormalise(E,n0,n1);

  return true;

};
