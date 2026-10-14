/** 
 * @file   drawx.cc
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


// XForms optimized versions
#include <string.h>
#include "drawx.h"
#include <VW/Image/imagemono.h>
#include <VW/Image/imagergb.h>
#include <VW/Image/imageyuv.h>
#include <VW/Image/imagehsv.h>
#include <VW/Image/imageinterleave.h>

using namespace std;
using namespace VW;

// {{{ void DrawFunctions::Draw(DisplayX& display, const ImageMono<unsigned char>& image)
void DrawFunctions::Draw(DisplayX& display, const ImageMono<unsigned char>& image) {

  // Find out which is bigger, the image or the display
  int im_w=image.GetWidth();
  int im_h=image.GetHeight();
  int ds_w=display.GetWidth();
  int ds_h=display.GetHeight();
  int width=im_w<ds_w?im_w:ds_w;
  int height=im_h<ds_h?im_h:ds_h;

  // Add a skip if the user image is smaller than the display's ximage
  int row_skip=0;
  if (im_w<ds_w) row_skip=ds_w-im_w;

  unsigned char* ximptr;
  unsigned short* xsimptr;
 
  ImageMono<unsigned char>::PixelPtr imptr;
  switch(display.GetDepth()) {

  case 8:
    ximptr=(unsigned char *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	char g=display.GreyLookUp(imptr++->y);
	*(ximptr++) = g;
      }
      ximptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 15:
    // 5 bits for each of RGB, most significant of 16 ignored
    xsimptr=(unsigned short *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	unsigned short pixel = (short)((imptr++->y) & 0xf8);
	*xsimptr++ = (pixel>>3) | (pixel<<2)  | (pixel<<7) ;
      }
      xsimptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 16:
    // Assume green has 6 bits but ignore least significant 
    xsimptr=(unsigned short *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	unsigned short pixel = (short)((imptr++->y) & 0xf8);
	*xsimptr++ = (pixel>>3) | (pixel<<3)  | (pixel<<8) ;
      }
      xsimptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 24:
  case 32:
    ximptr=(unsigned char *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	unsigned char g=(imptr++->y);
	
	*(ximptr++) = g;
	*(ximptr++) = g;
	*(ximptr++) = g;
	ximptr++; // skip alpha
      }
      ximptr+=row_skip;
    }
    display.DisplayXImage();
    break;
  default:
    cerr << "Draw(DisplayX, ImageMono<unsigned char>): attempt to display on " << display.GetDepth() << " bit screen (8, 15, 16 & 32 allowed)" << endl;
    break;
  }
}
// }}}

// {{{ void DrawFunctions::Draw(DisplayX& display, const ImageInterleave32& image)

void DrawFunctions::Draw(DisplayX& display, const ImageInterleave32& image) {

  // Find out which is bigger, the image or the display
  int im_w=image.GetWidth();
  int im_h=image.GetHeight();
  int ds_w=display.GetWidth();
  int ds_h=display.GetHeight();
  int width=im_w<ds_w?im_w:ds_w;
  int height=im_h<ds_h?im_h:ds_h;

  // Add a skip if the user image is smaller than the display's ximage
  int row_skip=0;
  if (im_w<ds_w) row_skip=ds_w-im_w;

  unsigned char *ximptr;
  unsigned short *xsimptr;

  ImageInterleave32::PixelPtr imptr;

  switch(display.GetDepth()) {

  case 8:
    ximptr=(unsigned char *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	char g=display.GreyLookUp(*(imptr++));
	*(ximptr++) = g;
      }
      ximptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 15:
    // 5 bits for each of RGB, most significant of 16 ignored
    xsimptr=(unsigned short *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	unsigned short pixel = (short)((*imptr++) & 0xf8);
	*xsimptr++ = (pixel>>3) | (pixel<<2)  | (pixel<<7) ;
      }
      xsimptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 16:
    // Assume green has 6 bits but ignore least significant 
    xsimptr=(unsigned short *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	unsigned short pixel = (short)((*imptr++) & 0xf8);
	*xsimptr++ = (pixel>>3) | (pixel<<3)  | (pixel<<8) ;
      }
      xsimptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 24:
  case 32:
    ximptr=(unsigned char *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	unsigned char g=(*(imptr++));
	
	*(ximptr++) = g;
	*(ximptr++) = g;
	*(ximptr++) = g;
	ximptr++; // skip alpha
      }
      ximptr+=row_skip;
    }
    display.DisplayXImage();
    break;
  default:
    cerr << "Draw(DisplayX, ImageInterleave32): attempt to display on " << display.GetDepth() << " bit screen (8, 15, 16 & 32 allowed)" << endl;
    break;
  }
}


void DrawFunctions::Draw(DisplayX& display, const ImageInterleave16& image) {
  cout << "Draw Monoimage using xforms optimised code" << endl;

  // Find out which is bigger, the image or the display
  int im_w=image.GetWidth();
  int im_h=image.GetHeight();
  int ds_w=display.GetWidth();
  int ds_h=display.GetHeight();
  int width=im_w<ds_w?im_w:ds_w;
  int height=im_h<ds_h?im_h:ds_h;

  // Add a skip if the user image is smaller than the display's ximage
  int row_skip=0;
  if (im_w<ds_w) row_skip=ds_w-im_w;

  unsigned char *ximptr;
  unsigned short *xsimptr;

  ImageInterleave16::PixelPtr imptr;

  switch(display.GetDepth()) {

  case 8:
    ximptr=(unsigned char *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	char g=display.GreyLookUp(*(imptr++));
	*(ximptr++) = g;
      }
      ximptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 15:
    // 5 bits for each of RGB, most significant of 16 ignored
    xsimptr=(unsigned short *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	unsigned short pixel = (short)((*imptr++) & 0xf8);
	*xsimptr++ = (pixel>>3) | (pixel<<2)  | (pixel<<7) ;
      }
      xsimptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 16:
    // Assume green has 6 bits but ignore least significant 
    xsimptr=(unsigned short *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	unsigned short pixel = (short)((*imptr++) & 0xf8);
	*xsimptr++ = (pixel>>3) | (pixel<<3)  | (pixel<<8) ;
      }
      xsimptr+=row_skip;
    }
    display.DisplayXImage();
    break;


  case 24:
  case 32:
    ximptr=(unsigned char *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	unsigned char g=(*(imptr++));
	
	*(ximptr++) = g;
	*(ximptr++) = g;
	*(ximptr++) = g;
	ximptr++; // skip alpha
      }
      ximptr+=row_skip;
    }
    display.DisplayXImage();
    break;
  default:
    cerr << "Draw(DisplayX, Interleave16): attempt to display on " << display.GetDepth() << " bit screen (8, 15, 16 & 32 allowed)" << endl;
    break;
  }
}

// }}}

// {{{ void DrawFunctions::Draw(DisplayX& display, const ImageRGB<unsigned char>& image)

void DrawFunctions::Draw(DisplayX& display, const ImageRGB<unsigned char>& image) {
  //  cout << "Draw RGBimage using xforms optimised code" << endl;
  // Find out which is bigger, the image or the display
  int im_w=image.GetWidth();
  int im_h=image.GetHeight();
  int ds_w=display.GetWidth();
  int ds_h=display.GetHeight();
  int width=im_w<ds_w?im_w:ds_w;
  int height=im_h<ds_h?im_h:ds_h;

  // Add a skip if the user image is smaller than the display's ximage
  int row_skip=0;
  if (im_w<ds_w) row_skip=ds_w-im_w;

  unsigned char *ximptr;
  unsigned short *xsimptr;
    
  ImageRGB<unsigned char>::PixelPtr imptr;
  
  switch(display.GetDepth()) {

  case 8:
    ximptr=(unsigned char *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++,imptr++) {
	char colindex=display.ColourLookUp(imptr->r,imptr->g,imptr->b);
	*(ximptr++) = colindex;
      }
      ximptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 15:
    // 5 bits for each of RGB, most significant of 16 ignored
    xsimptr=(unsigned short *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	PixelRGB<unsigned char> pixel = *imptr++;
	*xsimptr++ = ((pixel.b & 0xf8)>>3) | ((pixel.g & 0xf8)<<2)  | ((pixel.r & 0xf8)<<7) ;
      }
      xsimptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 16:
    // Assume green has 6 bits but ignore least significant 
    xsimptr=(unsigned short *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	PixelRGB<unsigned char> pixel = *imptr++;
	*xsimptr++ = ((pixel.b & 0xf8)>>3) | ((pixel.g & 0xf8)<<3)  | ((pixel.r & 0xf8)<<8) ;
      }
      xsimptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 24:
  case 32:
    ximptr=(unsigned char *)display.GetImagePointer();

    memcpy(ximptr, (unsigned char *)image.GetRawBuffer(), 
	   4*image.GetWidth()*image.GetHeight());

    /*
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++,imptr++) {
	*(ximptr++) = imptr->b;
	*(ximptr++) = imptr->g;
	*(ximptr++) = imptr->r;
	ximptr++; // skip alpha
      }
      ximptr+=row_skip;
    }
    */

    display.DisplayXImage();
    break;
  default:
    cerr << "Draw(DisplayX, ImageRGB<unsigned char>): attempt to display on " << display.GetDepth() << " bit screen (8, 15, 16 & 32 allowed)" << endl;
    break;
  }
}

