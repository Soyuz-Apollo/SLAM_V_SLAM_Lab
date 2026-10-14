/** 
 * @file   threedtoolglutewidget.h
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
/**
 * @class   VW::ThreeDToolGluteWidget
 * @ingroup GLTools
 * @author  BJ Tordoff
 * @date    03/12/2003
 * @brief   A version of the ThreeDTool which can be embedded in a glute GUI
 *
 * ***Please insert long descrition here...
 */


#ifndef _THREEDTOOL_GLUTE_WIDGET_H
#define _THREEDTOOL_GLUTE_WIDGET_H

#include <VWGL/Display/threedtoolglcommon.h>
#include <VWGL/Display/glutwindow.h>
#include "widget.h"

namespace VW {


class ThreeDToolGluteWidget : public VW::ThreeDToolGLCommon, 
			      public VW::GLUTWindow, 
			      public Glute2::Widget {

 public:
  ThreeDToolGluteWidget(const unsigned int camera_width, 
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

  void SetSize(const unsigned int newwidth, 
	       const unsigned int newheight);

 protected:
  void Resize(int newwidth, int newheight);

  void Motion(int newx, int newy);
  void Mouse(int button, int state, int x, int y);
  void Draw() { DoDrawing(); };

  // The following ones aren't used here (yet)
  void PassiveMotion(int newx, int newy) { };
  void Keyboard(unsigned char key, int x, int y) { };


  bool _initialised;

  // For dragging
  int last_x, last_y;
  int current_button;
};

};

#endif

