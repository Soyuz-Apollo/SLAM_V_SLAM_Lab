/** 
 * @file     sampling.cpp
 * @author   BJ Tordoff
 * @date     02/08/2002
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
#include <algorithm> // for sort

#include "sampling.h"


namespace VW {
class RandItem {
public:

  // Some stuff for sorting
  bool operator<(const RandItem& r) const {return (rank < r.rank);}
  bool operator>(const RandItem& r) const {return (rank > r.rank);}
  bool operator==(const RandItem& r) const {return (rank == r.rank);}

public: // Data
  double rank;
  int index;
};
}; // End namespace VW

void VW::CreateRandomisedList(std::vector<unsigned int>& indices,
										unsigned int max_index)
{
  std::vector<VW::RandItem> randomlist(max_index);
  for (unsigned int i=0;i<max_index;i++) {
	 randomlist[i].index=i;
	 randomlist[i].rank = double (rand()) / (double(RAND_MAX) + 1.0);
  }
  std::sort(randomlist.begin(), randomlist.end());
  
  indices.clear();
  indices.reserve(max_index);
  for (unsigned int i=0;i<max_index;i++) 
	 indices.push_back(randomlist[i].index);
}