// }}}


// {{{ void DrawFunctions::Draw(DisplayX& display, const ImageYUV<unsigned char>& image)

void DrawFunctions::Draw(DisplayX& display, const ImageYUV<unsigned char>& image) {
  //  cout << "Draw YUVimage using xforms optimised code" << endl;
  // Find out which is bigger, the image or the display
  int im_w=image.GetWidth();
  int im_h=image.GetHeight();
  int ds_w=display.GetWidth();
  int ds_h=display.GetHeight();
  int width=im_w<ds_w?im_w:ds_w;
  int height=im_h<ds_h?im_h:ds_h;

  // Add a skip if the user image is smaller than the display's ximage
  int row_skip=0;
  if (im_w<ds_w) row_skip=ds_w-im_w;

  unsigned char *ximptr;
  unsigned short *xsimptr;
    
  ImageYUV<unsigned char>::PixelPtr imptr;
  
  switch(display.GetDepth()) {

  case 8:
    ximptr=(unsigned char *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++,imptr++) {
	register unsigned char  y=imptr->y,u=imptr->u,v=imptr->v;
	register unsigned char r=0,b=0,g=0;
	ImageConversions::YUV2RGB(y, u, v, r, g, b);
	char colindex=display.ColourLookUp((unsigned char)r,(unsigned char)g,(unsigned char)b);
	*(ximptr++) = colindex;
      }
      ximptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 15:
    // 5 bits for each of RGB, most significant of 16 ignored
    xsimptr=(unsigned short *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	PixelYUV<unsigned char> pixel = *imptr++;
	register unsigned char y=pixel.y,u=pixel.u,v=pixel.v;
	register unsigned char r=0,b=0,g=0;
	ImageConversions::YUV2RGB(y, u, v, r, g, b);
	*xsimptr++ = (((unsigned char)b & 0xf8)>>3) | (((unsigned char)g & 0xf8)<<2)  | (((unsigned char)r & 0xf8)<<7) ;
      }
      xsimptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 16:
    // Assume green has 6 bits but ignore least significant 
    xsimptr=(unsigned short *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	PixelYUV<unsigned char> pixel = *imptr++;
	register unsigned char y=pixel.y,u=pixel.u,v=pixel.v;
	register unsigned char r=0,b=0,g=0;
	ImageConversions::YUV2RGB(y, u, v, r, g, b);
	*xsimptr++ = (((unsigned char)b & 0xf8)>>3) | (((unsigned char)g & 0xf8)<<3)  | (((unsigned char)r & 0xf8)<<8) ;
      }
      xsimptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 24:
  case 32:
    ximptr=(unsigned char *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++,imptr++) {
	register unsigned char y=imptr->y,u=imptr->u,v=imptr->v;
	register unsigned char r=0,b=0,g=0;
	//cerr << "Draw: YUV=("<<int(y)<<","<<int(u)<<","<<int(v)<<")"<<endl;
	ImageConversions::YUV2RGB(y, u, v, r, g, b);
	*(ximptr++) = b;
	*(ximptr++) = g;
	*(ximptr++) = r;
	ximptr++; // skip alpha
      }
      ximptr+=row_skip;
    }
    display.DisplayXImage();
    break;
  default:
    cerr << "Draw(DisplayX, ImageYUV<unsigned char>): only 8, 15, 16 & 32 bits_pp allowed" << endl;
    break;
  }
}

