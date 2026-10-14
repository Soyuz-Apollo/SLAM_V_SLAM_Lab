/** @file imagecopy.h
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

#ifndef _ImageCopy_h_
#define _ImageCopy_h_


namespace VW {
  /**
   * @author    ID Reid, BJ Tordoff
   * @ingroup   Image
   * @date      02/08/02
   * @brief     Functions for copying one image into another.
   *
   * These functions require two equal-dimension images between which to copy 
   * data. If the two images are of different type (eg. RGB and Mono) then
   * a conversion is attempted at every pixel (ie. this can be slow).
   */
  template <class inType, class outType>
  void ImageCopy(const inType &im, outType &res);

  template <class inType, class outType>
  void ImageFlipCopy(const inType &im, outType &res);

}

#endif
