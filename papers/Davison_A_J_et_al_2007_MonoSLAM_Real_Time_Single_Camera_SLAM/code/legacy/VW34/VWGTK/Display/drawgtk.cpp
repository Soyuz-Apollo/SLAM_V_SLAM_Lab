/** 
 * @file   drawgtk.cpp
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


#include "gtk--.h"
#include "gdk--.h"
#include "drawgtk.h"
#include "displaygtkwidget.h"


using namespace std;
using namespace VW;

// {{{ Draw(DisplayGTKCommon& display, const ImageMono<unsigned char>& image)

void DrawFunctions::Draw(DisplayGTKCommon& display, const ImageMono<unsigned char>& image) {
  DisplayGTKWidget* d=display.GetDrawingArea();

  gdk_draw_gray_image(d->get_window(), 
		      d->get_style()->get_fg_gc(GTK_STATE_NORMAL), 
		      0, 0, 
		      d->GetWidth(), d->GetHeight(), 
		      GDK_RGB_DITHER_NONE, (guchar*)image.GetRawBuffer(), (image.GetWidth()));
}

// }}}
// {{{ Draw(DisplayGTKCommon& display, const ImageRGB<unsigned char>& image)

void DrawFunctions::Draw(DisplayGTKCommon& display, const ImageRGB<unsigned char>& image) {
 DisplayGTKWidget* d=display.GetDrawingArea();


 if (1) {
   // Can only draw RGB, so convert
   unsigned char* rgbbuff = new unsigned char[image.GetWidth()*image.GetHeight()*3];
   ImageConversions::VWImage_to_RGBBuffer(&image,rgbbuff);
   
   gdk_draw_rgb_image(d->get_window(), 
		      d->get_style()->get_fg_gc(GTK_STATE_NORMAL), 
		      0, 0, 
		      d->GetWidth(), d->GetHeight(), 
		      GDK_RGB_DITHER_NONE, 
		      (guchar*)rgbbuff, 
		      (image.GetWidth())*3);
   
   delete[] rgbbuff;
 } else {
   // Try a direct draw (B+R swapped?)
   gdk_draw_rgb_32_image(d->get_window(), 
			 d->get_style()->get_fg_gc(GTK_STATE_NORMAL), 
			 0, 0, 
			 d->GetWidth(), d->GetHeight(), 
			 GDK_RGB_DITHER_NONE, 
			 (guchar*)image.GetRawBuffer(), 
			 (image.GetWidth())*4);
 }

}


void DrawFunctions::Draw(DisplayGTKCommon& display, 
			 const ImageYUV<unsigned char>& image) 
{

  // Can only draw RGB, so convert
  ImageRGB<unsigned char> rgb_image(image.GetWidth(),image.GetHeight());
  ImageCopy(image,rgb_image);

  DrawFunctions::Draw(display,rgb_image);

}


void DrawFunctions::Draw(DisplayGTKCommon& display, 
			 const ImageHSV<unsigned char>& image) 
{
  cerr << "Draw(DisplayGTKCommon, ImageHSV) not implemented." << endl;

}


