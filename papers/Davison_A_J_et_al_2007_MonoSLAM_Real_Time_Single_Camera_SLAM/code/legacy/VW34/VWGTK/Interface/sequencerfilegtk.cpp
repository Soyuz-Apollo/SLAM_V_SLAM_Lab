/** 
 * @file   sequencerfilegtk.cc
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
#include <gtk--.h>
#include <sstream>

// VW includes

// Local includes
#include "sequencerfilegtk.h"

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE true

using namespace std;

//******* Widget functions *******//

// {{{ InitForm(SequencerFileGTK* parent)

void VW::SequencerFileGTKWidget::InitForm(SequencerFileGTK* parent) {
  if (VERBOSE) cerr << "Create grabber form" << endl;

  this->set_label("File Sequencer");
  this->set_border_width(3);

  if (VERBOSE) cerr << "VW::SequencerFileGTKWidget::InitForm: Create widgets" << endl;
  Gtk::VBox*       main_vbox       = manage(new Gtk::VBox());
  Gtk::Frame*      files_frame     = manage(new Gtk::Frame("Files"));
                   files_box       = manage(new Gtk::VBox());
		   frame_no_label  = manage(new Gtk::Label("Current frame number: 0"));
  Gtk::HButtonBox* grab_button_bar = manage(new Gtk::HButtonBox());
  Gtk::HBox*       stat_box        = manage(new Gtk::HBox());
                   go_button       = manage(new Gtk::Button("Continuous"));
                   next_button     = manage(new Gtk::Button("Next"));
                   stop_button     = manage(new Gtk::Button("Stop"));
                   rewind_button   = manage(new Gtk::Button("Rewind"));
		   loop_menu       = manage(new Gtk::OptionMenu());
  Gtk::Label*      loop_label      = manage(new Gtk::Label("Looping: "));         
  { // Build a menu of loop options
    if (VERBOSE) cerr << "VW::SequencerFileGTKWidget::InitForm: Create loop menu" << endl;
    Gtk::Menu* lmenu = manage(new Gtk::Menu());
    Gtk::Menu_Helpers::MenuList& looplist = lmenu->items();
    Gtk::Menu_Helpers::MenuElem e("play once");
    looplist.push_back(Gtk::Menu_Helpers::MenuElem("play once", SigC::bind(SigC::slot(parent, &VW::SequencerFileGTK::OnLoopChanged),1)));
    looplist.push_back(Gtk::Menu_Helpers::MenuElem("repeat", SigC::bind(SigC::slot(parent, &VW::SequencerFileGTK::OnLoopChanged),2)));
    looplist.push_back(Gtk::Menu_Helpers::MenuElem("bounce", SigC::bind(SigC::slot(parent, &VW::SequencerFileGTK::OnLoopChanged),3)));
    loop_menu->set_menu(lmenu);
  }

  // Use the filename(s) to build the files box
  if (VERBOSE) cerr << "VW::SequencerFileGTKWidget::InitForm: Add channels" << endl;
  for (unsigned int i=0;i<parent->GetNumberOfChannels();i++) {
    AddFile (parent);
  }

  if (VERBOSE) cerr << "VW::SequencerFileGTKWidget::InitForm: Build form" << endl;
  // Build the button bar
  files_box->set_border_width(3);
  grab_button_bar->pack_end(*go_button);
  grab_button_bar->pack_end(*next_button);
  grab_button_bar->pack_end(*stop_button);
  grab_button_bar->pack_end(*rewind_button);

  // Build the frame no / looping bar
  stat_box->pack_start(*frame_no_label);
  stat_box->pack_start(*loop_label);
  stat_box->pack_start(*loop_menu);

  // Base class provides the toolbar
  CreateButtonBar<VW::SequencerFileGTKWidget>(parent,false);

  // Stick things on
  files_box->set_border_width(3);
  files_frame->set_border_width(3);
  files_frame->add(*files_box);
  main_vbox->pack_start(*button_bar);
  main_vbox->pack_start(*files_frame);
  main_vbox->pack_start(*stat_box);
  main_vbox->pack_start(*grab_button_bar);

  go_button->clicked.connect(SigC::slot(parent,&VW::SequencerFileGTK::Continuous));
   next_button->clicked.connect(SigC::slot(parent,&VW::SequencerFileGTK::Next));
   stop_button->clicked.connect(SigC::slot(parent,&VW::SequencerFileGTK::Stop));
   rewind_button->clicked.connect(SigC::slot(parent,&VW::SequencerFileGTK::Rewind));
  this->add(*main_vbox);
}

