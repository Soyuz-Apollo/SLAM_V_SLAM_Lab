/** 
 * @file   imagepatch.cpp
 * @author BJ Tordoff
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


#include <iostream>

#include "imagepatch.h"


#define VERBOSE true

using namespace std;
using namespace VW;

/*************************/
/* Some useful functions */
/*************************/

// {{{ GetInterpolatedImagePatch()

void VW::GetInterpolatedImagePatch(const ImageMono<unsigned char>& im,
					     const double x,
					     const double y,
					     VW::ImagePatch& patch) {

  // Check patch size
  if ((patch.GetWidth()==0) || (patch.GetHeight()==0)) {
    cerr << "VW::GetInterpolatedImagePatch: patch not initialised!"<<endl;
    return;
  }

  
  // Get half sizes
  unsigned int halfwidth=patch.GetWidth()/2;
  unsigned int halfheight=patch.GetHeight()/2;
  
  // Store integer positions and offsets
  int px=int(x);
  int py=int(y);
  const float deltax = float(x)-px;
  const float deltay = float(y)-py;

  // Coefficeints of interpolation are constant
  const float a00 = (1.0f-deltax)*(1.0f-deltay);
  const float a01 = (1.0f-deltax)*(     deltay);
  const float a10 = (     deltax)*(1.0f-deltay);
  const float a11 = (     deltax)*(     deltay);

  for (int iy=int(py-halfheight) ; iy<=int(py+halfheight) ; iy++) {
    for (int ix=int(px-halfwidth) ; ix<=int(px+halfwidth) ; ix++) {
      const float this_value 
	= a00 * im.GetPixel(ix  ,iy  ).y
	+ a01 * im.GetPixel(ix  ,iy+1).y
	+ a10 * im.GetPixel(ix+1,iy  ).y
	+ a11 * im.GetPixel(ix+1,iy+1).y;
      patch.SetData(ix-px+halfwidth,iy-py+halfheight,this_value);
    }
  }

}

void VW::GetInterpolatedImagePatch(const ImageMono<float>& im,
				   const double x,
				   const double y,
				   VW::ImagePatch& patch) {

  // Check patch size
  if ((patch.GetWidth()==0) || (patch.GetHeight()==0)) {
    cerr << "VW::GetInterpolatedImagePatch: patch not initialised!"<<endl;
    return;
  }

  
  // Get half sizes
  unsigned int halfwidth=patch.GetWidth()/2;
  unsigned int halfheight=patch.GetHeight()/2;
  
  // Store integer positions and offsets
  int px=int(x);
  int py=int(y);
  const float deltax = float(x)-px;
  const float deltay = float(y)-py;

  // Coefficeints of interpolation are constant
  const float a00 = (1.0f-deltax)*(1.0f-deltay);
  const float a01 = (1.0f-deltax)*(     deltay);
  const float a10 = (     deltax)*(1.0f-deltay);
  const float a11 = (     deltax)*(     deltay);

  for (int iy=int(py-halfheight) ; iy<=int(py+halfheight) ; iy++) {
    for (int ix=int(px-halfwidth) ; ix<=int(px+halfwidth) ; ix++) {
      const float this_value 
	= a00 * im.GetPixel(ix  ,iy  ).y
	+ a01 * im.GetPixel(ix  ,iy+1).y
	+ a10 * im.GetPixel(ix+1,iy  ).y
	+ a11 * im.GetPixel(ix+1,iy+1).y;
      patch.SetData(ix-px+halfwidth,iy-py+halfheight,this_value);
    }
  }

}
void VW::GetInterpolatedImagePatch(const ImageYUV<unsigned char>& im,
					 const double x,
					 const double y,
					 VW::ImagePatch& patch) {

  // Check patch size
  if ((patch.GetWidth()==0) || (patch.GetHeight()==0)) {
    cerr << "VW::GetInterpolatedImagePatch: patch not initialised!"<<endl;
    return;
  }

  // Get half sizes
  unsigned int halfwidth=patch.GetWidth()/2;
  unsigned int halfheight=patch.GetHeight()/2;

  // Store integer positions and offsets
  int px=int(x);
  int py=int(y);
  const float deltax=float(x)-px;
  const float deltay=float(y)-py;

  // Coefficeints of interpolation are constant
  const float a00 = (1.0f-deltax)*(1.0f-deltay);
  const float a01 = (1.0f-deltax)*(     deltay);
  const float a10 = (     deltax)*(1.0f-deltay);
  const float a11 = (     deltax)*(     deltay);

  for (int iy=int(py-halfheight) ; iy<=int(py+halfheight) ; iy++) {
    for (int ix=int(px-halfwidth) ; ix<=int(px+halfwidth) ; ix++) {
      const float this_value 
	= a00 * im.GetPixel(ix  ,iy  ).y
	+ a01 * im.GetPixel(ix  ,iy+1).y
	+ a10 * im.GetPixel(ix+1,iy  ).y
	+ a11 * im.GetPixel(ix+1,iy+1).y;
      patch.SetData(ix-px+halfwidth,iy-py+halfheight,this_value);
    }
  }

}

