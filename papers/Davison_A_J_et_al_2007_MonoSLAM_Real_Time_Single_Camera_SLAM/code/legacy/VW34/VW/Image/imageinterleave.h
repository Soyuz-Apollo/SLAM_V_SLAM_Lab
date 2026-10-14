/** @file imageinterleave.h
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
 * @class   VW::ImageInterleave
 * @ingroup Image
 * @author  ID Reid
 * @date    02/08/02
 * @brief   A class for storing interleaved (pixel-interlaced) images.
 *
 */

#ifndef _imageinterleave_h_
#define _imageinterleave_h_

#include "imagebase.h"
#include "pixptr.h"

namespace VW {

template <class Type, int Skip>
class ImageInterleave : public ImageBase<Type, PixPtr<Type,Skip> > {

 public:    

  typedef Type PixelType;
  typedef PixPtr<Type,Skip> PixelPtr;

  ImageInterleave(PixelType *addr, const int w, const int h) 
    : ImageBase<PixelType,PixelPtr>(addr, w, h) {};
  ImageInterleave() {};
  ~ImageInterleave() {};

    // IO - this does nothing right now...
  bool ReadImage(const std::string& filename);
  bool WriteImage(const std::string& filename) const;

};

typedef ImageInterleave<unsigned char, 4> ImageInterleave32;
typedef ImageInterleave<unsigned char, 2> ImageInterleave16;

}; // end namespace VW


#endif /* _imageinterleave_h_ */
