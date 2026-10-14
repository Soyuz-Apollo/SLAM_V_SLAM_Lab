/** 
 * @file   threedtoolglut.h
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
/**
 * @class   VW::ThreeDToolGLUT
 * @ingroup GLTools
 * @author  AJ Davison
 * @date    30/10/2002
 * @brief   A version of the ThreeDToolGLWidget which uses GLUT to open
 *          a window and handle events.
 *
 * ***Please insert long descrition here...
 */


#ifndef _THREEDTOOL_GLUT_H
#define _THREEDTOOL_GLUT_H

#include "threedtoolglcommon.h"
#include "glutwindow.h"

namespace VW {


class ThreeDToolGLUT : public ThreeDToolGLCommon, protected GLUTWindow {

 public:
  ThreeDToolGLUT(const unsigned int camera_width, 
			 const unsigned int camera_height,
			 const std::string title = "");


  // Redefined virtual functions
  unsigned int WindowWidth() const;
  unsigned int WindowHeight() const;
  void SwapBuffers();

  void MakeCurrent();
  void RequestDraw();

  /** Is the GL display id given mine? */
  bool IsMe(int display_id) const;

 protected:
  void Resize(int newwidth, int newheight);

  void Motion(int newx, int newy);
  void Mouse(int button, int state, int x, int y);
  void Draw() { DoDrawing(); };

  // The following ones aren't used here (yet)
  void PassiveMotion(int newx, int newy);
  void Keyboard(unsigned char key, int x, int y);


  int _window_id;

  int _width, _height;
  
  // For dragging
  int last_x, last_y;
  int current_button;
};

};

inline unsigned int 
VW::ThreeDToolGLUT::WindowWidth() const
{
  return _width;
}

inline unsigned int 
VW::ThreeDToolGLUT::WindowHeight() const
{
  return _height;
}

inline bool 
VW::ThreeDToolGLUT::IsMe(int display_id) const
{
  return (display_id == _window_id);
}

inline void VW::ThreeDToolGLUT::PassiveMotion(int, int) 
{ };

inline void VW::ThreeDToolGLUT::Keyboard(unsigned char, int, int) 
{ };

#endif

