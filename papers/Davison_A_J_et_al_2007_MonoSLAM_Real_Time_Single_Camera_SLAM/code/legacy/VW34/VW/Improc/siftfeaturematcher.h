#ifndef _siftfeaturematcher_h_
#define _siftfeaturematcher_h_

#include "siftfeature.h"
#include "cornermatcherbase.h"

namespace VW {
  class SIFTFeatureMatcher : public CornerMatcherBase<VW::SIFTFeature> {
  public:
    /** Construct and compute specifying all params. */
    SIFTFeatureMatcher(const MatcherType matcher_type = MatcherParams::DEF_TYPE,
			 const float maximum_range = MatcherParams::DEF_MAX_RANGE,
			 const float score_threshold = MatcherParams::DEF_SCORE_THRESH);
    /** Construct and compute specifying all params. */
    SIFTFeatureMatcher(const std::vector<VW::SIFTFeature>& plnew,
			 const std::vector<VW::SIFTFeature>& plold,
			 std::vector<VW::MatchData>& matchlist,
			 const MatcherType matcher_type = MatcherParams::DEF_TYPE,
			 const float maximum_range = MatcherParams::DEF_MAX_RANGE,
			 const float score_threshold = MatcherParams::DEF_SCORE_THRESH);


  protected:
    float GetMatchScore(const VW::SIFTFeature& f1, 
			const VW::SIFTFeature& f2) const;
    void MatchScoresToProbs(std::vector<std::vector<VW::MatchData> >& potentialmatches) const;
  };
}; // End namespace VW







inline float VW::SIFTFeatureMatcher::
GetMatchScore(const VW::SIFTFeature& f1, 
	      const VW::SIFTFeature& f2) const
{
  return float(f1.GetMatchScore(f2));
}


#endif
