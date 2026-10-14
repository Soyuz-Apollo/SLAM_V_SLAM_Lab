/** 
 * @file   pyramid.cpp
 * @author ID Reid, JGH Knight
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

// SYstem includes
#include <iostream>
#include <math.h>

// VW includes
#include "VW/image.h"
#include "VW/MathBits/nan.h"

// Local includes
#include "subsample.h"
#include "convolve.h"
#include "pyramid.h"
#include "scaleimage.h"

using namespace std;
using namespace VW;

template<class StorageType> void 
Pyramid<StorageType>::location(int c, int r, int nsrc, int ndest, int &cres, int &rres) 
{
  int c0src, r0src, clsrc, rlsrc;
  int c0dest, r0dest, cldest, rldest;
  int factor = (int)(pow(2.0, (double)(nsrc-ndest)));
  level[nsrc]->ROI().Get(c0src, r0src, clsrc, rlsrc);
  level[ndest]->ROI().Get(c0dest, r0dest, cldest, rldest);
  cres = factor*(c-c0dest) + c0src;
  rres = factor*(r-r0dest) + r0src;
}

template<class StorageType> void 
VW::Pyramid<StorageType>::AllocMemory(unsigned int base_width,
				      unsigned int base_height,
				      unsigned int minsize)
{
  //  cerr << "Alloc pyramid with no image" << endl;
  float size = base_width<base_height ? float(base_width) : float(base_height);
  _num_levels=1;
  level.push_back(new ImageMono<StorageType> (base_width, base_height));
  if (_num_levels_per_octave>1) {
    // Other initial images are formed by scaling
    scaling_factor = 1.0f/float(pow(2.0, 1.0/double(_num_levels_per_octave)));
    
    float current_scale = scaling_factor;
    for (unsigned int l=1; l<_num_levels_per_octave; 
	 l++, current_scale *= scaling_factor) {
      const unsigned int new_width = int(current_scale*base_width);
      const unsigned int new_height = int(current_scale*base_height);
      level.push_back(new ImageMono<StorageType>(new_width,new_height) );
      _num_levels++;
    }
  } else {
    scaling_factor = 0.5f;
  }
  //  cerr << "   built 1st octave (" << _num_levels << " levels)" << endl;
 // Done first octave, so size is half what it was
  size *= 0.5f;
  while (int(size+0.5f) >= minsize) {
    const ImageMono<StorageType>* this_lev = level[_num_levels-_num_levels_per_octave];
    
    // Add the new level
    const unsigned int new_width = this_lev->GetWidth()/2;
    const unsigned int new_height = this_lev->GetHeight()/2;
    //    cerr << "   create level " << _num_levels << " using level " << _num_levels-_num_levels_per_octave << " ("<<new_width<<"x"<<new_height<<")>"<< minsize << endl;
    level.push_back (new ImageMono<StorageType> (new_width, new_height));
    size *= scaling_factor;
    _num_levels++;
  }
  //   cerr << "   stopped at size "<< size << endl;
 
}

template<class StorageType> template <class ImageType> void 
Pyramid<StorageType>::Compute(const ImageType &im, 
			      unsigned int minsize, 
			      unsigned int num_levels_per_octave, 
			      bool quick_and_dirty)
{
  float size = im.ROI().Width() < im.ROI().Height() ? float(im.ROI().Width()) : float(im.ROI().Height());
  _num_levels=1;
  level.push_back(new ImageMono<StorageType> (im.GetWidth(), im.GetHeight()));

  // 1st level is always just original image
  VW::ImageCopy(im,*(level[0]));
  if (_num_levels_per_octave>1) {
    // Other initial images are formed by scaling
    scaling_factor = 1.0f / float(pow(2.0, 1.0/double(_num_levels_per_octave)));

    float current_scale = scaling_factor;
    for (unsigned int l=1; l<_num_levels_per_octave; 
	 l++, current_scale *= scaling_factor) {
      level.push_back(new ImageMono<StorageType>() );
      VW::ScaleImage s(current_scale,Bilinear);
      s.Compute( *(level[0]), *(level[l]) );
      _num_levels++;
   }

  } else {
    scaling_factor = 0.5f;
  }
  // Done first octave, so size is half what it was
  size *= 0.5f;

  if (quick_and_dirty) {
    // The BJT method
    while (size >= minsize) {
       ImageMono<StorageType>* this_lev 
	 = level[_num_levels - _num_levels_per_octave];

      // Add the new level
      const unsigned int new_width = this_lev->GetWidth()/2;
      const unsigned int new_height = this_lev->GetHeight()/2;
      level.push_back (new ImageMono<StorageType>(new_width, new_height));
      ImageMono<StorageType>* next_lev = level[_num_levels];

      // Fill it with stuff
      AverageAndSubsampleBy2(*this_lev, *next_lev);
      
      size *= scaling_factor;
      _num_levels++;
    }

  } else {
    // The IDR method
    ConvolutionMask1D<float> gaussian(5);
    gaussian.MakeGaussian(2.0);
    
    while (size >= minsize) {
       ImageMono<StorageType>* this_lev 
	 = level[_num_levels - _num_levels_per_octave];
      
      // Add the new level
      const unsigned int new_width = this_lev->GetWidth()/2;
      const unsigned int new_height = this_lev->GetHeight()/2;
      level.push_back (new ImageMono<StorageType> (new_width, new_height));
      ImageMono<StorageType>* next_lev = level[_num_levels];

      // Fill it with stuff
      Convolve<float> (*this_lev, 
		       gaussian, gaussian, 
		       *next_lev, 
		       2);
 
      size *= scaling_factor;
      _num_levels++;
    }  
  }

}


template<class StorageType> 
void Pyramid<StorageType>::AverageAndSubsampleBy2(const VW::ImageMono<StorageType>& source_image,
				     VW::ImageMono<StorageType>& dest_image) 
{
  int c0, r0, cl, rl;
  int r, c;

  VW::PixelMono<StorageType> *source_pixel0, *source_pixel1, *dest_pixel;

  const double scale = 0.25;

  if (!dest_image.IsAlloced()) {
    dest_image.AllocImageData(source_image.GetWidth()/2, source_image.GetHeight()/2);
  } else {
    if (dest_image.GetWidth() != source_image.GetWidth()/2 ||
	dest_image.GetHeight() != source_image.GetHeight()/2) {
      cerr << "Pyramid::AverageAndSubsample: Subsample image size incorrect\n";
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
      VW::PixelMono<double> accumulator(0.0);
      accumulator.y += (source_pixel0++)->y;
      accumulator.y += (source_pixel0++)->y;
      accumulator.y += (source_pixel1++)->y;
      accumulator.y += (source_pixel1++)->y;
      // Store the average result
      (*dest_pixel) = accumulator*scale;
      dest_pixel++;
    }
  }
  return;
}
			 

#ifdef INSTANTIATE
#  undef INSTANTIATE
#endif
# define INSTANTIATE(M) \
  template class Pyramid<M>; \

#ifdef INSTANTIATEFUNCS
#  undef INSTANTIATEFUNCS
#endif
# define INSTANTIATEFUNCS(M, ImType) \
  template void Pyramid<M>::Compute<ImType >(const ImType&, unsigned int, \
                                             unsigned int, bool);

INSTANTIATE(unsigned char); 
INSTANTIATE(float); 

// Mono versions
INSTANTIATEFUNCS(unsigned char, VW::ImageMono<unsigned char>); 
INSTANTIATEFUNCS(float,         VW::ImageMono<unsigned char>); 
INSTANTIATEFUNCS(float,         VW::ImageMono<float>); 

// YUV versions
INSTANTIATEFUNCS(unsigned char, VW::ImageYUV<unsigned char>); 
INSTANTIATEFUNCS(float,         VW::ImageYUV<unsigned char>); 

// RGB versions
INSTANTIATEFUNCS(unsigned char, VW::ImageRGB<unsigned char>);
INSTANTIATEFUNCS(float,         VW::ImageRGB<unsigned char>);

#undef INSTANTIATEFUNCS
#undef INSTANTIATE
