/** 
 * @file   interfacegtk.h
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


/**
 * @class  VW::InterfaceGTK
 * @author BJ Tordoff
 * @date   02/08/2002
 * @biref  *ABSTRACT* base class for interfaces, providing 
 *         common buttons and file selection stuff.
 */

#ifndef _InterfaceGTK_H
#define _InterfaceGTK_H

#include <VW/pixmaps.h>
#include <gtk--.h>

namespace VW {

  /** The interface type.
   * GTK Interfaces can be attached to a form as either a
   * complete encapsulated widget or as a button that pops up a window.
   */
  enum InterfaceType {INTERFACE_AS_WIDGET,
		      INTERFACE_AS_BUTTONWINDOW};

  /** The abstract interface widget.
   * All GTK interface widgets derrive from this, and this class merely
   * provides some of the house-keeping functions.
   */
  class InterfaceGTKWidget : public Gtk::Frame {
  public:
    template <class WidgetType,class T>
    void CreateButtonBar(T* parent, bool add_file_buttons=true);
  protected:
    // This class provides the button bar, and file load/save dialog
    
    Gtk::Tooltips       tooltips;
    Gtk::HBox*          button_bar;
   
  };

  /** The abstract interface class.
   * All GTK interfaces derrive from this, and this class merely
   * provides some of the house-keeping functions.
   */
template <class WidgetType>
class InterfaceGTK : public Gtk::HBox {
  friend class InterfaceGTKWidget;
public:
  InterfaceGTK();

  // Interface specific
  inline void Show(void);
  inline void Hide(void);
  inline void Quit(void) {Gtk::Main::quit();}
  inline VW::InterfaceType  GetType(void);

  // Stuff to overload
  virtual void UpdateForm()=0;

protected:
  // Interface specific
  inline void LoadStart();
  inline void LoadOK();
  inline void SaveStart();
  inline void SaveOK();
  inline void FileCancel();

 
  // Internal signal to load params
  SigC::Signal1<void,std::string> LoadEvent;
  SigC::Signal1<void,std::string> SaveEvent;

  // Functions to initialise either form
  virtual void InitButtonWindow();
  virtual void InitButtonWindow(unsigned int width, unsigned int height);
  virtual void InitWidget();

  // Stuff to open a window or add a widget
  WidgetType*             my_widget;
  Gtk::Window*            my_window;
  VW::InterfaceType       my_type;
  std::string             my_name;
  Gtk::FileSelection* file_selector;

};

}; // end namespace














/*
 *           INLINES
 */



template <class WidgetType>
inline void VW::InterfaceGTK<WidgetType>::Show(void) {
  if (my_type==VW::INTERFACE_AS_BUTTONWINDOW) {
    my_window->show_all();
  }
}
template <class WidgetType>
inline void VW::InterfaceGTK<WidgetType>::Hide(void) {
  if (my_type==VW::INTERFACE_AS_BUTTONWINDOW) {
    my_window->hide();
  }
}
 
template <class WidgetType>
inline VW::InterfaceType VW::InterfaceGTK<WidgetType>::GetType(void) {
  return(my_type);
}


template <class WidgetType>
inline void VW::InterfaceGTK<WidgetType>::LoadStart() {
  // Open a file browser
  file_selector = new Gtk::FileSelection("Select a params file");
  file_selector->get_ok_button()->clicked.connect(slot(this, &VW::InterfaceGTK<WidgetType>::LoadOK));
  file_selector->get_cancel_button()->clicked.connect(slot(this, &VW::InterfaceGTK<WidgetType>::FileCancel));
  file_selector->show();
}

#include <iostream> // For error reporting

template <class WidgetType>
inline void VW::InterfaceGTK<WidgetType>::LoadOK() {
  // Get the filename
  std::string filename=file_selector->get_filename();

  // Kill the selector
  delete file_selector;

  // Try to load the params (if name ends in a '/' then the filename was blank)
  std::cerr << "Load params?" << std::endl;
  if (filename[filename.length()-1]!='/') {
    LoadEvent.emit(filename);
  } else {
    std::cerr << "LoadParams: No file selected!" << std::endl;
  }
  

}
template <class WidgetType>
inline void VW::InterfaceGTK<WidgetType>::SaveStart() {
  // Open a file browser
  file_selector = new Gtk::FileSelection("Select a params file");
  file_selector->get_ok_button()->clicked.connect(slot(this, &VW::InterfaceGTK<WidgetType>::SaveOK));
  file_selector->get_cancel_button()->clicked.connect(slot(this, &VW::InterfaceGTK<WidgetType>::FileCancel));
  file_selector->show();
}
template <class WidgetType>
inline void VW::InterfaceGTK<WidgetType>::SaveOK() {
  // Get the filename
  std::string filename=file_selector->get_filename();

  // Kill the selector
  delete file_selector;

  // Try to load the params (if name ends in a '/' then the filename was blank)
  std::cerr << "Save params?" << std::endl;
  if (filename[filename.length()-1]!='/') {
    SaveEvent.emit(filename);
  } else {
    std::cerr << "SaveParams: No file selected!" << std::endl;
  }
  

}
template <class WidgetType>
inline void VW::InterfaceGTK<WidgetType>::FileCancel() {
  // Kill the selector
  delete file_selector;
}


