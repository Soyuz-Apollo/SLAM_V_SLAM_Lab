/** 
 * @file   imagediff.cc
 * @author ID Reid
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


#include <cstdlib>
#include <cmath>

#include "VW/image.h"
#include "imagediff.h"

using namespace VW;

template <class ImType>
void ImageDiff::ComputeAbsolute(const ImType &im1, 
				const ImType &im2, 
				ImType &diff)
{
  int c, r;
  int r0, c0, rl, cl;

  if (!diff.IsAlloced()) {
    diff.AllocImageData(im1.GetWidth(), im1.GetHeight());
  }

  assert(diff.GetWidth() == im1.GetWidth() &&
	 diff.GetHeight() == im1.GetHeight() &&
	 im2.GetWidth() == im1.GetWidth() &&
	 im2.GetHeight() == im1.GetHeight());

  im1.ROI().Get(c0,r0,cl,rl);
  diff.ROI().Set(c0,r0,cl,rl);

  typename ImType::PixelPtr im1r, im2r, diffr;

  for (r=r0; r<rl; r++) {
    im1r = im1.GetROIRowStart(r);
    im2r = im2.GetROIRowStart(r);
    diffr = diff.GetROIRowStart(r);
    for (c=c0; c<cl; c++)
      *diffr++ = typename ImType::DataType(std::abs(im1r++->y - im2r++->y)); // was fabs.
  }

  return;
}

template <class ImType>
void ImageDiff::ComputeSigned(const ImType &im1, 
			      const ImType &im2, 
			      ImType &diff)
{
  int c, r;
  int r0, c0, rl, cl;

  if (!diff.IsAlloced()) {
    diff.AllocImageData(im1.GetWidth(), im1.GetHeight());
  }

  assert(diff.GetWidth() == im1.GetWidth() &&
	 diff.GetHeight() == im1.GetHeight() &&
	 im2.GetWidth() == im1.GetWidth() &&
	 im2.GetHeight() == im1.GetHeight());

  im1.ROI().Get(c0,r0,cl,rl);
  diff.ROI().Set(c0,r0,cl,rl);

  typename ImType::PixelPtr im1r, im2r, diffr;

  for (r=r0; r<rl; r++) {
    im1r = im1.GetROIRowStart(r);
    im2r = im2.GetROIRowStart(r);
    diffr = diff.GetROIRowStart(r);
    for (c=c0; c<cl; c++)
      *diffr++ = typename ImType::DataType(im1r++->y - im2r++->y);
  }

  return;
}

namespace VW {

#ifdef INSTANTIATE
#  undef INSTANTIATE
#endif
# define INSTANTIATE(M) \
  template void ImageDiff::ComputeAbsolute<M >(const M&, const M&, M&); \
  template void ImageDiff::ComputeSigned<M >(const M&, const M&, M&);

INSTANTIATE(ImageMono<unsigned char>); 
INSTANTIATE(ImageMono<float>); 

#undef INSTANTIATE

}
