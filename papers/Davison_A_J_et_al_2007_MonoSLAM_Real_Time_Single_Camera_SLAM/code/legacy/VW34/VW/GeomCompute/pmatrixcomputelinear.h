/** 
 * @file   pmatrixcomputelinear.h
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
/**
 * @namespace VW::PMatrix2DCompute
 * @ingroup   GeometricAlgorithms
 * @author    BJ Tordoff
 * @date      14/08/2003
 * @brief     ComputeLinear functions for projection-matrices.
 */


#ifndef _pmatrixcomputelinear_h_
#define _pmatrixcomputelinear_h_

#include <vector>

namespace VW {

  // Forward declare the transform
  class ProjectionMatrix;
  class Point2D;
  class Point3D;


  namespace PMatrixCompute {
    
    /** Compute a ProjectionMatrix from a set of 3D-2D point correspondences.
     *
     * @param normalise  should the input data be normalised for the least-squares calculation?
     * @param inlierlist a list of the indices of feature-pairs to use (if not specified, 
     *                   will use all features)
     */
    void LinearFromPoints(VW::ProjectionMatrix& result,
			  const std::vector<VW::Point2D>& x,
			  const std::vector<VW::Point3D>& X,
			  const bool normalise = true,
			  const std::vector<unsigned int>& inlierlist = std::vector<unsigned int>());
				     


} // End namespace PMatrixCompute

} // End namespace VW




#endif // #ifndef _pmatrixcomputelinear_h_
