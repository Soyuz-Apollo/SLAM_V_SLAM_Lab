/** 
 * @file   line2dcomputeguisac.h
 * @author BJ Tordoff
 * @date   07/07/2003	   		
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


#ifndef _line2dcomputeguisac_h_
#define _line2dcomputeguisac_h_

#define DEFAULT_MAXITERS    500
#define DEFAULT_IMAGENOISE    1.0
#define DEFAULT_SEARCHDISTANCE 10.0


#include <vector>

namespace VW {

  // Forward declare the transform
  class Line2D;


  namespace Line2DCompute {
    
    /** Compute a 2D line from a list of 2D point features that lie on the line.
     * Any point class can be used, the requirement being that it has GetX() and GetY()
     * functions. Note that by default the calculation involves a normalisation, so it is
     * not necessary to pre-normalise the input points. If your data is already
     * normalised (or you don't care) then it can be disabled.
     *
     * @param normalise  should the input data be normalised for the least-squares calculation?
     * @param inlierlist a list of the indices of feature-pairs to use (if not specified, 
     *                   will use all features)
     * @param search_distance the total distance searched (ie if you search
     *                        either side of a line, its twice the distance)
     */
    void GUISACFromPoints(VW::Line2D& result,
			  const std::vector<VW::Point2D>& x,
			  const std::vector<double>& prior,
			  const std::vector<unsigned int>& inliers,
			  unsigned int max_iterations = DEFAULT_MAXITERS,
			  double image_noise = DEFAULT_IMAGENOISE,
			  double search_distance = DEFAULT_SEARCHDISTANCE);
				     


} // End namespace Line2DCompute

} // End namespace VW



#endif // #ifndef _line2dcomputelinear_h_
