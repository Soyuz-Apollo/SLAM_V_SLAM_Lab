/**
@file   threedtoolqglwidget.h
@author PA Smith
@date   25/11/2005

(c) Copyright University of Oxford

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef THREEDTOOLQGLWIDGET_H
#define THREEDTOOLQGLWIDGET_H

#include <qgl.h>
#include <VWGL/Display/threedtoolglcommon.h>

namespace VW
{

/**
 @ingroup Displays
 @author PA Smith
 @date 25/11/2005
 @brief A version of ThreeDToolGLCommon which is also a Qt widget.
*/
class ThreeDToolQGLWidget : public QGLWidget, public VW::ThreeDToolGLCommon
{
public:
  ThreeDToolQGLWidget(
    QWidget* parent, const char* name = "ThreeDToolQGLWidget");
  ThreeDToolQGLWidget(
    QWidget* parent, unsigned int width, unsigned int height,
    const char* name = "ThreeDToolQGLWidget");

  ~ThreeDToolQGLWidget () {}; ///< Destructor
	
public:
  /** Returns the current width of the window. Used by the ThreeDToolGLCommon
  framework to set the GL viewing parameters. */
  unsigned int WindowWidth() const {return width();}
  /** Returns the current height of the window. Used by the ThreeDToolGLCommon
  framework to set the GL viewing parameters. */
  unsigned int WindowHeight() const {return height();}
  /** Swap the front and back buffers (by calling QGLWidget::swapBuffers()). */
  void SwapBuffers() {swapBuffers();}
  /** Make this window the current GL context (by
  calling QGLWidget::makeCurrent())*/
  void MakeCurrent() {makeCurrent();}
  /** Request a redraw of this window (by
  calling QWidget::update(), which places a request into the event queue.)
  Multiple calls of this function usually only result in one redraw.*/
  void RequestDraw() {update();}
    
protected:
  /** Slot handler for Qt signal, called when the widget needs redrawing. This
  just calls ThreeDToolGLCommon::DoDrawing(). */
  virtual void paintGL() {DoDrawing();}
  virtual void mousePressEvent(QMouseEvent* e);
  virtual void mouseMoveEvent(QMouseEvent* e);
  virtual void mouseReleaseEvent(QMouseEvent* e);
  
private:
  int ButtonNumber(QMouseEvent* e);
  
private:
  int button;
  int prev_drag_x;
  int prev_drag_y;
};

//////////////////////////////////
// INLINE FUNCTIONS
	
/**
Construct a ThreeDToolQGLWidget in the standard QWidget manner. This
initialises the virtual camera to the default settings of ThreeDToolGLCommon,
so you will most likely want to later call ThreeDToolGLCommon::SetCameraSize()
and ThreeDToolGLCommon::SetCameraParameters() to set the correct virtual camera.
\param parent The parent widget. If set to 0 the widget will be displayed on
its own in a new window; this is only useful if you have derived from this
class to provide a drawing function
\param name The name of the widget (and the title if used as a window)
*/
inline ThreeDToolQGLWidget::ThreeDToolQGLWidget(
  QWidget* parent, const char* name) :
  QGLWidget(parent, name),
  ThreeDToolGLCommon(),
  button(0),
  prev_drag_x(-1),
  prev_drag_y(-1)
{
  setCaption(QString (name));
  resize(CameraWidth(), CameraHeight());
}

/**
Construct a ThreeDToolQGLWidget in the standard QWidget manner, also setting
the size of the virtual camera (this is not necessarily the same as the window
size - see the documentation for ThreeDToolGLCommon).
\param parent The parent widget. If set to 0 the widget will be displayed on
its own in a new window; this is only useful if you have derived from this
class to provide a drawing function
\param width The width of the virtual camera
\param height The height of the virtual camera
\param name The name of the widget (and the title if used as a window)
*/
inline ThreeDToolQGLWidget::ThreeDToolQGLWidget(
  QWidget* parent, unsigned int width, unsigned int height, const char* name) :
    QGLWidget(parent, name),
    ThreeDToolGLCommon(width, height),
    button(0),
    prev_drag_x(-1),
    prev_drag_y(-1)
{
  setCaption(QString (name));
  resize(width, height);
}

/**
Slot handler for Qt signal, called when the user presses a mouse button.
This function stores the button and pixel location to later pass onto the
ThreeDToolGLCommon framework.
*/
inline void ThreeDToolQGLWidget::mousePressEvent(QMouseEvent* e)
{
  button = ButtonNumber(e);
  prev_drag_x = e->x();
  prev_drag_y = e->y();
}

/**
Slot handler for Qt signal, called whenever the mouse
is moved <em>with a button pressed</em>. This function
passes this onto the ThreeDToolGLCommon framework by calling
ThreeDToolGLCommon::Dragged().
**/
inline void ThreeDToolQGLWidget::mouseMoveEvent(QMouseEvent* e)
{
  Dragged(prev_drag_x, prev_drag_y, e->x(), e->y(), button);
  prev_drag_x = e->x();
  prev_drag_y = e->y();
}

/**
Slot handler for Qt signal, called whenever the user releases a mouse
button. Calls ThreeDToolGLCommon::DrawRelease() or
ThreeDToolGLCommon::Clicked() as appropriate.
**/
inline void ThreeDToolQGLWidget::mouseReleaseEvent(QMouseEvent* e)
{
  if(Dragging())
  {
    DragRelease();
  }
  else
  {
    Clicked(e->x(), e->y(), button);
  }
}

/**
Internal function to convert the Qt ButtonState enum into a GTK-style number for
passing to VW::ThreeDToolGLCommon. (Left=1, Middle=2, Right=3).
**/
inline int ThreeDToolQGLWidget::ButtonNumber(QMouseEvent* e)
{
  int button = 0;
  switch(e->button())
  {
    case LeftButton:
      button = 1;
      break;
    case MidButton:
      button = 2;
      break;
    case RightButton:
      button = 3;
      break;
    case NoButton:
      button = 4;
      break;
    default:
      break;
  }
  return button;
}

} // namespace VW

#endif // THREEDTOOLQGLWIDGET_H


