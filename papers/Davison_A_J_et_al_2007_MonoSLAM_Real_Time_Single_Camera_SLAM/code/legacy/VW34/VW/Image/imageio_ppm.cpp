/** @file imageio_ppm.cpp
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

#include "imageio_ppm.tpp"


namespace VW {
#ifdef INSTANTIATE
#  undef INSTANTIATE
#endif
#define INSTANTIATE(M) \
  template bool ReadPPMImage(const std::string& filename, \
			     VW::ImageMono<M>& image); \
  template bool ReadPPMImage(const std::string& filename, \
			     VW::ImageYUV<M>& image); \
  template bool ReadPPMImage(const std::string& filename, \
			     VW::ImageRGB<M>& image); \
  template bool ReadPPMImage(const std::string& filename, \
			     VW::ImageHSV<M>& image); \
  template bool WritePPMImage(const std::string& filename, \
			      const VW::ImageMono<M>& image); \
  template bool WritePPMImage(const std::string& filename, \
			      const VW::ImageYUV<M>& image); \
  template bool WritePPMImage(const std::string& filename, \
			      const VW::ImageRGB<M>& image); \
  template bool WritePPMImage(const std::string& filename, \
			      const VW::ImageHSV<M>& image);


  INSTANTIATE(unsigned char);

#undef INSTANTIATE

} // end namespace VW
