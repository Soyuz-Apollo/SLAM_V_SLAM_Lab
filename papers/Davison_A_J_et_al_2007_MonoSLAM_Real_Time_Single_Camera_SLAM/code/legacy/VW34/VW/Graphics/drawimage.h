/** @file   drawimage.h 
 *  @author BJ Tordoff
 *  @date   02/08/02
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
 *
 *
 * Methods for drawing into an image
 *
 * This file provides some simple drawing operations to modify the
 * contents of an image. As far as possible they are similar to drawing on
 * a display.
 */

#ifndef _drawimage_h_
#define _drawimage_h_

// VW includes
#include <VW/Image/imagebase.h>

// local includes
#include "draw.h"

namespace VW {

  template <class T> class PixelRGB;

  namespace DrawImage {
    
    // RGB Images
    
    // Some image specifics
    template <class ImageType>
    void DrawRectangle(ImageType& image,
		       unsigned int x0, unsigned int y0,
		       unsigned int x1, unsigned int y1);
    template <class ImageType>
    void DrawRectangleOutline(ImageType& image,
			      unsigned int x0, unsigned int y0,
			      unsigned int x1, unsigned int y1,
			      unsigned int linewidth=1);
    template <class ImageType>
    void DrawRandomRectangles(ImageType& image,
			      unsigned int number_of_rectangles);

    template <class ImageType>
    void DrawCircle(ImageType& image, 
		    double x0, double y0, double radius);
    
   /** Draw one image onto another with optional alpha-blending.
     * @ingroup DrawImage
     * The foreground image is pasted over the background image, the result being
     * stored in the background image. The image will not be resized.
     * @param alpha the transparanecy of the foreground (0=opaque, 1=transparent)
     */
    void Draw(const ImageRGB<unsigned char>& foreground,
	      ImageRGB<unsigned char>& background,
	      unsigned int x0, unsigned int y0,
	      double alpha=0.0);

   /** Draw one image onto another using a colour as transparent.
     * @ingroup DrawImage
     * The foreground image is pasted over the background image, the result being
     * stored in the background image. The image will not be resized.
     * @param alpha the transparanecy of the foreground (0=opaque, 1=transparent)
     */
    void Draw(const ImageRGB<unsigned char>& foreground,
	      ImageRGB<unsigned char>& background,
	      unsigned int x0, unsigned int y0,
	      const VW::PixelRGB<unsigned char>& transparent_colour);
			    


    // The display-style drawing functions
    template <class ImageType>
    void Draw(ImageType& image, 
	      const std::vector<VW::PlesseyCorner>& corners,
	      VW::Normalisation C = Normalisation(),
	      float cross_size = 2);
    
    template <class ImageType>
    void Draw(ImageType& image, 
	      const LineSeg2D& ls, 
	      Normalisation C = Normalisation());
    
    template <class ImageType>
    void Draw(ImageType& image, 
	      const Point2D& p, 
	      Normalisation C = Normalisation(),
	      float cross_size = 2);
    
    template <class ImageType>
    void Draw(ImageType& display, 
	      const Line2D& l, 
	      Normalisation C = Normalisation());

    
  }; // End namespace DrawFunctions
}; // End namespace VW

#endif
