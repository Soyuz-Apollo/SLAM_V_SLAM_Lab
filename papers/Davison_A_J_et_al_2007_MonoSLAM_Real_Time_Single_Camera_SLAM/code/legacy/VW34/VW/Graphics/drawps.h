/** 
 * @file   drawps.h
 * @author ID Reid
 * @date   01/09/2004
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

#ifndef _DRAWPS_H_
#define _DRAWPS_H_

namespace VW {

  // Forward declare some classes
  class PSCanvas;
  class EdgeData;
  template<typename valType> class ImageMono;
  template<typename valType> class ImageRGB;
  template<typename valType> class ImageYUV;
  

  namespace DrawFunctions {

    void Draw(PSCanvas& psc, const ImageMono<unsigned char>& image);
    void Draw(PSCanvas& psc, const ImageRGB<unsigned char>& image);
    void Draw(PSCanvas& psc, const ImageYUV<unsigned char>& image);
    void Draw(PSCanvas& psc, const EdgeData& edges);

  }

// Make this universally available without specification
using namespace VW::DrawFunctions;

}


#endif
