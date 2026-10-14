/** 
 * @file   threedtoolglutewidget.cpp
 * @author BJ Tordoff
 * @date   03/12/2003
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
#ifdef _WIN32
#include <windows.h>
#endif

// Local includes
#include "threedtoolglutewidget.h"
#include "window.h"


using namespace std;
using namespace VW;

#define VERBOSE false

  

ThreeDToolGluteWidget::
ThreeDToolGluteWidget(const unsigned int camera_width, 
		       const unsigned int camera_height,
		       const std::string title)
  : ThreeDToolGLCommon(camera_width, camera_height),
    _initialised(false)
{
  _w = _minsize_x = camera_width;
  _h = _minsize_y = camera_height;
  _wants_to_grow_x = true;
  _wants_to_grow_y = true;
}

void ThreeDToolGluteWidget::SetSize(const unsigned int newwidth, 
				    const unsigned int newheight)
{
  cout << "ThreeDToolGluteWidget::SetSize(" << newwidth << "x" << newheight << ")" << endl;
  _w = newwidth;
  _h = newheight;

  if (!_initialised) {
    // Init at new size
    
    if (_minsize_x == 0) _minsize_x = _w;
    if (_minsize_y == 0) _minsize_y = _h;
    
    // Open a window (function from parent class)
    if (_parent_window != 0) {
      cout << "Create new subwindow on parent " << _parent_window << " at ("<<_x<<","<<_y<<") with size ("<<_w<<"x"<<_h<<")"<<endl;
      _CreateSubwindow(_parent_window->GetWindowID(), 
		       _x, _parent_window->GetHeight() - _h - _y,
		       _w, _h);


      cout << "Set viewport" << endl;
      MakeCurrent();
      glViewport(0, 0, _w, _h);
      cout << "Set GL view" << endl;
      _initialised = true;
      cout << "Done" << endl;
    }
  } else {
    // Already initialised, so resize
    Resize(_w,_h);
  }
  _sw = _w;
  _sh = _h;

}

void ThreeDToolGluteWidget::Resize(int newwidth, int newheight)
{
  _w = newwidth;
  _h = newheight;
  glutSetWindow(GetID());
  glutPositionWindow(_x,_parent_window->GetHeight() - _h - _y);
  glutReshapeWindow(_w,_h);
  glutSetWindow(_parent_window->GetWindowID());

  cout << "Set to ("<<_w<<"x"<<_h<<") at ("<<_x<<"x"<<_y<<")" << endl;

  RequestDraw();
}

void ThreeDToolGluteWidget::Motion(int newx, int newy)
{
  Dragged(last_x, last_y, newx, newy, current_button + 1);
  last_x = newx;
  last_y = newy;
}

void ThreeDToolGluteWidget::Mouse(int button, int state, int x, int y)
{
  if (state == 0) {
    // Click
    last_x = x;
    last_y = y;
    current_button = button;
  }
  else if (state == 1) {
    // Release
    if (Dragging()==true) {
      DragRelease();
    }
    else {
      // Glut uses 0, 1, 2 for buttons; ThreeDTool (and Gtk)
      // use 1, 2, 3
      Clicked(last_x, last_y, button + 1);
    }
  }
}


void ThreeDToolGluteWidget::SwapBuffers()
{
  glutSetWindow(GetID());
  glFlush();
  glutSwapBuffers();
  glutSetWindow(_parent_window->GetWindowID());
}

void ThreeDToolGluteWidget::MakeCurrent()
{
  glutSetWindow(GetID());
}

void ThreeDToolGluteWidget::RequestDraw()
{
  glutSetWindow(GetID());
  glutPostRedisplay();
  glutSetWindow(_parent_window->GetWindowID());
}

unsigned int ThreeDToolGluteWidget::WindowWidth() const
{
  return _w;
}

unsigned int ThreeDToolGluteWidget::WindowHeight() const
{
  return _h;
}
