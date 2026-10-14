/** 
 * @file   fmatrixcomputelinear.h
 * @author JGH Knight
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
 * @class    VW::FMatrixCompute::Linear
 * @ingroup  GeometricAlgorithms
 * @author   JGH Knight
 * @date     02/11/02
 * @brief    FMatrixComputeLinear
 *
 * general linear algorithm for FMatrices.
 */


#ifndef _fmatrix_computelinear_h_
#define _fmatrix_computelinear_h_

#include <vector>

namespace VW {

  class FMatrix;
  class FMatrixCovariance;
  class Point2D;

  class FMatrixComputeLinear {
  public:

    /** Create a linear computation object.
     * Note that the calculation can involve a normalisation, so it is
     * not necessary to pre-normalise the input points/lines (although it 
     * doesn't hurt).
     */
    FMatrixComputeLinear(const bool normalise = true);

    /** Compute an fmatrix from two lists of 2D point features.
     * Note that x0, x1 are defined such that x_1 = H x_0
     * @param inlierlist a list of the indices of feature-pairs to use
     */
    bool Compute(VW::FMatrix& result,
		 const std::vector<VW::Point2D>& x0,
		 const std::vector<VW::Point2D>& x1);
				     
     /** Compute an fmatrix from two lists of 2D point features.
     * Note that x0, x1 are defined such that x_1 = H x_0
     * @param which_features a list of the indices of feature-pairs to use
     */
    bool Compute(VW::FMatrix& result,
		 const std::vector<VW::Point2D>& x0,
		 const std::vector<VW::Point2D>& x1,
		 const std::vector<unsigned int>& which_features);
				     
     /** Compute an fmatrix from two lists of 2D point features, weighting
      * the contribution of the features.
     */
    bool ComputeWeighted(VW::FMatrix& result,
			 const std::vector<VW::Point2D>& x0,
			 const std::vector<VW::Point2D>& x1,
			 const std::vector<double>& weights);
				     

    /** Compute an fmatrix and its covariance from two lists of 2D point features.
     */
    bool Compute(VW::FMatrix& result,
					  VW::FMatrixCovariance& covar,
					  const std::vector<VW::Point2D>& x0,
					  const std::vector<VW::Point2D>& x1,
					  const double noise0,
					  const double noise1);
    
    /** Compute an fmatrix and its covariance from selected entries of
     * two lists of 2D point features.
     */
    bool Compute(VW::FMatrix& result,
		 VW::FMatrixCovariance& covar,
		 const std::vector<VW::Point2D>& x0,
		 const std::vector<VW::Point2D>& x1,
		 const double noise0,
		 const double noise1,
		 const std::vector<unsigned int>& which_features);
    



    /** Compute an fmatrix from 2D line features (not implemented yet).
     */
    bool Compute(VW::FMatrix& result,
					  const std::vector<VW::Line2D>& l0,
					  const std::vector<VW::Line2D>& l1);

  private:
    bool _normalise;

  }; // End class

}; // End namespace VW		 


inline VW::FMatrixComputeLinear::
FMatrixComputeLinear(const bool normalise)
  :  _normalise(normalise)
{
}

#endif // #ifndef _fmatrix_computelinear_h_
