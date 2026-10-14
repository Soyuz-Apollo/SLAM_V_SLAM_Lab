/** 
 * @file   threedtoolgtkglwidget.h
 * @author AJ Davison and BJ Tordoff
 * @date   30/10/2002
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
 * @class   VW::ThreeDToolGTKGLWidget
 * @ingroup GLTools 
 * @author  AJ Davison, BJ Tordoff
 * @date    30/10/2002
 * @brief   A version of the ThreeDToolGLWidget which is also a GTK widget, 
 *          and can be embedded in a GTK interface.
 *
 * (NB: This class *doesn't* open a window, and is only useful when embedded in an existing
 * GTK interface.)
 *
 * ***Please insert long descrition here...
 */

#ifndef _THREEDTOOL_GTKGL_WIDGET_H
#define _THREEDTOOL_GTKGL_WIDGET_H

#include <gtk--.h>
#include <gtkgl--/glarea.h>
#include <sigc++/signal_system.h>

// We need the main VW/GL library
#include <VWGL/display.h>


namespace VW {

// Forward declaration because these classes refer to each other
class ThreeDToolGTKGLWidget;


// Local version of Gtk::GLArea
// We need to override the interaction functions
// And we're not deriving the ThreeDToolGTKGLWidget class directly from
// GLArea because we want to force it to be an AspectFrame
class LocalGTKGLArea : public Gtk::GLArea {
 public:

  LocalGTKGLArea(ThreeDToolGTKGLWidget *parent);

  // take over some impl methods
  void draw_impl(GdkRectangle *p0);
  gint expose_event_impl(GdkEventExpose*); 
  //  void size_allocate_impl(GtkAllocation* p0);
  gint motion_notify_event_impl(GdkEventMotion*); 
  gint button_press_event_impl(GdkEventButton*); 
  gint button_release_event_impl(GdkEventButton*); 


 protected:
  ThreeDToolGTKGLWidget *parent_ptr;

  // For dragging
  int last_x, last_y;



};


class ThreeDToolGTKGLWidget : public ThreeDToolGLCommon, 
			      public Gtk::AspectFrame {

  friend class LocalGTKGLArea;

 public:
  ThreeDToolGTKGLWidget(const unsigned int camera_width, 
		      const unsigned int camera_height, 
		      const std::string title = "");


  // Redefined virtual functions
  unsigned int WindowWidth() const {return localglarea.width();}
  unsigned int WindowHeight() const {return localglarea.height();}
  void SwapBuffers();
  void MakeCurrent();
  void RequestDraw();

  // For GTK version, can either use VWEvents callbacks, or SigC
  SigC::Signal1<void,bool> DrawSignal;
  SigC::Signal3<void,int,double,double> ProcessHitsSignal;


 protected:
  // Overloaded from parent so that both VW events and SigC signals are
  // generated
  virtual void _CallDrawEvent(bool b);
  virtual void _CallProcessHitsEvent(int item, double x, double y);

  LocalGTKGLArea localglarea;

  std::string title;


};

};

#endif
