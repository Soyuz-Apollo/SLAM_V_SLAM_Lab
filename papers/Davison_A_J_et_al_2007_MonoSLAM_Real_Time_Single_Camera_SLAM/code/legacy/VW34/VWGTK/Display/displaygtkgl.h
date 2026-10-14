/** 
 * @file   displaygtkgl.h
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
 * @class   VW::DisplayGTKGL
 * @ingroup Displays
 * @author  BJ Tordoff bjt
 * @date    01.01.2001
 * @brief   A display that makes use of a GTK window.
 *
 * This display opens a GTK window and attaches a DisplayGTKGLWidget
 * to it. All drawing and stuff is forwarded to the enclosed widget.
 *
 */

#ifndef _DISPLAY_GTKMM_H
#define _DISPLAY_GTKMM_H

// System includes
/* The GTK-- Includes */
#include <gtk--/main.h>
#include <gtk--/window.h>
#include <gtk--/drawingarea.h>
/* This one requires both GTKGL and GTKGL-- */
#include <gtkgl--/glarea.h>
#include <GL/gl.h>

/* Need to know about those lurverly images.. */
#include <VWGL/Display/displayglcommon.h>

// The child class
#include "displaygtkglwidget.h"


namespace VW {

  class DisplayGTKGL : public Gtk::Window, 
		       public DisplayGLCommon, 
		       public VWEvents::EventHandler {
 public:
  // Genesis and revelations...
  DisplayGTKGL(const std::string title = "");
  DisplayGTKGL(const unsigned int width, const unsigned int height, 
	       const std::string title = "");
  virtual ~DisplayGTKGL(void) {delete drawingarea;};


  // Resize an existing display
  void SetSize(const unsigned int new_width, 
	       const unsigned int new_height);
  unsigned int GetWidth();
  unsigned int GetHeight();

  // Primitive drawing operations
  void SetPixel(const int x, const int y, const unsigned char red, 
		const unsigned char green, const unsigned char blue, 
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

  /** Make the GL-context for this window current.
   * This command must be called before any drawing commands to
   * ensure that the correct GL display is active
   */ 
  void MakeCurrent();

  /** Initialise and clear the display. */
  void Reset(unsigned char red=0,
	     unsigned char green=0,
	     unsigned char blue=127);

  // Overloaded from parent
  virtual unsigned int GetTextureName();
  virtual unsigned int GetTextureWidth() const;
  virtual unsigned int GetTextureHeight() const;
    virtual void GenerateTexture(unsigned int desired_width,
				 unsigned int desired_height);

  // SigC-style signals to attach to (you can also use the VWEvents ones)
  SigC::Signal3<void,int,int,int> ClickSignal;
  SigC::Signal5<void,int,int,int,int,int> DragSignal;
  SigC::Signal5<void,int,int,int,int,int> DragFinishedSignal;

 protected:
   void _HandleClick(int x, int y, int b);
   void _HandleDrag(int x0, int y0, int x1, int y1, int b);
   void _HandleDragFinished(int x0, int y0, int x1, int y1, int b);

  /** Force the GL buffer onto the screen.
   * For generality, use Flush() not SwapBuffers().
   */ 
  void SwapBuffers();

  void Init(const unsigned int width, 
	    const unsigned int height, 
	    const std::string title);
  DisplayGTKGLWidget* drawingarea;
};

} // end namespace VW



// Primitive drawing operations
inline void VW::DisplayGTKGL::SetPixel(const int x, const int y, 
				       const unsigned char red, 
				       const unsigned char green, 
				       const unsigned char blue, 
				       const unsigned char alpha) 
{
  drawingarea->SetPixel(x,y,red,green,blue,alpha);
}

inline void VW::DisplayGTKGL::DrawPoint(const float x, const float y) 
{
  drawingarea->DrawPoint(x,y);
}

inline void VW::DisplayGTKGL::DrawLine(const float x0, const float y0, 
				   const float x1, const float y1) 
{
  drawingarea->DrawLine(x0,y0,x1,y1);
}

inline void VW::DisplayGTKGL::SetPenColour(const unsigned char red, 
					   const unsigned char green, 
					   const unsigned char blue, 
					   const unsigned char alpha) 
{
  drawingarea->SetPenColour(red,green,blue,alpha);
}

inline void VW::DisplayGTKGL::GetPenColour(unsigned char& red, 
					   unsigned char& green, 
					   unsigned char& blue, 
					   unsigned char& alpha) const 
{
  drawingarea->GetPenColour(red,green,blue,alpha);
}

inline unsigned int VW::DisplayGTKGL::GetWidth() 
{
  return((_initialised) ? (drawingarea->GetWidth()) : (0));
};

inline unsigned int VW::DisplayGTKGL::GetHeight() 
{
  return((_initialised) ? (drawingarea->GetHeight()) : (0));
};


inline void VW::DisplayGTKGL::MakeCurrent() 
{
  drawingarea->MakeCurrent();
};

inline void VW::DisplayGTKGL::SwapBuffers() 
{
  drawingarea->Flush();
};

inline void VW::DisplayGTKGL::Flush() 
{
  drawingarea->Flush();
};

inline unsigned int VW::DisplayGTKGL::GetTextureName()
{
  return drawingarea->GetTextureName();
}

inline unsigned int VW::DisplayGTKGL::GetTextureWidth() const
{
  return drawingarea->GetTextureWidth();
}

inline unsigned int VW::DisplayGTKGL::GetTextureHeight() const
{
  return drawingarea->GetTextureHeight();
}

inline void 
VW::DisplayGTKGL::GenerateTexture(unsigned int desired_width,
				  unsigned int desired_height)
{
  drawingarea->GenerateTexture(desired_width,
			       desired_height);
}


inline void VW::DisplayGTKGL::Reset(unsigned char red,
				    unsigned char green,
				    unsigned char blue) 
{
  drawingarea->Reset(red,green,blue);
}


inline void 
VW::DisplayGTKGL::_HandleClick(int x, int y, int b)
{
  ClickEvent(x,y,b);          // VW event
  ClickSignal.emit(x,y,b);    // SigC signal
}

inline void 
VW::DisplayGTKGL::_HandleDrag(int x0, int y0, int x1, int y1, int b)
{
  DragEvent(x0,y0,x1,y1,b);          // VW event
  DragSignal.emit(x0,y0,x1,y1,b);    // SigC signal
}

inline void 
VW::DisplayGTKGL::_HandleDragFinished(int x0, int y0, int x1, int y1, int b)
{
  std::cerr << "VW::DisplayGTKGL::_HandleDragFinished: emit event (" << DragFinishedEvent.IsAttached()<<")" << std::endl;
  DragFinishedEvent(x0,y0,x1,y1,b);          // VW event
  std::cerr << "VW::DisplayGTKGL::_HandleDragFinished: emit signal" << std::endl;
  DragFinishedSignal.emit(x0,y0,x1,y1,b);    // SigC signal
}
#endif
