#include <wx/wx.h>
#include "evtsequencernewframe.h"

DEFINE_EVENT_TYPE(vwEVT_SEQUENCER_NEWFRAME);
IMPLEMENT_DYNAMIC_CLASS(vwNewFrameEvent, wxCommandEvent);
