/** 
 * @file   displaywxglwidget.h
 * @author BJ Tordoff
 * @date   14/05/2004
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
 * @class   VW::DisplayWxGLWidget
 * @ingroup Displays
 * @author  BJ Tordoff
 * @date    02/08/2003
 * @brief   A display widget for WxWidgets guis.
 *
 */

#ifndef _displaywxglwidget_h_
#define _displaywxglwidget_h_

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include <wx/glcanvas.h>
#include <VWGL/Display/displayglcommon.h>

namespace VW {

  class DisplayWxGLWidget : public wxGLCanvas, public DisplayGLCommon
{
public:
	DisplayWxGLWidget(); // This has to exist for wx reasons, but DON'T USE IT!
  DisplayWxGLWidget(wxWindow *parent,
		    int wx_id,
		    const unsigned int width, 
		    const unsigned int height, 
		    const std::string title = "");
  ~DisplayWxGLWidget(void) {};

  void SetSize(const unsigned int new_width, 
	       const unsigned int new_height);
  unsigned int GetWidth();
  unsigned int GetHeight();

  void MakeCurrent();


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
  void DisplayCoordsToPixelCoords(float dispx, float dispy,
				  float& pixx, float& pixy) const;
  void OnMouseDown(wxMouseEvent & event);
  void OnMouseUp(wxMouseEvent & event);
  void OnMouseMove(wxMouseEvent & event);
  void OnPaint(wxPaintEvent& event);
  void OnSize(wxSizeEvent& event);

  int _wx_id;

  // For dragging
  bool _dragging, _pressed;
  int _last_x, _last_y;
  int _current_button;
  unsigned int _width,_height;

  DECLARE_DYNAMIC_CLASS(DisplayWxGLWidget);
  DECLARE_EVENT_TABLE();

};

}; // end namespace VW


// Some stuff for wxEvent style callbacks
#include "vwmouseevents.h"
#include "vwdrawevent.h"



inline unsigned int VW::DisplayWxGLWidget::GetWidth() 
{
  return(_width);
};

inline unsigned int VW::DisplayWxGLWidget::GetHeight() 
{
  return(_height);
};

inline void 
VW::DisplayWxGLWidget::DisplayCoordsToPixelCoords(float dispx, float dispy,
						  float& pixx, float& pixy) const
{
  pixx = 2.0f * float(dispx) / float(_width) - 1.0f;
  pixy = 1.0f - 2.0f * float(dispy) / float(_height);
  ConvertToPixelCoords(pixx,pixy);
}

#endif
