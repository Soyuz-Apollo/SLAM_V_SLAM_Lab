/** 
 * @file   sequencermoviegtk.cc
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
#include "sequencermoviegtk.h"
#include "interfacegtk.h"

#define VERBOSE false

using namespace std;


//******* Widget functions *******//

// {{{ InitForm(SequencerMovieGTK* parent)

void VW::SequencerMovieGTKWidget::InitForm(SequencerMovieGTK* parent) {
  if (VERBOSE) cerr << "Create grabber form" << endl;

  this->set_label("Movie Sequencer");
  this->set_border_width(3);
  Gtk::VBox*       main_vbox       = manage(new Gtk::VBox());
  Gtk::Frame*      files_frame     = manage(new Gtk::Frame("Files"));
                   files_box       = manage(new Gtk::VBox());
  Gtk::HButtonBox* grab_button_bar = manage(new Gtk::HButtonBox());
                   go_button       = manage(new Gtk::Button("Continuous"));
                   next_button     = manage(new Gtk::Button("Next"));
                   stop_button     = manage(new Gtk::Button("Stop"));
                   rewind_button   = manage(new Gtk::Button("Rewind"));

  // Use the filename(s) to build the files box
  for (unsigned int i=0;i<parent->GetNumberOfChannels();i++) {
    cerr << "Creating file widget "<<i<<endl;
    file_widgets.push_back(manage(new VW::SequencerMovieChannelGTKWidget()));
    file_widgets[i]->InitForm(parent);
    file_widgets[i]->SetMovieName(parent->GetMovieName(i));
    files_box->pack_start(*file_widgets[i]);
  }

  // Build the button bar
  files_box->set_border_width(3);
  grab_button_bar->pack_end(*go_button);
  grab_button_bar->pack_end(*next_button);
  grab_button_bar->pack_end(*stop_button);
  grab_button_bar->pack_end(*rewind_button);

  // Base class provides the toolbar
  CreateButtonBar<VW::SequencerMovieGTKWidget>(parent,false);

  // Stick things on
  files_box->set_border_width(3);
  files_frame->set_border_width(3);
  files_frame->add(*files_box);
  main_vbox->pack_start(*button_bar);
  main_vbox->pack_start(*files_frame);
  main_vbox->pack_start(*grab_button_bar);

  go_button->clicked.connect(SigC::slot(parent,&VW::SequencerMovieGTK::Continuous));
   next_button->clicked.connect(SigC::slot(parent,&VW::SequencerMovieGTK::Next));
   stop_button->clicked.connect(SigC::slot(parent,&VW::SequencerMovieGTK::Stop));
   rewind_button->clicked.connect(SigC::slot(parent,&VW::SequencerMovieGTK::Rewind));
  this->add(*main_vbox);
}

// }}}


//******* Main class functions *******//



VW::SequencerMovieGTK::SequencerMovieGTK(const string& moviename,
					 VW::InterfaceType t) :
  SequencerMovie(moviename) {
  my_type=t;
  my_name="Movie grabber";
  if (t==INTERFACE_AS_BUTTONWINDOW) {
    InitButtonWindow();
  } else {
    InitWidget();
  }
  my_widget->InitForm(this);
  UpdateForm();
}
VW::SequencerMovieGTK::SequencerMovieGTK(const vector<string>& movienames,
					 VW::InterfaceType t) :
  SequencerMovie(movienames) {
  my_type=t;
  my_name="Movie grabber";
  if (t==INTERFACE_AS_BUTTONWINDOW) {
    InitButtonWindow();
  } else {
    InitWidget();
  }
  my_widget->InitForm(this);
  UpdateForm();
}
VW::SequencerMovieGTK::~SequencerMovieGTK() {
  Stop();
}




