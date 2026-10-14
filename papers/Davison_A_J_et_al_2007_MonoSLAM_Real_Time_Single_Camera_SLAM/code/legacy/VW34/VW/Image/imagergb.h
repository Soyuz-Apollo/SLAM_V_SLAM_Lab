/** @file imagergb.h
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
 * @class   VW::ImageRGB
 * @ingroup Image
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/02
 * @brief   A class for storing RGB images.
 *
 * This image class stores the image in a row-major format (ie. like a
 * standard C/C++ array), with pixels as BGRA where A is a spare (alpha?)
 * channel. This way pixels stored using 8-bit integers (ie. unsigned chars, as
 * are produced by many cameras, and used on most displays)
 * are aligned on 4-byte memory locations, adding a tad to performance. 
 *
 * (Note that the order is BGR rather than RGB as this matches what the Matrox
 * Meteor framegrabber produces, and what the X display server accepts.)
 */


#ifndef _imagergb_h_
#define _imagergb_h_

// Local includes
#include "imagebase.h"
#include "pixelrgb.h"


namespace VW {


template <class Type /*= unsigned char*/>
class ImageRGB : public ImageBase< PixelRGB<Type>, PixelRGB<Type> * > {

 public:    

  ImageRGB();
  ImageRGB(const std::string& filename);
  ImageRGB(const int w, const int h); 
  ImageRGB(PixelRGB<Type> *buf, const int w, const int h); 

  ~ImageRGB();

  bool ReadImage(FILE *file);
  bool WriteImage(FILE *file) const;
  bool ReadImage(const std::string& filename);
  bool WriteImage(const std::string& filename) const;

   // Pixel info
  typedef Type           DataType;
  typedef PixelRGB<Type> PixelType;
  typedef PixelType*     PixelPtr;

protected:
   void GetFormatStrings(std::string& format, 
			 std::string& type) const;

};

}; // end namespace VW











// Inlines
template <class Type> inline
VW::ImageRGB<Type>::ImageRGB(const std::string& filename)
{
  ReadImage(filename);
}

template <class Type> inline
VW::ImageRGB<Type>::ImageRGB(const int w, const int h) 
  : ImageBase<PixelType,PixelPtr>(w, h) 
{
}; 

template <class Type> inline
VW::ImageRGB<Type>::ImageRGB(PixelRGB<Type> *buf, const int w, const int h) 
  : ImageBase<PixelType,PixelPtr>(buf, w, h) 
{
}; 

template <class Type> inline
VW::ImageRGB<Type>::ImageRGB() 
{
};

template <class Type> inline
VW::ImageRGB<Type>::~ImageRGB() 
{
};


namespace VW {

// Now specialise for standard types
template <> inline void 
ImageRGB<unsigned char>::GetFormatStrings(std::string& format, 
				       std::string& type) const
{
  format = "BGRA";
  type =   "UChar";
}

template <> inline void 
ImageRGB<float>::GetFormatStrings(std::string& format, 
				       std::string& type) const
{
  format = "BGRA";
  type =   "Float";
}

template <> inline void 
ImageRGB<double>::GetFormatStrings(std::string& format, 
				       std::string& type) const
{
  format = "BGRA";
  type =   "Double";
}

}; //end namespace VW


#endif /* _imagergb_h_ */
