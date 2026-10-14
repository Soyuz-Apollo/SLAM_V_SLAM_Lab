/** 
 * @file   fmatrixcomputenonlinear.h
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


#ifndef _fmatrix_computenonlinear_h_
#define _fmatrix_computenonlinear_h_

#include <vector>

#define DEFAULT_TOLERANCE 1e-7
#define DEFAULT_MAXITERS  500

namespace VW {

  // Forward declare the transform
  class FMatrix;
  class Point2D;

  class FMatrixComputeNonLinear {
  public:
    FMatrixComputeNonLinear(double error_tolerance = DEFAULT_TOLERANCE,
									 unsigned int max_iters = DEFAULT_MAXITERS);


    /** Compute a fundamental matrix from two lists of 2D point features.
     * Minimisation is performed using Levenberg-Marquadt with Sampson
     * distance as the error measure. Note that no normalisation is performed, so
     * its best if you normalise the data before calling this.
     *
     * @returns true for successful minimisation, false otherwise (homography
     *          is left untouched in this case).
     */
    bool Compute(VW::FMatrix& result,
					  const std::vector<VW::Point2D>& x0,
					  const std::vector<VW::Point2D>& x1);
				     
    bool Compute(VW::FMatrix& result,
					  const std::vector<VW::Point2D>& x0,
					  const std::vector<VW::Point2D>& x1,
					  const std::vector<unsigned int>& whichfeatures);
				     
  private:
    double _tolerance;
    unsigned int _max_iters;
  };

} // End namespace VW




// INLINES

inline VW::FMatrixComputeNonLinear::
FMatrixComputeNonLinear(double error_tolerance,
			     unsigned int max_iters)
  : _tolerance(error_tolerance),
    _max_iters(max_iters)
{
}
#endif // #ifndef _fmatrix_computenonlinear_h_
