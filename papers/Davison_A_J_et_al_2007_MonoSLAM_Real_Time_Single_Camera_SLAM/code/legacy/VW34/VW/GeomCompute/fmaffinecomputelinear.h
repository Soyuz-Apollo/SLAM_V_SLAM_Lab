/** 
 * @file   fmaffinecomputelinear.h
 * @author BJ Tordoff
 * @date   08/04/2005		   		
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
/**
 * @class    VW::FMatrixAffineComputeLinear
 * @ingroup  GeometricAlgorithms
 * @author   BJ Tordoff
 * @date     08/04/2005		   		
 * @brief    FMatrixAffineComputeLinear
 *
 * linear algorithm for affine FMatrices.
 */


#ifndef _fmaffine_computelinear_h_
#define _fmaffine_computelinear_h_

#include <vector>

namespace VW {

  class FMatrixAffine;
  class FMatrixAffineCovariance;
  class Point2D;




  class FMatrixAffineComputeLinear {
  public:

    /** Create a linear computation object.
     * Note that the calculation can involve a normalisation, so it is
     * not necessary to pre-normalise the input points/lines (although it 
     * doesn't hurt).
     */
    FMatrixAffineComputeLinear(const bool normalise = true);

    /** Compute an affine fundamental matrix from two lists of 2D point features.
     * Note that x0, x1 are defined such that x_1' F x_0 = 0
     */
    bool Compute(VW::FMatrixAffine& result,
					  const std::vector<VW::Point2D>& x0,
					  const std::vector<VW::Point2D>& x1);
				     
    /** Compute an affine fundamental matrix from two lists of 2D point features.
     * Note that x0, x1 are defined such that x_1' F x_0 = 0
     * @param which_features a list of the indices of feature-pairs to use
     */
    bool Compute(VW::FMatrixAffine& result,
					  const std::vector<VW::Point2D>& x0,
					  const std::vector<VW::Point2D>& x1,
					  const std::vector<unsigned int>& which_features);
				     
     /** Compute an affine fundamental matrix from two lists of 2D point features, weighting
      * the contribution of the features.
     */
    bool ComputeWeighted(VW::FMatrixAffine& result,
								 const std::vector<VW::Point2D>& x0,
								 const std::vector<VW::Point2D>& x1,
								 const std::vector<double>& weights);
				     
    /** Compute a homography and its covariance from two lists of 2D point features.
     */
    bool Compute(VW::FMatrixAffine& result,
					  VW::FMatrixAffineCovariance& covar,
					  const std::vector<VW::Point2D>& x0,
					  const std::vector<VW::Point2D>& x1,
					  const double noise0,
					  const double noise1);
    
    /** Compute a homography and its covariance from selected entries of
     * two lists of 2D point features.
     */
    bool Compute(VW::FMatrixAffine& result,
					  VW::FMatrixAffineCovariance& covar,
					  const std::vector<VW::Point2D>& x0,
					  const std::vector<VW::Point2D>& x1,
					  const double noise0,
					  const double noise1,
					  const std::vector<unsigned int>& which_features);

  private:
    bool _normalise;

  }; // End class

} // End namespace VW




inline VW::FMatrixAffineComputeLinear::
FMatrixAffineComputeLinear(const bool normalise)
  :  _normalise(normalise)
{
}

#endif // #ifndef _fmaffine_computelinear_h_