// }}}

void VW::SequencerFileGTKWidget::AddFile(VW::SequencerFileGTK* parent)
{
  file_widgets.push_back (manage(new VW::SequencerFileChannelGTKWidget()));
  unsigned int index = file_widgets.size() - 1;
  file_widgets[index]->InitForm(parent);
  file_widgets[index]->SetFileName(parent->GetInitialFileName(index));
  files_box->pack_start(*file_widgets[index]);
}


//******* Main class functions *******//


VW::SequencerFileGTK::SequencerFileGTK(VW::InterfaceType t) 
  : SequencerFile() 
{
  cerr << "Construct." << endl;
  my_type=t;
  my_name="File grabber";
  if (t==INTERFACE_AS_BUTTONWINDOW) {
    InitButtonWindow();
  } else {
    InitWidget();
  }
  cerr << "Init done." << endl;
  my_widget->InitForm(this);
  UpdateForm();
}
VW::SequencerFileGTK::SequencerFileGTK(const string& moviename,
					 VW::InterfaceType t) :
  SequencerFile(moviename) {
  my_type=t;
  my_name="File grabber";
  if (t==INTERFACE_AS_BUTTONWINDOW) {
    InitButtonWindow();
  } else {
    InitWidget();
  }
  my_widget->InitForm(this);
  UpdateForm();
}
VW::SequencerFileGTK::SequencerFileGTK(const vector<string>& movienames,
					 VW::InterfaceType t) :
  SequencerFile(movienames) {
  my_type=t;
  my_name="File grabber";
  if (t==INTERFACE_AS_BUTTONWINDOW) {
    InitButtonWindow();
  } else {
    InitWidget();
  }
  my_widget->InitForm(this);
  UpdateForm();
}
VW::SequencerFileGTK::~SequencerFileGTK() {
  Stop();
}


void VW::SequencerFileGTK::AddChannel(const string& file_name)
{
  VW::SequencerFile::AddChannel (file_name);
  my_widget->AddFile (this);
}


bool VW::SequencerFileGTK::Init() {
  // Do the actual probing
  if (VERBOSE) cerr << "SequencerFileGTK::Init()" << endl;
  VW::SequencerFile::Init();
  UpdateForm();
  return(true);
}
void VW::SequencerFileGTK::Continuous() {
  if (VERBOSE) cerr << "Go!" << endl;
  VW::SequencerFile::Continuous();
  UpdateForm();
}
void VW::SequencerFileGTK::Stop() {
  if (VERBOSE) cerr << "Stop." << endl;
  VW::SequencerFile::Stop();
  UpdateForm();
}
void VW::SequencerFileGTK::Next() {
  if (VERBOSE) cerr << "Next please..." << endl;
  VW::SequencerFile::Next();
  UpdateForm();
}
void VW::SequencerFileGTK::Rewind() {
  if (VERBOSE) cerr << "Rewinding..." << endl;
  VW::SequencerFile::Rewind();
  UpdateForm();
}

void VW::SequencerFileGTK::OnLoopChanged(int which)
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
void VW::SequencerFileGTK::SetLooping(SequencerLoopMode mode)
{
  my_widget->loop_menu->set_history(mode);

  _loop = mode;
}

