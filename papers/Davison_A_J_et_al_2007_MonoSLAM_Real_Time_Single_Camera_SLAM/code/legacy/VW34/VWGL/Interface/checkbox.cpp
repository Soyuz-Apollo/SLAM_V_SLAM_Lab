/** 
 * @file   checkbox.cpp
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
#include "checkbox.h"
#include "theme.h"

#define BOXSIZE 13
#define RAISE 7

using namespace std;

Glute2::CheckBox::CheckBox(const std::string& label)
  : _down(false), _text(label)
{
  _text_w = glutBitmapLength(GLUT_BITMAP_HELVETICA_12,(unsigned char*)(_text.c_str()));
  _minsize_x = _sw = _text_w + 10 + BOXSIZE;
  _minsize_y = _sh = 20; // Should really be determined by font, but how?

  // check box does not need to grow
  _wants_to_grow_x = false;
  _wants_to_grow_y = false;
}

Glute2::CheckBox::~CheckBox()
{
}
    
void Glute2::CheckBox::Draw()
{
  // Now draw
  glMatrixMode(GL_PROJECTION);

  SetPen(normal_background_colour);
  glRecti(_x,_y,_x+_w,_y+_h);
   

  // Now the colours
  Colour background;

  if(_focus) {
    background = focused_background_colour;
  } else {
    background = normal_background_colour;
  }

  SetPen(background);
  glRecti(_x+_sx,_y+_sy,_x+_sx+_sw,_y+_sy+_sh);
      

  // Now the box
  int RAISE2 = RAISE/2 + 1;
  // Background
  SetPen(highlight_colour);
  glRecti(_x+_sx,          _y+_sy + RAISE2,
	  _x+_sx + BOXSIZE,_y+_sy + RAISE2 + BOXSIZE);
      
  // Border
  SetPen(border_colour);
  glBegin(GL_LINE_LOOP); {
    glVertex2i(_x+_sx,          _y+_sy + RAISE2); 
    glVertex2i(_x+_sx + BOXSIZE,_y+_sy + RAISE2);
    glVertex2i(_x+_sx + BOXSIZE,_y+_sy + RAISE2 + BOXSIZE); 
    glVertex2i(_x+_sx,          _y+_sy + RAISE2 + BOXSIZE);
  }; glEnd();

  // shadow
  SetPen(light_shadow_colour);
  glBegin(GL_LINE_STRIP); {
    glVertex2i(_x+_sx+1,        _y+_sy + RAISE2+1); 
    glVertex2i(_x+_sx+1,        _y+_sy + RAISE2+BOXSIZE-1);
    glVertex2i(_x+_sx+BOXSIZE-1,_y+_sy + RAISE2+BOXSIZE-1); 
  }; glEnd();
  
  // Draw a tick
  if (_down) {
    const double point = double(BOXSIZE-2)/2.0;
    const double turn = double(BOXSIZE-2)/2.5;
    SetPen(Colour(0,0,0.8));
    glBegin(GL_POLYGON); {
      glVertex2f(_x+_sx+2,         _y+_sy + RAISE2+2+point); 
      glVertex2f(_x+_sx+2+turn,    _y+_sy + RAISE2+2+turn+0.5); 
     glVertex2f(_x+_sx+2+turn,     _y+_sy + RAISE2+2); 
    }; glEnd();
    glBegin(GL_POLYGON); {
      glVertex2f(_x+_sx+2+turn,    _y+_sy + RAISE2+2+turn+0.5); 
      glVertex2f(_x+_sx+BOXSIZE-1, _y+_sy + RAISE2+BOXSIZE-2); 
     glVertex2f(_x+_sx+2+turn,     _y+_sy + RAISE2+2); 
    }; glEnd();
  }

  // Now draw the text
  {
    glColor3f(0.0,0.0,0.0);
    glRasterPos2f(_x+_sx + BOXSIZE+5,
		  _y+_sy + RAISE);
    int len = _text.length();
    for(int i = 0; i < len; i++) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,_text[i]);
    }
  }
      


  _needs_redraw = false;	
}
    
void Glute2::CheckBox::_MousePressed(int x, int y, int button)
{
}
    
void Glute2::CheckBox::_MouseReleased(int startx, int starty, 
				   int endx, int endy, int button)
{
  _down = !_down;
  _needs_redraw = true;
  ToggledEvent(_down);
}
void Glute2::CheckBox::SetSize(unsigned int width, unsigned int height)
{
  // Border region grows to fit space
  _w = width;
  _h = height;
 
  // CheckBox relative position depends on text width and demanded width
  _sx = (_w - _sw) / 2;
  _sy = (_h - _sh) / 2;


}
