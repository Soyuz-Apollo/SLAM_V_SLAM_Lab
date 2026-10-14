/** 
 * @file   displayglut.cpp
 * @author BJ Tordoff
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
#include "displayglut.h"

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false


using namespace std;

VW::DisplayGLUT::DisplayGLUT(const unsigned int width, 
			 const unsigned int height, 
			 const string title) 
  : DisplayGLCommon (width, height, title),
    _width(width),
    _height(height)
{
  if (VERBOSE) { 
    cerr << "New display ";
    if (title.length() == 0) cerr << "(unnamed) ";
    else cerr << "`" << title << "' ";
    cerr << "(" << _width << "x" << _height << ")" << endl;
  }

  // Open a window (function from parent class)
  _window_id = _CreateWindow( _width, _height, title.c_str() );

  // Must set init flag to false explicitly since base class
  // sets it to true
  _visible = true;
  _initialised = false;
};

VW::DisplayGLUT::DisplayGLUT(const string title) 
  : DisplayGLCommon (50, 50, title)
{
  if (VERBOSE) {
    cerr << "New display ";
    if (title.length() == 0) cerr << "(unnamed)" << endl;
    else cerr << "`" << title << "'" << endl;
  }  

  // Must set visible flag to false explicitly since base class
  // sets it to true
  _visible = false;
  _initialised = false;
}




void VW::DisplayGLUT::SetSize(const unsigned int newwidth, const unsigned int newheight)
{
  assert(newwidth>0);
  assert(newheight>0);
  _width = newwidth;
  _height = newheight;

  // Open a window (function from parent class)
  _window_id = _CreateWindow( _width, _height, this->GetTitle() );

  MakeCurrent();
  glViewport(0, 0, _width, _height);
  SetGLView();
  ResetCoords(_width, _height);
}

void VW::DisplayGLUT::Resize(int newwidth, int newheight)
{
  _width = newwidth;
  _height = newheight; 

  MakeCurrent();
  glViewport(0, 0, _width, _height);
  SetGLView();
}

void VW::DisplayGLUT::Motion(int newx, int newy)
{
  _dragging = true;
  DragEvent(_last_x, _last_y, newx, newy, _current_button + 1);
}

void VW::DisplayGLUT::Mouse(int button, int state, int x, int y)
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
      if (VERBOSE) cerr << "VW::DisplayGLUT::Mouse: Done dragging" << endl;
      DragFinishedEvent(_last_x, _last_y, x, y, _current_button + 1);
      _dragging=false;
    }
    else {
      if (VERBOSE) cerr << "VW::DisplayGLUT::Mouse: Got a click" << endl;
      // Glut uses 0, 1, 2 for buttons; ThreeDToolGLWidget (and Gtk)
      // use 1, 2, 3
      ClickEvent(_last_x, _last_y, button + 1);
    }
  }
}
void VW::DisplayGLUT::RequestDraw()
{
  glutPostRedisplay();
}
void VW::DisplayGLUT::Draw()
{
  if (VERBOSE) cerr << "VW::DisplayGLUT::Draw()" << endl;
  DrawEvent(true);
}

void VW::DisplayGLUT::SwapBuffers()
{
  glutSetWindow(_window_id);
  glFlush();
  glutSwapBuffers();
}
void VW::DisplayGLUT::MakeCurrent()
{
  if (!_initialised) InitGL();
  glutSetWindow(_window_id);
}
