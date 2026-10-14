/** @file imagergbplanes.h
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
 * @class   VW::ImageRGBPlanes
 * @ingroup Image
 * @author  ID Reid
 * @date    02/08/02
 * @brief   A class for storing an RGB image as seperate R, G and B images.
 *
 */
#ifndef _imagergbplanes_h_
#define _imagergbplanes_h_

#include <string>

#include "imagebase.h"
#include "imagemono.h"


namespace VW {

template <class Type>
class ImageRGBPlanes {

 public:    

  ImageRGBPlanes(const int w, const int h); 
  ImageRGBPlanes(Type *red, Type *green, Type *blue, const int w, const int h); 
  ImageRGBPlanes() {};
  ~ImageRGBPlanes() {};

  void AllocImageData(int w, int h);
  bool IsAlloced() const;
  unsigned int GetWidth() const;
  unsigned int GetHeight() const;

  bool ReadImage(FILE *file);
  bool WriteImage(FILE *file) const;
  bool ReadImage(const std::string& filename);
  bool WriteImage(const std::string& filename) const;

  // data
  ImageMono<Type> r, g, b;


 protected:
	bool _alloced;

};

}; // end namespace VW


template <class Type> inline bool
VW::ImageRGBPlanes<Type>::IsAlloced(void) const
{
	return _alloced;
};
template <class Type> inline unsigned int 
VW::ImageRGBPlanes<Type>::GetWidth(void) const
{
	return r.GetWidth();
};
template <class Type> inline unsigned int 
VW::ImageRGBPlanes<Type>::GetHeight(void) const
{
	return r.GetHeight();
};

#endif /* _imagergbplanes_h_ */
