/** 
 * @file   window.cpp
 * @author BJ Tordoff & KR Connor
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

#include <iostream>
#include "window.h"

#define PADDING 4

using namespace std;

Glute2::Window::Window(int w, int h, const std::string& title)
  : _x(0), _y(0), _width(w), _height(h), _focused(false), _havechild(false),
    _clear_colour(255,0,0)
{
  _window_id = VW::GLUTWindow::_CreateWindow(w,h,title);
}
  
    
Glute2::Window::~Window()
{
}
  
  
void Glute2::Window::Position(int x, int y)
{
  _x = x;
  _y = y;
  glutSetWindow(_window_id);
  glutPositionWindow(x,y);
}


void Glute2::Window::Resize(int w, int h)
{
  // Check against child size
  _width = w;
  _height = h;

  if (_havechild) {
    const int minw = _childwidget->GetMinimumWidth();
    const int minh = _childwidget->GetMinimumHeight();
    if (minw>(w-PADDING)) _width = minw+PADDING;
    if (minh>(h-PADDING)) _height = minh+PADDING;
  }
  glutSetWindow(_window_id);
  glutReshapeWindow(_width,_height);
  if (_havechild) {
    _childwidget->SetSize(_width-PADDING,_height-PADDING);
  }
  Draw();
}
  

void Glute2::Window::FullScreen()
{
  glutSetWindow(_window_id);
  glutFullScreen();
}
  

void Glute2::Window::Show()
{
  glutSetWindow(_window_id);
  glutShowWindow();
  Draw();
}


void Glute2::Window::Hide()
{
  glutSetWindow(_window_id);
  glutHideWindow();
}


void Glute2::Window::Iconify()
{
  glutSetWindow(_window_id);
  glutIconifyWindow();
}


void Glute2::Window::Push()
{
  glutSetWindow(_window_id);
  glutPushWindow();
}
  

void Glute2::Window::Pop()
{
  glutSetWindow(_window_id);
  glutPopWindow();
}
  

void Glute2::Window::SetClearColour(float r, float g, float b, float a)
{
  _clear_colour.Set(r,g,b,a);
}


void Glute2::Window::SetCursor(int cursor)
{
  glutSetCursor(cursor);
}
  

void Glute2::Window::Redisplay()
{
  glutPostWindowRedisplay(_window_id);
}


void Glute2::Window::MakeCurrent()
{
  // Just use parent's function
  VW::GLUTWindow::_MakeCurrent();
}
  
  
void Glute2::Window::SwapBuffers()
{
  glutSwapBuffers();
}
  
void Glute2::Window::Flush()
{
  glutSwapBuffers();
}
  
void Glute2::Window::Motion(int newx, int newy)
{
  _dragging = true;
  if (_focused) {
    _childwidget->_Drag(_last_x, _last_y, 
			newx, newy, _current_button + 1);
    // Finally check for redrawing
    if(_childwidget->_NeedsRedraw()) {
      //std::cout << "redraw<<" << std::endl;
      MakeCurrent();
      _childwidget->Draw();
      SwapBuffers();
    }
  }
}

void Glute2::Window::Mouse(int button, int state, int x, int y)
{
  if (_focused) {
  if (state == 0) {
    // Press
    _last_x = x;
    _last_y = y;
    _current_button = button;
    _dragging=false; // Not dragging unless motion between click and release
    //std::cerr << "Glute2::Window::MousePress ("<<_havechild<<")" << std::endl;
    if (_havechild) {
      _childwidget->_MousePressed(_last_x, _last_y, button + 1);
    }
  }
  else if (state == 1) {
    // Release
    if (_dragging) {
      if (_havechild) _childwidget->_MouseReleased(_last_x, _last_y, x, y, _current_button + 1);
      _dragging=false;
    }
    else {
      // Glut uses 0, 1, 2 for buttons; ThreeDToolGLWidget (and Gtk)
      // use 1, 2, 3
      if (_havechild) _childwidget->_MouseReleased(_last_x, _last_y, _last_x, _last_y, button + 1);
      //std::cerr << "Glute2::Window::MouseRelease ("<<_havechild<<")" << std::endl;
    }
  }

  // Finally check for redrawing
  if(_childwidget->_NeedsRedraw()) {
    //std::cout << "redraw<<" << std::endl;
    MakeCurrent();
    _childwidget->Draw();
    SwapBuffers();
  }
  }
}


void Glute2::Window::PassiveMotion(int x, int y)
{
  if (_havechild) {
    if(_childwidget->_IsIn(x,y)) {
      if (!_focused) {
	_childwidget->_SetFocus(true);
	_focused = true;
      }
    } else {
      if (_focused) {
	_childwidget->_SetFocus(false);
	_focused = false;
      }
    }

    // Pass the position to the focused widget
    if (_focused) _childwidget->_PassiveMotion(x,_height - y);
   
    if(_childwidget->_NeedsRedraw()) {
      MakeCurrent();
      _childwidget->Draw();
      SwapBuffers();
    }
  } else {
    _focused = false;
  }
}


void Glute2::Window::Entry(int state)
{
  glutEntryFunc(0);
}
  
  
void Glute2::Window::Keyboard(unsigned char key, int x, int y)
{
  if(_focused && _havechild)
    _childwidget->_KeyPress(x,y,key);
}


void Glute2::Window::Special(int key, int x, int y)
{
  glutSpecialFunc(0);
}
  

void Glute2::Window::Draw()
{
  //std::cout << "Draw" << std::endl;

  // Pre-drawing

  MakeCurrent();
  _clear_colour.MakeCurrentClear();
  glClear(GL_COLOR_BUFFER_BIT);

  // Drawing
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, _width, 0, _height);
  glViewport(0,0,_width,_height);
		
  glColor3f(0.5, 0.5, 0.5);
  glBegin(GL_LINE_LOOP);
  glVertex2i(0,0); glVertex2i(_width-1,0);
  glVertex2i(_width-1,_height-1); glVertex2i(0,_height-1);
  glEnd();
  glColor3f(0.7, 0.7, 0.7);
  glBegin(GL_LINE_LOOP);
  glVertex2i(1,1); glVertex2i(_width-2,1);
  glVertex2i(_width-2,_height-2); glVertex2i(1,_height-2);
  glEnd();
	
  if (_havechild) _childwidget->Draw();

  // Post-drawing
  SwapBuffers();
}

 
  
int Glute2::Window::GetWindowID() const
{
  return _window_id;
}
  
  
void Glute2::Window::Add(Widget* child)
{
  if (_havechild)
    std::cerr << "Glute2::Window::AddChild: This window already has a child!" << std::endl;
  else {
    _childwidget = child;
    // Set the child to fill the interior
    //_childwidget->_SetWindow(this);


    // Check the sizes
    const int minw = _childwidget->GetMinimumWidth();
    const int minh = _childwidget->GetMinimumHeight();

    bool resize = false;
    if (minw > (_width-PADDING)) {
      _width = minw + PADDING;
      resize = true;
    } 
    if (minh > (_height-PADDING)) {
      _height = minh + PADDING;
      resize = true;
    } 
    
    if (resize) {
      this->Resize(_width, _height);
    }

      _childwidget->SetPosition(2,2);
      
    _childwidget->SetSize(_width-PADDING,_height-PADDING);
    _childwidget->SetParentWindow(this);
    _havechild = true;
  }
}
  
