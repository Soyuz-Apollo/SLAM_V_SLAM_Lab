/** 
 * @file   slider.cpp
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
#include <GL/glut.h>
#include <iostream>
#include "window.h"
#include "slider.h"
#include "theme.h"

using namespace std;

// Size of button
#define BUTW 30
#define BUTH 14

Glute2::Slider::Slider(const std::string& label,
		       double min_val,
		       double max_val,
		       double default_val,
		       const std::string& numberformat)
  : _down(false), 
    _curr_val(default_val), _min_val(min_val), _max_val(max_val),
    _label(label),_format(numberformat)
{

  _FindMaxValStringLength();
  _UpdateValString();

  _label_w = glutBitmapLength(GLUT_BITMAP_HELVETICA_12,(unsigned char*)(_label.c_str()));

  _slider_w = _sw = 100;
  _sx = _label_w + 5;
  _minsize_x = _label_w + _val_w + _slider_w + 10;
  _minsize_y = _sh = 20; // Should really be determined by font, but how?

  // slider would like to grow in x but not y
  _wants_to_grow_x = true;
  _wants_to_grow_y = false;
}

Glute2::Slider::~Slider()
{
}
 
void Glute2::Slider::_FindMaxValStringLength()
{
  char buffer[256];
  sprintf(buffer,_format.c_str(),_max_val);
  int maxsize = glutBitmapLength(GLUT_BITMAP_HELVETICA_12,(unsigned char*)(buffer));
  sprintf(buffer,_format.c_str(),_min_val);
  int minsize = glutBitmapLength(GLUT_BITMAP_HELVETICA_12,(unsigned char*)(buffer));

  if (maxsize>minsize) {
    _val_w = maxsize;
  } else {
    _val_w = minsize;
  }
  
}
   
void Glute2::Slider::_UpdateValString()
{
  char buffer[256];
  sprintf(buffer,_format.c_str(),_curr_val);
  _valstring = std::string(buffer);
}
   
void Glute2::Slider::Draw()
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
    highlight = shadow_colour;
  } else if(_focus) {
    background = focused_background_colour;
    shadow = shadow_colour;
    highlight = highlight_colour;
  } else {
    background = normal_background_colour;
    shadow = shadow_colour;
    highlight = highlight_colour;
  }

  {
    glColor3f(0.0,0.0,0.0);
    glRasterPos2f(_x, _y + _sy + 7.5);
    int len = _label.length();
    for(int i = 0; i < len; i++) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,_label[i]);
    }
    glRasterPos2f(_x+_label_w+_slider_w+10, _y + _sy + 7.5);
    len = _valstring.length();
    for(int i = 0; i < len; i++) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,_valstring[i]);
    }
  }


  // Local coords
  const unsigned int lx0 = _x+_sx+int(0.4*BUTW);
  const unsigned int lx1 = _x+_sx+_sw-int(0.4*BUTW);
  const unsigned int ly0 = _y+_sy;
  const unsigned int ly1 = _y+_sy+_sh;

  //Draw runner
  glColor3f(0.75,0.75,0.75); // Background
  glRecti(lx0,ly0+8,lx1,ly0+12);
  glColor3f(0.55,0.55,0.55); // Border
  glBegin(GL_LINE_LOOP); {
    glVertex2i(lx0,ly0+8); 
    glVertex2i(lx1,ly0+8);
    glVertex2i(lx1,ly0+12); 
    glVertex2i(lx0,ly0+12);
  }; glEnd();
  glColor3f(0.65,0.65,0.65); // Shadow
  glBegin(GL_LINE_STRIP); {
    glVertex2i(lx0 + 1,ly0+9); 
    glVertex2i(lx0 + 1,ly0+11); 
    glVertex2i(lx1 - 1,ly0+11);
  }; glEnd();

  // Draw button
  const double frac = (_curr_val - _min_val)/(_max_val - _min_val);
  const int vx = int(double(_sw-BUTW) * frac);
  // Local coords
  const unsigned int bx0 = _x+_sx+vx;
  const unsigned int bxc = bx0+int(0.5*BUTW);
  const unsigned int bx1 = bx0+BUTW;
  const unsigned int by0 = _y+_sy+int(0.5*(_sh-BUTH));
  const unsigned int by1 = by0 + BUTH;
  const unsigned int byc = by0 + int(0.5*BUTH);
  // Background
  SetPen(background);
  glRecti(bx0+1,by0+1,bx1-1,by1-1);
  // Highlight/shadow
  SetPen(highlight);
  glBegin(GL_LINE_STRIP); {
    glVertex2i(bx0+1,by0+1); 
    glVertex2i(bx0+1,by1-1);
    glVertex2i(bx1-1,by1-1); 
  }; glEnd();
  SetPen(shadow);
  glBegin(GL_LINE_STRIP); {
    glVertex2i(bx0+1,by0+1); 
    glVertex2i(bx1-1,by0+1);
    glVertex2i(bx1-1,by1-1); 
  }; glEnd();
  // Border
  SetPen(border_colour);
  glBegin(GL_LINE_LOOP); {
    glVertex2i(bx0+2,by0); 
    glVertex2i(bx1-2,by0);
    glVertex2i(bx1,by0+2);
    glVertex2i(bx1,by1-2); 
    glVertex2i(bx1-2,by1); 
    glVertex2i(bx0+2,by1);
    glVertex2i(bx0,by1-2);
    glVertex2i(bx0,by0+2);
  }; glEnd();
  // Stripes
  SetPen(dark_shadow_colour);
  glBegin(GL_LINE_STRIP); {
    glVertex2i(bxc-3,byc-3); 
    glVertex2i(bxc+4,byc+4);
  }; glEnd();
  glBegin(GL_LINE_STRIP); {
    glVertex2i(bxc-5,byc-1); 
    glVertex2i(bxc-1,byc+3);
  }; glEnd();
   glBegin(GL_LINE_STRIP); {
    glVertex2i(bxc+2,byc-2); 
    glVertex2i(bxc+6,byc+2);
  }; glEnd();
  SetPen(highlight_colour);
  glBegin(GL_LINE_STRIP); {
    glVertex2i(bxc-2,byc-3); 
    glVertex2i(bxc+5,byc+4);
  }; glEnd();
  glBegin(GL_LINE_STRIP); {
    glVertex2i(bxc-4,byc-1); 
    glVertex2i(bxc,byc+3);
  }; glEnd();
   glBegin(GL_LINE_STRIP); {
    glVertex2i(bxc+3,byc-2); 
    glVertex2i(bxc+7,byc+2);
  }; glEnd();
  

  _needs_redraw = false;	
}
    
void Glute2::Slider::_MousePressed(int x, int y, int button)
{
  //cout << "Slider \"" << _text << "\" pressed" << endl;
  _down = true;
  _needs_redraw = true;

}
    
void Glute2::Slider::_MouseReleased(int startx, int starty, 
				   int endx, int endy, int button)
{
  //cout << "Slider \"" << _label << "\" released" << endl;

  // Same effect as drag
  _Drag(startx,starty,endx,endy,button);

  _down = false;
}
void Glute2::Slider::_Drag(int startx, int starty,
			   int endx, int endy, int button)
{
  //cout << "Slider \"" << _label << "\" dragged" << endl;
  // Only X end matters!
  if (endx < (_x+_sx+0.5*BUTW)) {
    _curr_val=_min_val;
  } else if (endx > (_x+_sx+_sw-0.5*BUTW)) {
    _curr_val=_max_val;
  } else {
    const double frac = double(endx - (_x+_sx+0.5*BUTW)) / double(_sw-BUTW);
    _curr_val = frac*_max_val + (1-frac)*_min_val;
  }
  _UpdateValString();
  ValueChangedEvent(_curr_val);

  _needs_redraw = true;
}


void Glute2::Slider::SetSize(unsigned int width, unsigned int height)
{
  // Border region grows to fit space
  _w = width;
  _h = height;
 
  // Slider grows to fill space
  _slider_w = _sw = (_w - _label_w - _val_w - 10);

  // Slider always vertically centered
  _sy = (_h - _sh) / 2;


}
