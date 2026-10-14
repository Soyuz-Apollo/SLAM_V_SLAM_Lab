/** @file imageyuv.h
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
 * @class   VW::ImageYUV
 * @ingroup Image
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/02
 * @brief   A class for storing YUV (YCrCb?) images.
 *
 * This image class stores the image in a row-major format (ie. like a
 * standard C/C++ array), with pixels as YUVA where A is a spare (alpha?)
 * channel. This way pixels stored using 8-bit integers (ie. unsigned chars, as
 * are produced by many cameras, and used on most displays)
 * are aligned on 4-byte memory locations, adding a tad to performance. 
 * Strictly speaking this (and the related conversions) are really Y Cr Cb
 * rather than YUV, but looking around the 'net the terminology seems to be 
 * interchangeable.
 */


#ifndef _imageyuv_h_
#define _imageyuv_h_

#include "imagebase.h"
#include "pixelyuv.h"


namespace VW {


template <class Type /*= unsigned char*/>
class ImageYUV : public ImageBase< PixelYUV<Type>, PixelYUV<Type> * > {
    // Shorthand for referring to base class
    typedef ImageBase<PixelYUV<Type>, PixelYUV<Type>* > Base;

 public:    

  /** Construct and read a file. */
  ImageYUV(const std::string& filename);

  ImageYUV(const int w, const int h);
  ImageYUV() {};
  ~ImageYUV() {};

  /** Get a linearly interpolated pixel value. */
  PixelYUV<double> GetInterpolatedPixel(double x,double y) const;

  bool ReadImage(const std::string& filename);
  bool WriteImage(const std::string& filename) const;

  // Pixel info
  typedef Type           DataType;
  typedef PixelYUV<Type> PixelType;
  typedef PixelType*     PixelPtr;

protected:
   void GetFormatStrings(std::string& format, 
			 std::string& type) const;
};


}; // end namespace VW






// Inlines
template <class Type> inline
VW::ImageYUV<Type>::ImageYUV(const std::string& filename)
{
  ReadImage(filename);
}

template <class Type> inline
VW::ImageYUV<Type>::ImageYUV(const int w, const int h) 
  : ImageBase<PixelType,PixelPtr>(w,h) 
{
};


namespace VW {

// Specialise header for standard types
template <> inline void 
ImageYUV<unsigned char>::GetFormatStrings(std::string& format, 
					      std::string& type) const
{
  format = "YUVA";
  type =   "UChar";
}

template <> inline void 
ImageYUV<char>::GetFormatStrings(std::string& format, 
					      std::string& type) const
{
  format = "YUVA";
  type =   "Char";
}

template <> inline void 
ImageYUV<int>::GetFormatStrings(std::string& format, 
				    std::string& type) const
{
  format = "YUVA";
  type =   "Int";
}

template <> inline void 
ImageYUV<float>::GetFormatStrings(std::string& format, 
					      std::string& type) const
{
  format = "YUVA";
  type =   "Float";
}

template <> inline void 
ImageYUV<double>::GetFormatStrings(std::string& format, 
					      std::string& type) const
{
  format = "YUVA";
  type =   "Double";
}

}; //end namespace VW

#endif /* _imageyuv_h_ */