// }}}

// {{{ GetImagePatch()

void VW::GetImagePatch(const ImageMono<unsigned char>& im,
			     const double x,
			     const double y,
			     VW::ImagePatch& patch) {

  // Check patch size
  if ((patch.GetWidth()==0) || (patch.GetHeight()==0)) {
    cerr << "VW::GetImagePatch: patch not initialised!"<<endl;
    return;
  }

  // Get half sizes
  unsigned int halfwidth=patch.GetWidth()/2;
  unsigned int halfheight=patch.GetHeight()/2;

  // Store integer positions and offsets
  int px=int(x+0.5);
  int py=int(y+0.5);


  for (int iy=int(py-halfheight) ; iy<=int(py+halfheight) ; iy++) {
    for (int ix=int(px-halfwidth) ; ix<=int(px+halfwidth) ; ix++) {
      patch.SetData(ix-px+halfwidth,iy-py+halfheight,im.GetPixel(ix,iy).y);
    }
  }
    
}

void VW::GetImagePatch(const ImageMono<float>& im,
		       const double x,
		       const double y,
		       VW::ImagePatch& patch) {

  // Check patch size
  if ((patch.GetWidth()==0) || (patch.GetHeight()==0)) {
    cerr << "VW::GetImagePatch: patch not initialised!"<<endl;
    return;
  }

  // Get half sizes
  unsigned int halfwidth=patch.GetWidth()/2;
  unsigned int halfheight=patch.GetHeight()/2;

  // Store integer positions and offsets
  int px=int(x+0.5);
  int py=int(y+0.5);


  for (int iy=int(py-halfheight) ; iy<=int(py+halfheight) ; iy++) {
    for (int ix=int(px-halfwidth) ; ix<=int(px+halfwidth) ; ix++) {
      patch.SetData(ix-px+halfwidth,iy-py+halfheight,im.GetPixel(ix,iy).y);
    }
  }
    
}
void VW::GetImagePatch(const ImageYUV<unsigned char>& im,
			     const double x,
			     const double y,
			     VW::ImagePatch& patch) {

  // Check patch size
  if ((patch.GetWidth()==0) || (patch.GetHeight()==0)) {
    cerr << "VW::GetImagePatch: patch not initialised!"<<endl;
    return;
  }

  // Get half sizes
  unsigned int halfwidth=patch.GetWidth()/2;
  unsigned int halfheight=patch.GetHeight()/2;

  // Store integer positions and offsets
  int px=int(x+0.5);
  int py=int(y+0.5);


  for (int iy=int(py-halfheight) ; iy<=int(py+halfheight) ; iy++) {
    for (int ix=int(px-halfwidth) ; ix<=int(px+halfwidth) ; ix++) {
      patch.SetData(ix-px+halfwidth,iy-py+halfheight,im.GetPixel(ix,iy).y);
    }
  }
    
}

// }}}
