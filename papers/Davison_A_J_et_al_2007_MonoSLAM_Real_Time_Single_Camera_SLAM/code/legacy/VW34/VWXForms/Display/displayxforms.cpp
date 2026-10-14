/** 
 * @file   displayxforms.cc
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


#include <iostream>

#include "VW/graphics.h"
#include "displayxforms.h"

#define DEF_SIZE 100

using namespace std;
using namespace VW;


DisplayXForms::DisplayXForms(const string title) 
  : DisplayBase(title),
	 _width(DEF_SIZE),
	 _height(DEF_SIZE)
{
  cerr << "Hurray, a new xforms display!" << endl;
}



DisplayXForms::DisplayXForms(const unsigned int width,
									  const unsigned int height,
									  const string title) 
  : DisplayBase(width,height,title),
	 _width(width),
	 _height(height)
{
  cerr << "Hurray, a new xforms display!" << endl;
  Init(width,height,title);
}



void DisplayXForms::Init(const unsigned int width,
								 const unsigned int height,
								 const string title) 
{
  cerr << "Initialising XForm display "<<title<<endl;
  int xpos=0;
  int ypos=0;
  // Leave shared memory off for safety
  shm_request=0;

  displayform = create_form_DisplayForm(width,height,reinterpret_cast<long>(this));
  fl_set_form_position(displayform->DisplayForm,xpos,ypos);
  fl_show_form(displayform->DisplayForm,FL_PLACE_GEOMETRY,FL_FULLBORDER,title.c_str());
  FL_OBJECT * ob= NULL;
  ob = displayform->canvas_graphic;
  window= FL_ObjWin( ob );
  InitXImage();
  // Activate events
  XSelectInput(fl_get_display(),window, ButtonMotionMask|ButtonPressMask|ButtonReleaseMask|ExposureMask);
  // Default pen colour is yellow
  SetPenColour(255, 255, 0, 0);

  _initialised=true;
}




void DisplayXForms::SetSize(const unsigned int new_width,
			    const unsigned int new_height) 
{
  _width  = new_width;
  _height = new_height;

  if (!_initialised) {
    // Just initialise to new size
    Init(_width,_height,_my_title);
  } else {
    
    // Hide form is presently on screen
    if(_visible)
      fl_hide_form(displayform->DisplayForm);
    
    // Leave shared memory off for safety
    shm_request=0;
    
    delete displayform;
  
    // Re-initialise at new size  
    Init(_width,_height,_my_title);
  }
  _visible=true;
}



// Drawing primitves

void DisplayXForms::SetPixel(const int x, const int y, 
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



void DisplayXForms::DrawPoint(const float x, const float y) {
  XDrawPoint (display, window, gc, 
	      static_cast<const int>(x+0.5), 
	      static_cast<const int>(y+0.5) );
}



void DisplayXForms::DrawLine(const float x0, const float y0,
			     const float x1, const float y1) {
  int thick=1;
  XSetLineAttributes(display,gc,thick,LineSolid,CapButt,JoinMiter);
  XDrawLine (display, window, gc, 
	     static_cast<const int>(x0+0.5), static_cast<const int>(y0+0.5), 
	     static_cast<const int>(x1+0.5), static_cast<const int>(y1+0.5) );
}




// Force drawing to screen (flush buffers)
void DisplayXForms::Flush(void) {
  XFlush(display);
}

//////////////////////
//                  //
//   GFXBOX Stuff   //
//                  //
//////////////////////


void DisplayXForms::InitXImage(void)
{
  // NB X drawing routines draw with respect to the origin of a Window.
  // As each separate form is a window, we need to supply x0 and y0 offsets of
  // the graphic box within the form. The width and height are required for
  // X-images.

   // Shared memory request
   shm_granted = 0; // so far

   // Get essential X Stuff first
   vclass      = fl_get_vclass();
   display     = fl_get_display();
   gc          = fl_state[vclass].gc[0];
   screen      = DefaultScreen(display);
   visual      = DefaultVisual(display, screen);
   depth       = DefaultDepth(display, screen);
   bytes_pp    = (depth==15 || depth==16) ? 2 : (depth==24 || depth==32) ? 4 : 1;
   colormap   =  XDefaultColormap(display, screen);

   // get bits per pixel --- courtesy IDR. Why so fancy?
   {
     int i,nformats;
     XPixmapFormatValues *formats = XListPixmapFormats(display, &nformats);
     for (i=0; i<nformats; i++) {
       if (formats[i].depth==depth) {
	 bits_pp=formats[i].bits_per_pixel;
	 break;
       }
     }
   }

   // Knowing the bits_pp allows us to make the colormaps
   SetColormaps();
//    // Set the gfxbox's integer pixel coordinates
//    SetBoxCoord(x0,y0,w,h);
   // Set the gfxbox's XImage
   SetXImage();

}



void DisplayXForms::SetColormaps(void) {
  int i,r,g,b,col;

  switch(depth) {
    
  case 8:  // Pseudocolor with compress Grey scale and 4-bit colourcube
    // Add colourcube to colourmap
    for(i=64,col=0;i<128;i++,col++) {
      r=static_cast<int>(col/16);
      g=static_cast<int>((col-(r*16))/4);
      b=static_cast<int>(col-(r*16)-(g*4));
      fl_mapcolor((FL_COLOR)i,r*85,g*85,b*85);
    }
    // Add greyscales to colourmap
    for(i=128,col=0;i<256;i++,col+=2) fl_mapcolor((FL_COLOR)i,col,col,col);
    // Make two luts (1) to map image grey values onto X colors
    //               (2) to map all FL numbers onto X colors
    for (i=0;i<256;i++) greylut[i] = fl_get_pixel(i/2+128);
    for (i=0;i<256;i++) colorlut[i] = (unsigned long)fl_get_pixel(i);
    
    break;
    
  case 15:
    // True color so no need to set the gray values 
    // Make color lut for drawing with XSetForeGround
    for (i=0;i<1024;i++) {
      fl_get_icm_color(i,&r,&g,&b);
      colorlut[i] = (unsigned long)( 32*(32*(b>>3) + (g>>3)) + (r>>3));
    }
    break;

  case 16:
    // True color so no need to set the gray values 
    // Make color lut for drawing with XSetForeGround
    for (i=0;i<1024;i++) {
      fl_get_icm_color(i,&r,&g,&b);
      colorlut[i] = (unsigned long)( 64*(32*(b>>3) + (g>>3)) + (r>>3));
    }
    break;

  case 24:
  case 32: // True color so no need to set the gray values 
    // Make color lut for drawing with XSetForeGround
    for (i=0;i<1024;i++) {
      fl_get_icm_color(i,&r,&g,&b);
      colorlut[i] = (unsigned long)( 256*(256*r + g) + b);
    }
    break;
  }
}



int DisplayXForms::ColourLookUp(int r, int g, int b) {
  int bx=static_cast<int>((b/64));
  int gx=static_cast<int>((g/64));
  int rx=static_cast<int>((r/64));
  return colorlut[64 + bx + 4* (gx + 4* rx)];
}



int DisplayXForms::GreyLookUp(int l) 
{
  return colorlut[l];
}



void DisplayXForms::SetXImage(void)
{
  int bitmap_pad;
  char   *xdata;

  if (!shm_request || !XShmQueryExtension(display)) {
    // do not use shared memory
    shm_granted = 0;
    //fprintf(stderr,"VW_GfxBox::SetXImage(): Not using MIT_SHM\n");
    // allocate xdata
    xdata  = new char[_width*_height*bytes_pp];
    if (xdata==NULL) {
      cerr << "DisplayXForms::SetXImage: failed to allocate xdata" << endl;
    }
    // allocate ximage
    bitmap_pad = 8*bytes_pp;
    ximage = XCreateImage (display, visual,
			   depth, ZPixmap, 0, xdata, _width, _height,
			   bitmap_pad, 0 );
      if (ximage==NULL) {
	cerr << "DisplayXForms::SetXImage(): XCreateImage failed" << endl;
      }
    } 
  else {
    // use shared memory
    shm_granted = 1;
    //fprintf(stderr,"VW_GfxBox::SetXImage(): Using MIT_SHM\n");
    shminfo = new XShmSegmentInfo [sizeof(XShmSegmentInfo)];
    ximage = XShmCreateImage(display, visual,
			     depth, ZPixmap, NULL, shminfo, _width,_height);
    if ((shminfo->shmid = shmget(IPC_PRIVATE,
				 ximage->bytes_per_line * ximage->height,
				 IPC_CREAT | 0777)) == -1) {
      cerr << "DisplayXForms::SetXImage(): fatal shmget failure" << endl;
    }
    if ((shminfo->shmaddr = (char *) shmat(shminfo->shmid, (void *) 0, 0)) == (char *)(-1)) {
      cerr << "DisplayXForms::SetXImage(): fatal shmat failure" << endl;
    }
    // Phew, all OK
    ximage->data = shminfo->shmaddr;
    XShmAttach(display,shminfo);
  }
}



void DisplayXForms::DisplayXImage(void)
{
  if (shm_granted) {
    XShmPutImage(display, window, gc, ximage,
					  0, 0, 0, 0, _width, _height, False);
  } else {
    XPutImage (display, window ,gc,
					ximage, 0, 0, 0, 0, _width, _height);
    
  }
  //  XSync(display, 0);
}







/// Urg, nast horrible pooey stuff. This callback is global and
// references the parent object using a crappy reinterpret cast.

int DisplayXFormsCanvasHandler(FL_OBJECT *ob, Window win, int width, int height, XEvent* event, void* data) {
  DisplayXForms* parent=reinterpret_cast<DisplayXForms*>(data);
  static int last_x=0;
  static int last_y=0;
  static int last_button=0;
  static bool dragging=false;

  //cout << event->type << "("<<Expose<<","<<ButtonPress<<","<<ButtonRelease<<","<<MotionNotify<<")"<<endl;
  switch (event->type) {
  case Expose:
    cout << "Expose" << endl;
    break;
  case ButtonPress:
    {
      last_x=int(event->xbutton.x);
      last_y=int(event->xbutton.y);
      last_button=int(event->xbutton.button);
      dragging=false;
      //parent->ClickEvent.emit(last_x,last_y,last_button);
    }
    break;
  case ButtonRelease:
    {
      if (dragging) {
	int x=int(event->xmotion.x);
	int y=int(event->xmotion.y);
	parent->DragFinishedEvent(last_x,last_y,x,y,last_button);
      } else {
	parent->ClickEvent(last_x,last_y,last_button);
      }
      dragging=false;
    }
    break;
  case MotionNotify:
    {
      int x=int(event->xmotion.x);
      int y=int(event->xmotion.y);
      // Drags always start with a click, so can use last click to work out
      // the start position and button
      if (last_button>0)
	parent->DragEvent(last_x,last_y,x,y,last_button);
      dragging=true;
    }
    break;
  default:
    cout << "Unbound event" << endl;
  }
  return(1);
}
