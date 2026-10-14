/** 
 * @file   container.cpp
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


//#include <iostream>
#include <GL/glut.h>
#include "container.h"

//using namespace std;

Glute2::Container::Container()
  : _focused(-1)
{
}


void Glute2::Container::SetSize(unsigned int width, unsigned int height)
{
  // Border region grows to fit space
  _w = width;
  _h = height;

  // Sensitive area is also whole space
  _sx = 0;
  _sy = 0;

  _sw = _w;
  _sh = _h;

  _RecalcSizes();

}
    
void Glute2::Container::Draw()
{
//   glMatrixMode(GL_PROJECTION);
// //   glLoadIdentity();
// //   gluOrtho2D(0, _w, 0, _h);
// //   glViewport(0,0,_w,_h);

// // Background
// if(_focus)
//     glColor3f(0.9, 0.9, 0.9);
//   else
//     glColor3f(0.85, 0.85, 0.85);
//   glRecti(_x,_y,_x+_w,_y+_h);

//   // Border
//   glColor3f(0.5, 0.5, 0.5);
//   glBegin(GL_LINE_LOOP);
//   glVertex2i(_x,_y); glVertex2i(_x+_w-1,_y);
//   glVertex2i(_x+_w-1,_y+_h-1); glVertex2i(_x,_y+_h-1);
//   glEnd();
//   glColor3f(0.7, 0.7, 0.7);
//   glBegin(GL_LINE_LOOP);
//   glVertex2i(_x+1,_y+1); glVertex2i(_x+_w-2,_y+1);
//   glVertex2i(_x+_w-2,_y+_h-2); glVertex2i(_x+1,_y+_h-2);
//   glEnd();
	
  //cout << "Draw "<<_widgets.size()<<" widgets" << endl;
  for(unsigned int i = 0; i < _widgets.size(); i++) {
    _widgets[i]->Draw();
  }
}
inline void Glute2::Container::SetParentWindow(Glute2::Window* parent_window)
{
  _parent_window = parent_window;
   for(unsigned int i = 0; i < _widgets.size(); i++) {
    _widgets[i]->SetParentWindow(parent_window);
  } 
}

void Glute2::Container::_PushBack(Widget* w)
{
  w->SetParentWindow(_parent_window);
  _widgets.push_back(w);
}

void Glute2::Container::_PushFront(Widget* w)
{
  w->SetParentWindow(_parent_window);
  _widgets.push_front(w);
}
    
void Glute2::Container::_PassiveMotion(int x, int y)
{
  //cout << "Container::_PassiveMotion("<<x<<","<<y<<")"<<endl;
  int num = _widgets.size();
  int i = 0;
  for(; i < num; i++) {
    if(_widgets[i]->_IsIn(x,y)) {
      if(i != _focused) {
	if(_focused != -1) {
	  _widgets[_focused]->_SetFocus(false);
	}
	_widgets[i]->_SetFocus(true);
	_focused = i;
	_needs_redraw = true;
	//cout << "Container::_PassiveMotion: Set focus to widget "<<i<<endl;
     }
      break;
    }
  }
  if(i == num && _focused != -1) {
    _widgets[_focused]->_SetFocus(false);
    _focused = -1;
    _needs_redraw = true;
    //cout << "Container::_PassiveMotion: Lost focus"<<endl;
  }

  if (_focused != -1) _widgets[_focused]->_PassiveMotion(x,y);


}
void Glute2::Container::_MousePressed(int x, int y, int button)
{
  if (_focused != -1) {
    _widgets[_focused]->_MousePressed(x, y, button);
    _needs_redraw = _widgets[_focused]->_NeedsRedraw();
  }
}
    
void Glute2::Container::_MouseReleased(int startx, int starty, 
				   int endx, int endy, int button)
{
  if (_focused != -1) {
    _widgets[_focused]->_MouseReleased(startx,starty,endx, endy, button);
    _needs_redraw = _widgets[_focused]->_NeedsRedraw();
  }
}

void Glute2::Container::_Drag(int startx, int starty, 
				   int endx, int endy, int button)
{
  if (_focused != -1) {
    _widgets[_focused]->_Drag(startx,starty,endx, endy, button);
    _needs_redraw = _widgets[_focused]->_NeedsRedraw();
  }
}
