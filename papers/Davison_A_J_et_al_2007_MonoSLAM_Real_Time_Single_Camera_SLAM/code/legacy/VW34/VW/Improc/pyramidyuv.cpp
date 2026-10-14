/** 
 * @file   pyramidyuv.cpp
 * @author BJ Tordoff
 * @date   10/08/2004
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

// System includes
#include <iostream>
#include <math.h>

// VW includes
#include <VW/image.h>
#include <VW/MathBits/nan.h>

// Local includes
#include <VW/Improc/subsample.h>
#include <VW/Improc/convolve.h>
#include "pyramidyuv.h"
#include "scaleimage.h"

using namespace std;
using namespace VW;

// template<class StorageType> void 
// PyramidYUV<StorageType>::location(int c, int r, int nsrc, int ndest, int &cres, int &rres) 
// {
//   int c0src, r0src, clsrc, rlsrc;
//   int c0dest, r0dest, cldest, rldest;
//   int factor = (int)(pow(2.0, (double)(nsrc-ndest)));
//   level[nsrc]->GetROI().Get(c0src, r0src, clsrc, rlsrc);
//   level[ndest]->GetROI().Get(c0dest, r0dest, cldest, rldest);
//   cres = factor*(c-c0dest) + c0src;
//   rres = factor*(r-r0dest) + r0src;
// }


template<class StorageType> template <class ImageType> void 
PyramidYUV<StorageType>::Compute(const ImageType &im, 
			      unsigned int minsize, 
			      unsigned int num_levels_per_octave, 
			      bool quick_and_dirty)
{
  float size = im.ROI().Width() < im.ROI().Height() ? float(im.ROI().Width()) : float(im.ROI().Height());
  num_levels=1;
  level.push_back(new ImageYUV<StorageType> (im.GetWidth(), im.GetHeight()));

  // 1st level is always just original image
  VW::ImageCopy(im,*(level[0]));
  if (num_levels_per_octave>1) {
    // Other initial images are formed by scaling
    scaling_factor = 1.0f / float(pow(2.0, 1.0/double(num_levels_per_octave)));
	
    float current_scale = scaling_factor;
    for (unsigned int l=1; l<num_levels_per_octave; 
	 l++, current_scale *= scaling_factor) {
      level.push_back(new ImageYUV<StorageType>() );
      VW::ScaleImage s(current_scale,Bilinear);
      s.Compute( *(level[0]), *(level[l]) );
      num_levels++;
  }

  } else {
    scaling_factor = 0.5f;
  }
  // Done first octave, so size is half what it was
  size *= 0.5;

  if (quick_and_dirty) {
    // The BJT method
    while (size >= minsize) {
       ImageYUV<StorageType>* this_lev = level[num_levels-num_levels_per_octave];

      // Add the new level
      const unsigned int new_width = this_lev->GetWidth()/2;
      const unsigned int new_height = this_lev->GetHeight()/2;
      level.push_back (new ImageYUV<StorageType>(new_width, new_height));
      ImageYUV<StorageType>* next_lev = level[num_levels];

      // Fill it with stuff
      _AverageAndSubsampleBy2(*this_lev, *next_lev);
      
      size *= scaling_factor;
      //cerr << "   pyramid size="<<size<<" (min is "<<minsize<<")"<<endl;
      num_levels++;
    }

  } else {
    // The IDR method
    ConvolutionMask1D<float> gaussian(5);
    gaussian.MakeGaussian(2.0);
    
    while (size >= minsize) {
       ImageYUV<StorageType>* this_lev = level[num_levels-num_levels_per_octave];
      
      // Add the new level
      const unsigned int new_width = this_lev->GetWidth()/2;
      const unsigned int new_height = this_lev->GetHeight()/2;
      level.push_back (new ImageYUV<StorageType> (new_width, new_height));
      ImageYUV<StorageType>* next_lev = level[num_levels];

      // Fill it with stuff
      Convolve<float> (*this_lev, 
		       gaussian, gaussian, 
		       *next_lev, 
		       2);
 
      size *= scaling_factor;
      //cerr << "   pyramid size="<<size<<" (min is "<<minsize<<")"<<endl;
      num_levels++;
    }  
  }


}


template<class StorageType> void 
PyramidYUV<StorageType>::_AverageAndSubsampleBy2(const VW::ImageYUV<StorageType>& source_image,
				     VW::ImageYUV<StorageType>& dest_image) 
{
  int c0, r0, cl, rl;
  int r, c;

  VW::PixelYUV<StorageType> *source_pixel0, *source_pixel1, *dest_pixel;

  const double scale = 0.25;

  if (!dest_image.IsAlloced()) {
    dest_image.AllocImageData(source_image.GetWidth()/2, source_image.GetHeight()/2);
  } else {
    if (dest_image.GetWidth() != source_image.GetWidth()/2 ||
	dest_image.GetHeight() != source_image.GetHeight()/2) {
      cerr << "PyramidYUV::AverageAndSubsample: Subsample image size incorrect\n";
      return;
    }
  }

  // Set the region of interest
  source_image.ROI().Get(c0,r0,cl,rl);
  c0 /= 2; r0 /= 2;
  cl /= 2; rl /= 2;
  dest_image.ROI().Set(c0, r0, cl, rl);
  
  for (r=r0; r<rl; r++) {
    dest_pixel = dest_image.GetROIRowStart(r);
    source_pixel0 = source_image.GetROIRowStart(2*r);
    source_pixel1 = source_image.GetROIRowStart(2*r+1);
    for (c=c0; c<cl; c++) {
      // Sum the pixels to be discarded
      VW::PixelYUV<double> accumulator(0.0,0.0,0.0,0.0);
      accumulator.y += source_pixel0->y;
      accumulator.u += source_pixel0->u;
      accumulator.v += source_pixel0->v;
      accumulator.alpha += (source_pixel0++)->alpha;

      accumulator.y += (source_pixel0)->y;
      accumulator.u += (source_pixel0)->u;
      accumulator.v += (source_pixel0)->v;
      accumulator.alpha += (source_pixel0++)->alpha;

      accumulator.y += (source_pixel1)->y;
      accumulator.u += (source_pixel1)->u;
      accumulator.v += (source_pixel1)->v;
      accumulator.alpha += (source_pixel1++)->alpha;

      accumulator.y += (source_pixel1)->y;
      accumulator.u += (source_pixel1)->u;
      accumulator.v += (source_pixel1)->v;
      accumulator.alpha += (source_pixel1++)->alpha;

      // Store the average result
      dest_pixel->y = StorageType(accumulator.y*scale);
      dest_pixel->u = StorageType(accumulator.u*scale);
      dest_pixel->v = StorageType(accumulator.v*scale);
      dest_pixel->alpha = StorageType(accumulator.alpha*scale);
      dest_pixel++;
    }
  }
  return;
}
			 

#ifdef INSTANTIATE
#  undef INSTANTIATE
#endif
# define INSTANTIATE(M, ImType) \
  template void PyramidYUV<M>::Compute<ImType >(const ImType&, unsigned int, \
                                             unsigned int, bool);

// Mono versions
// INSTANTIATE(unsigned char, VW::ImageMono<unsigned char>); 
// INSTANTIATE(float, VW::ImageMono<unsigned char>); 
// INSTANTIATE(float, VW::ImageMono<float>); 

// YUV versions
INSTANTIATE(unsigned char, VW::ImageYUV<unsigned char>); 
INSTANTIATE(float, VW::ImageYUV<unsigned char>); 

#undef INSTANTIATE
