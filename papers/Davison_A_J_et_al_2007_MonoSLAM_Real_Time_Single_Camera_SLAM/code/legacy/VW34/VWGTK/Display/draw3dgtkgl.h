/** 
 * @file   draw3dgtkgl.h
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


#include "displaygtkglcommon.h"
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_quaternion.h>
#include "draw.h"

namespace VW {
  namespace DrawFunctions {

    // GL specific
    void Draw(DisplayGTKGLCommon& display, 
	      const ImageMono<unsigned char>& image,
	      const vnl_vector_fixed<float,3> position,
	      const vnl_quaternion<float> orientation);
    void Draw(DisplayGTKGLCommon& display,
	      const Point3D& p);

  }; // end namespace DrawFunctions
}; // end namespace VW

// Make this universally available without specification
using namespace VW::DrawFunctions;
