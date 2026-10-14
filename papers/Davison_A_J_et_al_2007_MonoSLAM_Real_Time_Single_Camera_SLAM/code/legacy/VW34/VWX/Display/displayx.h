/** 
 * @file   displayx.h
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
/**
 * @class   VW::DisplayX
 * @ingroup Displays
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/2002
 * @brief   A display which uses only Raw X library calls.
 *
 * This display opens a window using only X library calls, and allows all the usual
 * drawing operations.
 */


#ifndef _DISPLAY_X_H
#define _DISPLAY_X_H

// Display class based on raw X
#include <iostream>
#include <string>
#include <vector>
extern "C" {
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

// Include base class
#include "VW/graphics.h"

namespace VW {

class DisplayX : public DisplayBase {
public:
  // Genesis and revelations...
  DisplayX(const std::string title = ""); 
  // Note that if no size is specified the display shouldn't get shown
  DisplayX(const unsigned int width,
	   const unsigned int height, 
	   const std::string title = "");
  ~DisplayX(); 


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
  void SetPixel(const int x, const int y);
  void SetPixel(const int x, const int y, const unsigned char red, 
		const unsigned char green, const unsigned char blue, const unsigned char alpha=0);
  void DrawPoint(const float x, const float y);
  void DrawLine(const float x0, const float y0, 
		const float x1, const float y1);

  // Force drawing to screen (flush buffers)
  void Flush(void);

  // X specific stuff
  char* GetImagePointer(void);
  int GetBPP(void);
  int GetDepth(void);
  int ColourLookUp(int r, int g, int b);
  int GreyLookUp(int l);
  void DisplayXImage(void);

  Window   window;

protected:
  unsigned int width,height;
  // X-windies kind of stuff
  int vclass;
  int screen;
  int depth;
  int bits_pp;      //bits per pixel
  int bytes_pp; //bytes per pixel
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
inline unsigned int VW::DisplayX::GetWidth(void) {return(width);}
inline unsigned int VW::DisplayX::GetHeight(void) {return(height);}
inline char* VW::DisplayX::GetImagePointer(void) {return(ximage->data);};
inline int VW::DisplayX::GetBPP(void) {return(bits_pp);};
inline int VW::DisplayX::GetDepth(void) {return(depth);};

inline void VW::DisplayX::SetPenColour(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha) {
  this->blue=blue;this->green=green;this->blue=blue;this->alpha=alpha;
  XColor c;c.red=red<<8; c.green=green<<8; c.blue=blue<<8;  
   XAllocColor(this->display, colormap, &c);
   pencolour = c.pixel;
   XSetForeground(this->display,gc,pencolour);
}
inline void VW::DisplayX::GetPenColour(unsigned char& red, unsigned char& green, unsigned char& blue, unsigned char& alpha) const {
  red=this->red;
  green=this->green;
  blue=this->blue;
  alpha=this->alpha;
}


#endif
