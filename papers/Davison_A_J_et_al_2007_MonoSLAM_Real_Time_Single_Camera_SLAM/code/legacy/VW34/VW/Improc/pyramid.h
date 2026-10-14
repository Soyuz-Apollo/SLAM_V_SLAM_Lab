/** 
 * @file   pyramid.h
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
 * Modified 14/6/2004 bjt: Add ability to have multiple images per scale octave
 */
/** 
 * @class   VW::Pyramid
 * @ingroup ImageProcessing
 * @author  ID Reid, JGH Knight
 * @date    02/08/2002
 * @brief   Construct an image pyramid from an image.
 *
 * The image pyramid consists of subsampled octaves of the base image, but
 * whereas the input image can be in any format (providing it can be converted
 * to the storage type), the pyramid is always monochrome (perhaps should 
 * PyramidMono?).
 * Subsampling can be performed either using the correct method of
 * Gaussian smoothing at the nyquist frequency followed by decimation, or
 * using the faster method of averaging 2x2 blocks of pixels before decimation.
 */


#ifndef _imagepyramid_h_
#define _imagepyramid_h_

#include <vector>
#include <VW/Image/imagemono.h>

namespace VW {

template<class StorageType>
class Pyramid {
 public:

  /** Default constructor.
   * If you do not supply an image at object creation, you will have to subsequently
   * call one of the compute methods below.
   */
  Pyramid();

  /** Construct and reserve memory for manual manipulation.
   * Only use this if you want to fill the pyramid yourself later.
   */
  Pyramid(unsigned int base_width, unsigned int base_height,
	  unsigned int minsize = 32,
	  unsigned int num_levels_per_octave = 1);

  /** Construct and compute (note that the pyramid
   * is monochrome regardless of the input image type).
   * This constructs the object and calls the compute function with (optional)
   * parameters for size, number of levels etc.
   */
  template <class ImageType>
  explicit Pyramid(const ImageType &im, 
	  unsigned int minsize = 32,
	  unsigned int num_levels_per_octave = 1,
	  bool quick_and_dirty = false);

  ~Pyramid();

  /** Compute the pyramid based on the supplied image (note that the pyramid
   * is monochrome regardless of the input image type).
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

  /** BJT: Anyone know what this is for? Why it is lower case etc? */
  void location(int c, int r, int nsrc, int ndest, int &cres, int &rres);

  /** The number of levels (images) contained in this pyramid. */
  unsigned int GetNumLevels () const;
  unsigned int NumLevels () const;

  /** The number of levels (images) in each scale octave of this pyramid. */
  unsigned int GetNumLevelsPerOctave() const;

  /** The number of (complete) scale octaves in this pyramid. */
  unsigned int GetNumOctaves() const;

  /** The smallest dimension of the levels. */
  unsigned int GetMinSize() const;

  /** The width of the specified level (or the largest width if 
   * not specified). */
  unsigned int GetWidth(unsigned int which_level=0) const;
  /** The height of the specified level (or the largest height if 
   * not specified). */
  unsigned int GetHeight(unsigned int which_level=0) const;

  /** Get access to the array of images. */
  const std::vector<ImageMono<StorageType>*>& Level () const;

  /** Access a particular level (read only). */
  const ImageMono<StorageType>& operator[] (int i) const;

  /** Access a particular level (read/write). */
  ImageMono<StorageType>& operator[] (int i);

  /** The scale factor between successive levels (0.5 for sumbsample by 2) */
  float ScalingFactor() const;

  /** Get the scale factor associated with a level of the pyramid (this
   * is the ratio of the image size compared to the base image size, and
   * is always less than 1).
   */
  float GetScale(unsigned int which_level) const;

  /** Determine the level that is closest to this scale. */
  int ClosestLevel(float scale) const;

protected:
  unsigned int _num_levels, _num_levels_per_octave;
  float scaling_factor;
  std::vector<ImageMono<StorageType> *> level;   // a vector of pointers to image

