/** 
 * @file   cornermatcherbase.h
 * @author BJ Tordoff
 * @date   14/01/2000
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
/** 
 * @class   VW::CornerMatcherBase
 * @ingroup ImageProcessing
 * @author  BJ Tordoff
 * @date    14/01/2000
 *
 * Abstract base class for cornermatchers, providing much of the shared
 * functionality.
 */

#ifndef _cornermatcherbase_h
#define _cornermatcherbase_h

// System includes
#include <iostream>
#include <vector>

#include "matchdata.h"

namespace VW {
  
  // Types
  enum MatcherType {
    BRAINDEAD_MATCHER=0,
    REPEATEDLY_STUPID_MATCHER
  };

  namespace MatcherParams {
    const float DEF_SCORE_THRESH = 0.5;
    const float DEF_MAX_RANGE = 10.0;
    const VW::MatcherType DEF_TYPE = VW::REPEATEDLY_STUPID_MATCHER;
  }



template <class Feature>
class CornerMatcherBase {
public:


  // Constructors
  //  - Default (params only)
  CornerMatcherBase(const MatcherType matcher_type = MatcherParams::DEF_TYPE,
		    const float maximum_range = MatcherParams::DEF_MAX_RANGE,
		    const float score_threshold = MatcherParams::DEF_SCORE_THRESH);

  //  - Two feature lists 
  CornerMatcherBase(const std::vector<Feature>& plnew,
		    const std::vector<Feature>& plold,
		    std::vector<MatchData>& matchlist,
		    const MatcherType matcher_type = MatcherParams::DEF_TYPE,
		    const float maximum_range = MatcherParams::DEF_MAX_RANGE,
		    const float score_threshold = MatcherParams::DEF_SCORE_THRESH);


  virtual ~CornerMatcherBase() {};

  operator bool() const { return true; }

  /** 
   * Compute using two corner lists.
   */
  void Compute(const std::vector<Feature> &plnew,
	       const std::vector<Feature> &plold,
	       std::vector<MatchData>& matchlist) const;

  // Form a lookup table style match list
  void FormLookupTable(const std::vector<MatchData>& matchlist,
		       std::vector<int>& list1, 
		       std::vector<int>& list2) const;

  // Parameter adjustments
  void SetMaximumRange(const float new_max_range);
  void SetScoreThreshold(const float score_thresh);

protected:
  // First the functions that derrived classes must overload:
  virtual float
  GetMatchScore(const Feature& f1, const Feature& f2) const = 0;
  virtual void 
  MatchScoresToProbs(std::vector<std::vector<MatchData> >& potentialmatches) const = 0;

  // Now other funcs that are used internally (and may be overloaded as 
  // required)

  /** Use GetMatchScore to obtain a set of potential matches for each point */
  void FindPotentialMatches(const std::vector<Feature>& old_pointlist, 
			    const std::vector<Feature>& new_pointlist,
			    std::vector<std::vector<MatchData> >& potentialmatches) const;
  /** The actual matchers - 1. a pretty daft but quick matcher. 
   * This one takes the list of potential matches for each
   * feature and selects the best, resolving conflicts by first-come-
   * first-served.
   */
  bool BraindeadMatcher(const std::vector<std::vector<MatchData> >& potentialmatches,
			const unsigned int number_in_list1,
			const unsigned int number_in_list2,
			std::vector<MatchData>& matchlist) const;
  /** The actual matchers - 2. a simple iterative matcher.
   * This one takes the list of potential matches for each
   * feature and selects the best, resolving conflicts by iteration
   * (usually only needs 3-4 passes).
   */
  bool RepeatedlyStupidMatcher(const std::vector<std::vector<MatchData> >& potentialmatches,
			       const unsigned int number_in_list1,
			       const unsigned int number_in_list2,
			       std::vector<MatchData>& matchlist) const;

private:

  // The data for 'compute'
  MatcherType my_matcher_type;
  float my_maximum_range;
  float my_score_threshold;
};




}; // End of namespace VW








// Inlines are below


// This is an attempt to make it reasonably easy to change number/name of
// parameters and their defaults in only one part of the file


// Default constructor.  If parameter file is provided then constructor
// will get any values from that it can.
template <class Feature>
inline VW::CornerMatcherBase<Feature>::
CornerMatcherBase(const MatcherType matcher_type,
		    const float maximum_range,
		    const float score_threshold) :
  my_matcher_type (matcher_type),
  my_maximum_range (maximum_range), 
  my_score_threshold (score_threshold)
{
}

// Compute constructor - two corner lists (but no images)
template <class Feature>
inline VW::CornerMatcherBase<Feature>::CornerMatcherBase
(const std::vector<Feature>& plnew,
 const std::vector<Feature>& plold,
 std::vector<VW::MatchData>& matchlist,
 const VW::MatcherType matcher_type, 
 const float maximum_range, 
 const float score_threshold) :
  my_matcher_type (matcher_type),
  my_maximum_range (maximum_range), 
  my_score_threshold (score_threshold)
{
  Compute(plnew, plold, matchlist);
}



template <class Feature>
inline void VW::CornerMatcherBase<Feature>::FormLookupTable
(const std::vector<VW::MatchData>& matchlist,
 std::vector<int>& list1,
 std::vector<int>& list2) const
{
  list1.assign (list1.size(), -1);
  list2.assign (list2.size(), -1);

  for (std::vector<VW::MatchData>::const_iterator it = matchlist.begin();
       it != matchlist.end(); it++) {
    unsigned int old = it->old_index;
    unsigned int nue = it->new_index;
    if (old >= list1.size()) list1.resize (old, -1);
    if (nue >= list2.size()) list2.resize (nue, -1);
    list1[old] = nue;
    list2[nue] = old;
  }
}

template <class Feature> inline void 
VW::CornerMatcherBase<Feature>::SetMaximumRange(const float new_max_range) 
{
  my_maximum_range=new_max_range;
}

template <class Feature> inline void 
VW::CornerMatcherBase<Feature>::SetScoreThreshold(const float new_score_thresh)
{
  my_score_threshold=new_score_thresh;
}




#endif
