/** 
 * @file   sequencerfilewx.cpp
 * @author BJ Tordoff
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
#include "sequencerfilewx.h"

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
  SEQUENCERFILE_CONTBUTTON = 1001,
  SEQUENCERFILE_NEXTBUTTON,
  SEQUENCERFILE_REWDBUTTON,
  SEQUENCERFILE_STOPBUTTON,
};


BEGIN_EVENT_TABLE (VW::SequencerFileWx, wxPanel)
  EVT_BUTTON( SEQUENCERFILE_CONTBUTTON , VW::SequencerFileWx::OnContinuousClicked )
  EVT_BUTTON( SEQUENCERFILE_NEXTBUTTON , VW::SequencerFileWx::OnNextClicked )
  EVT_BUTTON( SEQUENCERFILE_REWDBUTTON , VW::SequencerFileWx::OnRewindClicked )
  EVT_BUTTON( SEQUENCERFILE_STOPBUTTON , VW::SequencerFileWx::OnStopClicked )
END_EVENT_TABLE();

namespace VW {
  
  IMPLEMENT_CLASS(SequencerFileWx,wxPanel);
    
}

VW::SequencerFileWx::SequencerFileWx(wxWindow *parent,
				     int wx_id)
  : wxPanel(parent,wx_id),
    _wx_id(wx_id)
{
  CreateForm(parent);
}

VW::SequencerFileWx::SequencerFileWx(wxWindow *parent,
				     int wx_id,
				     const std::string& first_file_name)
  : wxPanel(parent,wx_id),
    SequencerFile(first_file_name),
    _wx_id(wx_id)
{
  CreateForm(parent);
}

VW::SequencerFileWx::SequencerFileWx(wxWindow *parent,
				     int wx_id,
				     const std::vector<std::string>& first_file_names)
  : wxPanel(parent,wx_id),
    SequencerFile(first_file_names),
    _wx_id(wx_id)
{
  CreateForm(parent);
}

VW::SequencerFileWx::~SequencerFileWx()
{
  Stop();
}

bool VW::SequencerFileWx::Init()
{
  if (VERBOSE) cerr << "SequencerFileWx::Init()" << endl;
  VW::SequencerFile::Init();
  UpdateForm();
  return(true);
}

void VW::SequencerFileWx::AddChannel(const std::string& file_name)
{
  // Just call parent for now (should update interface too)
  VW::SequencerFile::AddChannel(file_name);
}

void VW::SequencerFileWx::Continuous()
{
  if (VERBOSE) cerr << "Go!" << endl;
  VW::SequencerFile::Continuous();
  UpdateForm();
}

void VW::SequencerFileWx::Stop()
{
  if (VERBOSE) cerr << "Stop." << endl;
  VW::SequencerFile::Stop();
  UpdateForm();
}

void VW::SequencerFileWx::Next()
{
  if (VERBOSE) cerr << "Next please..." << endl;
  VW::SequencerFile::Next();
  UpdateForm();
}

void VW::SequencerFileWx::Rewind()
{
  if (VERBOSE) cerr << "Rewinding..." << endl;
  VW::SequencerFile::Rewind();
  UpdateForm();
}

void VW::SequencerFileWx::SetLooping(SequencerLoopMode mode)
{
  //  my_widget->loop_menu->set_history(mode);

  _loop = mode;
}


void VW::SequencerFileWx::CreateForm(wxWindow *parent)
{
  // Create buttons
  _cont_button = new wxButton(this,SEQUENCERFILE_CONTBUTTON,"Continuous");
  _next_button = new wxButton(this,SEQUENCERFILE_NEXTBUTTON,"Next");
  _rewind_button = new wxButton(this,SEQUENCERFILE_REWDBUTTON,"Rewind");
  _stop_button = new wxButton(this,SEQUENCERFILE_STOPBUTTON,"Stop");

  // Put them in a box
  wxBoxSizer* buttonbox = new wxBoxSizer(wxHORIZONTAL);
  buttonbox->Add(_cont_button,1,wxALL|wxGROW,10);
  buttonbox->Add(_next_button,1,wxALL|wxEXPAND,10);
  buttonbox->Add(_rewind_button,1,wxALL|wxEXPAND,10);
  buttonbox->Add(_stop_button,1,wxALL|wxEXPAND,10);

  // Create frame number and loop strings 
  _framenumber_label = new wxStaticText(this,-1,"Frame number: 0");


  // Stack vertically
   wxStaticBox* frame = new wxStaticBox(this,-1,"File sequencer");
   wxStaticBoxSizer* vbox = new wxStaticBoxSizer(frame,wxVERTICAL);
   vbox->Add(_framenumber_label);
   vbox->Add(buttonbox);

  // And finally stick it inside the frame
  this->SetSizer(vbox);
  vbox->Fit(this);
  vbox->SetSizeHints(this);



}

void VW::SequencerFileWx::UpdateForm()
{
  if (VERBOSE) cerr << "Update form." << endl;
  // Check whether file is valid
  bool all_systems_go=true;
  for (unsigned int i=0;i<number_of_channels;i++) {
    all_systems_go = all_systems_go && file_ok[i];
    //my_widget->SetLED(i,file_ok[i]);
  }
  if (VERBOSE) cerr << "All systems go? "<<all_systems_go<<endl;
  if (all_systems_go) {
    // Make sure all buttons are enabled
    _next_button->Enable();
    _rewind_button->Enable();
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
  // Update frame number
  ostringstream str;
  str << "Current frame number: " << frame_number;
  _framenumber_label->SetLabel(str.str().c_str());
}

void VW::SequencerFileWx::UpdateFromForm()
{
}

void VW::SequencerFileWx::OnLoopChanged(int which)
{
  cerr << "Change looping." << endl;
  switch(which) {
  default:
  case 1:
    SetLooping(NO_LOOPING);
    break;
  case 2:
    SetLooping(LOOP);
    break;
  case 3:
    SetLooping(BOUNCE);
    break;
  };
}

void VW::SequencerFileWx::_FrameArrived() 
{
  user_finished=false;
  NewFrameEvent(0l); 
  vwNewFrameEvent event(_wx_id);
  event.SetEventObject((wxEvtHandler*)this);
  this->ProcessEvent(event);
  user_finished=true;
}

void VW::SequencerFileWx::OnStopClicked(wxCommandEvent & event) 
{ 
  Stop(); 
}

void VW::SequencerFileWx::OnNextClicked(wxCommandEvent & event) 
{ 
  Next(); 
};

void VW::SequencerFileWx::OnRewindClicked(wxCommandEvent & event) 
{ 
  Rewind(); 
};

void VW::SequencerFileWx::OnContinuousClicked(wxCommandEvent & event) 
{ 
  Continuous(); 
};
