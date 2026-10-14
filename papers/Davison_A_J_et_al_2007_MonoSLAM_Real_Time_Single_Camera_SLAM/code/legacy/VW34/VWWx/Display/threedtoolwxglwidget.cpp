/** 
 * @file   threedtoolwxglwidget.cpp
 * @author BJ Tordoff and AJ Davison
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
// System includes
#ifdef _WIN32
#include <windows.h>
#endif

// Local includes
#include "threedtoolwxglwidget.h"


using namespace std;
using namespace VW;

#define VERBOSE false

//// Event stuff (draw event in vwdrawevent.h/.cpp)
DEFINE_EVENT_TYPE(vwEVT_3DTOOL_PROCESSHITS);
IMPLEMENT_DYNAMIC_CLASS(vwProcessHitsEvent, wxCommandEvent)




//// Display stuff


BEGIN_EVENT_TABLE (VW::ThreeDToolWxGLWidget, wxGLCanvas)
  EVT_LEFT_DOWN (VW::ThreeDToolWxGLWidget::OnMouseDown)
  EVT_MIDDLE_DOWN (VW::ThreeDToolWxGLWidget::OnMouseDown)
  EVT_RIGHT_DOWN (VW::ThreeDToolWxGLWidget::OnMouseDown)
  EVT_LEFT_UP (VW::ThreeDToolWxGLWidget::OnMouseUp)
  EVT_MIDDLE_UP (VW::ThreeDToolWxGLWidget::OnMouseUp)
  EVT_RIGHT_UP (VW::ThreeDToolWxGLWidget::OnMouseUp)
  EVT_MOTION (VW::ThreeDToolWxGLWidget::OnMouseMove)
  EVT_PAINT(VW::ThreeDToolWxGLWidget::OnPaint)
  EVT_SIZE(VW::ThreeDToolWxGLWidget::OnSize)
END_EVENT_TABLE()
  


ThreeDToolWxGLWidget::
ThreeDToolWxGLWidget(wxWindow *parent,
		     int wx_id,
		     const unsigned int camera_width, 
		     const unsigned int camera_height,
		     const std::string title)
  : ThreeDToolGLCommon(camera_width, camera_height), 
    wxGLCanvas(parent,wx_id,wxPoint(0,0),wxSize(camera_width,camera_height)),
    _wx_id(wx_id),
    _width(camera_width), 
    _height(camera_height)
{
}

void ThreeDToolWxGLWidget::SwapBuffers()
{
  wxGLCanvas::SetCurrent();
  wxGLCanvas::SwapBuffers();
}

void ThreeDToolWxGLWidget::MakeCurrent()
{
  wxGLCanvas::SetCurrent();
}


void ThreeDToolWxGLWidget::RequestDraw()
{
  // Let the system know that the window needs refreshing
  Refresh(FALSE);
}

void ThreeDToolWxGLWidget::OnMouseDown(wxMouseEvent & event)
{
  _last_x = event.m_x;
  _last_y = event.m_y;
  if (event.m_leftDown) {
    _current_button = 1;
  } else if (event.m_middleDown) {
    _current_button = 2;
  } else if (event.m_rightDown) {
    _current_button = 3;
  } else {
    _current_button = 0;
  }
  _pressed = true;
  dragging = false;
}

void ThreeDToolWxGLWidget::OnMouseUp(wxMouseEvent & event)
{
  if (dragging) {
    DragRelease();
  } else {
    // If we get but not down, just ignore it!
    if (_pressed) {
      Clicked(_last_x, _last_y, _current_button);
    }
  }
  _pressed = false;
  dragging = false;
}

void ThreeDToolWxGLWidget::OnMouseMove(wxMouseEvent & event)
{
  if (_pressed) {
    dragging = true;
    Dragged(_last_x, _last_y, 
	    event.m_x, event.m_y, 
	    _current_button);
    _last_x = event.m_x;
    _last_y = event.m_y;
  }
}

void ThreeDToolWxGLWidget::OnPaint(wxPaintEvent& event)
{
  if (VERBOSE) cerr << "OnPaint." << endl;

  // This is a dummy, to avoid an endless succession of paint messages.
  // OnPaint handlers must always create a wxPaintDC --- whatever that means!
  wxPaintDC dc(this);

  DoDrawing();
}

void ThreeDToolWxGLWidget::OnSize(wxSizeEvent& event)
{
  if (VERBOSE) cerr << "OnSize." << endl;

  // this is also necessary to update the context on some platforms
  wxGLCanvas::OnSize(event);

  int w, h;
  GetClientSize(&w, &h);


  _width = w;
  _height = h;

  if (VERBOSE) cerr << _width << ", " << _height << endl;


  RequestDraw();
}


void ThreeDToolWxGLWidget::_CallDrawEvent(bool b)
{
  // Call the VWEvents callback
  DrawEvent(b);

  // Also generate a wx-style event
  vwDrawEvent event(_wx_id);
  event.SetEventObject(this);
  this->GetEventHandler()->ProcessEvent(event);
}

void ThreeDToolWxGLWidget::_CallProcessHitsEvent(int item, double x, double y)
{
  // Simply call the callback
  ProcessHitsEvent(item,x,y);

  // Also generate a wx-style event
  vwProcessHitsEvent event(_wx_id,item,x,y);
  event.SetEventObject(this);
  this->GetEventHandler()->ProcessEvent(event);
}
