/** 
 * @file   scaleimage.h
 * @author BJ Tordoff
 * @date   14/06/2004
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


#ifndef _scaleimage_h_
#define _scaleimage_h_

#include "interpolationmethod.h"

namespace VW {



/** 
 * @class   VW::ScaleImage
 * @ingroup ImageProcessing
 * @author  BJ Tordoff
 * @date    14/06/2004
 * @brief   Scaling of an image (isotropic or XY aligned).
 *
 */
class ScaleImage {

 public:

  /** Construct for isotropic scaling. */
  ScaleImage(const float scale,
	     const InterpolationMethod method = Bilinear);
  /** Construct for XY aligned scaling. */
  ScaleImage(const float x_scale, const float y_scale,
	     const InterpolationMethod method = Bilinear);

  /** Construct and immediately scale isotropically. */
  template <class ImType>
  ScaleImage(const ImType &im, const float scale, 
	     ImType &result,
	     const InterpolationMethod method = Bilinear); 

  /** Construct and immediately scale XY aligned. */
  template <class ImType>
  ScaleImage(const ImType &im, 
	     const float x_scale, const float y_scale, 
	     ImType &result,
	     const InterpolationMethod method = Bilinear); 

  /** Scale an image, storing the result in result. */
  template <class ImType>
  void Compute(const ImType &im, 
					ImType &result) const;

 private:
  float _scale_x, _scale_y;
  InterpolationMethod _method;
};











// INLINES
inline
VW::ScaleImage::ScaleImage(const float scale,
			   const InterpolationMethod method)
  : _scale_x(scale), _scale_y(scale), _method(method)
{
}

inline
VW::ScaleImage::ScaleImage(const float x_scale, const float y_scale,
			   const InterpolationMethod method)
  : _scale_x(x_scale), _scale_y(y_scale), _method(method)
{
}

template <class ImType> inline
VW::ScaleImage::ScaleImage(const ImType &im, 
			   const float scale, 
			   ImType &result,
			   const InterpolationMethod method)
  : _scale_x(scale), _scale_y(scale), _method(method)
{
  Compute(im,result);
}


template <class ImType> inline
VW::ScaleImage::ScaleImage(const ImType &im, 
			   const float x_scale, const float y_scale, 
			   ImType &result,
			   const InterpolationMethod method)
  : _scale_x(x_scale), _scale_y(y_scale), _method(method)
{
  Compute(im,result);
}



}; // end namespace VW



#endif // _scaleimage_h_
