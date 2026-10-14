/** 
 * @file   cornermatcherbase.tpp
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

#ifndef _cornermatcherbase_tpp
#define _cornermatcherbase_tpp

#include "cornermatcherbase.h"
#include <cmath>

#define VERBOSE false

using namespace std;

template <class Feature>
inline void VW::CornerMatcherBase<Feature>::
FindPotentialMatches(const vector<Feature>& old_pointlist, 
		     const vector<Feature>& new_pointlist,
		     vector<vector<MatchData> >& potentialmatches) const
{
  if (VERBOSE) 
    cerr << "   Finding potential matches (ver 2), threshold is "<<my_score_threshold<<" and max range " << my_maximum_range << endl;

  const float rangesquared=my_maximum_range*my_maximum_range;

  // Go through points in list 1 and find matches in list 2
  potentialmatches.resize(old_pointlist.size());
  vector<vector<MatchData> >::iterator matches=potentialmatches.begin();
  unsigned int old_index=0;
  for (typename vector<Feature>::const_iterator p1=old_pointlist.begin();
	p1!=old_pointlist.end();
	p1++,matches++,old_index++) {
    matches->clear();

    /* search for a match in the vicinity of the p1 */
    if (VERBOSE) cerr << "   Searching for matches for point "<<old_index<<" ("<<(*p1)<<")"<<endl;
    unsigned int new_index=0;
    for (typename vector<Feature>::const_iterator p2 = new_pointlist.begin();
	 p2 != new_pointlist.end();
	 p2++,new_index++) {
      const float xdist = float(p2->GetX() - p1->GetX());
      const float ydist = float(p2->GetY() - p1->GetY());
      if ((xdist*xdist + ydist*ydist) > rangesquared) {
	if (VERBOSE) cerr << "Out of range" << endl;
      } else {
	if (VERBOSE) 
	  cerr << "      Point "<<new_index<<" ("<<p2->GetX()<<","<<p2->GetY()<<") is in range ("<<sqrt(xdist*xdist + ydist*ydist)<<"), get score..."<<endl;
	const float match_score = GetMatchScore(*p1,*p2);
	  
	// Only add matches that score above the threshold
	if (match_score > my_score_threshold) {
	  matches->push_back(MatchData(old_index,new_index,match_score));
	  if (VERBOSE) 
	    cerr << "     Adding match ("<<old_index<<","<<new_index
		 <<") with score "<<match_score<<endl;
	}
      }
    }
    if (VERBOSE) cerr << "   Got "<<matches->size()<<" matches for point "<<old_index<<endl;
  }
}


template <class Feature>
inline void VW::CornerMatcherBase<Feature>::
Compute(const std::vector<Feature> &plnew,
	const std::vector<Feature> &plold,
	std::vector<MatchData>& matchlist) const
{
  if (VERBOSE) cerr << "VW::CornerMatcherBase::Compute..." << endl;

  // All matchers need potential matches from old->new
  std::vector<std::vector<MatchData> > forwardmatches;
  forwardmatches.reserve(plnew.size());

  FindPotentialMatches(plold, plnew, forwardmatches);
  unsigned int number_old=plold.size();
  unsigned int number_new=plnew.size();

  if (VERBOSE) cerr << "Got putative matches, now for the algorithm" << endl;

  switch (my_matcher_type) {
  case BRAINDEAD_MATCHER:
    BraindeadMatcher(forwardmatches,number_old,number_new,matchlist);
    break;
  case REPEATEDLY_STUPID_MATCHER:
    RepeatedlyStupidMatcher(forwardmatches,number_old,number_new,matchlist);
    break;
  }
}




template <class Feature>
inline bool VW::CornerMatcherBase<Feature>::
BraindeadMatcher(const vector<vector<MatchData> >& potentialmatches,
			  const unsigned int number_in_list1,
			  const unsigned int number_in_list2,
			  vector<MatchData>& matchlist) const
{
  // Scan through the potential matches, assigning a match to
  // the first unmatched one.

  if (VERBOSE) cerr << "I'm a braindead matcher." << endl;

  // Clear any existing matches
  matchlist.clear();

  // Set up an array to check off when they're matched
  vector<bool> is_matched_already;
  is_matched_already.resize(number_in_list2);
  for (unsigned int i=0;i<is_matched_already.size();i++) is_matched_already[i]=false;

  // GO!
  unsigned int index1=0;
  for (vector<vector<MatchData> >::const_iterator i=potentialmatches.begin();
       i!=potentialmatches.end();
       i++,index1++) {
    if (VERBOSE) cerr << "Examining point "<<index1<<" which has "<<i->size()<<" matches."<<endl;
    bool matched=false;
    MatchData best_match(0,0,0);
  unsigned int index2=0;
    for (vector<MatchData>::const_iterator j=i->begin();
	 j!=i->end();
	 j++,index2++) {
      if (VERBOSE) cerr << "   putative match "<<index2<<" has score "<<j->score<<"(best is "<<best_match.score<<") and status "<<is_matched_already[j->new_index]<<endl;
      if ((j->score > best_match.score) && (!is_matched_already[j->new_index])) {
	// This one is better and free
	best_match=(*j);
	matched=true;
	is_matched_already[j->new_index]=true;
      }
    }
    if (matched) matchlist.push_back(best_match);
  }

  if (VERBOSE) cerr << "Got "<< matchlist.size() << " matches overall." << endl;

  return(true);

}


