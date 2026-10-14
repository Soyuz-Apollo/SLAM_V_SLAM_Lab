#ifndef _patchfeaturematcher_h_
#define _patchfeaturematcher_h_

#include "patchfeature.h"
#include "cornermatcherbase.h"

namespace VW {
  class PatchFeatureMatcher : public CornerMatcherBase<VW::PatchFeature> {
  public:
    /** Construct and compute specifying all params. */
    PatchFeatureMatcher(const MatcherType matcher_type = MatcherParams::DEF_TYPE,
			 const float maximum_range = MatcherParams::DEF_MAX_RANGE,
			 const float score_threshold = MatcherParams::DEF_SCORE_THRESH);
    /** Construct and compute specifying all params. */
    PatchFeatureMatcher(const std::vector<VW::PatchFeature>& plnew,
			 const std::vector<VW::PatchFeature>& plold,
			 std::vector<VW::MatchData>& matchlist,
			 const MatcherType matcher_type = MatcherParams::DEF_TYPE,
			 const float maximum_range = MatcherParams::DEF_MAX_RANGE,
			 const float score_threshold = MatcherParams::DEF_SCORE_THRESH);


  protected:
    float GetMatchScore(const VW::PatchFeature& f1, 
			const VW::PatchFeature& f2) const;
    void MatchScoresToProbs(std::vector<std::vector<VW::MatchData> >& potentialmatches) const;
  };
}; // End namespace VW







inline float VW::PatchFeatureMatcher::
GetMatchScore(const VW::PatchFeature& f1, 
	      const VW::PatchFeature& f2) const
{
  return float(f1.GetMatchScore(f2));
}


#endif
