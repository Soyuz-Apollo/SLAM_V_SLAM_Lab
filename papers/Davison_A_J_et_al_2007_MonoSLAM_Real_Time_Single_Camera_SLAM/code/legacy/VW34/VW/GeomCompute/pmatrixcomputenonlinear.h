/** 
 * @file   pmatrixcomputenonlinear.h
 * @author BJ Tordoff
 * @date   14/08/2003		   		
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


#ifndef _pmatrixcomputenonlinear_h_
#define _pmatrixcomputenonlinear_h_

#include <vector>

#define DEFAULT_TOLERANCE 1e-7
#define DEFAULT_MAXITERS  500

namespace VW {

  // Forward declare the transform
  class ProjectionMatrix;
  class Point2D;
 class Point3D;


  namespace PMatrixCompute {
    
    /** Compute a ProjectionMatrix from a 
     * set of 3D-2D point correspondences.
     *
     * @returns true for success, false if there was a problem
     */
    bool NonLinearFromPoints(VW::ProjectionMatrix& P,
			     const std::vector<VW::Point2D>& x,
			     const std::vector<VW::Point3D>& X,
				const bool zero_skew = true,
				const bool unity_aspect = false,
			     double error_tolerance = DEFAULT_TOLERANCE,
			     unsigned int max_iters = DEFAULT_MAXITERS,
			     const std::vector<unsigned int>& whichfeatures = std::vector<unsigned int>());

    /** Compute a ProjectionMatrix and distortion parameter from a 
     * set of 3D-2D point correspondences. The distortion is assumed
     * to be centred on the principal point.
     * 
     * @returns true for success, false if there was a problem
     *
     */
    bool NonLinearFromPoints(VW::ProjectionMatrix& P,
			     double& kappa,
				VW::Point2D& distortion_centre,
			     const std::vector<VW::Point2D>& x,
			     const std::vector<VW::Point3D>& X,
				const bool zero_skew = true,
				const bool unity_aspect = false,
			     double error_tolerance = DEFAULT_TOLERANCE,
			     unsigned int max_iters = DEFAULT_MAXITERS,
			     const std::vector<unsigned int>& whichfeatures = std::vector<unsigned int>());
				     


} // End namespace PMatrixCompute

} // End namespace VW



#endif // #ifndef _pmatrixcomputenonlinear_h_
