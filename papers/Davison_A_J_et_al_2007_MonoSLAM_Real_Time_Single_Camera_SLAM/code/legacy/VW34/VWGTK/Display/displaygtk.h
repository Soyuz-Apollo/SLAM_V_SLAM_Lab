/** 
 * @file   displaygtk.h
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
/**
 * @class   VW::DisplayGTK 
 * @ingroup Displays
 * @author  BJ Tordoff
 * @date    29/01/2002
 * @brief   A display that makes use of a GTK window.
 *
 * This display opens a GTK window and attaches a DisplayGTKWidget
 * to it. All drawing and stuff is forwarded to the enclosed widget.
 *
 */


#ifndef _DISPLAY_GTK_H
#define _DISPLAY_GTK_H

// System includes

/* The GTK-- Includes */
#include <gtk--/main.h>
#include <gtk--/window.h>
#include <gtk--/drawingarea.h>

/* Need to know about those lurverly images.. */
#include "VW/image.h"
#include "VW/graphics.h"
#include "displaygtkcommon.h"
#include "displaygtkwidget.h"


namespace VW {

class DisplayGTK : public Gtk::Window, 
		   public DisplayGTKCommon, 
		   public VWEvents::EventHandler {
 public:
  // Genesis and revelations...
  DisplayGTK(const std::string title = "");
  DisplayGTK(const unsigned int width, const unsigned int height, 
	       const std::string title = "");
  virtual ~DisplayGTK(void) {delete drawingarea;};


  // Resize an existing display
  void SetSize(const unsigned int new_width, 
	       const unsigned int new_height);
  unsigned int GetWidth();
  unsigned int GetHeight();

  // Primitive drawing operations
  void SetPixel(const int x, const int y);

  void SetPixel(const int x, const int y, 
		const unsigned char red, 
		const unsigned char green, 
		const unsigned char blue, 
		const unsigned char alpha=0);

  void DrawPoint(const float x, const float y);
  void DrawLine(const float x0, const float y0, 
		const float x1, const float y1);
  void SetPenColour(const unsigned char red, const unsigned char green, 
		    const unsigned char blue, const unsigned char alpha);
  void GetPenColour(unsigned char& red, unsigned char& green, 
		    unsigned char& blue, unsigned char& alpha) const;

  // Force drawing to screen (flush buffers)
  void Flush(void);

  /** Initialise and clear the display. */
  void Reset() {drawingarea->Reset();}

  // Particular to GTK displays
  DisplayGTKWidget* GetDrawingArea() {return(drawingarea);};

  // SigC-style signals to attach to (you can also use the VWEvents ones)
  SigC::Signal3<void,int,int,int> ClickSignal;
  SigC::Signal5<void,int,int,int,int,int> DragSignal;
  SigC::Signal5<void,int,int,int,int,int> DragFinishedSignal;

 protected:
  void Init(const unsigned int width, const unsigned int height, 
	    const std::string title);

   void _HandleClick(int x, int y, int b);
  void _HandleDrag(int x0, int y0, int x1, int y1, int b);
   void _HandleDragFinished(int x0, int y0, int x1, int y1, int b);

  DisplayGTKWidget* drawingarea;
};

} // end namespace VW














// Primitive drawing operations
inline void 
VW::DisplayGTK::SetPixel(const int x, const int y) 
{
  drawingarea->SetPixel(x,y);
}

inline void 
VW::DisplayGTK::SetPixel(const int x, const int y, 
			 const unsigned char red, 
			 const unsigned char green, 
			 const unsigned char blue, 
			 const unsigned char alpha) 
{
  drawingarea->SetPixel(x,y,red,green,blue,alpha);
}

inline void 
VW::DisplayGTK::DrawPoint(const float x, const float y) 
{
  drawingarea->DrawPoint(x,y);
}

inline void 
VW::DisplayGTK::DrawLine(const float x0, const float y0, 
			 const float x1, const float y1) 
{
  drawingarea->DrawLine(x0,y0,x1,y1);
}

inline void 
VW::DisplayGTK::SetPenColour(const unsigned char red, 
			     const unsigned char green, 
			     const unsigned char blue, 
			     const unsigned char alpha) 
{
  drawingarea->SetPenColour(red,green,blue,alpha);
}

inline void 
VW::DisplayGTK::GetPenColour(unsigned char& red, 
			     unsigned char& green, 
			     unsigned char& blue, 
			     unsigned char& alpha) const 
{
  drawingarea->GetPenColour(red,green,blue,alpha);
}

inline unsigned int 
VW::DisplayGTK::GetWidth() 
{
  return((_initialised) ? (drawingarea->GetWidth()) : (0));
};

inline unsigned int 
VW::DisplayGTK::GetHeight() 
{
  return((_initialised) ? (drawingarea->GetHeight()) : (0));
};

inline void 
VW::DisplayGTK::Flush() 
{
  drawingarea->Flush();
};

inline void 
VW::DisplayGTK::_HandleClick(int x, int y, int b)
{
  ClickEvent(x,y,b);          // VW event
  ClickSignal.emit(x,y,b);    // SigC signal
}

inline void 
VW::DisplayGTK::_HandleDrag(int x0, int y0, int x1, int y1, int b) 
{
  DragEvent(x0,y0,x1,y1,b);          // VW event
  DragSignal.emit(x0,y0,x1,y1,b);    // SigC signal
}

inline void 
VW::DisplayGTK::_HandleDragFinished(int x0, int y0, int x1, int y1, int b) 
{
  DragFinishedEvent(x0,y0,x1,y1,b);          // VW event
  DragFinishedSignal.emit(x0,y0,x1,y1,b);    // SigC signal
}


#endif
