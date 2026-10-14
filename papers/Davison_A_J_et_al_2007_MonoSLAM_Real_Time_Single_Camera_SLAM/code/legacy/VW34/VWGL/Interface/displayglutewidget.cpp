/** 
 * @file   displayglutewidget.cpp
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
#include "displayglutewidget.h"
#include "window.h"

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE true


using namespace std;

VW::DisplayGluteWidget::DisplayGluteWidget(const unsigned int width, 
					   const unsigned int height, 
					   const string title) 
  : DisplayGLCommon (width, height, title)
{
  if (VERBOSE) { 
    cerr << "New display ";
    if (title.length() == 0) cerr << "(unnamed) ";
    else cerr << "`" << title << "' ";
    cerr << "(" << _w << "x" << _h << ")" << endl;
  }

  _minsize_x = width;
  _minsize_y = height;
  _wants_to_grow_x = true;
  _wants_to_grow_y = true;

  // Open a window (function from parent class)
  //  cout << "Create new subwindow" << endl;
  //  _window_id = glutCreateSubWindow(_parent_window_id, _x, _y,_w, _h);
  //_CreateWindow( _width, _height, title.c_str() );

  // Must set init flag to false explicitly since base class
  // sets it to true
  _visible = true;
  _initialised = false;
  _needs_redraw = false;
  cout << "Done" << endl;
};

VW::DisplayGluteWidget::DisplayGluteWidget(const string title) 
  : DisplayGLCommon (50, 50, title)
{
  _wants_to_grow_x = true;
  _wants_to_grow_y = true;

  if (VERBOSE) {
    cerr << "New display ";
    if (title.length() == 0) cerr << "(unnamed)" << endl;
    else cerr << "`" << title << "'" << endl;
  }  

  // Must set visible flag to false explicitly since base class
  // sets it to true
  _visible = false;
  _initialised = false;
  _needs_redraw = false;
}



void VW::DisplayGluteWidget::SetSize(const unsigned int newwidth, 
				     const unsigned int newheight)
{
  cout << "DisplayGluteWidget::SetSize(" << newwidth << "x" << newheight << ")" << endl;
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
      _MakeCurrent();
      glViewport(0, 0, _w, _h);
      cout << "Set GL view" << endl;
      SetGLView();
      ResetCoords(_w, _h);
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

void VW::DisplayGluteWidget::Resize(int newwidth, int newheight)
{
  cout << "Resize to size ("<<_w<<"x"<<_h<<") at pos ("<<_x<<","<<_y<<")" << endl;
  _w = newwidth;
  _h = newheight;
  _sw = _w;
  _sh = _h;
   _MakeCurrent();
      glViewport(0, 0, _w, _h);
  glutPositionWindow(_x,_parent_window->GetHeight() - _h - _y);
  glutReshapeWindow(_w,_h);
  
  SetGLView();
  glutSetWindow(_parent_window->GetWindowID());
}

void VW::DisplayGluteWidget::Motion(int newx, int newy)
{
  _dragging = true;
  DragEvent(_last_x, _last_y, newx, newy, _current_button + 1);
}

void VW::DisplayGluteWidget::Mouse(int button, int state, int x, int y)
{
  if (state == 0) {
    // Click
    _last_x = x;
    _last_y = y;
    _current_button = button;
    _dragging=false; // Not dragging unless motion between click and release
  }
  else if (state == 1) {
    // Release
    if (_dragging) {
      if (VERBOSE) cerr << "VW::DisplayGluteWidget::Mouse: Done dragging" << endl;
      DragFinishedEvent(_last_x, _last_y, x, y, _current_button + 1);
      _dragging=false;
    }
    else {
      if (VERBOSE) cerr << "VW::DisplayGluteWidget::Mouse: Got a click" << endl;
      // Glut uses 0, 1, 2 for buttons; ThreeDToolGLWidget (and Gtk)
      // use 1, 2, 3
      ClickEvent(_last_x, _last_y, button + 1);
    }
  }
}
void VW::DisplayGluteWidget::RequestDraw()
{
  _needs_redraw = true;
}
void VW::DisplayGluteWidget::Draw()
{
  if (_needs_redraw) {
    if (VERBOSE) cerr << "VW::DisplayGluteWidget::Draw()" << endl;
    _needs_redraw = false;
    DrawEvent(true);
  }
}

void VW::DisplayGluteWidget::SwapBuffers()
{
   _MakeCurrent();
   glFlush();
   glutSwapBuffers();
}
void VW::DisplayGluteWidget::MakeCurrent()
{
   _MakeCurrent();
}
