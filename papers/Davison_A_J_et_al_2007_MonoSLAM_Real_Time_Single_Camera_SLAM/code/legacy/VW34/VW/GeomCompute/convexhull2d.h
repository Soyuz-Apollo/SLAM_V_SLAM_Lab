/** 
 * @file   convexhull2d.h
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
/**
 * @class    VW::ConvexHull2D
 * @ingroup  GeometricAlgorithms
 * @author   BJ Tordoff
 * @date     03/02/2004		   		
 * @brief    Computation of the convex hull of a set of 2D points.
 *
 * The input is always a set of VW::Point2D's, but the output can either be a 
 * list of indices into this array, or a new array containing the points on the
 * hull.
 *
 * Computation time is approximately n^2 + n*log(n) for best case, and 2*n^2 at worst.
 * The convex hull computation itself is just n*log(n) (if I coded it right), but
 * requires pre-sorted corner lists, which can take up to n^2 operations. If the points
 * happen to be in y-index order (such as output from corner detection), the sort will
 * be pretty quick.
 *
 */

#ifndef _convexhull2d_h_
#define _convexhull2d_h_

#include <vector>
#include <VW/GeomObjects/point2d.h>

namespace VW {


class ConvexHull2D {
public:
  /** Construct only. */
  ConvexHull2D();

  /** Construct and compute, storing the data internally. 
   *
   * ### What use is this? - at present you can't access the stored data!
   *
   */
  ConvexHull2D(const std::vector<VW::Point2D>& input_points);

  /** Construct and compute, returning a list of points on the hull ordered clockwise. */
  ConvexHull2D(const std::vector<VW::Point2D>& input_points,
	       std::vector<VW::Point2D>& output_points);

  /** Construct and compute, returning the indices of points on the hull 
   * ordered clockwise. 
   */
  ConvexHull2D(const std::vector<VW::Point2D>& input_points,
	       std::vector<unsigned int>& hull_indices);

  /** Compute, returning a list of points on the hull ordered clockwise. */
  bool Compute(const std::vector<VW::Point2D>& input_points,
	       std::vector<VW::Point2D>& output_points);
  /** Compute, returning the indices of points on the hull ordered clockwise. */
  bool Compute(const std::vector<VW::Point2D>& input_points,
	       std::vector<unsigned int>& hull_indices);

protected:
  double _IsClockwise(const VW::Point2D& p,
		      const VW::Point2D& q,
		      const VW::Point2D& r) const;
  bool _have_hull;
  std::vector<VW::Point2D> _hull;
};


}; // end namespace VW

#endif
