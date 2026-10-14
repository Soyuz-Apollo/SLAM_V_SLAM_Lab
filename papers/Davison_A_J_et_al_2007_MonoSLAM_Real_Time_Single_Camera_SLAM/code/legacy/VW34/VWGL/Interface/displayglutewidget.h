/** 
 * @file   displayglutewidget.h
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
 * @class   VW::DisplayGluteWidget
 * @ingroup Displays
 * @author  BJ Tordoff
 * @date    03/12/2003
 * @brief   A display which uses only OpenGL/GLUT library calls.
 *
 * This display opens a window using GLUT library calls, and allows all the usual
 * drawing operations.
 */

#ifndef _displayglutewidget_h_
#define _displayglutewidget_h_

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#ifdef _WIN32
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif

#include <VW/events.h>
#include <VWGL/Display/displayglcommon.h>
#include <VWGL/Display/glutwindow.h>
#include "widget.h"


namespace VW {

  class DisplayGluteWidget : public DisplayGLCommon,
			     public VW::GLUTWindow, 
			     public Glute2::Widget
  {
  public:
    DisplayGluteWidget(const std::string title = "");
    DisplayGluteWidget(const unsigned int width, 
			const unsigned int height, 
			const std::string title = "");
    ~DisplayGluteWidget(void) {};
    
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
    
    bool _dragging;
    // For dragging
    int _last_x, _last_y;
    int _current_button;
  };
  
}; // end namespace VW





inline unsigned int VW::DisplayGluteWidget::GetWidth() 
{
  return(_w);
};

inline unsigned int VW::DisplayGluteWidget::GetHeight() 
{
  return(_h);
};


#endif
