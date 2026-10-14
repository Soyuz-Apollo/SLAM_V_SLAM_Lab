/** 
 * @file   displayxforms.h
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


#ifndef _DISPLAY_XFORMS_H
#define _DISPLAY_XFORMS_H

// Display class based on XForms
#include <iostream>
#include <string>
extern "C" {
#include "displayxformsform.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/extensions/XShm.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
}
// Signal (callback) handling
#include <VW/events.h>

// Include base class
#include <VW/Graphics/displaybase.h>

namespace VW {
class DisplayXForms : public DisplayBase {
public:
  // Genesis and revelations...
  DisplayXForms(const std::string title = ""); 
  // Note that if no size is specified the display shouldn't get shown
  DisplayXForms(const unsigned int width,
					 const unsigned int height, 
					 const std::string title = "");
  ~DisplayXForms(); 


  // Useful functions
  void SetSize(const unsigned int new_width,
	       const unsigned int new_height);
  unsigned int GetWidth();
  unsigned int GetHeight();

  // Primitive drawing operations
  void SetPenColour(const unsigned char red, const unsigned char green, 
		    const unsigned char blue, const unsigned char alpha);
  void GetPenColour(unsigned char& red, unsigned char& green, 
		    unsigned char& blue, unsigned char& alpha) const;
  void SetPixel(const int x, const int y, const unsigned char red, 
		const unsigned char green, const unsigned char blue, const unsigned char alpha=0);
  void DrawPoint(const float x, const float y);
  void DrawLine(const float x0, const float y0, 
		const float x1, const float y1);

  // Force drawing to screen (flush buffers)
  void Flush(void);

  // XForms specific stuff
  char* GetImagePointer(void);
  int GetBPP(void);
  int GetDepth(void);
  int ColourLookUp(int r, int g, int b);
  int GreyLookUp(int l);
  void DisplayXImage(void);



protected:
  FD_DisplayForm *displayform;
  // X-windies kind of stuff
  unsigned int _width,_height;
  int vclass;
  int screen;
  int depth;
  int bits_pp;      //bits per pixel
  int bytes_pp; //bytes per pixel
  Window   window;
  Display *display;
  GC       gc;
  Visual  *visual;
  Colormap colormap;
  // XImages
  XImage *ximage;
  // shared memory information
  XShmSegmentInfo*  shminfo;
  // Requested shared memory? Granted shared memory
  int shm_request;
  int shm_granted;
  // vw kind of stuff
  //  Color maps: grey lut used for 8 bpp
  //              colorlut for 8 and 32bpp
  //              colorcubelut used for 8bpp
  unsigned char greylut[256];
  unsigned long colorlut[1024];
  int pencolour;
  unsigned int red,green,blue,alpha;

  void Init(const unsigned int width,
				const unsigned int height,
				const std::string title);
  void InitXImage(void);
  void SetColormaps(void);
  void SetXImage(void);



};

} // end namespace VW

// Inliners
inline unsigned int 
VW::DisplayXForms::GetWidth(void) 
{
  return(_width);
}

inline unsigned int 
VW::DisplayXForms::GetHeight(void) 
{
  return(_height);
}

inline char* 
VW::DisplayXForms::GetImagePointer(void) 
{
  return(ximage->data);
};

inline int 
VW::DisplayXForms::GetBPP(void) 
{
  return(bits_pp);
};

inline int 
VW::DisplayXForms::GetDepth(void) 
{
  return(depth);
};

inline void 
VW::DisplayXForms::SetPenColour(const unsigned char red, 
										  const unsigned char green, 
										  const unsigned char blue, 
										  const unsigned char alpha) 
{
  this->blue=blue;this->green=green;this->blue=blue;this->alpha=alpha;
  XColor c;c.red=red<<8; c.green=green<<8; c.blue=blue<<8;  
  XAllocColor(display, colormap, &c);
  pencolour = c.pixel;
  XSetForeground(display,gc,pencolour);
}

inline void 
VW::DisplayXForms::GetPenColour(unsigned char& red, 
										  unsigned char& green, 
										  unsigned char& blue, 
										  unsigned char& alpha) const 
{
  red=this->red;
  green=this->green;
  blue=this->blue;
  alpha=this->alpha;
}

inline 
VW::DisplayXForms::~DisplayXForms(void) 
{
  //  cout << "Aaargh! I'm dead." << endl;
}

// Calback for form
int DisplayXFormsCanvasHandler(FL_OBJECT *ob, Window win, int width, int height, XEvent* xev, void* data);

#endif
