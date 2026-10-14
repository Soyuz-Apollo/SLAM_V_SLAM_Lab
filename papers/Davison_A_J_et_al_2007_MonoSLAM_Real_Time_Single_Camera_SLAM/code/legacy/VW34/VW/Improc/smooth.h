/** 
 * @file   smooth.h
 * @author BJ Tordoff
 * @date   31/03/2004
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


#ifndef _smooth_h_
#define _smooth_h_

#include "convolve.h"

namespace VW {

/** 
 * @class   VW::SmoothGaussian
 * @ingroup ImageProcessing
 * @author  BJ Tordoff
 * @date    31/03/2004
 * @brief   Smooth an image using a Gaussian kernels.
 *
 * The patch size *must* be an odd number. Note that if all you want is an approximately 
 * smoothed image, VW::SmoothUniform can be *much* faster (usually at least twice as fast).
 */
class SmoothGaussian {

 public:

  SmoothGaussian(const float sigma=1.0, const unsigned int patch_size = 5);

  template <class ImType>
  SmoothGaussian(ImType &im, const float sigma=1.0, const unsigned int patch_size = 5); 

  template <class ImType>
  SmoothGaussian(const ImType &input_im, ImType &output_im, 
		 const float sigma=1.0, const unsigned int patch_size = 5); 

  template <class ImType>
  void Compute(ImType &im);

  template <class ImType>
  void Compute(const ImType &input_im, ImType &output_im);

 private:
  VW::ConvolutionMask1D<float> _mask;

};


/** 
 * @class   VW::SmoothUniform
 * @ingroup ImageProcessing
 * @author  BJ Tordoff
 * @date    31/03/2004
 * @brief   Smooth an image using a uniform kernel --- this is an optimised version, and
 *          is usually at least twice as fast as the gaussian smoothing.
 *
 * The patch size *must* be an odd number.
 */
class SmoothUniform {

 public:

  SmoothUniform(const unsigned int patch_size = 5);

  template <class ImType>
  SmoothUniform(ImType &im, const unsigned int patch_size = 5); 

  template <class ImType>
  void Compute(ImType &im);

 private:
  int _half_size;
};

}; // end namespace VW





















#endif // _smooth_h_
