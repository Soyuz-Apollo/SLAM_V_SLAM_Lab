/** 
 * @file   widget.cpp
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
#include "widget.h"

using namespace std;


namespace Glute2 {
  
  void Widget::Draw()
  {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, _w, 0, _h);
    glViewport(_x, _y, _w, _h);
    glColor3f(0.0, 1.0, 0.0);
    glRecti(0,0,_w,_h);
  }

  bool Widget::_IsIn(int x, int y)
  {
    //cout << "Cursor=("<<x<<","<<y<<") and sensitive area=("<<_sx<<","<<_sy
    //<<")-("<<_sx+_sw<<","<<_sy+_sh<<")"<<endl;
    
    if(x >= (_x + _sx) && x < (_x + _sx+_sw) 
       && y >= (_y + _sy) && y < (_y + _sy+_sh))
      return true;
    else
      return false;
  }
  void Widget::SetParentWindow(Glute2::Window* parent_window)
  {
    _parent_window = parent_window;
  }

} // namespace Glute
