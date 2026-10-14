/** 
 * @file   threshold.cc
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
#include "threshold.h"

using namespace VW;

template <class Type>
void Threshold<Type>::Compute(const ImageMono<Type> &im, ImageMono<Type> &result)
{
  int c, r; 
  int r0, c0, rl, cl;

  if (!result.IsAlloced()) {
    result.AllocImageData(im.GetWidth(), im.GetHeight());
  } else {
    assert(result.GetWidth() == im.GetWidth() &&
	   result.GetHeight() == im.GetHeight());
  }

  im.ROI().Get(c0,r0,cl,rl);
  result.ROI().Set(c0,r0,cl,rl);

  typename ImageMono<Type>::PixelPtr imr, resultr;

  for (r=r0; r<rl; r++) {
    imr = im.GetROIRowStart(r);
    resultr = result.GetROIRowStart(r);
    for (c=c0; c<cl; c++)
      *resultr++ = (imr++->Luminance() > threshold) ? upper : lower;
  }

  return;
}


template void Threshold<unsigned char>::Compute(const ImageMono<unsigned char> &im, 
					ImageMono<unsigned char> &result);
template void Threshold<float>::Compute(const ImageMono<float> &im, 
					ImageMono<float> &result);


template <class Type>
void Threshold<Type>::Compute(ImageMono<Type> &im)
{
  int c, r; 
  int r0, c0, rl, cl;

  assert(im.IsAlloced());

  im.ROI().Get(c0,r0,cl,rl);

  typename ImageMono<Type>::PixelPtr imr;

  for (r=r0; r<rl; r++) {
    imr = im.GetROIRowStart(r);
    for (c=c0; c<cl; ++c, ++imr)
      *imr = (imr->Luminance() > threshold) ? upper : lower;
  }

  return;
}

template void Threshold<unsigned char>::Compute(ImageMono<unsigned char> &im);
template void Threshold<float>::Compute(ImageMono<float> &im);




