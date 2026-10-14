/** 
 * @file    cornerfeature.cpp
 * @author  ID Reid and BJ Tordoff
 * @date    02/08/2002
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

// Standard includes
#include <algorithm> // for sort

// local includes
#include "cornerfeature.h"

void VW::SortByStrength(std::vector<CornerFeature>& list)
{
  // Use the built in >/< operators
  std::sort(list.begin(),list.end());
}

class CompareX {
public:
  int operator()(const VW::CornerFeature& c1,
		 const VW::CornerFeature& c2) const
  {
    return c1.GetX()<c2.GetX();
  }	 
};

class CompareY {
public:
  int operator()(const VW::CornerFeature& c1,
		 const VW::CornerFeature& c2) const
  {
    return c1.GetY()<c2.GetY();
  }	 
};


class CompareStrength {
public:
  int operator()(const VW::CornerFeature& c1,
					  const VW::CornerFeature& c2) const
  {
    return c1.GetCornerStrength()>c2.GetCornerStrength();
  }	 
};


void VW::SortStrongestFirst(std::vector<CornerFeature>& list)
{
  // Use the "CompareY" function-object to get ascending Y vals
  std::sort(list.begin(),list.end(),CompareStrength());
}

void VW::SortByYValue(std::vector<CornerFeature>& list)
{
  // Use the "CompareY" function-object to get ascending Y vals
  std::sort(list.begin(),list.end(),CompareY());
}

void VW::SortByXValue(std::vector<CornerFeature>& list)
{
  // Use the "CompareX" function-object to get ascending X vals
  std::sort(list.begin(),list.end(),CompareX());
}
