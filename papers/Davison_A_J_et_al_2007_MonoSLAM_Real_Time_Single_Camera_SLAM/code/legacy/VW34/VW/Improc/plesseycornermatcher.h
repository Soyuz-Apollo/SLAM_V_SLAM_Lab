#ifndef _plesseycornermatcher_h_
#define _plesseycornermatcher_h_

#include "plesseycorner.h"
#include "cornermatcherbase.h"

namespace VW {
  class PlesseyCornerMatcher : public CornerMatcherBase<VW::PlesseyCorner> {
  public:
    /** Construct and compute specifying all params. */
    PlesseyCornerMatcher(const MatcherType matcher_type = MatcherParams::DEF_TYPE,
			 const float maximum_range = MatcherParams::DEF_MAX_RANGE,
			 const float score_threshold = MatcherParams::DEF_SCORE_THRESH);
    /** Construct and compute specifying all params. */
    PlesseyCornerMatcher(const std::vector<VW::PlesseyCorner>& plnew,
			 const std::vector<VW::PlesseyCorner>& plold,
			 std::vector<VW::MatchData>& matchlist,
			 const MatcherType matcher_type = MatcherParams::DEF_TYPE,
			 const float maximum_range = MatcherParams::DEF_MAX_RANGE,
			 const float score_threshold = MatcherParams::DEF_SCORE_THRESH);


  protected:
    float GetMatchScore(const VW::PlesseyCorner& f1, 
			const VW::PlesseyCorner& f2) const;
    void MatchScoresToProbs(std::vector<std::vector<VW::MatchData> >& potentialmatches) const;
  };
}; // End namespace VW







inline float VW::PlesseyCornerMatcher::
GetMatchScore(const VW::PlesseyCorner& f1, 
	      const VW::PlesseyCorner& f2) const
{
  return f1.GetImagePatch()->GetZNCC(*(f2.GetImagePatch()));
}


#endif
