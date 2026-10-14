/** 
 * @file   essentialmatrixcomputelinear.h
 * @author BJ Tordoff
 * @date   19/04/2005		   		
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
 * @class    VW::EssentialMatrixComputeLinear
 * @ingroup  GeometricAlgorithms
 * @author   BJ Tordoff
 * @date     19/04/2005		   		
 * @brief    EssentialMatrixComputeLinear
 *
 * general linear algorithm for EssentialMatrices.
 */


#ifndef _ematrix_computelinear_h_
#define _ematrix_computelinear_h_

#include <vector>

namespace VW {

  class EssentialMatrix;
  class Point2D;

  class EssentialMatrixComputeLinear {
  public:

    /** Create a linear computation object.
     * Note that the calculation can involve a normalisation, so it is
     * not necessary to pre-normalise the input points/lines (although it 
     * doesn't hurt).
     */
    EssentialMatrixComputeLinear(const bool normalise = true);

    /** Compute an fmatrix from two lists of 2D point features.
     * Note that x0, x1 are defined such that x_1' E x_0 = 0
     * @param inlierlist a list of the indices of feature-pairs to use
     */
    bool Compute(VW::EssentialMatrix& result,
					  const std::vector<VW::Point2D>& x0,
					  const std::vector<VW::Point2D>& x1);
	 
	 /** Compute an essential matrix from two lists of 2D point features.
     * Note that x0, x1 are defined such that x_1' E x_0 = 0
     * @param which_features a list of the indices of feature-pairs to use
     */
    bool Compute(VW::EssentialMatrix& result,
					  const std::vector<VW::Point2D>& x0,
					  const std::vector<VW::Point2D>& x1,
					  const std::vector<unsigned int>& which_features);
	 
     /** Compute an essential matrix from two lists of 2D point features, weighting
      * the contribution of the features.
		*/
    bool ComputeWeighted(VW::EssentialMatrix& result,
								 const std::vector<VW::Point2D>& x0,
								 const std::vector<VW::Point2D>& x1,
								 const std::vector<double>& weights);
				     



  private:
    bool _normalise;

  }; // End class

}; // End namespace VW		 


inline VW::EssentialMatrixComputeLinear::
EssentialMatrixComputeLinear(const bool normalise)
  :  _normalise(normalise)
{
}

#endif // #ifndef _ematrix_computelinear_h_
