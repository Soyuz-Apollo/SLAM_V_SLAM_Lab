/** 
 * @file   label.cpp
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
#include "label.h"
#include "theme.h"

using namespace std;

Glute2::Label::Label(const std::string& label)
{
  // Scan the text and break into lines
  string curr_str;
  _max_text_w = 0;
  for (unsigned int pos=0;pos<label.size();pos++) {
    char c=label[pos];
    if (c=='\n') {
      _text.push_back(curr_str);
      curr_str.clear();
    } else {
      curr_str.push_back(c);
    }
  }
  if (curr_str.size()>0) _text.push_back(curr_str);

  for (unsigned int i=0;i<_text.size();i++) {
    unsigned int textw = glutBitmapLength(GLUT_BITMAP_HELVETICA_12,
				 (unsigned char*)(_text[i].c_str()));
    if (textw>_max_text_w) _max_text_w = textw;
  }

  _minsize_x = _max_text_w + 20;
  _sw = _sh = 0;
  _minsize_y = _text.size()*20; // Should really be determined by font

  // Label does not need to grow
  _wants_to_grow_x = false;
  _wants_to_grow_y = false;

}

Glute2::Label::~Label()
{
}
    
void Glute2::Label::Draw()
{
  // Now draw
  glMatrixMode(GL_PROJECTION);

  SetPen(normal_background_colour);
  glRecti(_x,_y,_x+_w,_y+_h);
   
  {
    glColor3f(0.0,0.0,0.0);
    for (unsigned int t=0;t<_text.size();t++) {
      int ydelta = 20*(_text.size()-t) - 20;
      glRasterPos2f(_x+_sx,
		    _y+_sy + 7.5 + ydelta);
      int len = _text[t].length();
      for(int i = 0; i < len; i++) {
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,_text[t][i]);
      }
    }
  }

  _needs_redraw = false;	
}
    

void Glute2::Label::SetSize(unsigned int width, unsigned int height)
{
  cout << "Label: Resize to size ("<<_w<<"x"<<_h<<") at pos ("<<_x<<","<<_y<<")" << endl;
  // Border region grows to fit space
  _w = width;
  _h = height;
 
  // Label relative position depends on text width and demanded width
  _sx = (_w - _minsize_x) / 2;
  _sy = (_h - _minsize_y) / 2;


}
