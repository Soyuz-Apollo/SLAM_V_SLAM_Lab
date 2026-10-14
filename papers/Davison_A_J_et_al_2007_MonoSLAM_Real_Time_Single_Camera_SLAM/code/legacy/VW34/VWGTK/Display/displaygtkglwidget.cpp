/** 
 * @file   displaygtkglwidget.cc
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


// System includes
#include <GL/gl.h>
#include <GL/glu.h>
#include <unistd.h>

// Local includes
#include "displaygtkglwidget.h"

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false

#define WINDOW_BORDER 5

using namespace std;
using namespace VW;

DisplayGTKGLWidget::DisplayGTKGLWidget(const unsigned int width, 
				       const unsigned int height, 
				       const string title) 
: DisplayGLCommon (width, height, title)
{
  if (VERBOSE) { 
    cerr << "New display ";
    if (title.length() == 0) cerr << "(unnamed) ";
    else cerr << "`" << title << "' ";
    cerr << "(" << width << "x" << height << ")" << endl;
  }

  //set_resize_mode(GTK_RESIZE_IMMEDIATE);
  set_usize(width,height);
  size(width,height);
  set_events(GDK_EXPOSURE_MASK|
	     GDK_POINTER_MOTION_HINT_MASK |
	     GDK_BUTTON_MOTION_MASK|
	     GDK_BUTTON_PRESS_MASK|
	     GDK_BUTTON_RELEASE_MASK);

  // Must set init flag to false explicitly since base class
  // sets it to true
  _visible = true;
  _initialised = false;
};

DisplayGTKGLWidget::DisplayGTKGLWidget(const string title) 
  : DisplayGLCommon (50, 50, title)
{
  if (VERBOSE) {
    cerr << "New display ";
    if (title.length() == 0) cerr << "(unnamed)" << endl;
    else cerr << "`" << title << "'" << endl;
  }  
  //set_resize_mode(GTK_RESIZE_IMMEDIATE);
  set_usize(50,50);
  size(50,50);
  set_events(GDK_EXPOSURE_MASK|
	     GDK_POINTER_MOTION_HINT_MASK |
	     GDK_BUTTON_MOTION_MASK|
	     GDK_BUTTON_PRESS_MASK |
	     GDK_BUTTON_RELEASE_MASK);

  // Must set visible flag to false explicitly since base class
  // sets it to true
  _visible = false;
  _initialised = false;
}

void DisplayGTKGLWidget::SetSize(const unsigned int new_width,
				 const unsigned int new_height) 
{
  // To get the window to resize, must hide and show it.
  // nasty nasty nasty
  if (is_visible()) Gtk::GLArea::hide ();
  set_usize(new_width,new_height);
  size(new_width,new_height);
  set_events(GDK_EXPOSURE_MASK|
	     GDK_POINTER_MOTION_HINT_MASK |
	     GDK_BUTTON_MOTION_MASK|
	     GDK_BUTTON_PRESS_MASK |
	     GDK_BUTTON_RELEASE_MASK);
  if (is_visible()) Gtk::GLArea::show ();

  // ***** Need to re-allocate some texture memory?


  _visible = true;

}


gint DisplayGTKGLWidget::expose_event_impl(GdkEventExpose* event) { 
  // Only handle the last event
   if (event->count>0) return (FALSE);
  make_current();
   glViewport(0, 0, (GLint)(width()), (GLint)(height()));
   swap_buffers();
   return (TRUE);
}; 


gint DisplayGTKGLWidget::motion_notify_event_impl(GdkEventMotion* event) { 
  dragging=true;
  GdkModifierType state;
  int x=0,y=0;
  int b=0;
   if (event->is_hint) {
     gint tx,ty;
     gdk_window_get_pointer(event->window, &tx, &ty, &state);
     x=int(tx);
     y=int(ty);
     if (state&GDK_BUTTON1_MASK) b=1;
     if (state&GDK_BUTTON2_MASK) b=2;
     if (state&GDK_BUTTON3_MASK) b=3;
   } else {
     return(1); // Take only hints, to speed things along
   }
   // Use the zoom factors to get pixel coords
   MakeCurrent();
   {
     float zx=0,zy=0;
     glGetFloatv(GL_ZOOM_X,&zx);
     glGetFloatv(GL_ZOOM_Y,&zy);
     x=int(float(x)/zx);
     y=int(float(y)/zy);
   }

   // Drags start with a click, so use that to find the start end
   if (VERBOSE) cerr << "DisplayGTKGLWidget::emit drag event" << endl;
   DragEvent(last_x,last_y,x,y,b);
   if (VERBOSE) cerr << "DisplayGTKGLWidget::emit drag signal" << endl;
   DragSignal.emit(last_x,last_y,x,y,b);
   return (TRUE);
}; 


gint DisplayGTKGLWidget::button_press_event_impl(GdkEventButton* event) { 
    if (VERBOSE) cerr << "DisplayGTKGLWidget::button_press_event" << endl;
  last_x=int(event->x);
  last_y=int(event->y);
   // Use the zoom factors to get pixel coords
   MakeCurrent();
   {
     float zx=0,zy=0;
     glGetFloatv(GL_ZOOM_X,&zx);
     glGetFloatv(GL_ZOOM_Y,&zy);
     last_x=int(float(last_x)/zx);
     last_y=int(float(last_y)/zy);
   }
  //  int b=int(event->button);
  // Not dragging yet
  dragging=false;

  // Note that the clicked even is not generated until the button
  // is released

  return (TRUE);
}; 


gint DisplayGTKGLWidget::button_release_event_impl(GdkEventButton* event) { 
    if (VERBOSE) cerr << "DisplayGTKGLWidget::button_release_event" << endl;
  if (dragging==true) {
    int x=int(event->x);
    int y=int(event->y);
    int b=int(event->button);
   // Use the zoom factors to get pixel coords
   MakeCurrent();
   {
     float zx=0,zy=0;
     glGetFloatv(GL_ZOOM_X,&zx);
     glGetFloatv(GL_ZOOM_Y,&zy);
     x=int(float(x)/zx);
     y=int(float(y)/zy);
   }
   if (VERBOSE) cerr << "DisplayGTKGLWidget::emit drag-finished event (" << DragFinishedEvent.IsAttached()<<")"<<endl;
    DragFinishedEvent(last_x,last_y,x,y,b);
    if (VERBOSE) cerr << "DisplayGTKGLWidget::emit drag-finished signal" << endl;
    DragFinishedSignal.emit(last_x,last_y,x,y,b);
  } else {
    // If we weren't dragging, it's a straight click
    int b=int(event->button);
    if (VERBOSE) cerr << "DisplayGTKGLWidget::emit click signal" << endl;
    ClickEvent(last_x,last_y,b);
    ClickSignal.emit(last_x,last_y,b);
  }
    if (VERBOSE) cerr << "DisplayGTKGLWidget::done" << endl;
  return (TRUE);
}; 
