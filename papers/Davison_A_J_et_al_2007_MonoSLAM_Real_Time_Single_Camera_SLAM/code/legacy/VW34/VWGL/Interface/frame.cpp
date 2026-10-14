/** 
 * @file   frame.cpp
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
using namespace std;
#include "frame.h"
#include "theme.h"

#define BORDER 2
Glute2::Frame::Frame(const std::string label)
  : _label(label)
{
}

void Glute2::Frame::Add(Widget* w)
{
  if ( _widgets.size() != 0 ) {
    cerr << "Glute2::Frame::Add: Already have a widget!" << endl;
    exit(0);
  }
  this->_PushFront(w);
  _RecalcSizes();
}

void Glute2::Frame::_RecalcSizes()
{
  const unsigned int n = _widgets.size();
  if (n > 0) {
    
    
    // First work out the minimums
    int top_border=BORDER*2+2;
    int other_border=BORDER*2+2;
    if (_label.size()>0) top_border += 18;
    _minsize_x = top_border+other_border;
    _minsize_y = 2*other_border;

    // Now check with the contained widget
    _minsize_y += _widgets[0]->GetMinimumHeight();
    _minsize_x += _widgets[0]->GetMinimumWidth();
    _wants_to_grow_x=_widgets[0]->WantsToGrowX();
    _wants_to_grow_y=_widgets[0]->WantsToGrowY();

    cout << "Frame: Current size = ("<<_w<<"x"<<_h<<") but minimum is ("<<_minsize_x<<"x"<<_minsize_y<<")"<<endl;


    // Pass on resize to widget
    _widgets[0]->SetPosition(_x + other_border,
			     _y + other_border);
    _widgets[0]->SetSize(_w - 2*other_border,
			 _h - (top_border+other_border));

  }
  _needs_redraw = true;
}


void Glute2::Frame::Draw()
{
  int len = _label.length();

  // Background
  SetPen(normal_background_colour);
  glRecti(_x,_y,_x+_w,_y+_h);

  // Border
    int bx0 = _x + BORDER;
    int bx1 = _x + _w - 1 - BORDER;
    int by0 = _y + BORDER;
    int by1 = _y + _h - 1 - BORDER;
  if (len>0) {
    by1 -= 9;
  }
  SetPen(highlight_colour);
  glBegin(GL_LINE_STRIP);
  glVertex2i(bx0,by0); 
  glVertex2i(bx0,by1);
  glVertex2i(bx1,by1); 
  glEnd();
  glBegin(GL_LINE_STRIP);
  glVertex2i(bx0+2,by0+2); 
  glVertex2i(bx1-2,by0+2);
  glVertex2i(bx1-2,by1-2); 
  glEnd();
 
  SetPen(dark_shadow_colour);
  glBegin(GL_LINE_STRIP);
  glVertex2i(bx0+2,by0+2); 
  glVertex2i(bx0+2,by1-2);
  glVertex2i(bx1-2,by1-2); 
  glEnd();
  glBegin(GL_LINE_STRIP);
  glVertex2i(bx0,by0); 
  glVertex2i(bx1,by0);
  glVertex2i(bx1,by1); 
  glEnd();

  if (len>0) {
    {
      int text_w = glutBitmapLength(GLUT_BITMAP_HELVETICA_12,(unsigned char*)(_label.c_str()));

      // First the box
      int tx0 = _x+20;
      int tx1 = tx0 + 6+text_w;
      int ty0 = _y+_h-BORDER-20;
      int ty1 = ty0 + 20;
      SetPen(normal_background_colour);
      //glColor3f(1.0,0.0,0.0);
      glRecti(tx0,ty0,tx1,ty1);
  SetPen(highlight_colour);
  glBegin(GL_LINE_STRIP);
  glVertex2i(tx0,  by1); 
  glVertex2i(tx0,  ty1);
  glVertex2i(tx1-1,ty1); 
  glVertex2i(tx1-1,by1); 
  glEnd();
  SetPen(dark_shadow_colour);
  glBegin(GL_LINE_STRIP);
  glVertex2i(tx0,  by1-2); 
  glVertex2i(tx0,  ty0);
  glVertex2i(tx1-1,ty0); 
  glVertex2i(tx1-1,by1-2); 
  glEnd();

  // Now the text
      glColor3f(0.0,0.0,0.0);
      glRasterPos2f(tx0+3,
		    ty0+5.5);
      for(int i = 0; i < len; i++) {
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,_label[i]);
      }
    } 
  }

  //cout << "Draw "<<_widgets.size()<<" widgets" << endl;
  for(unsigned int i = 0; i < _widgets.size(); i++) {
    _widgets[i]->Draw();
  }
}
