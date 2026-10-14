/** 
 * @file   threedtoolglowwidget.h
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


#ifndef _threedtoolglowwidget_h_
#define _threedtoolglowwidget_h_

#include <VWGL/Display/threedtoolglcommon.h>
#include <glow.h>


GLOW_NAMESPACE_USING

namespace VW
{

class ThreeDToolGlowWidget : public VW::ThreeDToolGLCommon,
  public GlowSubwindow,
  public GlowIdleReceiver
{
public:
  ThreeDToolGlowWidget(GlowComponent *parent,
		       int xpos, int ypos,
		       const unsigned int camera_width, 
		       const unsigned int camera_height);

  // Functions overridden from ThreeDToolGLCommon
  unsigned int WindowWidth() const {return Width();}
  unsigned int WindowHeight() const {return Height();}
  // SwapBuffers and MakeCurrent are handled by Glow
  void SwapBuffers() {}
  void MakeCurrent() {MakeCurGlutWindow();}
  // RequestDraw sets a flag so that the display will redraw once in response
  // to idle events
  void RequestDraw() {request_draw_flag = true;}

protected:

  // Some data on the current mouse drag
  bool draggingflag;
  int currentbutton;
  int last_x, last_y;

  double width_height_ratio;




  // We override these methods to draw the widget
  bool OnBeginPaint();
  void OnEndPaint();		

  // We override these methods to handle widget events. For this
  // widget, we're only going to handle mouse events.
  void OnMouseDown(Glow::MouseButton button,
				 int x,
				 int y,
				 Glow::Modifiers modifiers);
  void OnMouseUp(Glow::MouseButton button,
			       int x,
			       int y,
			       Glow::Modifiers modifiers);
  void OnMouseDrag(int x, int y);

  // We override this method to make sure the widget can be
  // redrawn when the system is idle
  bool request_draw_flag;
  void OnMessage(const GlowIdleMessage& message);
};

} // namespace VW

#endif