template <class Feature>
inline bool VW::CornerMatcherBase<Feature>::
RepeatedlyStupidMatcher(const vector<vector<MatchData> >& potentialmatches,
			const unsigned int number_in_list1,
			const unsigned int number_in_list2,
			vector<MatchData>& matchlist) const
{
  // Scan through the potential matches, assigning a match to
  // the match with highest score. Where a conflict arises, the
  // previous match is displaced if this one scores higher, and
  // a new iteration is required to re-match the displaced match
  // to its second best choice.

  if (VERBOSE) cerr << "I'm a repeatedly stupid matcher." << endl;

  // Clear any existing matches
  matchlist.clear();

  // Set up an array to check off when they're matched
  vector<int> is_matched_already;
  is_matched_already.resize(number_in_list2);
   for (unsigned int i=0;i<is_matched_already.size();i++) is_matched_already[i]=-1;
   // Also, a list of the best match for each corner
  vector<MatchData> best_match;
  best_match.resize(number_in_list1);
  for (unsigned int i=0;i<best_match.size();i++) best_match[i].valid=false;

  // Now set up the main iteration
  bool there_is_a_conflict=true;
  unsigned int iteration=1;
  while (there_is_a_conflict) {
    there_is_a_conflict=false;
    if (VERBOSE) cerr << "Beginning iteration "<<iteration++<<endl;
    // GO!
    unsigned int index1=0;
    for (vector<vector<MatchData> >::const_iterator i=potentialmatches.begin();
	 i!=potentialmatches.end();
	 i++,index1++) {
      // Only check ones that don't currently have a valid match
      if (!(best_match[index1].valid)) {
	if (VERBOSE) cerr << "Examining point "<<index1<<" which has "<<i->size()<<" matches."<<endl;
	// Reset the score
	best_match[index1].score=0;
	// Set some flags
	bool matched=false;
	unsigned int index2=0;
	int existing_match_to_usurp=-1;
	for (vector<MatchData>::const_iterator j=i->begin();
	     j!=i->end();
	     j++,index2++) {
	  if (VERBOSE) cerr << "   putative match "<<index2
			    << " has score "<<j->score
			    <<" (best is "<<best_match[index1].score
			    <<") and is currently matched to "
			    <<is_matched_already[j->new_index]<<endl;
	  if (j->score > best_match[index1].score) {
	    // This match scores better, but we need to look for conflicts
	    int existing_match=is_matched_already[j->new_index];
	    if (existing_match==-1) {
	      // This is a free match, so match it!
	      existing_match_to_usurp=-1;
	      best_match[index1]=(*j);
	      best_match[index1].valid=true;
	      matched=true;
	    } else {
	      // There is a conflict, so only usurp if this match is stronger
	      if (VERBOSE) cerr << "   found conflict between this (score="
				<<j->score<<") and previous match to point "
				<<existing_match<<" (score="
				<<best_match[existing_match].score<<")"<<endl;
	      if (j->score > best_match[existing_match].score) {
		// This is the stronger match, so mark the existing 
		// match for usurping
		if (VERBOSE) cerr<<"      I win!"<<endl;
		existing_match_to_usurp=existing_match;
		best_match[index1]=(*j);
		best_match[index1].valid=true;
		matched=true;
	      }
	    }
	  }
	}
	if (matched) {
	  // Check if this match caused a conflict, and resolve
	  if (existing_match_to_usurp==-1) {
	    // no conflict
	    is_matched_already[best_match[index1].new_index]=index1;
	  } else {
	    if (VERBOSE) cerr << "   usurping point "<<existing_match_to_usurp<<endl;
	    best_match[existing_match_to_usurp].valid=false;
	    is_matched_already[best_match[index1].new_index]=index1;
	    there_is_a_conflict=true;
	  }
	} else {
	  // If it didn't match this time, it won't in future, so mark it
	  // to be ignored
	  best_match[index1].valid=true;
	}
      }
    } 
  } // End of while loop 
  
  // Now that the conflicts are all resolved, pile the bestmatches onto the matchlist
  for (vector<MatchData>::const_iterator i=best_match.begin();
       i!=best_match.end();
       i++) {
    if ((i->valid) && (i->score>0)) {
      matchlist.push_back(*i);
    }
  }
  if (VERBOSE) cerr << "Overall, I've found "<<matchlist.size()<<" matches."<<endl;
  return(true);
}



#endif
