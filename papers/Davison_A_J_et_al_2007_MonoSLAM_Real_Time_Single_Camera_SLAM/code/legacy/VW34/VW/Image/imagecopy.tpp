/** @file imagecopy.tpp
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

#ifndef _vw_imagecopy_tpp_
#define _vw_imagecopy_tpp_

#include "pixelconversions.h"
#include "imagecopy.h"

// generic versions here, specifics in the cpp file

namespace VW {

template <class inType, class outType>
void ImageCopy(const inType &im, outType &res)
{
  if (!res.IsAlloced()) {
    res.AllocImageData(im.GetWidth(), im.GetHeight());
  } else {
    assert(res.GetWidth() == im.GetWidth() &&
	   res.GetHeight() == im.GetHeight());
  }

  res.SetROI(im.ROI());

  typename inType::PixelPtr imr;
  typename outType::PixelPtr resr;

  int r0 = im.ROI().Top();
  int r1 = im.ROI().Bottom();
  int w = im.ROI().Width();

  for (int r=r0; r<r1; r++) {
    imr = im.GetROIRowStart(r);
    resr = res.GetROIRowStart(r);
    for (int c=0; c<w; c++)
      *resr++ = *imr++;
  }

  return;
}

template <class inType, class outType>
void ImageFlipCopy(const inType &im, outType &res)
{
  if (!res.IsAlloced()) {
    res.AllocImageData(im.GetWidth(), im.GetHeight());
  } else {
    assert(res.GetWidth() == im.GetWidth() &&
	   res.GetHeight() == im.GetHeight());
  }

  res.SetROI(im.ROI());

  typename inType::PixelPtr imr;
  typename outType::PixelPtr resr;

  int r0 = im.ROI().Top();
  int r1 = im.ROI().Bottom();
  int w = im.ROI().Width();

  for (int r=r0; r<r1; r++) {
    imr = im.GetROIRowStart(r)+im.GetWidth()-1;
    resr = res.GetROIRowStart(r);
    for (int c=0; c<w; c++)
      *resr++ = *imr--;
  }

  return;
}



// Special cases are in .cpp
template <>
void ImageCopy(const ImageMono<float> &im, ImageMono<unsigned char> &res);
template <>
void ImageCopy(const ImageYUV<unsigned char> &im, ImageMono<float> &res);
template <>
void ImageCopy(const ImageRGB<unsigned char> &im, ImageMono<float> &res);
template <>
void ImageCopy(const ImageMono<int> &im, ImageMono<unsigned char> &res);
template <>
void ImageCopy(const ImageRGB<float> &im, ImageRGB<unsigned char> &res);
template <>
void ImageCopy(const ImageMono<float> &im, ImageRGB<unsigned char> &res);

} // End namespace VW
#endif // _vw_imagecopy_tpp_
