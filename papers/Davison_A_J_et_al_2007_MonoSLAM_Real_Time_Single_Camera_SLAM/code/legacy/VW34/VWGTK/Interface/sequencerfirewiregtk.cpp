/** 
 * @file   sequencerfirewiregtk.cc
 * @author BJ Tordoff
 * @date   02/08/2002
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
#include <gtk--/table.h>
#include <gtk--/label.h>
#include <gtk--/frame.h>
#include <gtk--/box.h>
#include <gtk--/button.h>
#include <gtk--/buttonbox.h>

// VW includes

// Local includes
#include "firewirecameragtk.h"
#include "sequencerfirewiregtk.h"
#include "interfacegtk.h"

#define VERBOSE false

using namespace std;




//******* Widget functions *******//

// {{{ InitForm(SequencerFirewireGTK* parent)
void VW::SequencerFirewireGTKWidget::InitForm(SequencerFirewireGTK* parent) {
  if (VERBOSE) cerr << "Create grabber form" << endl;

  this->set_label("Firewire Sequencer");
  this->set_border_width(3);
  Gtk::VBox*       main_vbox       = manage(new Gtk::VBox());
  Gtk::Frame*      cameras_frame   = manage(new Gtk::Frame("Cameras"));
                   cameras_box     = manage(new Gtk::VBox());
  Gtk::HButtonBox* grab_button_bar = manage(new Gtk::HButtonBox());
  Gtk::Button*     go_button       = manage(new Gtk::Button("Continuous"));
  Gtk::Button*     next_button     = manage(new Gtk::Button("Next"));
  Gtk::Button*     stop_button     = manage(new Gtk::Button("Stop"));

  // Build the button bar
  cameras_box->set_border_width(3);
  grab_button_bar->pack_end(*go_button);
  grab_button_bar->pack_end(*next_button);
  grab_button_bar->pack_end(*stop_button);

  // Base class provides the toolbar
  CreateButtonBar<VW::SequencerFirewireGTKWidget>(parent,false);

  // Stick things on
  cameras_box->set_border_width(3);
  cameras_frame->set_border_width(3);
  cameras_frame->add(*cameras_box);
  main_vbox->pack_start(*button_bar);
  main_vbox->pack_start(*cameras_frame);
  main_vbox->pack_start(*grab_button_bar);

  go_button->clicked.connect(SigC::slot(parent,&VW::SequencerFirewireGTK::Continuous));
   next_button->clicked.connect(SigC::slot(parent,&VW::SequencerFirewireGTK::Next));
   stop_button->clicked.connect(SigC::slot(parent,&VW::SequencerFirewireGTK::Stop));
  this->add(*main_vbox);
}
// }}}


//******* Main class functions *******//



VW::SequencerFirewireGTK::SequencerFirewireGTK(VW::InterfaceType t,
					       unsigned int x_reduction,
					       unsigned int y_reduction) :
  SequencerFirewire(x_reduction,y_reduction)
{
  my_type=t;
  my_name="Firewire grabber";
  if (t==INTERFACE_AS_BUTTONWINDOW) {
    InitButtonWindow();
  } else {
    InitWidget();
  }
  my_widget->InitForm(this);
}
VW::SequencerFirewireGTK::~SequencerFirewireGTK() {
  Stop();
  StopISO();
  StopDMA();
}


void VW::SequencerFirewireGTK::AddNewCamera(std::vector<VW::FirewireCamera*>& c_list,
					       unsigned int which_element) {
  c_list[which_element]=new FirewireCameraGTK(INTERFACE_AS_BUTTONWINDOW,
					      VERBOSE);
}


bool VW::SequencerFirewireGTK::Init() {

  SequencerBase::Init();

  // Do the actual probing
  ProbeForCameras();
  SetDefaultCameraSettings();
  // Add buttons to the form
  cerr << "Probing found "<<camera_list.size()<<" cameras."<<endl;
  for (unsigned int i=0;i<camera_list.size();i++) {
    string cam_name =  camera_list[i]->GetVendorString();
           cam_name += ":  ";
           cam_name += camera_list[i]->GetModelString();
    cerr << "Add camera to form: "<<cam_name<<endl;
    my_widget->cameras_box->pack_end(*dynamic_cast<FirewireCameraGTK*>(camera_list[i]));
  }

  return(true);
}
void VW::SequencerFirewireGTK::Continuous() {
  cerr << "Go!" << endl;
  VW::SequencerFirewire::Continuous();
}
void VW::SequencerFirewireGTK::Stop() {
  cerr << "Stop." << endl;
  VW::SequencerFirewire::Stop();
}
void VW::SequencerFirewireGTK::Next() {
  cerr << "Next please..." << endl;
  VW::SequencerFirewire::Next();
}
