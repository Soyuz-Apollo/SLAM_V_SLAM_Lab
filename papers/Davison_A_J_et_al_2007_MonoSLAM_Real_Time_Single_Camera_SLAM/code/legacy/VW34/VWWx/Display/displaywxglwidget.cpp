/** 
 * @file   displaywxglwidget.cpp
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
#include <wx/wx.h>
#include "displaywxglwidget.h"

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false


using namespace std;



BEGIN_EVENT_TABLE (VW::DisplayWxGLWidget, wxGLCanvas)
  EVT_LEFT_DOWN (VW::DisplayWxGLWidget::OnMouseDown)
  EVT_MIDDLE_DOWN (VW::DisplayWxGLWidget::OnMouseDown)
  EVT_RIGHT_DOWN (VW::DisplayWxGLWidget::OnMouseDown)
  EVT_LEFT_UP (VW::DisplayWxGLWidget::OnMouseUp)
  EVT_MIDDLE_UP (VW::DisplayWxGLWidget::OnMouseUp)
  EVT_RIGHT_UP (VW::DisplayWxGLWidget::OnMouseUp)
  EVT_MOTION (VW::DisplayWxGLWidget::OnMouseMove)
  EVT_PAINT(VW::DisplayWxGLWidget::OnPaint)
  EVT_SIZE(VW::DisplayWxGLWidget::OnSize)
END_EVENT_TABLE()


namespace VW {
  
  IMPLEMENT_DYNAMIC_CLASS(DisplayWxGLWidget,wxGLCanvas);
    
}



VW::DisplayWxGLWidget::DisplayWxGLWidget() 
  : wxGLCanvas (NULL, -1)
{
	assert(false);
}

VW::DisplayWxGLWidget::DisplayWxGLWidget(wxWindow *parent,
					 int wx_id,
					 const unsigned int width, 
					 const unsigned int height, 
					 const string title) 
  : DisplayGLCommon (width, height, title),
    wxGLCanvas (parent,wx_id,wxPoint(0,0),wxSize(width,height)),
    _wx_id(wx_id),
    _width(width),
    _height(height),
    _pressed(false)
{
	assert(width>0);
	assert(height>0);
    if (VERBOSE) { 
  cerr << "New display ";
  if (title.length() == 0) cerr << "(unnamed) ";
  else cerr << "`" << title << "' ";
  cerr << "(" << _width << "x" << _height << ")" << endl;
  }

  // Open a window (function from parent class)
  //  _window_id = _CreateWindow( _width, _height, title.c_str() );

  // Must set init flag to false explicitly since base class
  // sets it to true
  _visible = true;
  _initialised = false;
};




void VW::DisplayWxGLWidget::SetSize(const unsigned int newwidth, 
			      const unsigned int newheight)
{
  if (VERBOSE) cerr << "DisplayWxGLWidget::SetSize" << endl;
  assert(newwidth>0);
   assert(newheight>0);
   _width = newwidth;
   _height = newheight;

   // Since wx won't allow an unalloced display, just resize away
   // from zero
   wxWindow::SetSize(0,0,newwidth,newheight);
}

void VW::DisplayWxGLWidget::Resize(int newwidth, int newheight)
{
  if (VERBOSE) cerr << "DisplayWxGLWidget::ReSize" << endl;
  assert(newwidth>0);
  assert(newheight>0);
  _width = newwidth;
  _height = newheight;

   wxWindow::SetSize(0,0,newwidth,newheight);
}

void VW::DisplayWxGLWidget::RequestDraw()
{
  if (VERBOSE) cerr << "DisplayWxGLWidget::RequestDraw" << endl;
  // Let the system know that the window needs refreshing
  Refresh(FALSE);
}
void VW::DisplayWxGLWidget::Draw()
{
  if (VERBOSE) cerr << "VW::DisplayWxGLWidget::Draw()" << endl;
  vwDrawEvent event(_wx_id);
  event.SetEventObject(this);
  //wxEvtHandler* eh = this->GetEventHandler();
  /*this->*/
  wxEvtHandler::ProcessEvent(event);
}

void VW::DisplayWxGLWidget::SwapBuffers()
{
  if (VERBOSE)  cerr << "DisplayWxGLWidget::SwapBuffers" << endl;
 wxGLCanvas::SetCurrent();
  wxGLCanvas::SwapBuffers();
}

void VW::DisplayWxGLWidget::MakeCurrent()
{
   if (VERBOSE) cerr << "DisplayWxGLWidget::MakeCurrent" << endl;
   if (!_initialised) InitGL();
   wxGLCanvas::SetCurrent();
 }

void VW::DisplayWxGLWidget::OnMouseDown(wxMouseEvent & event)
{
  {
    float x,y;
    DisplayCoordsToPixelCoords(float(event.m_x), float(event.m_y),x,y);
    _last_x=int(x+0.5f); _last_y=int(y+0.5f);
  }
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
  _dragging = false;
}

void VW::DisplayWxGLWidget::OnMouseUp(wxMouseEvent & event)
{
  float x,y;
  DisplayCoordsToPixelCoords(float(event.m_x), float(event.m_y), x, y);
  if (_dragging) {
    // The VW event
    DragFinishedEvent(_last_x, _last_y, 
		      int(x), int(y), 
		      _current_button);
    // And the wx event
    vwDragFinishedEvent event(_wx_id,
			      _last_x, _last_y, 
			      int(x), int(y), 
			      _current_button);
    event.SetEventObject(this);
    this->GetEventHandler()->ProcessEvent(event);
  } else {
    // If we get but not down, just ignore it!
    if (_pressed) {
      // The VW event
      ClickEvent(_last_x, _last_y, _current_button);
      // And the wx event
      vwClickEvent event(_wx_id,
			 _last_x, _last_y,
			 _current_button);
      event.SetEventObject(this);
      this->GetEventHandler()->ProcessEvent(event);
    }
  }
  _pressed = false;
  _dragging = false;
}

void VW::DisplayWxGLWidget::OnMouseMove(wxMouseEvent & event)
{
  if (_pressed) {
    float x,y;
    DisplayCoordsToPixelCoords(event.m_x, event.m_y, x, y);
    // VW event
    DragEvent(_last_x, _last_y, 
	      int(x+0.5f), int(y+0.5f), 
	      _current_button);
    // And the wx event
    vwDragEvent event(_wx_id,
		      _last_x, _last_y, 
		      int(x+0.5f), int(y+0.5f), 
		      _current_button);
    event.SetEventObject(this);
    this->GetEventHandler()->ProcessEvent(event);
    _dragging = true;
  }
}
void VW::DisplayWxGLWidget::OnPaint(wxPaintEvent& event)
{
  if (VERBOSE) cerr << "OnPaint." << endl;

  // This is a dummy, to avoid an endless succession of paint messages.
  // OnPaint handlers must always create a wxPaintDC --- whatever that means!
  wxPaintDC dc(this);

  Draw();
}

void VW::DisplayWxGLWidget::OnSize(wxSizeEvent& event)
{
  if (VERBOSE) cerr << "OnSize." << endl;

  // this is also necessary to update the context on some platforms
  wxGLCanvas::OnSize(event);

  int newwidth, newheight;
  GetClientSize(&newwidth, &newheight);

  assert(newwidth>0);
  assert(newheight>0);
  

  _width = newwidth;
  _height = newheight;

  if (VERBOSE) cerr << _width << ", " << _height << endl;


  RequestDraw();
}
