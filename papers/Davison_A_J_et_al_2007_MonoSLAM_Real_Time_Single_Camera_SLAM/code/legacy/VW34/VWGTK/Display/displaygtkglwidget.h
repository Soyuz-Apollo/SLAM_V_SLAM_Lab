/** 
 * @file   displaygtkglwidget.h
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
 * @class   VW::DisplayGTKGLWidget
 * @ingroup Displays 
 * @author  BJ Tordoff
 * @date    29/01/2002
 * @brief A display that uses a GTK GL canvas widget for drawing.
 *
 * (NB: This class *doesn't* open a window, and is only useful when embedded in an existing
 * GTK interface.)
 *
 * ***Please insert long descrition here...
 */


#ifndef _DISPLAY_GTKMM_WIDGET_H
#define _DISPLAY_GTKMM_WIDGET_H

// System includes
#include <gtk--.h>

/* This one requires both GTKGL and GTKGL-- */
#include <gtkgl--/glarea.h>

/* Need to know about those lurverly images.. */
#include <VW/image.h>
#include <VW/graphics.h>
#include <VWGL/Display/displayglcommon.h>

namespace VW {

class DisplayGTKGLWidget : public Gtk::GLArea, public DisplayGLCommon {
public:
  // Genesis and revelations...
  DisplayGTKGLWidget(const std::string title = "");
  DisplayGTKGLWidget(const unsigned int width, 
		     const unsigned int height, 
		     const std::string title = "");
  ~DisplayGTKGLWidget(void) {};


  // Resize an existing display
  void SetSize(const unsigned int new_width, 
	       const unsigned int new_height);
  unsigned int GetWidth();
  unsigned int GetHeight();

  void MakeCurrent();

  // Sometimes, other (parent) classes need to know the drawing params
   void GetGLParams(float& xscale, float& yscale,
		    float& xoffset, float& yoffset,
		    GLuint& texid) const;

  // SigC-style signals to attach to (you can also use the VWEvents ones)
  SigC::Signal3<void,int,int,int> ClickSignal;
  SigC::Signal5<void,int,int,int,int,int> DragSignal;
  SigC::Signal5<void,int,int,int,int,int> DragFinishedSignal;

protected:
  void SwapBuffers();


  // take over some impl methods
  gint expose_event_impl(GdkEventExpose*); 
  //  void size_allocate_impl(GtkAllocation* p0);
  gint motion_notify_event_impl(GdkEventMotion*); 
  gint button_press_event_impl(GdkEventButton*); 
  gint button_release_event_impl(GdkEventButton*); 


};


} // end namespace VW

// Drawing primitves



inline unsigned int VW::DisplayGTKGLWidget::GetWidth() 
{
  return(width());
};

inline unsigned int VW::DisplayGTKGLWidget::GetHeight() 
{
  return(height());
};




inline void VW::DisplayGTKGLWidget::MakeCurrent() 
{
  if (!_initialised) InitGL();
  make_current();
};


inline void VW::DisplayGTKGLWidget::SwapBuffers() 
{
  swap_buffers();
};

inline void 
VW::DisplayGTKGLWidget::GetGLParams(float& xscale, float& yscale,
				    float& xoffset, float& yoffset,
				    unsigned int& texid) const
{
  xscale = _xscale;
  yscale = _yscale;
  xoffset = _xoffset;
  yoffset = _yoffset;
  texid = texName;
}

#endif
