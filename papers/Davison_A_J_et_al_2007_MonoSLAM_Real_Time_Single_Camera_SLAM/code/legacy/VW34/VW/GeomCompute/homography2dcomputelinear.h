/** 
 * @file   homography2dcomputelinear.h
 * @author JGH Knight, BJ Tordoff
 * @date   02/08/2002		   		
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
 * @namespace VW::Homography2DCompute
 * @ingroup   GeometricAlgorithms
 * @author    JGH Knight, BJ Tordoff
 * @date      02/11/2002
 * @brief     ComputeLinear functions for Homography2Ds.
 */


#ifndef _homography2dcomputelinear_h_
#define _homography2dcomputelinear_h_

#include <vector>

// Forward declarations
namespace VW {
  class Homography2D;
  class Homography2DCovariance;
  class Point2D;
  class Line2D;
};





namespace VW {
  class Homography2DComputeLinear {
  public:

    /** Create a linear computation object.
     * Note that the calculation can involve a normalisation, so it is
     * not necessary to pre-normalise the input points/lines (although it 
     * doesn't hurt).
     */
    Homography2DComputeLinear(const bool normalise = true);

    /** Compute a homography from two lists of 2D point features.
     * Note that x0, x1 are defined such that x_1 = H x_0
     * @param inlierlist a list of the indices of feature-pairs to use
     */
    bool Compute(VW::Homography2D& result,
		 const std::vector<VW::Point2D>& x0,
		 const std::vector<VW::Point2D>& x1);
				     
     /** Compute a homography from two lists of 2D point features.
     * Note that x0, x1 are defined such that x_1 = H x_0
     * @param which_features a list of the indices of feature-pairs to use
     */
    bool Compute(VW::Homography2D& result,
		 const std::vector<VW::Point2D>& x0,
		 const std::vector<VW::Point2D>& x1,
		 const std::vector<unsigned int>& which_features);
				     
     /** Compute a homography from two lists of 2D point features, weighting
      * the contribution of the features.
     */
    bool ComputeWeighted(VW::Homography2D& result,
			 const std::vector<VW::Point2D>& x0,
			 const std::vector<VW::Point2D>& x1,
			 const std::vector<double>& weights);
				     

    /** Compute a homography and its covariance from two lists of 2D point features.
     */
    bool Compute(VW::Homography2D& result,
		 VW::Homography2DCovariance& covar,
		 const std::vector<VW::Point2D>& x0,
		 const std::vector<VW::Point2D>& x1,
		 const double noise0,
		 const double noise1);
    
    /** Compute a homography and its covariance from selected entries of
     * two lists of 2D point features.
     */
    bool Compute(VW::Homography2D& result,
		 VW::Homography2DCovariance& covar,
		 const std::vector<VW::Point2D>& x0,
		 const std::vector<VW::Point2D>& x1,
		 const double noise0,
		 const double noise1,
		 const std::vector<unsigned int>& which_features);
    



    /** Compute a homography from 2D line features (not implemented yet).
     */
    bool Compute(VW::Homography2D& result,
		 const std::vector<VW::Line2D>& l0,
		 const std::vector<VW::Line2D>& l1);

  private:
    bool _normalise;

  }; // End class

} // End namespace VW




inline VW::Homography2DComputeLinear::
Homography2DComputeLinear(const bool normalise)
  :  _normalise(normalise)
{
}

#endif // #ifndef _homography2d_computelinear_h_
