/** 
 * @file   sequencerfirewirewx.cpp
 * @author BJ Tordoff and AJ Davison
 * @date   27/05/2004
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

#include <iostream>
#include <sstream>
#include <wx/wx.h>
#include "sequencerfirewirewx.h"

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false

using namespace std;

//// Event stuff

// These numbers are used by the events system to identify which
// object generated the event. That means you need one constant
// per control for which events are required!
enum
{ 
  SEQUENCERFIREWIRE_CONTBUTTON = 1001,
  SEQUENCERFIREWIRE_NEXTBUTTON,
  SEQUENCERFIREWIRE_STOPBUTTON,
};


BEGIN_EVENT_TABLE (VW::SequencerFirewireWx, wxPanel)
  EVT_BUTTON( SEQUENCERFIREWIRE_CONTBUTTON , VW::SequencerFirewireWx::OnContinuousClicked )
  EVT_BUTTON( SEQUENCERFIREWIRE_NEXTBUTTON , VW::SequencerFirewireWx::OnNextClicked )
  EVT_BUTTON( SEQUENCERFIREWIRE_STOPBUTTON , VW::SequencerFirewireWx::OnStopClicked )
END_EVENT_TABLE();

namespace VW {
  
  IMPLEMENT_CLASS(SequencerFirewireWx,wxPanel);
    
}

VW::SequencerFirewireWx::SequencerFirewireWx(wxWindow *parent,
				     int wx_id)
  : wxPanel(parent,wx_id),
    _wx_id(wx_id)
{
  CreateForm(parent);
}

VW::SequencerFirewireWx::~SequencerFirewireWx()
{
  Stop();
  StopISO();
  StopDMA();
}

bool VW::SequencerFirewireWx::Init()
{
  if (VERBOSE) cerr << "SequencerFirewireWx::Init()" << endl;
  VW::SequencerFirewire::Init();
  UpdateForm();
  return(true);
}

void VW::SequencerFirewireWx::AddNewCamera(std::vector<VW::FirewireCamera*>& c_list,
					    unsigned int which_element) 
{
  c_list[which_element]=new FirewireCamera();
}

void VW::SequencerFirewireWx::Continuous()
{
  if (VERBOSE) cerr << "Go!" << endl;
  VW::SequencerFirewire::Continuous();
  UpdateForm();
}

void VW::SequencerFirewireWx::Stop()
{
  if (VERBOSE) cerr << "Stop." << endl;
  VW::SequencerFirewire::Stop();
  UpdateForm();
}

void VW::SequencerFirewireWx::Next()
{
  if (VERBOSE) cerr << "Next please..." << endl;
  VW::SequencerFirewire::Next();
  UpdateForm();
}

void VW::SequencerFirewireWx::CreateForm(wxWindow *parent)
{
  // Create buttons
  _cont_button = new wxButton(this,SEQUENCERFIREWIRE_CONTBUTTON,"Continuous");
  _next_button = new wxButton(this,SEQUENCERFIREWIRE_NEXTBUTTON,"Next");
  _stop_button = new wxButton(this,SEQUENCERFIREWIRE_STOPBUTTON,"Stop");

  // Put them in a box
  wxBoxSizer* buttonbox = new wxBoxSizer(wxHORIZONTAL);
  buttonbox->Add(_cont_button,1,wxALL|wxGROW,10);
  buttonbox->Add(_next_button,1,wxALL|wxEXPAND,10);
  buttonbox->Add(_stop_button,1,wxALL|wxEXPAND,10);

  // Stack vertically
   wxStaticBox* frame = new wxStaticBox(this,-1,"Firewire sequencer");
   wxStaticBoxSizer* vbox = new wxStaticBoxSizer(frame,wxVERTICAL);
   vbox->Add(buttonbox);

  // And finally stick it inside the frame
  this->SetSizer(vbox);
  vbox->Fit(this);
  vbox->SetSizeHints(this);

}

void VW::SequencerFirewireWx::UpdateForm()
{
  if (VERBOSE) cerr << "Update form." << endl;
  // Check whether file is valid
  bool all_systems_go=true;

  //for (unsigned int i=0;i<number_of_channels;i++) {
  //all_systems_go = all_systems_go && file_ok[i];
    //my_widget->SetLED(i,file_ok[i]);
  //}
  if (VERBOSE) cerr << "All systems go? "<<all_systems_go<<endl;
  if (all_systems_go) {
    // Make sure all buttons are enabled
    _next_button->Enable();
    // Only one of Go/Stop can be active
    if (cont) {
      _cont_button->Disable();
      _stop_button->Enable();
    } else {
      _cont_button->Enable();
      _stop_button->Disable();
    }
  } else {
    // Something is wrong, so disable controls except rewind
      _cont_button->Disable();
      _stop_button->Disable();
      _next_button->Disable();
  }
}

void VW::SequencerFirewireWx::UpdateFromForm()
{
}


void VW::SequencerFirewireWx::_FrameArrived() 
{
  user_finished=false;
  NewFrameEvent(0l); 
  vwNewFrameEvent event(_wx_id);
  event.SetEventObject((wxEvtHandler*)this);
  this->ProcessEvent(event);
  user_finished=true;
}

void VW::SequencerFirewireWx::OnStopClicked(wxCommandEvent & event) 
{ 
  Stop(); 
}

void VW::SequencerFirewireWx::OnNextClicked(wxCommandEvent & event) 
{ 
  Next(); 
};


void VW::SequencerFirewireWx::OnContinuousClicked(wxCommandEvent & event) 
{ 
  Continuous(); 
};
