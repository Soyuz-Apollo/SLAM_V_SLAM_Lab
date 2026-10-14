/** 
 * @file   subsample.cpp
 * @author ID Reid and BJ Tordoff
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


#include <iostream>

#include "VW/image.h"
#include "subsample.h"

namespace VW {

template <class ImType>
void Subsample::Compute(ImType &im)
{
  ImType result;
  Compute(im,result);
  im.ExchangeData(result);
}

template <class ImType>
void Subsample::Compute(const ImType &im, ImType &result)
{
  int c0, r0, cl, rl;
  int r, c;

  typename ImType::PixelPtr imr, resr;

  if (!result.IsAlloced()) {
    result.AllocImageData(im.GetWidth()/factor, im.GetHeight()/factor);
  } else {
    if (result.GetWidth() != im.GetWidth()/factor ||
	result.GetHeight() != im.GetHeight()/factor) {
      std::cerr << "Subsample image size incorrect\n";
      return;
    }
  }


  im.ROI().Get(c0,r0,cl,rl);
  c0 /= factor; r0 /= factor;
  cl /= factor; rl /= factor;
  result.ROI().Set(c0, r0, cl, rl);


    for (r=r0; r<rl; r++) {
      imr = im.GetROIRowStart(factor*r);
      resr = result.GetROIRowStart(r);
      for (c=c0; c<cl; c++, imr+=factor)
	*resr++ = *imr;
    }
  return;
}

#ifdef INSTANTIATE
#  undef INSTANTIATE
#endif
# define INSTANTIATE(M) \
  template void Subsample::Compute<M >(M &); \
  template void Subsample::Compute<M >(const M &, M &);

INSTANTIATE(ImageMono<unsigned char>); 
INSTANTIATE(ImageMono<float>); 
INSTANTIATE(ImageRGB<unsigned char>); 
INSTANTIATE(ImageYUV<unsigned char>); 

#undef INSTANTIATE

} // end namespace VW
