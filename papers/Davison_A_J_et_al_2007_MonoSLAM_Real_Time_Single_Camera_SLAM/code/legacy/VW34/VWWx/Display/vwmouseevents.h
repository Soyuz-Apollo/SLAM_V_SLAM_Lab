#ifndef _vwmouseevents_h_
#define _vwmouseevents_h_

#include <wx/event.h>

extern const wxEventType vwEVT_CLICK;
extern const wxEventType vwEVT_DRAG;
extern const wxEventType vwEVT_DRAG_FINISHED;

/** An event for a mouse click (press and release without motion). */
class vwClickEvent : public wxCommandEvent {
public:
  vwClickEvent();
  vwClickEvent(int id, 
	       int x, int y, int button);
  vwClickEvent(const vwClickEvent & event);

  virtual wxEvent *Clone() const;

  int GetX() const;
  int GetY() const;
  int GetButton() const;

 private:
  int _x,_y,_but;
    DECLARE_DYNAMIC_CLASS(vwClickEvent)
};


/** An event for a mouse drag (motion whilst pressed - see also vwDragFinishedEvent for capturing only the end of a drag). */
class vwDragEvent : public wxCommandEvent {
public:
  vwDragEvent();
  vwDragEvent(int id, 
	      int start_x, int start_y,
	      int end_x, int end_y,
	      int button);
  vwDragEvent(const vwDragEvent & event);

  virtual wxEvent *Clone() const;

  int GetStartX() const;
  int GetStartY() const;
  int GetEndX() const;
  int GetEndY() const;
  int GetButton() const;

 private:
  int _sx,_sy,_ex,_ey,_but;
    DECLARE_DYNAMIC_CLASS(vwDragEvent)
};


/** An event for the end of a mouse drag (end of motion whilst pressed - see also vwDragEvent for capturing motion during a drag). */
class vwDragFinishedEvent : public wxCommandEvent {
public:
  vwDragFinishedEvent();
  vwDragFinishedEvent(int id, 
		      int start_x, int start_y,
		      int end_x, int end_y,
		      int button);
  vwDragFinishedEvent(const vwDragFinishedEvent & event);

  virtual wxEvent *Clone() const;

  int GetStartX() const;
  int GetStartY() const;
  int GetEndX() const;
  int GetEndY() const;
  int GetButton() const;

 private:
  int _sx,_sy,_ex,_ey,_but;
    DECLARE_DYNAMIC_CLASS(vwDragFinishedEvent)
};














// INLINES

inline vwClickEvent::vwClickEvent()
  : wxCommandEvent(vwEVT_CLICK,0),
  _x(0), _y(0),
  _but(0)
{ 
}


inline vwClickEvent::vwClickEvent(int id, 
			   int x, int y, int button)
  : wxCommandEvent(vwEVT_CLICK,id),
  _x(x), _y(y), _but(button)
{ 
}

inline vwClickEvent::vwClickEvent(const vwClickEvent & event)
  : wxCommandEvent(event),
  _x(event._x), _y(event._y), _but(event._but)
{ 
}

inline wxEvent* 
vwClickEvent::Clone() const 
{ 
  return new vwClickEvent(*this); 
}


inline int vwClickEvent::GetX() const 
{
  return _x;
}


inline int vwClickEvent::GetY() const 
{
  return _y;
}


inline int vwClickEvent::GetButton() const 
{
  return _but;
}



inline vwDragEvent::vwDragEvent()
  : wxCommandEvent(vwEVT_DRAG,0),
  _sx(0), _sy(0),
  _ex(0), _ey(0),
  _but(0)
{ 
}


inline vwDragEvent::vwDragEvent(int id, 
			 int start_x, int start_y,
			 int end_x, int end_y,
			 int button)
  : wxCommandEvent(vwEVT_DRAG,id),
  _sx(start_x), _sy(start_y),
  _ex(end_x), _ey(end_y),
  _but(button)
{ 
}


inline vwDragEvent::vwDragEvent(const vwDragEvent & event)
  : wxCommandEvent(event),
  _sx(event._sx), _sy(event._sy),
  _ex(event._ex), _ey(event._ey),
  _but(event._but)
{ 
}


inline wxEvent*
vwDragEvent::Clone() const 
{ 
  return new vwDragEvent(*this); 
}


inline int vwDragEvent::GetStartX() const 
{
  return _sx;
}

inline int vwDragEvent::GetStartY() const 
{
  return _sy;
}

inline int vwDragEvent::GetEndX() const 
{
  return _ex;
}

inline int vwDragEvent::GetEndY() const 
{
  return _ey;
}

inline int vwDragEvent::GetButton() const 
{
  return _but;
}


inline vwDragFinishedEvent::vwDragFinishedEvent()
  : wxCommandEvent(vwEVT_DRAG_FINISHED,0),
  _sx(0), _sy(0),
  _ex(0), _ey(0),
  _but(0)
{ 
}

inline vwDragFinishedEvent::vwDragFinishedEvent(int id, 
						int start_x, int start_y,
						int end_x, int end_y,
						int button)
  : wxCommandEvent(vwEVT_DRAG_FINISHED,id),
  _sx(start_x), _sy(start_y),
  _ex(end_x), _ey(end_y),
  _but(button)
{ 
}

inline vwDragFinishedEvent::vwDragFinishedEvent(const vwDragFinishedEvent & event)
  : wxCommandEvent(event),
  _sx(event._sx), _sy(event._sy),
  _ex(event._ex), _ey(event._ey),
  _but(event._but)
{ 
}

inline wxEvent*
vwDragFinishedEvent::Clone() const 
{ 
  return new vwDragFinishedEvent(*this); 
}

inline int vwDragFinishedEvent::GetStartX() const 
{
  return _sx;
}

inline int vwDragFinishedEvent::GetStartY() const 
{
  return _sy;
}

inline int vwDragFinishedEvent::GetEndX() const 
{
  return _ex;
}

inline int vwDragFinishedEvent::GetEndY() const 
{
  return _ey;
}

inline int vwDragFinishedEvent::GetButton() const 
{
  return _but;
}

// WX Declarations

#define EVT_VWCLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( vwEVT_CLICK, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),

#define EVT_VWDRAG(id, fn) DECLARE_EVENT_TABLE_ENTRY( vwEVT_DRAG, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),

#define EVT_VWDRAGFINISHED(id, fn) DECLARE_EVENT_TABLE_ENTRY( vwEVT_DRAG_FINISHED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),

#endif
