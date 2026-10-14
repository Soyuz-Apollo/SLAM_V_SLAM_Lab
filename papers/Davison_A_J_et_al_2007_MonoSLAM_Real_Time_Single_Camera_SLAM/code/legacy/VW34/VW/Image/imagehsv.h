/** @file imagehsv.h
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
 *
 * @class   VW::ImageHSV
 * @ingroup Image
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/02
 * @brief   A class for storing HSV (Hue, Saturation, Value) images.
 *
 */
#ifndef _imagehsv_h_
#define _imagehsv_h_

#include <string>

#include "imagebase.h"
#include "pixelhsv.h"


namespace VW {


template <class Type>
class ImageHSV : public ImageBase< PixelHSV<Type>, PixelHSV<Type> * > {

 public:    

  ImageHSV(const int w, const int h) : ImageBase<PixelType,PixelPtr>(w,h) {};
  ImageHSV() {};
  ~ImageHSV() {};

  typedef PixelHSV<Type> PixelType;
  typedef PixelType *PixelPtr;

  bool ReadImage(const std::string& filename);
  bool WriteImage(const std::string& filename) const;
protected:
   void GetFormatStrings(std::string& format, 
			 std::string& type) const;
};




// Inlines

// Specialise header for standard types
template <> inline void 
ImageHSV<unsigned char>::GetFormatStrings(std::string& format, 
					      std::string& type) const
{
  format = "HSVA";
  type =   "UChar";
}
template <> inline void 
ImageHSV<double>::GetFormatStrings(std::string& format, 
					      std::string& type) const
{
  format = "HSVA";
  type =   "Double";
}

}; // end namespace VW

#endif /* _imagehsv_h_ */
