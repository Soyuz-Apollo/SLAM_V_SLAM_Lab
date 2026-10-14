/** 
 * @file   essentialmatrixcompute5point.h
 * @author G Vogiatzis, BJ Tordoff
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
 * @class    VW::EssentialMatrixCompute5Point
 * @ingroup  GeometricAlgorithms
 * @author   G Vogiatzis, BJ Tordoff
 * @date     19/04/2005		   		
 * @brief    EssentialMatrixCompute5Point
 *
 * Five point minimal-set algorithm for computing essential matrices. Using only
 * five correspondences results in a 4D nullspace and requires solution of a 10th
 * degree polynomial (and hence produces between 0 and 10 essential matrices). 
 * Despite this, it's actually very fast!
 *
 * Ref: Nister, D. ICCV2003 "An efficient solution to the five-point relative pose problem"
 *      (see also his thesis)
 */



#ifndef _ematrix_compute5point_h_
#define _ematrix_compute5point_h_

#include <vector>

namespace VW {

  class EssentialMatrix;
  class Point2D;

  class EssentialMatrixCompute5Point {
  public:

    /** Create a linear computation object.
     * Note that the calculation can involve a normalisation, so it is
     * not necessary to pre-normalise the input points/lines (although it 
     * doesn't hurt).
     */
    EssentialMatrixCompute5Point(const bool normalise = true);

    /** Compute essential matrices from two lists of 2D point features.
     * Note that x0, x1 are defined such that x_1 E x_0 = 0. Returns false
	  * if no solutions could be found.
     */
    bool Compute(std::vector<EssentialMatrix>& ematrices,
					  const std::vector<VW::Point2D>& x0,
					  const std::vector<VW::Point2D>& x1);
				     
    /** Compute an fmatrix from two lists of 2D point features.
     * Note that x0, x1 are defined such that x_1 E x_0 = 0.
     * @param which_features a list of the indices of feature-pairs to use
	  * @returns false if no solutions are found
     */
    bool Compute(std::vector<EssentialMatrix>& ematrices,
					  const std::vector<VW::Point2D>& x0,
					  const std::vector<VW::Point2D>& x1,
					  const std::vector<unsigned int>& which_features);

  private:
    bool _normalise;

  }; // End class

}; // End namespace VW		 


inline VW::EssentialMatrixCompute5Point::
EssentialMatrixCompute5Point(const bool normalise)
  :  _normalise(normalise)
{
}

#endif // #ifndef _ematrix_compute5point_h_