void VW::SequencerFileGTK::UpdateFromForm() {
  if (VERBOSE) cerr << "Update from form." << endl;
  for (unsigned int i=0;i<number_of_channels;i++) {
    string form_name=my_widget->GetFileName(i);
    if (form_name!=first_file_name[i]) {
      cerr << "SequencerFileGTK::UpdateFromForm() Aaargh, the user has changed the name." << endl;
      first_file_name[i]=form_name;
      file_ok[i]=_DetermineFilenamePatterns(i);
    }
  }
  UpdateForm();
}
void VW::SequencerFileGTK::UpdateForm() {
  if (VERBOSE) cerr << "Update form." << endl;
  // Check whether file is valid
  bool all_systems_go=true;
  for (unsigned int i=0;i<number_of_channels;i++) {
    all_systems_go = all_systems_go && file_ok[i];
    my_widget->SetLED(i,file_ok[i]);
  }
  if (VERBOSE) cerr << "All systems go? "<<all_systems_go<<endl;
  if (all_systems_go) {
    // Make sure all buttons are enabled
    my_widget->go_button->set_sensitive(true);
    my_widget->next_button->set_sensitive(true);
    my_widget->stop_button->set_sensitive(true);
    my_widget->rewind_button->set_sensitive(true);
    // Only one of Go/Stop can be active
    if (cont) {
      my_widget->go_button->set_sensitive(false);
      my_widget->stop_button->set_sensitive(true);
    } else {
      my_widget->go_button->set_sensitive(true);
      my_widget->stop_button->set_sensitive(false);
    }
  } else {
    // Something is wrong, so disable controls except rewind
    my_widget->go_button->set_sensitive(false);
    my_widget->next_button->set_sensitive(false);
    my_widget->stop_button->set_sensitive(false);
  }
  // Update frame number
  ostringstream str;
  str << "Current frame number: " << frame_number;
  my_widget->frame_no_label->set_text(str.str());
}




/*** Stuff for the FileChannel widget ***/

void VW::SequencerFileChannelGTKWidget::InitForm(SequencerFileGTK* main_form) {
  _main_form=main_form;

  file_name=manage(new Gtk::Entry());
  file_name->set_text("<no file>");

  Gtk::Pixmap*     load_pixmap    = manage(new Gtk::Pixmap(load_icon_xpm));
  browse_button    = manage(new Gtk::Button());
  browse_button -> add(*load_pixmap);

  Gtk::Label*     file_label    = manage(new Gtk::Label("File: "));

  status_led = manage(new Gtk::Pixmap(led_off_xpm));

  this->pack_start(*status_led,false); // no expand
  this->pack_start(*file_label,false); // no expand
  this->pack_start(*file_name);
  this->pack_start(*browse_button,false); // no expand
  
  browse_button->clicked.connect(SigC::slot(this,&SequencerFileChannelGTKWidget::LoadStart));
  file_name->activate.connect(SigC::slot(main_form,&SequencerFileGTK::UpdateFromForm));
}

void VW::SequencerFileChannelGTKWidget::LoadStart() {
  // Open a file browser
  file_selector = new Gtk::FileSelection("Select the first file for this channel");
  file_selector->get_ok_button()->clicked.connect(slot(this, &VW::SequencerFileChannelGTKWidget::LoadOK));
  file_selector->get_cancel_button()->clicked.connect(slot(this, &VW::SequencerFileChannelGTKWidget::LoadCancel));
  file_selector->show();
}
void VW::SequencerFileChannelGTKWidget::LoadOK() {
  // Get the filename
  string filename=file_selector->get_filename();

  // Kill the selector
  delete file_selector;

  // Try to load the params (if name ends in a '/' then the filename was blank)
  if (VERBOSE) cerr << "Load image?" << endl;
  if (filename[filename.length()-1]!='/') {
    file_name->set_text(filename);
    _main_form->UpdateFromForm();
  } else {
    cerr << "Load: No file selected!" << endl;
  }
  

}
void VW::SequencerFileChannelGTKWidget::LoadCancel() {
  // Kill the selector
  file_name->set_text("<no file>");
  _main_form->UpdateFromForm();
  delete file_selector;
}

void VW::SequencerFileGTKWidget::SetLED(unsigned int which_channel, bool valid) {
  file_widgets[which_channel]->SetLED(valid);
}
void VW::SequencerFileChannelGTKWidget::SetLED(bool valid) {
  if (valid) {
    status_led->set(led_green_xpm);
  } else {
    status_led->set(led_red_xpm);
  }
}
