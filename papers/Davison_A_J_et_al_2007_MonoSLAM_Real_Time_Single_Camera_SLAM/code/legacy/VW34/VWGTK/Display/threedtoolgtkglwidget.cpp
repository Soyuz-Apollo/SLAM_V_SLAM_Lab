/** 
 * @file   threedtoolgtkglwidget.cc
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


// Local includes
#include "threedtoolgtkglwidget.h"


using namespace std;
using namespace VW;

#define VERBOSE false

LocalGTKGLArea::LocalGTKGLArea(ThreeDToolGTKGLWidget *parent)
  : parent_ptr(parent)
{
  set_events(GDK_EXPOSURE_MASK|
	     GDK_POINTER_MOTION_HINT_MASK |
	     GDK_BUTTON_MOTION_MASK|
	     GDK_BUTTON_PRESS_MASK|
	     GDK_BUTTON_RELEASE_MASK);
}


// {{{ void LocalGTKGLArea::draw_impl()

void LocalGTKGLArea::draw_impl(GdkRectangle *p0) 
{ 
  if (VERBOSE) cerr << "Hello from LocalGTKGLArea::draw_impl" << endl;
  parent_ptr->DoDrawing();
}; 

// }}}



// {{{ gint LocalGTKGLArea::expose_event_impl()

gint LocalGTKGLArea::expose_event_impl(GdkEventExpose* event) 
{ 
  // Only handle the last event
  if (event->count>0) return (FALSE);
  if (VERBOSE) cerr << "Expose: width = " << width() 
		    << " height = " << height()
		    << endl;
  queue_draw();

  return (TRUE);
}; 

// }}}


// {{{ gint LocalGTKGLArea::motion_notify_event_impl(GdkEventMotion* event)

gint LocalGTKGLArea::motion_notify_event_impl(GdkEventMotion* event) { 
  if (VERBOSE) cerr << "*** motion_notify_event_impl" << endl;
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

  // Drags start with a click, so use that to find the start end
  parent_ptr->Dragged(last_x, last_y, x, y, b);

  last_x=int(event->x);
  last_y=int(event->y);

  return (TRUE);
}; 

// }}}
// {{{ gint LocalGTKGLArea::button_press_event_impl(GdkEventButton* event)

gint LocalGTKGLArea::button_press_event_impl(GdkEventButton* event) { 
  if (VERBOSE) cerr << "*** button_press_event_impl" << endl;
  last_x=int(event->x);
  last_y=int(event->y);

  // Note that the clicked even is not generated until the button
  // is released

  return (TRUE);
}; 

// }}}
// {{{ gint LocalGTKGLArea::button_release_event_impl(GdkEventButton* event)

gint LocalGTKGLArea::button_release_event_impl(GdkEventButton* event) { 
  if (VERBOSE) cerr << "*** button_release_event_impl" << endl;
  if (parent_ptr->Dragging()==true) {
    parent_ptr->DragRelease();
  } else {
    // If we weren't dragging, it's a straight click
    int b=int(event->button);
    parent_ptr->Clicked(last_x, last_y, b);
  }
  return (TRUE);
}; 

// }}}

ThreeDToolGTKGLWidget::ThreeDToolGTKGLWidget(const unsigned int camera_width, 
					     const unsigned int camera_height, 
					     const std::string t)
  : ThreeDToolGLCommon(camera_width, camera_height), 
    AspectFrame(0.5, 0.5, 
		float (camera_width) / float(camera_height)),
    localglarea(this), title(t)
{
  this->set_shadow_type(GTK_SHADOW_NONE);
  localglarea.set_usize(camera_width, camera_height);
  localglarea.size(camera_width, camera_height);

  set_border_width(0);
  add (localglarea);
}

void ThreeDToolGTKGLWidget::SwapBuffers()
{
  localglarea.swap_buffers();
}
 
void ThreeDToolGTKGLWidget::MakeCurrent()
{
  localglarea.make_current();
}

void ThreeDToolGTKGLWidget::RequestDraw()
{
  localglarea.queue_draw();
}

void ThreeDToolGTKGLWidget::_CallDrawEvent(bool b)
{
  // Call the VWEvents callback and the SigC one
  DrawEvent(b);
  DrawSignal.emit(b);
}
void ThreeDToolGTKGLWidget::_CallProcessHitsEvent(int item, double x, double y)
{
  // Simply call the callback
  ProcessHitsEvent(item,x,y);
  ProcessHitsSignal.emit(item,x,y);
}