// }}}
// {{{ void DrawFunctions::Draw(DisplayX& display, const ImageHSV<unsigned char>& image)

void DrawFunctions::Draw(DisplayX& display, const ImageHSV<unsigned char>& image) {
  //  cout << "Draw HSVimage using xforms optimised code" << endl;
  // Find out which is bigger, the image or the display
  int im_w=image.GetWidth();
  int im_h=image.GetHeight();
  int ds_w=display.GetWidth();
  int ds_h=display.GetHeight();
  int width=im_w<ds_w?im_w:ds_w;
  int height=im_h<ds_h?im_h:ds_h;

  // Add a skip if the user image is smaller than the display's ximage
  int row_skip=0;
  if (im_w<ds_w) row_skip=ds_w-im_w;

  unsigned char *ximptr;
  unsigned short *xsimptr;
    
  ImageHSV<unsigned char>::PixelPtr imptr;
  
  switch(display.GetDepth()) {

  case 8:
    ximptr=(unsigned char *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++,imptr++) {
	register unsigned char  h=imptr->h,s=imptr->s,v=imptr->v;
	register unsigned char r=0,b=0,g=0;
	ImageConversions::HSV2RGB(h, s, v, r, g, b);
	char colindex=display.ColourLookUp((unsigned char)r,(unsigned char)g,(unsigned char)b);
	*(ximptr++) = colindex;
      }
      ximptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 15:
    // 5 bits for each of RGB, most significant of 16 ignored
    xsimptr=(unsigned short *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	PixelHSV<unsigned char> pixel = *imptr++;
	register unsigned char h=pixel.h,s=pixel.s,v=pixel.v;
	register unsigned char r=0,b=0,g=0;
	ImageConversions::HSV2RGB(h, s, v, r, g, b);
	*xsimptr++ = (((unsigned char)b & 0xf8)>>3) | (((unsigned char)g & 0xf8)<<2)  | (((unsigned char)r & 0xf8)<<7) ;
      }
      xsimptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 16:
    // Assume green has 6 bits but ignore least significant 
    xsimptr=(unsigned short *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++) {
	PixelHSV<unsigned char> pixel = *imptr++;
	register unsigned char h=pixel.h,s=pixel.s,v=pixel.v;
	register unsigned char r=0,b=0,g=0;
	ImageConversions::HSV2RGB(h, s, v, r, g, b);
	*xsimptr++ = (((unsigned char)b & 0xf8)>>3) | (((unsigned char)g & 0xf8)<<3)  | (((unsigned char)r & 0xf8)<<8) ;
      }
      xsimptr+=row_skip;
    }
    display.DisplayXImage();
    break;

  case 24:
  case 32:
    ximptr=(unsigned char *)display.GetImagePointer();
    for (int row=0;row<height;row++) {
      imptr=image.GetRowStart(row);
      for(int col=0;col<width;col++,imptr++) {
	register unsigned char h=imptr->h,s=imptr->s,v=imptr->v;
	register unsigned char r=0,b=0,g=0;
	ImageConversions::HSV2RGB(h, s, v, r, g, b);
// 	cerr << " HSV=("
// 	     <<int(h)<<","<<int(s)<<","<<int(v)<<") RGB=("
// 	     <<int(r)<<","<<int(g)<<","<<int(b)<<")"<<endl;
	*(ximptr++) = b;
	*(ximptr++) = g;
	*(ximptr++) = r;
	ximptr++; // skip alpha
      }
      ximptr+=row_skip;
    }
    display.DisplayXImage();
    break;
  default:
    cerr << "Draw(DisplayX, ImageHSV<unsigned char>): only 8, 15, 16 & 32 bits_pp allowed" << endl;
    break;
  }
}

// }}}

