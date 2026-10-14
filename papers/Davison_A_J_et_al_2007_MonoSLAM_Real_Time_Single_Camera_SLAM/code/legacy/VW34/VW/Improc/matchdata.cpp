/** 
 * @file   matchdata.h
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
 * 2/12/04: Separated from cornermatcher.cpp
 *
 */
#include <algorithm> // for sort


#include "matchdata.h"


/*************************/
/* Some useful functions */
/*************************/
class CompareLower {
public:
  int operator()(const VW::MatchData& m1,
		 const VW::MatchData& m2) const
  {
    return m1.GetScore()<m2.GetScore();
  }	 
};

class CompareHigher {
public:
  int operator()(const VW::MatchData& m1,
		 const VW::MatchData& m2) const
  {
    return m1.GetScore()>m2.GetScore();
  }	 
};

void VW::SortAscending(std::vector<MatchData>& list)
{
  // Use the "CompareY" function-object to get ascending scores
  std::sort(list.begin(),list.end(),CompareLower());
}

void VW::SortDescending(std::vector<MatchData>& list)
{
  // Use the "CompareHigher" function-object to get descending scores
  std::sort(list.begin(),list.end(),CompareHigher());
}
