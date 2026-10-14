/** 
 * @file   smooth.cpp
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


#include <VW/Image/imagemono.h>
#include "integralimage.h"
#include "smooth.h"
  

// INLINES

static bool IsAnOddNumber(int n) {return ((n-1)/2) == (n/2); }


namespace VW {

  SmoothGaussian::SmoothGaussian(const float sigma, 
				 const unsigned int patch_size)
    : _mask(patch_size)
  {
    assert (sigma > 0.0f);
    assert (patch_size > 0);
    assert (IsAnOddNumber(patch_size));

    // Create the patch
    _mask.MakeGaussian(sigma);  
  }

  template <class ImType>
  SmoothGaussian::SmoothGaussian(ImType &im, 
				 const float sigma, 
				 const unsigned int patch_size)
    : _mask(patch_size)
  {
    assert (sigma > 0.0f);
    assert (patch_size > 0);
    assert (IsAnOddNumber(patch_size));

    // Create the patch
    _mask.MakeGaussian(sigma);  

    Compute(im);
  }

  template <class ImType>
  SmoothGaussian::SmoothGaussian(const ImType &input_im, 
				 ImType &output_im, 
				 const float sigma, 
				 const unsigned int patch_size)
    : _mask(patch_size)
  {
    assert (sigma > 0.0f);
    assert (patch_size > 0);
    assert (IsAnOddNumber(patch_size));

    // Create the patch
    _mask.MakeGaussian(sigma);  

    Compute(input_im, output_im);
  }



  template <class ImType>
  void SmoothGaussian::Compute(ImType &im)
  {
    ImType result(im.GetWidth(), im.GetHeight());

    // Do the work
    VW::Convolve<float>(im, _mask, _mask, result);
    
    // Now exchange im's data for result's
    im.ExchangeData(result);
    return;
  }
  

  template <class ImType>
  void SmoothGaussian::Compute(const ImType &input_im, ImType &output_im)
  {
    // Allocate the result image
    if (output_im.IsAlloced()) {
      if ((input_im.GetWidth() != output_im.GetWidth())
	  || (input_im.GetHeight() != output_im.GetHeight())) {
	std::cerr << "SmoothGaussian::Compute image sizes are not the same: "<<input_im.GetWidth()<<"x"<<output_im.GetHeight()<<" vs "<<output_im.GetWidth()<<"x"<<input_im.GetHeight()<<std::endl;
      }
    } else {
      output_im.AllocImageData(input_im.GetWidth(), input_im.GetHeight());
    }

    // Do the work
    VW::Convolve<float>(input_im, _mask, _mask, output_im);
    return;
  }
  

  SmoothUniform::SmoothUniform(const unsigned int patch_size)
    : _half_size((patch_size-1)/2)
  {
    assert (patch_size > 0);
    assert (IsAnOddNumber(patch_size));
  }

  template <class ImType>
  SmoothUniform::SmoothUniform(ImType &im, 
			       const unsigned int patch_size)
    : _half_size((patch_size-1)/2)
  {
    assert (patch_size > 0);
    assert (IsAnOddNumber(patch_size));

    // Do it!
    Compute(im);
  }

  template <class ImType>
  void SmoothUniform::Compute(ImType &im)
  {
   const float scale = 1.0f / ((2*_half_size+1)*(2*_half_size+1));
 
    // For uniform we can do something a bit faster making use of the integral image
    // incase input is floating point, use double
    VW::IntegralImage<double> integral_image(im);

    im.ROI().Shrink(_half_size);
    
    // Now scan to get smoothed region
    const int r0 = im.ROI().Top();
    const int r1 = im.ROI().Bottom();
    const int c0 = im.ROI().Left();
    const int c1 = im.ROI().Right();

    const int col_offset =  c0 - _half_size - 1;

    for (int row=r0;row<r1;row++) {
      double* tl_ptr = integral_image[row-_half_size-1] + col_offset;
      double* bl_ptr = integral_image[row+_half_size] + col_offset;
      double* tr_ptr = tl_ptr + 2*_half_size + 1;
      double* br_ptr = bl_ptr + 2*_half_size + 1;


      typename ImType::PixelPtr result_ptr = im.GetROIRowStart(row);
      for (int col=c0;col<c1;col++) {
	result_ptr++->y = typename ImType::DataType( scale*((*br_ptr++)
							  - (*bl_ptr++)
							  + (*tl_ptr++)
							  - (*tr_ptr++)) + 0.5f);
      }
    }


    return;
  }
  


  // Instantiate some versions
#ifdef INSTANTIATE
#  undef INSTANTIATE
#endif
# define INSTANTIATE(M) \
  template SmoothGaussian::SmoothGaussian(M&, float, unsigned int); \
  template SmoothGaussian::SmoothGaussian(const M&, M&, float, unsigned int); \
  template SmoothUniform::SmoothUniform(M&, unsigned int); \
  template void SmoothGaussian::Compute<M >(M&); \
  template void SmoothGaussian::Compute<M >(const M&, M&);	\
  template void SmoothUniform::Compute<M >(M&);

INSTANTIATE(VW::ImageMono<unsigned char>); 
  //INSTANTIATE(VW::ImageYUV<unsigned char>); 
INSTANTIATE(VW::ImageMono<float>); 

#undef INSTANTIATE

} // end namespace VW
