/** 
 * @file   displayx.cc
 * @author ID Reid, BJ Tordoff
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

#include <VW/graphics.h>
#include "displayx.h"

#define DEF_SIZE 100

using namespace std;
using namespace VW;

extern Display* GetXDisplay();

DisplayX::DisplayX(const string title) {
  _visible=false;
  cerr << "Hurray, a new X display!" << endl;
  width=DEF_SIZE;
  height=DEF_SIZE;
  _my_title=title;

}


DisplayX::DisplayX(const unsigned int width,
		   const unsigned int height,
		   const string title) {
  cerr << "Hurray, a new X display!" << endl;
  this->width=width;
  this->height=height;

  Init(width,height,title);

  _visible=true;
}


void DisplayX::Init(const unsigned int width,
		    const unsigned int height,
		    const string title) {
  cerr << "Initialising raw X display "<<title<<endl;
  int xpos=0;
  int ypos=0;
  // Leave shared memory off for safety
  shm_request=0;

  display = GetXDisplay();

  window = XCreateSimpleWindow(display, DefaultRootWindow(display),
			       xpos, ypos, width, height, 2,
			       BlackPixel(display, DefaultScreen(display)),
			       BlackPixel(display, DefaultScreen(display))); 
  
  XSetWindowAttributes set_attributes;
  set_attributes.backing_store = Always;
  XChangeWindowAttributes(display, window, CWBackingStore, &set_attributes);

  XSelectInput(display, window, ButtonPressMask|ButtonReleaseMask|ButtonMotionMask|ExposureMask);

  XGCValues gc_values;
  gc_values.graphics_exposures = False;
  gc = XCreateGC(display, window, GCGraphicsExposures, &gc_values);  
  screen      = DefaultScreen(display);
  visual      = DefaultVisual(display, screen);
  depth       = DefaultDepth(display, screen);
  bytes_pp    = (depth==15 || depth==16) ? 2 : (depth==24 || depth==32) ? 4 : 1;
  colormap   =  XDefaultColormap(display, screen);

  // get bits per pixel
  int i,nformats;
  XPixmapFormatValues *formats = XListPixmapFormats(display, &nformats);
  for (i=0; i<nformats; i++) {
    if (formats[i].depth==depth) {
      bits_pp=formats[i].bits_per_pixel;
      break;
    }
  }

  InitXImage();
  SetPenColour(255, 255, 0, 0);

  XMapWindow(display, window);

  extern void XAddDisplay(VW::DisplayX *d);
  XAddDisplay(this);
  _initialised=true;
}



// {{{ DisplayX::~DisplayX(void)
DisplayX::~DisplayX(void) {
  cout << "Aaargh! I'm dead." << endl;
}

// }}}


void DisplayX::SetSize(const unsigned int new_width,
			    const unsigned int new_height) {
  width=new_width;
  height=new_height;

  if (!_initialised) {
    // Just initialise to new size
    Init(width,height,_my_title);
  } else {
    
    // Hide window if presently on screen
    if(_visible)
      XUnmapWindow(display,window);
    
    // Leave shared memory off for safety
    shm_request=0;

    XDestroyWindow(display, window);

    // Re-initialise at new size  
    Init(width,height,_my_title);
  }
  _visible=true;
}


// Drawing primitves
// {{{ void DisplayX::SetPixel

void DisplayX::SetPixel(const int x, const int y, 
			const unsigned char red, 
			const unsigned char green, 
			const unsigned char blue, 
			const unsigned char alpha) 
{
  switch (depth) {
  case 8:
    XSetForeground(display,gc,ColourLookUp(red,green,blue));
    XDrawPoint (display, window, gc, x, y );
    break;
  case 15:
    XSetForeground(display,gc,(32*(32*(red>>3) + (green>>3)) + (blue>>3)));
    XDrawPoint (display, window, gc, x, y );
    break;
  case 16:
    XSetForeground(display,gc,(64*(32*(red>>3) + (green>>3)) + (blue>>3)));
    XDrawPoint (display, window, gc, x, y );
    break;
  case 24:
  case 32:
    XSetForeground(display,gc,blue + ((green + (red<<8)) << 8));
    XDrawPoint (display, window, gc, x, y );
    break;
  default:
    cerr << "Only 8, 15, 16 or 32 bpp!" << endl;
  }
  XSetForeground(display,gc,pencolour);   // restore default drawing colour
}

// }}}
// {{{ void DisplayX::SetPixel

void DisplayX::SetPixel(const int x, const int y) 
{
  XDrawPoint (display, window, gc, x, y);
}

// }}}
// {{{ void DisplayX::DrawPoint(const float x, const float y)

void DisplayX::DrawPoint(const float x, const float y) {
  XDrawPoint (display, window, gc, 
	      static_cast<const int>(x+0.5), 
	      static_cast<const int>(y+0.5) );
}

// }}}
// {{{ void DisplayX::DrawLine

void DisplayX::DrawLine(const float x0, const float y0,
			     const float x1, const float y1) {
  int thick=1;
  XSetLineAttributes(display,gc,thick,LineSolid,CapButt,JoinMiter);
  XDrawLine (display, window, gc, 
	     static_cast<const int>(x0+0.5), static_cast<const int>(y0+0.5), 
	     static_cast<const int>(x1+0.5), static_cast<const int>(y1+0.5) );
}

// }}}


// Force drawing to screen (flush buffers)
void DisplayX::Flush(void) {
  XFlush(display);
}

// {{{ void DisplayX::InitXImage(void)

void DisplayX::InitXImage(void)
{
  // NB X drawing routines draw with respect to the origin of a Window.
  // As each separate form is a window, we need to supply x0 and y0 offsets of
  // the graphic box within the form. The width and height are required for
  // X-images.

   // Shared memory request
   shm_granted = 0; // so far

   // Knowing the bits_pp allows us to make the colormaps
   SetColormaps();
   SetXImage();
}

// }}}
// {{{ void DisplayX::SetColormaps(void)

void DisplayX::SetColormaps(void) {

  int i;

  switch(depth) {
    
  case 8:  

    // Pseudocolor with compressed Grey-scale and 5-bit colourcube
    {
      Colormap wtcolormap = XCreateColormap(display, window, visual, AllocAll);
      XColor *palette = (XColor *)malloc(256 * sizeof(XColor));
      // create 5x5x5 colour cube from 0-124
      for (i=0; i<126; i++) {
	palette[i].pixel=i;
	palette[i].red=static_cast<int>(i%5)*(65535/4);
	palette[i].green=static_cast<int>((i/5)%5)*(65535/4);
	palette[i].blue=static_cast<int>((i/25)%5)*(65535/4);
	palette[i].flags=DoRed|DoGreen|DoBlue;
      }
      // Add greyscales to colourmap
      for (i=0; i<128; i++) {
	palette[i+128].pixel=i+128;
	palette[i+128].red=i<<9;
	palette[i+128].green=i<<9;
	palette[i+128].blue=i<<9;
	palette[i+128].flags=DoRed|DoGreen|DoBlue;
      }

      XStoreColors(display, wtcolormap, palette, 256);
      XSetWindowColormap(display, window, wtcolormap);
    }
    break;
    
  case 15:
  case 16:
  case 24:
  case 32: 
    // True color so no need to set colourmap
    break;

  }
}

// }}}

// {{{ int DisplayX::ColourLookUp(int r, int g, int b)

int DisplayX::ColourLookUp(int r, int g, int b) {
  int rx = r/(255/5);
  int gx = g/(255/4);
  int bx = b/(255/4);
  return rx + 5*(gx + 5*bx);
}

// }}}
// {{{ int DisplayX::GreyLookUp(int l)

int DisplayX::GreyLookUp(int l) {
  return l/2+128;
}

// }}}

// {{{ void DisplayX::SetXImage(void)

void DisplayX::SetXImage(void)
{
  int bitmap_pad;
  char   *xdata;

  if (!shm_request || !XShmQueryExtension(display)) {
    // do not use shared memory
    shm_granted = 0;
    xdata  = new char[width*height*bytes_pp];
    if (xdata==NULL) {
      cerr << "DisplayX::SetXImage: failed to allocate xdata" << endl;
    }
    // allocate ximage
    bitmap_pad = 8*bytes_pp;
    ximage = XCreateImage (display, visual,
			   depth, ZPixmap, 0, xdata, width, height,
			   bitmap_pad, 0 );
      if (ximage==NULL) {
	cerr << "DisplayX::SetXImage(): XCreateImage failed" << endl;
      }
    } 
  else {
    // use shared memory
    shm_granted = 1;
    shminfo = new XShmSegmentInfo [sizeof(XShmSegmentInfo)];
    ximage = XShmCreateImage(display, visual,
			     depth, ZPixmap, NULL, shminfo, width,height);
    if ((shminfo->shmid = shmget(IPC_PRIVATE,
				 ximage->bytes_per_line * ximage->height,
				 IPC_CREAT | 0777)) == -1) {
      cerr << "DisplayX::SetXImage(): fatal shmget failure" << endl;
    }
    if ((shminfo->shmaddr = (char *) shmat(shminfo->shmid, (void *) 0, 0)) == (char *)(-1)) {
      cerr << "DisplayX::SetXImage(): fatal shmat failure" << endl;
    }
    // Phew, all OK
    ximage->data = shminfo->shmaddr;
    XShmAttach(display,shminfo);
  }
}

// }}}
// {{{ void DisplayX::DisplayXImage(void)

void DisplayX::DisplayXImage(void)
{
  if (shm_granted) {
    XShmPutImage(display, window, gc, ximage,
		 0, 0, 0,0,width,height,False);
  } else {
    XPutImage (display, window ,gc,
	       ximage, 0, 0, 0,0,width,height);
    
  }
  //  XSync(display, 0);
}

// }}}