template <class WidgetType>
VW::InterfaceGTK<WidgetType>::InterfaceGTK() {


}
template <class WidgetType>
void VW::InterfaceGTK<WidgetType>::InitButtonWindow() {
  Gtk::Button* show_form_button = manage(new Gtk::Button(my_name));
  //params_widget = manage(new VW::PlesseyParamsGTKWidget());
  my_window = manage(new Gtk::Window());
  my_widget = manage(new WidgetType());
  my_window->add(*my_widget);
  my_window->set_title(my_name);
  this->pack_end(*show_form_button);

  show_form_button->clicked.connect(slot(this,&VW::InterfaceGTK<WidgetType>::Show));
}

template <class WidgetType>
void VW::InterfaceGTK<WidgetType>::InitButtonWindow(unsigned int width, unsigned int height) {
  Gtk::Button* show_form_button = manage(new Gtk::Button(my_name));
  //params_widget = manage(new VW::PlesseyParamsGTKWidget());
  my_window = manage(new Gtk::Window());
  my_widget = manage(new WidgetType());
  my_window->set_default_size(width, height);
  
  my_window->add(*my_widget);
  my_window->set_title(my_name);
  this->pack_end(*show_form_button);

  show_form_button->clicked.connect(slot(this,&VW::InterfaceGTK<WidgetType>::Show));
}

template <class WidgetType>
void VW::InterfaceGTK<WidgetType>::InitWidget() {
  //params_widget = manage(new VW::PlesseyParamsGTKWidget());
  Gtk::HandleBox* box=manage(new Gtk::HandleBox());
  box->set_handle_position(GTK_POS_LEFT);
  box->set_snap_edge(GTK_POS_TOP);
  //box->set_shadow_type(GTK_SHADOW_NONE);
  my_widget=manage(new WidgetType());
  box->add(*my_widget);
  this->pack_end(*box);
}

template <class WidgetType,class T>
void VW::InterfaceGTKWidget::CreateButtonBar(T* parent,
					     bool add_file_buttons) {
  //////////////////////
  // Button bar stuff //
  //////////////////////

  // Hide button is only needed for the pop-up window
  bool add_hide_button=true;
  if (parent->GetType()==INTERFACE_AS_WIDGET)       add_hide_button=false;
  if (parent->GetType()==INTERFACE_AS_BUTTONWINDOW) add_hide_button=true;

  // Hide button bar
  Gtk::Toolbar*    toolbar2   = manage(new Gtk::Toolbar(GTK_ORIENTATION_HORIZONTAL));

  if (add_hide_button) {
    // Hide button
    Gtk::Button*     hide_button    = manage(new Gtk::Button());
    Gtk::Pixmap*     hide_pixmap    = manage(new Gtk::Pixmap(hide_icon_xpm));
    hide_button -> add(*hide_pixmap);
    hide_button->set_relief(GTK_RELIEF_NONE);
    tooltips.set_tip(*hide_button, "Hide this form.", "");
    hide_button->clicked.connect(SigC::slot(parent,&VW::InterfaceGTK<WidgetType>::Hide));
    // Add to bar 2
    toolbar2->tools().push_back(*hide_button);
  }


  // file button bar
  Gtk::Toolbar*    toolbar1   = manage(new Gtk::Toolbar(GTK_ORIENTATION_HORIZONTAL));

  if (add_file_buttons) {
    // Load button
    Gtk::Button*     load_button    = manage(new Gtk::Button());
    Gtk::Pixmap*     load_pixmap    = manage(new Gtk::Pixmap(load_icon_xpm));
    load_button -> add(*load_pixmap);
    load_button->set_relief(GTK_RELIEF_NONE);
    tooltips.set_tip(*load_button, "Load parameter data from a file.", "");
    // Save button
    Gtk::Button*     save_button    = manage(new Gtk::Button());
    Gtk::Pixmap*     save_pixmap    = manage(new Gtk::Pixmap(save_icon_xpm));
    save_button -> add(*save_pixmap);
    save_button->set_relief(GTK_RELIEF_NONE);
    tooltips.set_tip(*save_button, "Save parameter data to a file.", "");
    
    // Until *JOSS* gets round to implementing it
    // the save button is disabled.
    //    save_button->set_sensitive(false);

    // Callbacks
    load_button->clicked.connect(SigC::slot(parent,&VW::InterfaceGTK<WidgetType>::LoadStart));
    save_button->clicked.connect(SigC::slot(parent,&VW::InterfaceGTK<WidgetType>::SaveStart));
    parent->LoadEvent.connect(SigC::slot(parent,&T::LoadParameterFile));
    parent->SaveEvent.connect(SigC::slot(parent,&T::SaveParameterFile));
    //save_button->clicked.connect(SigC::slot(parent,&VW::InterfaceGTK<WidgetType>::SaveStart));

    toolbar1->tools().push_back(*load_button);
    toolbar1->tools().push_back(*save_button);
  }



  // Hbox for them
                   button_bar     = manage(new Gtk::HBox());
  Gtk::Table*      dummy          = manage(new Gtk::Table(1,1));
  button_bar->pack_start(*toolbar1, false,false);
  button_bar->pack_start(*dummy, true,true);
  button_bar->pack_start(*toolbar2, false,false);
}

#endif
