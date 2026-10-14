/** 
 * @file   displayglut.h
 * @author BJ Tordoff
 * @date   02/08/2002
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
 * @class   VW::DisplayGLUT
 * @ingroup Displays
 * @author  BJ Tordoff
 * @date    02/08/2003
 * @brief   A display which uses only OpenGL/GLUT library calls.
 *
 * This display opens a window using GLUT library calls, and allows all the usual
 * drawing operations.
 */

#ifndef _displayglut_h_
#define _displayglut_h_

#ifdef _WIN32 // on windows GL barfs if you don't include all of windows first...
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <VW/events.h>
#include "displayglcommon.h"
#include "glutwindow.h"

namespace VW {

  class DisplayGLUT : public DisplayGLCommon, protected GLUTWindow
{
public:
  DisplayGLUT(const std::string title = "");
  DisplayGLUT(const unsigned int width, 
	      const unsigned int height, 
	      const std::string title = "");
  ~DisplayGLUT(void) {};

  void SetSize(const unsigned int new_width, 
	       const unsigned int new_height);
  unsigned int GetWidth();
  unsigned int GetHeight();

  void MakeCurrent();


  bool IsMe(int display_id) const;

  void Motion(int newx, int newy);
  void Mouse(int button, int state, int x, int y);


  // Signals which can be connected to user functions

  /** The display needs redrawing.
   * When this signal is emitted the display has already been cleared and
   * the viewpoint set appropriately. The user just needs to draw the scene
   * at the origin.
   */
  VWEvents::Event1<void,bool> DrawEvent;

  void RequestDraw();


  /** For internal use only - call it at your own risk - you have been warned. */
  void Draw();

protected:
  void SwapBuffers();
  void Resize(int new_width, 
	       int new_height);

  // The following ones aren't used here (yet)
  void PassiveMotion(int newx, int newy) { };
  void Keyboard(unsigned char key, int x, int y) { };

  int _window_id;
  bool _dragging;
  // For dragging
  int _last_x, _last_y;
  int _current_button;
  unsigned int _width,_height;
};

}; // end namespace VW





inline unsigned int VW::DisplayGLUT::GetWidth() 
{
  return(_width);
};

inline unsigned int VW::DisplayGLUT::GetHeight() 
{
  return(_height);
};


#endif
