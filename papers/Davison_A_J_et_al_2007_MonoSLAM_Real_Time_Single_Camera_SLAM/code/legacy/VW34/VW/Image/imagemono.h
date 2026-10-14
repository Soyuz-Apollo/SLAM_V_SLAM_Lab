/** @file imagemono.h
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
/**
 * @class   VW::ImageMono
 * @ingroup Image
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/02
 * @brief   A class for storing monochrome images.
 *
 * This image class stores the image in a row-major packed format (ie. like a
 * standard C/C++ array), with pixels as single values of type Type.
 */


#ifndef _imagemono_h_
#define _imagemono_h_

#include "imagebase.h"
#include "pixelmono.h"


namespace VW {

template <class Type>
class ImageMono : public ImageBase< PixelMono<Type>, PixelMono<Type> * > {
    // Shorthand for referring to base class
    typedef ImageBase<PixelMono<Type>, PixelMono<Type>* > Base;

 public:    

  /** Construct and read a file. */
  ImageMono(const std::string& filename);

  /** Construct and allocate the image data array. */
  ImageMono(const int w, const int h);

  /** Construct without allocating the data array. 
   * Before use, AllocImageData should be used to specify the image size.
   */
  ImageMono();

  /** Construct from a data block */
  ImageMono(PixelMono<Type> *buf, const int w, const int h); 

  /** Destroy all data. */
  ~ImageMono();

  /** Get a linearly interpolated pixel value. */
  PixelMono<double> GetInterpolatedPixel(double x,double y) const;

  /** Get a linearly interpolated pixel value. */
  PixelMono<float> GetInterpolatedPixel(float x,float y) const;

  // Pixel info
  typedef Type            DataType;
  typedef PixelMono<Type> PixelType;
  typedef PixelType*      PixelPtr;


  // Image IO (NUMIOFILTERS from imageio.h)
  //static const IOFilter iofilters[NUMIOFILTERS];

  /** Read the image from an open stream. */
  bool ReadImage(FILE *fp);

  /** Write the image to an open stream. */
  bool WriteImage(FILE *fp) const;

  /** Read the image from a file on disk. The filename suffix is used to
   * determine the desired image format (eg gif, jpeg, png) a la Windows. If the
   * image size has already been specified, the stored image must have the
   * same size as the target, otherwise the size will be determined from the 
   * file image. 
   * @returns true for successful loading, false if there was an error.
   */
  bool ReadImage(const std::string& filename);

  /** Write the image as a file. The filename suffix is used to determine the
   * desired image format (eg gif, jpeg, png) a la Windows. If the file
   * already exists, it will be overwritten.
   * @returns true for successful loading, false if there was an error.
   */
  bool WriteImage(const std::string& filename) const;



protected:
   void GetFormatStrings(std::string& format, 
			 std::string& type) const;
};

}; // end namespace VW










// Inlines

namespace VW {

template <class Type> inline
VW::ImageMono<Type>::ImageMono(const std::string& filename)
{
  ReadImage(filename);
}

template <class Type> inline
VW::ImageMono<Type>::ImageMono(const int w, const int h) 
    : VW::ImageBase<VW::PixelMono<Type>, VW::PixelMono<Type> * >(w, h) 
{
};

template <class T> inline
VW::ImageMono<T>::ImageMono() 
{
};

template <class Type> inline
VW::ImageMono<Type>::ImageMono(PixelMono<Type> *buf, const int w, const int h) 
  : ImageBase<PixelType,PixelPtr>(buf, w, h) 
{
}; 

template <class T> inline
VW::ImageMono<T>::~ImageMono() 
{
};

// Specialise header for standard types
template <> inline void 
VW::ImageMono<unsigned char>::GetFormatStrings(std::string& format, 
					      std::string& type) const
{
  format = "Mono";
  type =   "UChar";
}

template <> inline void 
VW::ImageMono<unsigned int>::GetFormatStrings(std::string& format, 
					      std::string& type) const
{
  format = "Mono";
  type =   "UInt";
}

template <> inline void 
VW::ImageMono<float>::GetFormatStrings(std::string& format, 
					      std::string& type) const
{
  format = "Mono";
  type =   "Float";
}

template <> inline void 
VW::ImageMono<bool>::GetFormatStrings(std::string& format, 
					      std::string& type) const
{
  format = "Mono";
  type =   "Bool";
}

}; //namespace VW

// Default is that it is not supported
template <class T> inline void 
VW::ImageMono<T>::GetFormatStrings(std::string& format, 
				   std::string& type) const
{
  format = "Mono";
  type =   "Unknown";
}

#endif /* _imagemono_h_ */
