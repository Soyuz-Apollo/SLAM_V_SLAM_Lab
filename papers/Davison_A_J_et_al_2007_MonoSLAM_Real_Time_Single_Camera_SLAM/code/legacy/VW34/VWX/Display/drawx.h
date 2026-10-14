/** 
 * @file   drawx.h
 * @author ID Reid
 * @date   02/08/2002
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



#include <VW/Graphics/draw.h>
#include "displayx.h"

namespace VW {

  namespace DrawFunctions {
    void Draw(DisplayX& display, const ImageMono<unsigned char>& image);
    void Draw(DisplayX& display, const ImageRGB<unsigned char>& image);
    void Draw(DisplayX& display, const ImageYUV<unsigned char>& image);
    void Draw(DisplayX& display, const ImageHSV<unsigned char>& image);
    void Draw(DisplayX& display, const ImageInterleave<unsigned char,4>& image);
    void Draw(DisplayX& display, const ImageInterleave<unsigned char,2>& image);
  }; // end namespace DrawFunctions;
}; // end namespace VW;
// Make this universally available without specification
using namespace VW::DrawFunctions;
