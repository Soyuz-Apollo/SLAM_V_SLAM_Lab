/** @file imageio.cpp
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

#include "imagemono.h"
#include "imagergb.h"
#include "imageyuv.h"
#include "imagehsv.h"
#include "imageinterleave.h"
#include "imagergbplanes.h"

#ifdef _WIN32
#  include "imageio_win32.cpp"
#else
#  include "imageio_linux.cpp"
#endif


// And force instantiate some types
namespace VW {

#ifdef INSTANTIATE
#  undef INSTANTIATE
#endif
#define INSTANTIATE(T) \
  template bool \
  ReadImage<T >(const std::string& filename, T& image); \
  template bool \
  WriteImage<T >(const std::string& filename, const T& image); \
  template bool \
  ReadImage<T >(FILE* file, T& image); \
  template bool \
  WriteImage<T >(FILE* file, const T& image);

  INSTANTIATE(ImageMono<unsigned char>);
  INSTANTIATE(ImageMono<unsigned int>);
  INSTANTIATE(ImageMono<int>);
  INSTANTIATE(ImageMono<float>);
  INSTANTIATE(ImageMono<double>);

  INSTANTIATE(ImageRGB<unsigned char>);
  INSTANTIATE(ImageRGB<unsigned int>);
  INSTANTIATE(ImageRGB<int>);
  INSTANTIATE(ImageRGB<float>);
  INSTANTIATE(ImageRGB<double>);

  INSTANTIATE(ImageYUV<unsigned char>);
  INSTANTIATE(ImageYUV<unsigned int>);
  INSTANTIATE(ImageYUV<int>);
  INSTANTIATE(ImageYUV<float>);
  INSTANTIATE(ImageYUV<double>);

  INSTANTIATE(ImageHSV<unsigned char>);
  INSTANTIATE(ImageHSV<unsigned int>);
  INSTANTIATE(ImageHSV<int>);
  INSTANTIATE(ImageHSV<float>);
  INSTANTIATE(ImageHSV<double>);

#undef INSTANTIATE

}; // End namespace VW
