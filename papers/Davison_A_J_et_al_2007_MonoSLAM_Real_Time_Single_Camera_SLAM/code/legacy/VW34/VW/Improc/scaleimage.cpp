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


#include <VW/image.h>
#include "scaleimage.h"
  


template <class ImType>
void VW::ScaleImage::Compute(const ImType &im, 
									  ImType &result) const
{
  const unsigned int new_width = int(_scale_x*im.GetWidth());
  const unsigned int new_height = int(_scale_y*im.GetHeight());
  if (result.IsAlloced()) {
    result.FreeImageData();
  }
  result.AllocImageData(new_width, new_height);

  const float x_step_size = 1.0f / _scale_x;
  const float y_step_size = 1.0f / _scale_y;

  switch(_method) {
  case NearestNeighbour:
    for (unsigned int dest_row=0; dest_row<new_height; dest_row++) {
      // Vertical interpolation factor is same along row
      const int   y_int = int(float(dest_row)*y_step_size);
      float x = 0.0;
      
      typename ImType::PixelPtr src_row0 = im.GetRowStart(y_int);
      typename ImType::PixelPtr dest_ptr = result.GetRowStart(dest_row);
      for (unsigned int dest_col=0; dest_col<new_width; 
	   dest_col++, dest_ptr++, x+=x_step_size) {
	const int x_int = int(x);
	*dest_ptr = src_row0[x_int];
      }
    }
    break;

  default:
  case Bilinear:
    for (unsigned int dest_row=0; dest_row<new_height; dest_row++) {
      // Vertical interpolation factor is same along row
      const float y = float(dest_row)*y_step_size;
      const int   y_int = int(y);
      const float y_frac = y - y_int;
      float x = 0.0;
      
      typename ImType::PixelPtr src_row0 = im.GetRowStart(y_int);
      typename ImType::PixelPtr src_row1 = im.GetRowStart(y_int+1);
      typename ImType::PixelPtr dest_ptr = result.GetRowStart(dest_row);
      for (unsigned int dest_col=0; dest_col<new_width; 
	   dest_col++, dest_ptr++, x+=x_step_size) {
	const int   x_int = int(x);
	const float x_frac = x - x_int;
	typename ImType::PixelType val;
	val =
	  (1.0f-x_frac)*(1.0f-y_frac) *src_row0[x_int] +
	  (x_frac)     *(1.0f-y_frac) *src_row0[x_int+1] +
	  (1.0f-x_frac)*(y_frac)      *src_row1[x_int] +
	  (x_frac)     *(y_frac)      *src_row1[x_int+1];
	
	*dest_ptr = val;
      }
    }
    break;
  case Biquadratic:
    std::cerr << "VW::ScaleImage::Compute: Method \"Biquadratic\" is not yet implemented." << std::endl;
    break;
  case Bicubic:
    std::cerr << "VW::ScaleImage::Compute: Method \"Bicubic\" is not yet implemented." << std::endl;
    break;
  }
}

  // Instantiate some versions
namespace VW {
#ifdef INSTANTIATE
#  undef INSTANTIATE
#endif
#define INSTANTIATE(M) \
  template void ScaleImage::Compute<M >(const M&, M&) const;

INSTANTIATE(VW::ImageMono<unsigned char>); 
INSTANTIATE(VW::ImageYUV<unsigned char>); 
INSTANTIATE(VW::ImageRGB<unsigned char>); 
INSTANTIATE(VW::ImageMono<float>); 
INSTANTIATE(VW::ImageYUV<float>); 

#undef INSTANTIATE

} // end namespace VW
