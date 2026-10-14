/** 
 * @file   button.cpp
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

#include <GL/glut.h>
#include <iostream>
#include "button.h"
#include "theme.h"

using namespace std;

Glute2::Button::Button(const std::string& label)
  : _down(false), _text(label)
{
  _text_w = glutBitmapLength(GLUT_BITMAP_HELVETICA_12,(unsigned char*)(_text.c_str()));
  _minsize_x = _text_w + 20;
  _sw = _text_w + 10;
  _minsize_y = _sh = 20; // Should really be determined by font, but how?

  // Button does not need to grow
  _wants_to_grow_x = false;
  _wants_to_grow_y = false;

}

Glute2::Button::~Button()
{
}
    
void Glute2::Button::Draw()
{
  // Now draw
  glMatrixMode(GL_PROJECTION);

  SetPen(normal_background_colour);
  glRecti(_x,_y,_x+_w,_y+_h);
   

  // Now the colours
  Colour background,highlight,shadow;

  if(_down) {
    background = shadow_colour;
    shadow = highlight_colour;
    highlight = dark_shadow_colour;
  } else if(_focus) {
    background = focused_background_colour;
    shadow = shadow_colour;
    highlight = highlight_colour;
  } else {
    background = normal_background_colour;
    shadow = shadow_colour;
    highlight = highlight_colour;
  }

  //cout << "Draw rect ("<<_sx<<","<<_sy<<") to ("<<_sx+_sw<<","<<_sy+_sh<<")"<<endl;
  SetPen(background);
  glRecti(_x+_sx,_y+_sy,_x+_sx+_sw,_y+_sy+_sh);
      
  {
    glColor3f(0.0,0.0,0.0);
    glRasterPos2f(_x+_sx + (_sw-_text_w)/2.0,
		  _y+_sy + 7.5);
    int len = _text.length();
    for(int i = 0; i < len; i++) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,_text[i]);
    }
  }
      
  // Border
  SetPen(border_colour);
  glBegin(GL_LINE_LOOP); {
    glVertex2i(_x+_sx,      _y+_sy); 
    glVertex2i(_x+_sx + _sw,_y+_sy);
    glVertex2i(_x+_sx + _sw,_y+_sy + _sh); 
    glVertex2i(_x+_sx,      _y+_sy + _sh);
  }; glEnd();

  // Highlight/shadow
  SetPen(highlight);
  glBegin(GL_LINE_STRIP); {
    glVertex2i(_x+_sx+1,    _y+_sy+2); 
    glVertex2i(_x+_sx+1,    _y+_sy+_sh-1);
    glVertex2i(_x+_sx+_sw-1,_y+_sy+_sh-1); 
  }; glEnd();
  SetPen(shadow);
  glBegin(GL_LINE_STRIP); {
    glVertex2i(_x+_sx+2,    _y+_sy+1); 
    glVertex2i(_x+_sx+_sw-1,_y+_sy+1);
    glVertex2i(_x+_sx+_sw-1,_y+_sy+_sh-1); 
  }; glEnd();
  

  _needs_redraw = false;	
}
    
void Glute2::Button::_MousePressed(int x, int y, int button)
{
  //cout << "Button \"" << _text << "\" pressed" << endl;
  _down = true;
  _needs_redraw = true;

}
    
void Glute2::Button::_MouseReleased(int startx, int starty, 
				   int endx, int endy, int button)
{
  //cout << "Button \"" << _text << "\" released" << endl;
  _down = false;
  _needs_redraw = true;
  PressedEvent();
}
void Glute2::Button::SetSize(unsigned int width, unsigned int height)
{
  cout << "Button: Resize to size ("<<_w<<"x"<<_h<<") at pos ("<<_x<<","<<_y<<")" << endl;
  // Border region grows to fit space
  _w = width;
  _h = height;
 
  // Button relative position depends on text width and demanded width
  _sx = (_w - _sw) / 2;
  _sy = (_h - _sh) / 2;


}
