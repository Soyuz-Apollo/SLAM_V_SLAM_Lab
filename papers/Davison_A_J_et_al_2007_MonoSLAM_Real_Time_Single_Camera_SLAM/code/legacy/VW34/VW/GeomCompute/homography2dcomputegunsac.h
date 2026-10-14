/** 
 * @file   homography2dcomputegunsac.h
 * @author JGH Knight, BJ Tordoff
 * @date   28/02/2005		   		
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


#ifndef _homography2dcomputegunsac_h_
#define _homography2dcomputegunsac_h_

#include <vector>

namespace VW {

  // Forward declare the transform
  class Homography2D;
  class Point2D;

  /** Compute a homography from two lists of 2D point features using
   * BJT's guided uncertain sampling and consensus method.
   */
  class Homography2DComputeGUNSAC {
  public:
    Homography2DComputeGUNSAC(double image_noise,
			      double search_area,
			      double desired_confidence=99.0);
    
    bool Compute(VW::Homography2D& H,
		 const std::vector<VW::Point2D>& x0,
		 const std::vector<VW::Point2D>& x1,
		 const std::vector<double>& prior,
		 std::vector<double>& posterior);
    
    /** Set the desired confidence (%). Once this level of confidence is 
     * reached sampling will terminate. Note that if max_iters is also
     * set, both will be used. */
    void SetDesiredConfidence(double desired_confidence);

    /** Set the expected image noise standard deviation (in pixels). 
     * This is used to determine the Gaussian model for the inlier process. */
    void SetImageNoise(double image_noise);

    /** Set the are searched for each feature. This determines the uniform
     * distribution used to model the outlier process. */
    void SetSearchArea(double search_area);

    /** Set an upper bound on the number of iterations. If not set (or set 
     * to zero), sampling will continue until the desired confidence level 
     * is reached. */
    void SetMaxIterations(unsigned int max_iters);
 
  private:
    double _image_noise;
    double _search_area;
    double _desired_confidence;
    unsigned int _max_iters;
    
		
  };

} // End namespace VW



inline VW::Homography2DComputeGUNSAC::
Homography2DComputeGUNSAC(double image_noise,
			  double search_area,
			  double desired_confidence)
  : _image_noise(image_noise),
    _search_area(search_area),
    _desired_confidence(desired_confidence),
    _max_iters(0)
{
}

inline void VW::Homography2DComputeGUNSAC::
SetDesiredConfidence(double desired_confidence)
{
  _desired_confidence = desired_confidence;
}

inline void VW::Homography2DComputeGUNSAC::
SetImageNoise(double image_noise)
{
  _image_noise = image_noise;
}

inline void VW::Homography2DComputeGUNSAC::
SetSearchArea(double search_area)
{
  _search_area = search_area;
}

inline void VW::Homography2DComputeGUNSAC::
SetMaxIterations(unsigned int max_iters)
{
  _max_iters = max_iters;
}


#endif // #ifndef _homography2dcomputeguisac_h_
