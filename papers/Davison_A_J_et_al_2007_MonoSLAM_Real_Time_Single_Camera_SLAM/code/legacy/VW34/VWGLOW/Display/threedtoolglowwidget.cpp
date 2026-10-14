/** 
 * @file   threedtoolglowwidget.cpp
 * @author AJ Davison
 * @date   20/08/2005
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


#include "threedtoolglowwidget.h"

using namespace std;
using namespace VW;

ThreeDToolGlowWidget::ThreeDToolGlowWidget(GlowComponent *parent,
					   int xpos, int ypos,
					   const unsigned int camera_width, 
					   const unsigned int camera_height)
  : ThreeDToolGLCommon(camera_width, camera_height),
    GlowSubwindow(parent, xpos, ypos, int(camera_width), int(camera_height), 
		  Glow::rgbaBuffer | Glow::depthBuffer | Glow::doubleBuffer,
		  Glow::mouseEvents | Glow::dragEvents)
{
  width_height_ratio = double(camera_width) / double(camera_height);

  draggingflag = false;
  last_x = 0;
  last_y = 0;

  // Only way I can find to get it to reliably redraw when driven by 
  // sequencer events is to respond to idle events all the time...
  request_draw_flag = false;
  Glow::RegisterIdle(this);

}

bool ThreeDToolGlowWidget::OnBeginPaint()
{
  // Call draw function of ThreeDToolGLCommon
  DoDrawing();

  return true;
}
		
void ThreeDToolGlowWidget::OnEndPaint()
{
}


void ThreeDToolGlowWidget::OnMouseDown(Glow::MouseButton button,
					     int x,
					     int y,
					     Glow::Modifiers modifiers)
{
  //  cerr << "MouseDown " << x << ", " << y << " button " << 
  //  button << endl;

  // Call function of ThreeDToolGLCommon
  last_x = x;
  last_y = y;
  // Note that in Glow the left button is 0 but in VW it's 1
  currentbutton = (int) (button) + 1;
  draggingflag = false;
}
 
void ThreeDToolGlowWidget:: OnMouseUp(Glow::MouseButton button,
					    int x,
					    int y,
					    Glow::Modifiers modifiers)
{
  //  cerr << "MouseUp " << x << ", " << y << " button " << 
  //  button << endl;

  if (draggingflag == false) {
    Clicked(x, y, int(button) + 1);
  }

  if(draggingflag) {
    // Call function of ThreeDToolGLCommon
    DragRelease();
    draggingflag = false;
  }
}
 
void ThreeDToolGlowWidget::OnMouseDrag(int x,
				       int y)
{
  //  cerr << "Drag " << x << ", " << y << endl;

  // Call function of ThreeDToolGLCommon
  Dragged(last_x, last_y, x, y, currentbutton);
  draggingflag = true;

  last_x = x;
  last_y = y;
}

void ThreeDToolGlowWidget::OnMessage(const GlowIdleMessage& message)
{
  if (request_draw_flag) {
    Refresh();
    request_draw_flag = false;
  }
}
