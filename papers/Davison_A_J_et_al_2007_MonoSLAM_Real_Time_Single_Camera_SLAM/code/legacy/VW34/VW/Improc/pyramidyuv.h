/** 
 * @file   pyramidyuv.h
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
 * Modified 14/6/2004 bjt: Add ability to have multiple images per scale octave
 */
/** 
 * @class   VW::PyramidYUV
 * @ingroup ImageProcessing
 * @author  BJ Tordoff
 * @date    10/08/2004
 * @brief   Construct an image pyramid from a YUV image.
 *
 * The YUV image pyramid consists of subsampled octaves of the base image.
 * Subsampling can be performed either using the correct method of
 * Gaussian smoothing at the nyquist frequency followed by decimation, or
 * using the faster method of averaging 2x2 blocks of pixels before decimation.
 */


#ifndef _imagepyramidyuv_h_
#define _imagepyramidyuv_h_

#include <vector>
#include <VW/Image/imageyuv.h>

namespace VW {

template<class StorageType>
class PyramidYUV {
 public:

  /** Default constructor.
   * If you do not supply an image at object creation, you will have to subsequently
   * call one of the compute methods below.
   */
  PyramidYUV() : num_levels(0), scaling_factor(0.5) {}

  /** Construct and compute (note that the pyramid
   * is monochrome regardless of the input image type).
   * This constructs the object and calls the compute function with (optional)
   * parameters for size, number of levels etc.
   */
  template <class ImageType>
  PyramidYUV(const ImageType &im, 
	  unsigned int minsize = 32,
	  unsigned int num_levels_per_octave = 1,
	  bool quick_and_dirty = false);

  ~PyramidYUV();

  /** Compute the pyramid based on the supplied image (note that the pyramid
   * is YUV regardless of the input image type).
   * Only call the compute function if you didn't construct the object with
   * an image. If quick and dirty is false (default) then Ian's method
   * is used, where convolution is with an approximately Gaussian kernel.
   * The alternative method just averages 2x2 blocks of pixels, which may
   * give some marginal speed improvements.
   */
  template <class ImageType>
  void Compute(const ImageType &im, 
	       unsigned int minsize = 32, 
	       unsigned int num_levels_per_octave = 1,
	       bool quick_and_dirty = false);

  /** The number of levels (images) contained in this pyramid. */
  unsigned int NumLevels() const;

  /** The width of the specified level (or the largest width if 
   * not specified). */
  unsigned int GetWidth(unsigned int which_level=0) const;
  /** The height of the specified level (or the largest height if 
   * not specified). */
  unsigned int GetHeight(unsigned int which_level=0) const;

  /** Get access to the array of images. */
  const std::vector<ImageYUV<StorageType>*>& Level () const;

  /** Access a particular level (read only). */
  const ImageYUV<StorageType>& operator[] (int i) const;

  /** Access a particular level (read/write). */
  ImageYUV<StorageType>& operator[] (int i);

  /** The scale factor between successive levels (0.5 for sumbsample by 2) */
  float ScalingFactor() const;

  /** Get the sclae factor associated with a level of the pyramid (this
   * is the ratio of the image size compared to the base image size, and
   * is always less than 1).
   */
  float GetScale(unsigned int which_level) const;

protected: // Data
  unsigned int num_levels;
  float scaling_factor;
  std::vector<ImageYUV<StorageType> *> level;

protected: // Funcs
  /** A function to combine smoothing and subsampling in a quick and dirty
   * fashion.
   */
  void _AverageAndSubsampleBy2(const VW::ImageYUV<StorageType>& source_image,
			      VW::ImageYUV<StorageType>& dest_image);
};



}; // end namespace VW



// INLINE Stuff


template<class StorageType> template <class ImageType> inline 
VW::PyramidYUV<StorageType>::PyramidYUV(const ImageType &im, 
				  unsigned int minsize,
				  unsigned int num_levels_per_octave,
				  bool quick_and_dirty) 
{
  Compute(im,minsize,num_levels_per_octave,quick_and_dirty); 
}



template<class StorageType> inline 
VW::PyramidYUV<StorageType>::~PyramidYUV()
{ 
  for (unsigned int i=0; i<num_levels; i++) delete level[i]; 
}


template<class StorageType>
inline unsigned int 
VW::PyramidYUV<StorageType>::NumLevels () const 
{
  return num_levels;
}

template<class StorageType>
inline unsigned int 
VW::PyramidYUV<StorageType>::GetWidth(unsigned int which_level) const 
{
  return(level[which_level]->GetWidth());
}

template<class StorageType>
inline unsigned int 
VW::PyramidYUV<StorageType>::GetHeight(unsigned int which_level) const 
{
  return(level[which_level]->GetHeight());
}

template<class StorageType>
inline const std::vector<VW::ImageYUV<StorageType>*>& 
VW::PyramidYUV<StorageType>::Level () const 
{
  return level;
}

template<class StorageType>
inline const VW::ImageYUV<StorageType>&
VW::PyramidYUV<StorageType>::operator[] (int i) const
{
  return *(level[i]);
}

template<class StorageType>
inline VW::ImageYUV<StorageType>&
VW::PyramidYUV<StorageType>::operator[] (int i)
{
  return *(level[i]);
}

template<class StorageType>
inline float VW::PyramidYUV<StorageType>::ScalingFactor() const
{
  return scaling_factor;
}

template<class StorageType> inline 
float VW::PyramidYUV<StorageType>::GetScale(unsigned int which_level) const
{
  /* Unfortunately, we have to force it to use pow(double, int) by lots of
     casting (note that the int version can be significantly faster than the 
     pow(float,float) version) */
  return float(pow(double(scaling_factor),int(which_level)));
}

#endif // #ifndef _pyramidyuv_h_
