/** 
 * @file    plesseycorneryuv.cpp
 * @author  BJ Tordoff
 * @date    04/08/2004
 *
 *              (c) Copyright University of Cambridge
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
#include "plesseycorneryuv.h"

class CompareX {
public:
  int operator()(const VW::PlesseyCornerYUV& c1,
		 const VW::PlesseyCornerYUV& c2) const
  {
    return c1.GetX()<c2.GetX();
  }	 
};

class CompareY {
public:
  int operator()(const VW::PlesseyCornerYUV& c1,
		 const VW::PlesseyCornerYUV& c2) const
  {
    return c1.GetY()<c2.GetY();
  }	 
};

class CompareStrength {
public:
  int operator()(const VW::PlesseyCornerYUV& c1,
		 const VW::PlesseyCornerYUV& c2) const
  {
    return c1.GetCornerStrength()>c2.GetCornerStrength();
  }	 
};



void VW::SortByStrength(std::vector<PlesseyCornerYUV>& list, 
			bool strongest_first)
{
  if (strongest_first) {
    // Use the "CompareStrength" function-object for descending strength vals
    std::sort(list.begin(),list.end(),CompareStrength());
  } else {
    // Use the built in >/< operators (smallest strength first)
    std::sort(list.begin(),list.end());
  }
}
void VW::SortByYValue(std::vector<PlesseyCornerYUV>& list)
{
  // Use the "CompareY" function-object to get ascending Y vals
  std::sort(list.begin(),list.end(),CompareY());
}

void VW::SortByXValue(std::vector<PlesseyCornerYUV>& list)
{
  // Use the "CompareX" function-object to get ascending X vals
  std::sort(list.begin(),list.end(),CompareX());
}
