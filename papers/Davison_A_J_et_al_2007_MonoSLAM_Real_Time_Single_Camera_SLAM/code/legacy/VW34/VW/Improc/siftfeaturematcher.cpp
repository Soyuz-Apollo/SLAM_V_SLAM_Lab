#include <VW/MathBits/pdfgaussian1d.h>
#include "cornermatcherbase.tpp"
#include "siftfeaturematcher.h"

using namespace std;

VW::SIFTFeatureMatcher::
SIFTFeatureMatcher(const std::vector<VW::SIFTFeature>& plnew,
		     const std::vector<VW::SIFTFeature>& plold,
		     std::vector<VW::MatchData>& matchlist,
		     const MatcherType matcher_type,
		     const float maximum_range,
		     const float score_threshold)
  : VW::CornerMatcherBase<VW::SIFTFeature>(matcher_type, 
					     maximum_range, 
					     score_threshold)
{
  Compute(plnew,plold,matchlist);
}

VW::SIFTFeatureMatcher::
SIFTFeatureMatcher(const MatcherType matcher_type,
		     const float maximum_range,
		     const float score_threshold)
  : VW::CornerMatcherBase<VW::SIFTFeature>(matcher_type, 
					     maximum_range, 
					     score_threshold)
{
}

/** Convert angles to probabilities
 * We use the follwing approximations to the inlier/outlier PDFs
 * Out: 1D gaussian with mean 0.18, variance 0.04
 * In:  1D exp with std dev 0.1 ->  pdf(s|v) = exp((s-1)/sigma) / sigma
 */
void VW::SIFTFeatureMatcher::
MatchScoresToProbs(vector<vector<VW::MatchData> >& potentialmatches) const
{
  static const VW::PDF::Gaussian1D out_pdf(0.18,0.04);
  static const double inv_std_out = 10;

  if (VERBOSE) cerr << "Converting match scores to probabilities..." << endl;

  // Now go through each set of potential matches
  for (vector<vector<MatchData> >::iterator i=potentialmatches.begin();
       i!=potentialmatches.end();
	 i++) {
    if (i->size()==0) {
      // No matches, so ignore
    } else if (i->size()==1) {
      // Only one match, so just convert this one
      const double x = (*i)[0].GetScore();
      double outlier_score = out_pdf.EvaluateDensityForValue(x);
      double inlier_score = inv_std_out * exp((x-1)*inv_std_out);
      (*i)[0].SetScore(float(inlier_score / (outlier_score + inlier_score)));
    } else {
      // First sum up the outlier scores
      double all_outliers_score = 1.0;
      vector<double> outlier_scores;
      vector<double> inlier_scores;
      outlier_scores.reserve(i->size());
      inlier_scores.reserve(i->size());
      for (vector<MatchData>::iterator j=i->begin();
	     j!=i->end();
	     j++) {
	const double x = j->GetScore();
	double outlier_score = out_pdf.EvaluateDensityForValue(x);
	double inlier_score = inv_std_out * exp((x-1)*inv_std_out);
	if (VERBOSE) cerr << "Score="<<j->GetScore()
			  <<"   Inlier="<<inlier_score
			  <<"   Outlier="<<outlier_score<<endl;
	outlier_scores.push_back(outlier_score);
	inlier_scores.push_back(inlier_score);
	all_outliers_score*=outlier_score;
      }
      if (VERBOSE) cerr << "All outliers prob="<<all_outliers_score<<endl;
      // Now go through and do the Bayesian thang
      double total_possibility=all_outliers_score;
      for (unsigned int index=0;index<inlier_scores.size();index++) {
	// Top is prob that this is valid and all others aren't
	inlier_scores[index] = inlier_scores[index] 
	                            * all_outliers_score
	                            / outlier_scores[index];
	total_possibility += inlier_scores[index];
      }
      int index2=0;
      if (VERBOSE) cerr << "Probs: ";
      for (vector<MatchData>::iterator j=i->begin();
	     j!=i->end();
	     j++,index2++) {
	 j->SetScore(float(inlier_scores[index2] / total_possibility));
	 if (VERBOSE) cerr << j->GetScore() << " ";
       }
      if (VERBOSE) cerr << endl;
    }
  }
}
