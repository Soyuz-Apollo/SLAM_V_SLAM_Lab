/** @file imagecopy.cpp
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


#ifdef _WIN32
#include <float.h>
#define finite _finite
#endif
#include <cmath>


#include "imagemono.h"
#include "imagergb.h"
#include "imageinterleave.h"
#include "imageyuv.h"
#include "imagehsv.h"
#include "imagergbplanes.h"

#include "imagecopy.tpp"


namespace VW {


// clamp to 255 max
template <>
void ImageCopy(const ImageMono<float> &im, 
	       ImageMono<unsigned char> &res)
{
  if (!res.IsAlloced()) {
    res.AllocImageData(im.GetWidth(), im.GetHeight());
  } else {
    assert(res.GetWidth() == im.GetWidth() &&
	   res.GetHeight() == im.GetHeight());
  }

  res.SetROI(im.ROI());

  ImageMono<float>::PixelPtr imr;
  ImageMono<unsigned char>::PixelPtr resr;

  int r0 = im.ROI().Top();
  int r1 = im.ROI().Bottom();
  int w = im.ROI().Width();
  for (int r=r0; r<r1; r++) {
    imr = im.GetROIRowStart(r);
    resr = res.GetROIRowStart(r);
    for (int c=0; c<w; c++, imr++) {
      if (finite(imr->y)) 
	resr++->Set(VW::ImageConversions::CLIPVALUE(int(imr->y + 0.5f)));
      else
	resr++->Set(0);
    }
  }

  return;
}

// Convert both value and type (only works for certain simple cases)
template <>
void ImageCopy(const ImageYUV<unsigned char> &im, 
	       ImageMono<float> &res)
{
  if (!res.IsAlloced()) {
    res.AllocImageData(im.GetWidth(), im.GetHeight());
  } else {
    assert(res.GetWidth() == im.GetWidth() &&
	   res.GetHeight() == im.GetHeight());
  }

  res.SetROI(im.ROI());

  ImageYUV<unsigned char>::PixelPtr imr;
  ImageMono<float>::PixelPtr resr;

  int r0 = im.ROI().Top();
  int r1 = im.ROI().Bottom();
  int w = im.ROI().Width();
  for (int r=r0; r<r1; r++) {
    imr = im.GetROIRowStart(r);
    resr = res.GetROIRowStart(r);
    for (int c=0; c<w; c++) {
      resr++->y = imr++->y;
    }
  }

  return;
}
template <>
void ImageCopy(const ImageRGB<unsigned char> &im, 
	       ImageMono<float> &res)
{
  if (!res.IsAlloced()) {
    res.AllocImageData(im.GetWidth(), im.GetHeight());
  } else {
    assert(res.GetWidth() == im.GetWidth() &&
	   res.GetHeight() == im.GetHeight());
  }

  res.SetROI(im.ROI());

  ImageRGB<unsigned char>::PixelPtr imr;
  ImageMono<float>::PixelPtr resr;

  int r0 = im.ROI().Top();
  int r1 = im.ROI().Bottom();
  int w = im.ROI().Width();
  for (int r=r0; r<r1; r++) {
    imr = im.GetROIRowStart(r);
    resr = res.GetROIRowStart(r);
    for (int c=0; c<w; c++) {
      resr++->y = imr++->Luminance();
    }
  }

  return;
}
template <>
void ImageCopy(const ImageMono<float> &im, 
	       ImageRGB<unsigned char> &res)
{
  if (!res.IsAlloced()) {
    res.AllocImageData(im.GetWidth(), im.GetHeight());
  } else {
    assert(res.GetWidth() == im.GetWidth() &&
	   res.GetHeight() == im.GetHeight());
  }

  res.SetROI(im.ROI());

   ImageMono<float>::PixelPtr imr;
 ImageRGB<unsigned char>::PixelPtr resr;

  int r0 = im.ROI().Top();
  int r1 = im.ROI().Bottom();
  int w = im.ROI().Width();
  for (int r=r0; r<r1; r++) {
    imr = im.GetROIRowStart(r);
    resr = res.GetROIRowStart(r);
    for (int c=0; c<w; c++, resr++, imr++) {
      resr->r = resr->g = resr->b = (unsigned char) imr->y;
	  resr->alpha = 0;
    }
  }

  return;
}


// take least significant byte
template <>
void ImageCopy(const ImageMono<int> &im, 
	       ImageMono<unsigned char> &res)
{
  if (!res.IsAlloced()) {
    res.AllocImageData(im.GetWidth(), im.GetHeight());
  } else {
    assert(res.GetWidth() == im.GetWidth() &&
	   res.GetHeight() == im.GetHeight());
  }

  res.SetROI(im.ROI());


  ImageMono<int>::PixelPtr imr;
  ImageMono<unsigned char>::PixelPtr resr;

  int r0 = im.ROI().Top();
  int r1 = im.ROI().Bottom();
  int w = im.ROI().Width();
  for (int r=r0; r<r1; r++) {
    imr = im.GetROIRowStart(r);
    resr = res.GetROIRowStart(r);
    for (int c=0; c<w; c++, imr++, resr++) {
      resr->Set(imr->y & 0xFF);
    }
  }

  return;
}


template <>
void ImageCopy(const ImageRGB<float> &im, 
	       ImageRGB<unsigned char> &res)
{
  if (!res.IsAlloced()) {
    res.AllocImageData(im.GetWidth(), im.GetHeight());
  } else {
    assert(res.GetWidth() == im.GetWidth() &&
	   res.GetHeight() == im.GetHeight());
  }

  res.SetROI(im.ROI());


  ImageRGB<float>::PixelPtr imr;
  ImageRGB<unsigned char>::PixelPtr resr;

  int r0 = im.ROI().Top();
  int r1 = im.ROI().Bottom();
  int w = im.ROI().Width();
  for (int r=r0; r<r1; r++) {
    imr = im.GetROIRowStart(r);
    resr = res.GetROIRowStart(r);
    for (int c=0; c<w; c++, imr++, resr++) {
      if (finite(imr->r)) 
	resr->r = VW::ImageConversions::CLIPVALUE(int(imr->r + 0.5f));
      else resr->r = 0;

      if (finite(imr->g)) 
	resr->g = VW::ImageConversions::CLIPVALUE(int(imr->g + 0.5f));
      else resr->g = 0;

      if (finite(imr->b)) 
	resr->b = VW::ImageConversions::CLIPVALUE(int(imr->b + 0.5f));
      else resr->b = 0;

      if (finite(imr->alpha)) 
	resr->alpha = VW::ImageConversions::CLIPVALUE(int(imr->alpha + 0.5f));
      else resr->alpha = 0;
    }
  }

  return;
}




#ifdef INSTANTIATE
#  undef INSTANTIATE
#endif
# define INSTANTIATE(M, N) \
  template void ImageCopy<M,N >(const M&, N&); \
  template void ImageFlipCopy<M,N >(const M&, N&);

// Direct copies
INSTANTIATE(ImageMono<unsigned char>, ImageMono<unsigned char>); 
INSTANTIATE(ImageMono<float>,         ImageMono<float>); 
INSTANTIATE(ImageRGB<unsigned char>,  ImageRGB<unsigned char>); 
INSTANTIATE(ImageRGB<float>,          ImageRGB<float>); 
INSTANTIATE(ImageYUV<unsigned char>,  ImageYUV<unsigned char>); 
INSTANTIATE(ImageYUV<float>,          ImageYUV<float>); 

// Value type conversion
INSTANTIATE(ImageMono<unsigned char>, ImageMono<float>); 
INSTANTIATE(ImageMono<unsigned int>,  ImageMono<unsigned char>); 
INSTANTIATE(ImageMono<double>,        ImageMono<unsigned char>); 
INSTANTIATE(ImageMono<unsigned char>, ImageMono<double>); 
INSTANTIATE(ImageYUV<unsigned char>,  ImageYUV<float>); 
INSTANTIATE(ImageYUV<unsigned int>,   ImageYUV<unsigned char>); 

// Format conversion (to Mono)
INSTANTIATE(ImageRGB<unsigned char>, ImageMono<unsigned char>); 
INSTANTIATE(ImageYUV<unsigned char>, ImageMono<unsigned char>); 
INSTANTIATE(ImageRGB<float>,         ImageMono<float>); 
INSTANTIATE(ImageYUV<float>,         ImageMono<float>); 

// Format conversion (to RGB)
INSTANTIATE(ImageMono<unsigned char>, ImageRGB<unsigned char>); 
INSTANTIATE(ImageYUV<unsigned char>,  ImageRGB<unsigned char>); 
INSTANTIATE(ImageHSV<unsigned char>,  ImageRGB<unsigned char>); 
INSTANTIATE(ImageMono<float>,         ImageRGB<float>); 
INSTANTIATE(ImageYUV<float>,          ImageRGB<float>); 
INSTANTIATE(ImageHSV<float>,          ImageRGB<float>); 

// Format conversion (to YUV)
INSTANTIATE(ImageMono<unsigned char>, ImageYUV<unsigned char>); 
INSTANTIATE(ImageRGB<unsigned char>,  ImageYUV<unsigned char>); 
INSTANTIATE(ImageMono<float>,         ImageYUV<float>); 
INSTANTIATE(ImageRGB<float>,          ImageYUV<float>); 


#undef INSTANTIATE







} // End namespace VW