  void AllocMemory(unsigned int base_width,
		   unsigned int base_height,
		   unsigned int minsize);

  /** A function to combine smoothing and subsampling in a quick and dirty
   * fashion.
   */
  void AverageAndSubsampleBy2(const VW::ImageMono<StorageType>& source_image,
			      VW::ImageMono<StorageType>& dest_image);
};



}; // end namespace VW



// INLINE Stuff


template<class StorageType> inline 
VW::Pyramid<StorageType>::Pyramid() 
  : _num_levels(0), 
    _num_levels_per_octave(1), 
    scaling_factor(0.5) 
{
}

template<class StorageType> template <class ImageType> inline 
VW::Pyramid<StorageType>::Pyramid(const ImageType &im, 
				  unsigned int minsize,
				  unsigned int num_levels_per_octave,
				  bool quick_and_dirty) 
  : _num_levels_per_octave(num_levels_per_octave)
{
  Compute(im,minsize,num_levels_per_octave,quick_and_dirty); 
}


template<class StorageType> inline
VW::Pyramid<StorageType>::Pyramid(unsigned int base_width,
			      unsigned int base_height,
			      unsigned int minsize,
			      unsigned int num_levels_per_octave)
  : _num_levels_per_octave(num_levels_per_octave)
{
  AllocMemory(base_width,base_height,minsize);
}

template<class StorageType> inline 
VW::Pyramid<StorageType>::~Pyramid()
{ 
  for (unsigned int i=0; i<_num_levels; i++) delete level[i]; 
}


template<class StorageType>
inline unsigned int 
VW::Pyramid<StorageType>::NumLevels () const 
{
  return _num_levels;
}

template<class StorageType>
inline unsigned int 
VW::Pyramid<StorageType>::GetNumLevels () const 
{
  return _num_levels;
}

template<class StorageType>
inline unsigned int VW::Pyramid<StorageType>::GetNumLevelsPerOctave() const
{
  return _num_levels_per_octave;
}

template<class StorageType>
inline unsigned int VW::Pyramid<StorageType>::GetNumOctaves() const
{
  return (_num_levels-1)/_num_levels_per_octave;
}

template<class StorageType>
inline unsigned int VW::Pyramid<StorageType>::GetMinSize() const
{
  const unsigned int w = level[_num_levels-1]->GetWidth();
  const unsigned int h = level[_num_levels-1]->GetHeight();
  if (w>h) return h;
  return w;
}

template<class StorageType>
inline unsigned int 
VW::Pyramid<StorageType>::GetWidth(unsigned int which_level) const 
{
  return(level[which_level]->GetWidth());
}

template<class StorageType>
inline unsigned int 
VW::Pyramid<StorageType>::GetHeight(unsigned int which_level) const 
{
  return(level[which_level]->GetHeight());
}

template<class StorageType>
inline const std::vector<VW::ImageMono<StorageType>*>& 
VW::Pyramid<StorageType>::Level () const 
{
  return level;
}

template<class StorageType>
inline const VW::ImageMono<StorageType>&
VW::Pyramid<StorageType>::operator[] (int i) const
{
  return *(level[i]);
}

template<class StorageType>
inline VW::ImageMono<StorageType>&
VW::Pyramid<StorageType>::operator[] (int i)
{
  return *(level[i]);
}

template<class StorageType>
inline float VW::Pyramid<StorageType>::ScalingFactor() const
{
  return scaling_factor;
}

template<class StorageType> inline 
float VW::Pyramid<StorageType>::GetScale(unsigned int which_level) const
{
  /* Unfortunately, we have to force it to use pow(double, int) by lots of
     casting (note that the int version can be significantly faster than the 
     pow(float,float) version) */
  return float(pow(double(scaling_factor),int(which_level)));
}

template<class StorageType> inline int 
VW::Pyramid<StorageType>::ClosestLevel(float scale) const
{
  const double lev = log(scale)/log(scaling_factor);
  return int(lev+0.5);
}

#endif // #ifndef _pyramid_h_
