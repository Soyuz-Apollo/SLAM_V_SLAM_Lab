#include "cornermatcherbase.tpp"
#include "patchfeaturematcher.h"

#define VERBOSE false
#define alpha 0.15 // Constant for PDF of ZNCC score

using namespace std;

VW::PatchFeatureMatcher::
PatchFeatureMatcher(const std::vector<VW::PatchFeature>& plnew,
		     const std::vector<VW::PatchFeature>& plold,
		     std::vector<VW::MatchData>& matchlist,
		     const MatcherType matcher_type,
		     const float maximum_range,
		     const float score_threshold)
  : VW::CornerMatcherBase<VW::PatchFeature>(matcher_type, 
					     maximum_range, 
					     score_threshold)
{
  Compute(plnew,plold,matchlist);
}

VW::PatchFeatureMatcher::
PatchFeatureMatcher(const MatcherType matcher_type,
		     const float maximum_range,
		     const float score_threshold)
  : VW::CornerMatcherBase<VW::PatchFeature>(matcher_type, 
					     maximum_range, 
					     score_threshold)
{
}

/** Convert ZNCC scores to probabilities as outlined in:
 * Tordoff:Murray:ECCV2002, Tordoff:PhDThesis2002
 */
void VW::PatchFeatureMatcher::
MatchScoresToProbs(vector<vector<VW::MatchData> >& potentialmatches) const
{
  if (VERBOSE) cerr << "Converting match scores to probabilities..." << endl;
  // First calculate the normalisation
  const double norm = 2 / (1-exp(-4.0/(alpha*alpha)));

  // Now go through each set of potential matches
  for (vector<vector<MatchData> >::iterator i=potentialmatches.begin();
       i!=potentialmatches.end();
	 i++) {
    if (i->size()==0) {
      // No matches, so ignore
    } else if (i->size()==1) {
      // Only one match, so just convert this one
      double x1 = (1 - (*i)[0].GetScore());
      double x2 = x1 / alpha;
      double outlier_score = (4/3) * x1 * x1;
      double inlier_score = (norm*x2/alpha)*exp(-(x2*x2));
      if (outlier_score<0.000001) outlier_score=0.000001;
      if ((*i)[0].GetScore()>0.99f) inlier_score=1.0f;
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
	double x1 = (1 - j->GetScore());
	double x2 = x1 / alpha;
	double outlier_score=(4/3) * x1 * x1;
	double inlier_score=((norm*x2/alpha)*exp(-(x2*x2)));
	if (VERBOSE) cerr << "Score="<<j->GetScore()
			  <<"   Inlier="<<inlier_score
			  <<"   Outlier="<<outlier_score<<endl;
	// prevent roundoff errors close to 0/0
	if (outlier_score<0.000001) outlier_score=0.000001;
	if (j->GetScore()>0.99) inlier_score=1.0;

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
