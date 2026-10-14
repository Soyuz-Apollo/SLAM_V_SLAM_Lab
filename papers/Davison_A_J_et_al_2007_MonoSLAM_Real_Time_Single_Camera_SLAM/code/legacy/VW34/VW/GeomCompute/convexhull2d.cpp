/** 
 * @file   convexhull2d.cpp
 * @author BJ Tordoff
 * @date   03/02/2004		   		
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
#include <algorithm> // for sort()
#include "convexhull2d.h"

using namespace std;

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE 0


class IndexedPoint : public VW::Point2D {
public:
  IndexedPoint(const VW::Point2D& p, unsigned int index)
    : VW::Point2D(p), _index(index)
  {
  }

  bool operator<(const IndexedPoint& p) const
  {
    return this->GetY() < p.GetY();
  }

  unsigned int _index;
};



/** Return positive if p-q-r are clockwise, neg if ccw, zero if colinear. */
double VW::ConvexHull2D::_IsClockwise(const VW::Point2D& p,
			      const VW::Point2D& q,
			      const VW::Point2D& r) const 
{
  return ((q.GetY()-p.GetY())*(r.GetX()-p.GetX())) 
             - ((q.GetX()-p.GetX())*(r.GetY()-p.GetY()));
}

VW::ConvexHull2D::ConvexHull2D()
  : _have_hull(false)
{
}

VW::ConvexHull2D::ConvexHull2D(const std::vector<VW::Point2D>& input_points,
			       std::vector<VW::Point2D>& output_points)
  : _have_hull(false)
{
  Compute(input_points,output_points);
}

VW::ConvexHull2D::ConvexHull2D(const std::vector<VW::Point2D>& input_points,
			  std::vector<unsigned int>& hull_indices)
  : _have_hull(false)
{
  Compute(input_points,hull_indices);
}


VW::ConvexHull2D::ConvexHull2D(const std::vector<VW::Point2D>& input_points)
  : _have_hull(false)
{
  Compute(input_points,_hull);
}

bool 
VW::ConvexHull2D::Compute(const std::vector<VW::Point2D>& input_points,
			  std::vector<VW::Point2D>& output_points)
{
  std::vector<unsigned int> hull_indices;
  if (!Compute(input_points, hull_indices)) {
    std::cerr << "ConvexHull2D::Compute: Compute failed." << std::endl;
    return false;
  } else {

    const unsigned int n = hull_indices.size();
    output_points.clear();
    output_points.reserve(n);
    for (unsigned int i=0;i<n;i++) {
      output_points.push_back(input_points[hull_indices[i]]);
    }
  }
  return true;
}

bool 
VW::ConvexHull2D::Compute(const std::vector<VW::Point2D>& input_points,
		      std::vector<unsigned int>& hull_indices)
{
  using namespace VW;

  // Make sure the output array is big enough for at most the same number of points as
  // input
  const unsigned int num_points = input_points.size();
  hull_indices.clear();
  hull_indices.reserve(num_points);

  // Some temporary arrays
  std::vector<IndexedPoint> input_temp, left_hull, right_hull; 
  left_hull.reserve(num_points);
  right_hull.reserve(num_points);
  input_temp.reserve(num_points);

  // Copy the input points into indexed points for sorting (so that we can recover
  // the point indices later)
  for (unsigned int i=0;i<num_points;i++) 
    input_temp.push_back(IndexedPoint(input_points[i],i));

  std::sort(input_temp.begin(), input_temp.end());


  // Note that first point is top point, which is guaranteed to be on the hull
  left_hull.push_back(input_temp[0]);
  right_hull.push_back(input_temp[0]);
  unsigned int nlhull = 1; // number on left hull
  unsigned int nrhull = 1; // number on right hull

  // Now do a gift-wrap scan around the edge in both directions.
  for (unsigned int i=1;i<num_points;i++) {
    const IndexedPoint& p = input_temp[i];

    if (VERBOSE) cout << "Lists: "<<nlhull<<", "<<nrhull<<endl;

    // If this point turns us anticlockwise, backtrack until it doesn't
    while ((nrhull>1) 
	   && (_IsClockwise(right_hull[nrhull-2], right_hull[nrhull-1], p) <= 0)) 
      {
	if (VERBOSE) cout << "Removing point ("<<right_hull[nrhull-1]<<") from right." << endl;
	right_hull.pop_back();
	nrhull--;
      }

    // If this point turns us clockwise, backtrack until it doesn't
    while ((nlhull>1) 
	   && (_IsClockwise(left_hull[nlhull-2], left_hull[nlhull-1], p) >= 0)) 
      {
	if (VERBOSE) cout << "Removing point ("<<left_hull[nlhull-1]<<") from left." << endl;
	left_hull.pop_back();
	nlhull--;
      }

    // Now add this point
    left_hull.push_back(p);
    right_hull.push_back(p);
    nlhull++;
    nrhull++;
  
    if (VERBOSE) {
      cout << "Lists are:\n";
      for (unsigned int j=0;j<left_hull.size();j++) 
	cout << "   " << left_hull[j] << endl;
      cout << "and:\n";
      for (unsigned int j=0;j<right_hull.size();j++) 
	cout << "   " << right_hull[j] << endl;
    }
  }

  // Remove common paths from the end of the chains
  IndexedPoint discarded_point(VW::Point2D(0,0),0);
  bool have_discarded = false;
  while (left_hull[nlhull-1] == right_hull[nrhull-1]) {
    discarded_point=left_hull[nlhull-1];
    if (VERBOSE) cout << "Discard ("<<discarded_point<<")"<<endl;
    left_hull.pop_back();
    nlhull--;
    right_hull.pop_back();
    nrhull--;
    have_discarded = true;
  }
  // Add the final discarded point back onto one list
  if (have_discarded) right_hull.push_back(discarded_point);

  // Finally scan the hulls to get a single chain
  {
    vector<IndexedPoint>::const_iterator i = left_hull.begin();
    vector<IndexedPoint>::const_iterator ie = left_hull.end();
    for ( ; i != ie ; i++) {
      hull_indices.push_back(i->_index);
    }
  } {
    vector<IndexedPoint>::const_reverse_iterator i = right_hull.rbegin();
    vector<IndexedPoint>::const_reverse_iterator ie = right_hull.rend();
    ie--; // Don't include the first entry (already at start of list)
    for ( ; i != ie ; i++) {
      hull_indices.push_back(i->_index);
    }
  }
  
  if (VERBOSE) {
    cout << "Final list:\n";
    for (unsigned int j=0;j<hull_indices.size();j++) 
      cout << "   " << hull_indices[j] << endl;
  }

  _have_hull = true;

  return (hull_indices.size()>2);
  
}





