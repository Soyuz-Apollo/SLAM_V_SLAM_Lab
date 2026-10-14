/** @file imageio_gdi.h
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
 */
#ifndef _vw_imageio_gdi_h_
#define _vw_imageio_gdi_h_

namespace VW {
  // Forward declare the image classes
  template<typename valType> class ImageMono;
  template<typename valType> class ImageRGB;
  template<typename valType> class ImageRGBPlanes;
  template<typename valType> class ImageYUV;
  template<typename valType> class ImageHSV;
  template<typename valType, int n> class ImageInterleave;


  template <class valType>
  bool ReadGDIImage(const std::string& filename,
                    VW::ImageMono<valType>& image);

  template <class valType>
  bool ReadGDIImage(const std::string& filename,
                    VW::ImageYUV<valType>& image);

  template <class valType>
  bool ReadGDIImage(const std::string& filename,
                    VW::ImageHSV<valType>& image);

  template <class valType>
  bool ReadGDIImage(const std::string& filename,
                    VW::ImageRGB<valType>& image);

  template <class valType>
  bool ReadGDIImage(const std::string& filename,
                    VW::ImageRGBPlanes<valType>& image);



  template <class valType>
  bool WriteGDIImage(const std::string& filename,
                     const VW::ImageMono<valType>& image);

  template <class valType>
  bool WriteGDIImage(const std::string& filename,
                     const VW::ImageYUV<valType>& image);

  template <class valType>
  bool WriteGDIImage(const std::string& filename,
                     const VW::ImageHSV<valType>& image);

  template <class valType>
  bool WriteGDIImage(const std::string& filename,
                     const VW::ImageRGB<valType>& image);

  template <class valType>
  bool WriteGDIImage(const std::string& filename,
                     const VW::ImageRGBPlanes<valType>& image);

}				// End namespace VW

#endif // _vw_imageio_gdi_h_
