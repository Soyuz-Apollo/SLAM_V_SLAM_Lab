#ifndef _vwdrawevent_h_
#define _vwdrawevent_h_

#include <wx/event.h>

extern const wxEventType vwEVT_DRAW;

class vwDrawEvent : public wxCommandEvent {
public:
  vwDrawEvent()
    : wxCommandEvent(vwEVT_DRAW,0)
  { }
  vwDrawEvent(int id)
      : wxCommandEvent(vwEVT_DRAW,id)
  { }
  vwDrawEvent(const vwDrawEvent & event)
    : wxCommandEvent(event)
  { }

  virtual wxEvent *Clone() const { return new vwDrawEvent(*this); }

 private:
    DECLARE_DYNAMIC_CLASS(vwDrawEvent)
};

#define EVT_VWDRAW(id, fn) DECLARE_EVENT_TABLE_ENTRY( vwEVT_DRAW, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),

#endif