bool VW::SequencerMovieGTK::Init() {
  // Do the actual probing
  cerr << "SequencerMovieGTK::Init()" << endl;
  UpdateForm();
  return(true);
}
void VW::SequencerMovieGTK::Continuous() {
  cerr << "Go!" << endl;
  VW::SequencerMovie::Continuous();
  UpdateForm();
}
void VW::SequencerMovieGTK::Stop() {
  cerr << "Stop." << endl;
  VW::SequencerMovie::Stop();
  UpdateForm();
}
void VW::SequencerMovieGTK::Next() {
  cerr << "Next please..." << endl;
  VW::SequencerMovie::Next();
  UpdateForm();
}
void VW::SequencerMovieGTK::Rewind() {
  cerr << "Rewinding..." << endl;
  VW::SequencerMovie::Rewind();
  UpdateForm();
}
void VW::SequencerMovieGTK::UpdateFromForm() {
  cerr << "Update from form." << endl;
  for (unsigned int i=0;i<number_of_channels;i++) {
    string form_name=my_widget->GetMovieName(i);
    if (form_name!=movienames[i]) {
      cerr << "SequencerMovieGTK::UpdateFromForm() Aaargh, the user has changed the name." << endl;
      movienames[i]=form_name;
      _OpenFile(i);
    }
  }
  UpdateForm();
}
void VW::SequencerMovieGTK::UpdateForm() {
  cerr << "Update form." << endl;
  bool all_systems_go=true;
  for (unsigned int i=0;i<number_of_channels;i++) {
    all_systems_go = all_systems_go && file_ok[i];
    my_widget->SetLED(i,file_ok[i]);
  }
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

  // TO DO: Individual LED's?
}




/*** Stuff for the MovieChannel widget ***/

void VW::SequencerMovieChannelGTKWidget::InitForm(SequencerMovieGTK* main_form) {
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
  
  browse_button->clicked.connect(SigC::slot(this,&SequencerMovieChannelGTKWidget::LoadStart));
  file_name->activate.connect(SigC::slot(main_form,&SequencerMovieGTK::UpdateFromForm));
}

void VW::SequencerMovieChannelGTKWidget::LoadStart() {
  // Open a file browser
  file_selector = new Gtk::FileSelection("Select a params file");
  file_selector->get_ok_button()->clicked.connect(slot(this, &VW::SequencerMovieChannelGTKWidget::LoadOK));
  file_selector->get_cancel_button()->clicked.connect(slot(this, &VW::SequencerMovieChannelGTKWidget::LoadCancel));
  file_selector->show();
}
void VW::SequencerMovieChannelGTKWidget::LoadOK() {
  // Get the filename
  string filename=file_selector->get_filename();

  // Kill the selector
  delete file_selector;

  // Try to load the params (if name ends in a '/' then the filename was blank)
  cerr << "Load params?" << endl;
  if (filename[filename.length()-1]!='/') {
    file_name->set_text(filename);
    _main_form->UpdateFromForm();
  } else {
    cerr << "LoadParams: No file selected!" << endl;
  }
  

}
void VW::SequencerMovieChannelGTKWidget::LoadCancel() {
  // Kill the selector
  file_name->set_text("<no file>");
  _main_form->UpdateFromForm();
  delete file_selector;
}

void VW::SequencerMovieGTKWidget::SetLED(unsigned int which_channel, bool valid) {
  file_widgets[which_channel]->SetLED(valid);
}
void VW::SequencerMovieChannelGTKWidget::SetLED(bool valid) {
  if (valid) {
    status_led->set(led_green_xpm);
  } else {
    status_led->set(led_red_xpm);
  }
}



std::string VW::SequencerMovieChannelGTKWidget::GetMovieName() 
{
  return(file_name->get_text());
}

void VW::SequencerMovieChannelGTKWidget::SetMovieName(const std::string s) 
{
  file_name->set_text(s);
}

std::string VW::SequencerMovieGTKWidget::GetMovieName(unsigned int which_channel) 
{
  return(file_widgets[which_channel]->GetMovieName());
}
