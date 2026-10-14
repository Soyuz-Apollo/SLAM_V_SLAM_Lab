/** 
 * @file   threedtoolglut.cpp
 * @author AJ Davison
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
// System includes

// Local includes
#include "threedtoolglut.h"


using namespace std;
using namespace VW;

#define VERBOSE false

  

ThreeDToolGLUT::
ThreeDToolGLUT(const unsigned int camera_width, 
		       const unsigned int camera_height,
		       const std::string title)
  : ThreeDToolGLCommon(camera_width, camera_height), 
    _width(camera_width), 
    _height(camera_height)
{
  // Open a window (function from parent class)
  _window_id = _CreateWindow( _width, _height, title );

}

void ThreeDToolGLUT::Resize(int newwidth, int newheight)
{
  _width = newwidth;
  _height = newheight;

  RequestDraw();
}

void ThreeDToolGLUT::Motion(int newx, int newy)
{
  Dragged(last_x, last_y, newx, newy, current_button + 1);
  last_x = newx;
  last_y = newy;
}

void ThreeDToolGLUT::Mouse(int button, int state, int x, int y)
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


void ThreeDToolGLUT::SwapBuffers()
{
  glutSetWindow(_window_id);
  glFlush();
  glutSwapBuffers();
}

void ThreeDToolGLUT::MakeCurrent()
{
  glutSetWindow(_window_id);
}

void ThreeDToolGLUT::RequestDraw()
{
  glutPostRedisplay();
}
