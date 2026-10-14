/** 
 * @file   evtsequencernewframe.h
 * @author BJ Tordoff and AJ Davison
 * @date   23/06/2004
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
 * @class   VW::wxNewFrameEvent
 * @ingroup Sequencers Interfaces
 * @author  BJ Tordoff
 * @date    27/05/2004
 * @brief A Wx-based interface to the file sequencer.
 *
 * This sequencer is a version of SequencerFile which also provides
 * a WxWidgets interface. The sequencer functionality is identical to the parent
 * class, but with added functions to control the interface. As it is just a
 * Wx widget it can be embedded in any WxWidgets form in the usual manner.
 */

#ifndef _vwEVT_SEQUENCER_NEWFRAME_h_
#define _vwEVT_SEQUENCER_NEWFRAME_h_

// Some stuff for wxEvent style callbacks

extern const wxEventType vwEVT_SEQUENCER_NEWFRAME;

class vwNewFrameEvent : public wxCommandEvent {
public:
  vwNewFrameEvent() 
    : wxCommandEvent(vwEVT_SEQUENCER_NEWFRAME,0)
    { }
    
  vwNewFrameEvent(int id) 
    : wxCommandEvent(vwEVT_SEQUENCER_NEWFRAME,id)
    { }
    
  vwNewFrameEvent(const vwNewFrameEvent & event)
      : wxCommandEvent(event)
    { }

  virtual wxEvent *Clone() const { return new vwNewFrameEvent(*this); }

public:

private:
  DECLARE_DYNAMIC_CLASS(vwNewFrameEvent);
};


#define EVT_SEQUENCER_NEWFRAME(id, fn) DECLARE_EVENT_TABLE_ENTRY( vwEVT_SEQUENCER_NEWFRAME, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),


#endif
