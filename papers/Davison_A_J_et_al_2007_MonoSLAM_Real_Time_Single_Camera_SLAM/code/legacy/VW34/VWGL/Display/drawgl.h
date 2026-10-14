/** 
 * @file   drawgl.h
 * @author BJ Tordoff
 * @date   29/01/2002
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


#ifndef _drawgl_h_
#define _drawgl_h_

#include <vector>

#include "displayglcommon.h"
#include "VW/graphics.h"

namespace VW {
  namespace DrawFunctions {
    void Draw(DisplayGLCommon& display, 
	      const ImageMono<unsigned char>& image);
    void Draw(DisplayGLCommon& display, 
	      const ImageRGB<unsigned char>& image);
    void Draw(DisplayGLCommon& display, 
	      const ImageYUV<unsigned char>& image);
    void Draw(DisplayGLCommon& display, 
	      const ImageHSV<unsigned char>& image);

    // Processed objects
    void Draw(DisplayGLCommon& display, 
	      const std::vector<VW::PlesseyCorner>& corners,
	      VW::Normalisation C = Normalisation(),
	      float size = 2, CornerShape shape = PLUS,
	      const std::vector<Colour> *colours = 0);


  }; // end namespace DrawFunctions
}; // end namespace VW

// Make this universally available without specification
using namespace VW::DrawFunctions;


#endif // #ifndef _drawgtkgl_h_
