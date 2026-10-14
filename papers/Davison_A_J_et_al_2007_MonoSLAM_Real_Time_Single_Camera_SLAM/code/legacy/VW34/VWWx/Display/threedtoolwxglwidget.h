/** 
 * @file   threedtoolwxglwidget.h
 * @author BJ Tordoff and AJ Davison
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
 * @class   VW::ThreeDToolWxGLWidget
 * @ingroup GLTools
 * @author  BJ Tordoff
 * @date    14/05/2004
 * @brief   A version of the ThreeDToolGLWidget for embedding in 
 *          WxWidgets apps.
 *
 * ***Please insert long descrition here...
 */


#ifndef _vw_threedtoolwxglwidget_h_
#define _vw_threedtoolwxglwidget_h_

#include <VWGL/Display/threedtoolglcommon.h>
#include "wx/glcanvas.h"
#include "wx/event.h"

namespace VW {


class ThreeDToolWxGLWidget : public ThreeDToolGLCommon, public wxGLCanvas {

 public:
  ThreeDToolWxGLWidget(wxWindow *parent,
		       int wx_id,
		       const unsigned int camera_width, 
		       const unsigned int camera_height,
		       const std::string title = "");


  // Redefined virtual functions
  unsigned int WindowWidth() const;
  unsigned int WindowHeight() const;
  void SwapBuffers();

  void MakeCurrent();
  void RequestDraw();

 protected:
  // Overloaded from parent so that both VW events and SigC signals are
  // generated
  virtual void _CallDrawEvent(bool b);
  virtual void _CallProcessHitsEvent(int item, double x, double y);


  void OnMouseDown(wxMouseEvent & event);
  void OnMouseUp(wxMouseEvent & event);
  void OnMouseMove(wxMouseEvent & event);
  void OnPaint(wxPaintEvent& event);
  void OnSize(wxSizeEvent& event);


  int _wx_id;

  int _width, _height;
  
  // For dragging (NB: "dragging" bool from parent)
  bool _pressed;
  int _last_x, _last_y;
  int _current_button;

  // We will be handling some GUI events, so need to declare it
  DECLARE_EVENT_TABLE();

};

};

inline unsigned int 
VW::ThreeDToolWxGLWidget::WindowWidth() const
{
  return _width;
}

inline unsigned int 
VW::ThreeDToolWxGLWidget::WindowHeight() const
{
  return _height;
}


// Some stuff for wxEvent style callbacks

#include "vwdrawevent.h"


// Process hits is unqiue to this display
extern const wxEventType vwEVT_3DTOOL_PROCESSHITS;

class vwProcessHitsEvent : public wxCommandEvent {
public:
  vwProcessHitsEvent() 
    : wxCommandEvent(vwEVT_3DTOOL_PROCESSHITS,0),
	item(-1), x(0.0), y(0.0)
        { }
    
  vwProcessHitsEvent(int id, int selected_item, 
		     double location_x, double location_y)
    : wxCommandEvent(vwEVT_3DTOOL_PROCESSHITS, id),
	item(selected_item), x(location_x), y(location_y)
        { }
  vwProcessHitsEvent(const vwProcessHitsEvent & event)
      : wxCommandEvent(event),
	item(event.item), x(event.x), y(event.y)
      { }

    virtual wxEvent *Clone() const { return new vwProcessHitsEvent(*this); }

public:
  int item;
  double x,y;

private:
    DECLARE_DYNAMIC_CLASS(vwProcessHitsEvent)
};



#define EVT_3DTOOL_PROCESSHITS(id, fn) DECLARE_EVENT_TABLE_ENTRY( vwEVT_3DTOOL_PROCESSHITS, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),


#endif

