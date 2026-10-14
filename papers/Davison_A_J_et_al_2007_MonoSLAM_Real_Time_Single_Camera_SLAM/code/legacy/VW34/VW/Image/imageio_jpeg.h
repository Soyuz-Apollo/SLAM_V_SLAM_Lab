/** @file imageio_jpeg.h
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

namespace VW {

  bool ReadJPEGImage(FILE* filepointer,
                    VW::ImageMono<unsigned char>& image);

  bool ReadJPEGImage(FILE* filepointer, 
                    VW::ImageMono<float>& image);

  bool ReadJPEGImage(FILE* filepointer,
                    VW::ImageYUV<unsigned char>& image);

  bool ReadJPEGImage(FILE* filepointer,
                    VW::ImageRGB<unsigned char>& image);



  bool WriteJPEGImage(FILE* filepointer,
                     const VW::ImageMono<unsigned char>& image);

  bool WriteJPEGImage(FILE* filepointer,
                     const VW::ImageMono<float>& image);

  bool WriteJPEGImage(FILE* filepointer,
                     const VW::ImageYUV<unsigned char>& image);

  bool WriteJPEGImage(FILE* filepointer,
                     const VW::ImageRGB<unsigned char>& image);

}				// End namespace VW
