/** 
 * @file   yorickgtk.cc
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
#include <iostream>
#include <gtk--.h>

// VW includes

// Local includes
#include "yorickgtk.h"
#include "interfacegtk.h"

#define VERBOSE false


using namespace std;


VW::YorickDragPad::YorickDragPad(VW::Yorick *yorick_pointer)
  : Gtk::DrawingArea(), pixmap (0),
    _yorick_p(yorick_pointer),_yo_sensitivity (_DEFAULT_SENSITIVITY) 
{
    set_events (GDK_EXPOSURE_MASK
		| GDK_LEAVE_NOTIFY_MASK
		| GDK_BUTTON_PRESS_MASK
		| GDK_POINTER_MOTION_MASK
		| GDK_POINTER_MOTION_HINT_MASK);
}

VW::YorickDragPad::~YorickDragPad()
  {}

// {{{ int  VW::YorickDragPad::configure_event_impl (GdkEventConfigure *)

/* Create a new backing pixmap of the appropriate size */
int VW::YorickDragPad::configure_event_impl (GdkEventConfigure * /* event */) {
  win = get_window();
  visual = win.get_visual();
  
  if (pixmap)
    pixmap.release();
  
  gc = get_style()->get_white_gc();
  pixmap.create(get_window(),  width(), height());
  
  pixmap.draw_rectangle (gc, true, 0, 0, width(), height());
  
  // Check we haven't been screwed over with our robot objects
  assert (_yorick_p != 0);
  
  // Initialise position
  _yorick_p->GetPosition (&_pan, &_tilt, &_vergel, &_verger);
  return true;
}

// }}}

// {{{ void VW::YorickDragPad::button_press_event_impl ()

gint VW::YorickDragPad::button_press_event_impl (GdkEventButton* event)
{
  _pressed_at_x = int (event->x);
  _pressed_at_y = int (event->y);

  _pressed_at_pan = _yorick_p->GetPan();
  _pressed_at_tilt = _yorick_p->GetTilt();
  _pressed_at_vergel = _yorick_p->GetVergeLeft();
  _pressed_at_verger = _yorick_p->GetVergeRight();

  return 0;
}

// }}}

// {{{ gint VW::YorickDragPad::motion_notify_event_impl (GdkEventMotion* event)

gint VW::YorickDragPad::motion_notify_event_impl (GdkEventMotion* event)
{
  timeval start_time;
  gettimeofday (&start_time, NULL);

  GdkModifierType state;
  int x = 0, y = 0;
  if (event->is_hint) {
    gint tx, ty;
    gdk_window_get_pointer (event->window, &tx, &ty, &state);
    x = int (tx);
    y = int (ty);
  }
  else {
    return TRUE;
    x = int (event->x);
    y = int (event->y);
    state = GdkModifierType (event->state);
  }
  int button_pressed = 0;
  if (state & GDK_BUTTON1_MASK) button_pressed = 1;
  if (state & GDK_BUTTON2_MASK) button_pressed = 2;
  if (state & GDK_BUTTON3_MASK) button_pressed = 3;

  // Button 1 means move pan/tilt
  if (button_pressed == 1) {
    // Calculate motion
    _pan = _pressed_at_pan - float (x - _pressed_at_x) / _yo_sensitivity;
    _tilt = _pressed_at_tilt - float (y - _pressed_at_y) / _yo_sensitivity;
    //    cerr << "Requesting pan of " << _pan << " and tilt of " << _tilt << endl;
    // Move yorick
    _yorick_p->DemandPanTilt (_pan, _tilt);
  }

  // Button 2 means move verge
  else if (button_pressed == 2) {
    // Calculate motion
    _vergel = _pressed_at_vergel + float (y - _pressed_at_y) / (_yo_sensitivity*2);
    _verger = _pressed_at_verger - float (y - _pressed_at_y) / (_yo_sensitivity*2);
    //    cerr << "Requesting vergel of " << _vergel << " and verger of ";
    //    cerr << _verger << endl;
    // Move yorick
    _yorick_p->DemandVerge (_vergel, _verger);
  }

  // Button 3 means recentre
  else if (button_pressed == 3) {
    _yorick_p->SlowRecentre ();
    _pan = _tilt = _vergel = _verger = 0.0;
  }

  timeval finish_time;
  gettimeofday (&finish_time, NULL);
  //  double time_taken = double (finish_time.tv_sec - start_time.tv_sec) + double (finish_time.tv_usec - start_time.tv_usec)/1e6;
  //  cerr << "Time to service motion request: " << time_taken << "secs" << endl;
  return 0;
}

// }}}

// {{{ int  VW::YorickDragPad::expose_event_impl (GdkEventExpose *event)

/* Redraw the screen from the backing pixmap */
int VW::YorickDragPad::expose_event_impl (GdkEventExpose *event)
  {
    
    gc = get_style()->get_fg_gc(get_state());
    win.draw_pixmap(gc ,
		    pixmap,
		    event->area.x, event->area.y,
		    event->area.x, event->area.y,
		    event->area.width, event->area.height);

    return false;
  }

// }}}

// {{{ void VW::YorickDragPad::Redraw()

void VW::YorickDragPad::Redraw() {

  // Clear first
  gc = get_style()->get_white_gc();
  pixmap.draw_rectangle (gc, true, 0, 0, width(), height());

  // Get central lines (pan and tilt)
  float mxx=0,mnx=0,vlx=0;
  _yorick_p->GetPanLimits(&mnx,&mxx);
  vlx=_yorick_p->GetPan();
  float mxy=0,mny=0,vly=0;
  _yorick_p->GetTiltLimits(&mny,&mxy);
  vly=_yorick_p->GetTilt();

  // Vergence gives a delta
  float mxvl=0,mnvl,vlvl=0;
   _yorick_p->GetVergeLeftLimits(&mnvl,&mxvl);
  vlvl=_yorick_p->GetVergeLeft();
  float mxvr=0,mnvr,vlvr=0;
   _yorick_p->GetVergeRightLimits(&mnvr,&mxvr);
  vlvr=_yorick_p->GetVergeRight();
 
  // Get proportions for pan/tilt
  float x_pos=width()*(vlx-mnx)/(mxx-mnx);
  float y_pos=height()*(vly-mny)/(mxy-mny);
  
  // Get proportions for verge (10% of total width)
  // If totally verged in, lines touch, if totally out they are far apart
  float verge_dist=0.25*width();
  float vl_frac=(vlvl-mnvl)/(mxvl-mnvl);
  float vr_frac=(vlvr-mxvr)/(mxvr-mnvr); // NB: goes opposite way (max not min)
  float vl_pos=int(x_pos - (verge_dist*vl_frac));
  float vr_pos=int(x_pos - (verge_dist*vr_frac));


  // Also calculate the zero-position
  float x_zero_pos=width()*(0.0-mnx)/(mxx-mnx);
  float y_zero_pos=height()*(0.0-mny)/(mxy-mny);
  float vl_zero_frac=(0.0-mnvl)/(mxvl-mnvl);
  float vr_zero_frac=(0.0-mxvr)/(mxvr-mnvr);
  int vl_zero_pos=int(x_zero_pos - (verge_dist*vl_zero_frac));
  int vr_zero_pos=int(x_zero_pos - (verge_dist*vr_zero_frac));

  // Set up draw colours
  static Gdk_Color red("red");
  static Gdk_Color grey("gray");
  if (!brush_gc) {
    brush_gc = Gdk_GC(pixmap);
    get_colormap().alloc(red);
    get_colormap().alloc(grey);
  }
  
  // Draw zero position
  brush_gc.set_foreground(grey);
  pixmap.draw_line(brush_gc, 
		   int(width()-vl_zero_pos), int(0), 
		   int(width()-vl_zero_pos), int(height()));
  pixmap.draw_line(brush_gc, 
		   int(width()-vr_zero_pos), int(0), 
		   int(width()-vr_zero_pos), int(height()));
  pixmap.draw_line(brush_gc, 
		   int(0),       int(height()-y_zero_pos), 
		   int(width()), int(height()-y_zero_pos));

  // Draw current position
  brush_gc.set_foreground(red);
  pixmap.draw_line(brush_gc, 
		   int(width()-vl_pos), int(0), 
		   int(width()-vl_pos), int(height()));
  pixmap.draw_line(brush_gc, 
		   int(width()-vr_pos), int(0), 
		   int(width()-vr_pos), int(height()));
  pixmap.draw_line(brush_gc, 
		   int(0), int(height()-y_pos), 
		   int(width()), int(height()-y_pos));
  
  // request a refresh of whole area.
  draw(0);
  

}

// }}}

// {{{ void VW::YorickDragPad::Erase()

void VW::YorickDragPad::Erase()
  {

    // clear pixmap area to white.
gc = get_style()->get_white_gc();
    pixmap.draw_rectangle (gc, true, 0, 0, width(), height());

    // request a refresh of whole area.
draw(0);
  }

// }}}








//******* Widget functions *******//

// {{{ InitForm(YorickGTK* parent)

void VW::YorickGTKWidget::InitForm(YorickGTK* parent) {
  if (VERBOSE) cerr << "Create yorick form" << endl;

  this->set_label("Yorick controls");
  this->set_border_width(3);
  Gtk::VBox*       main_vbox           = manage(new Gtk::VBox());
  Gtk::Frame*      axes_frame          = manage(new Gtk::Frame("Axis controls"));
  Gtk::Frame*      drag_frame          = manage(new Gtk::Frame());
  Gtk::HBox*       axes_box            = manage(new Gtk::HBox());
  Gtk::VBox*       axis_labels_box     = manage(new Gtk::VBox());
  Gtk::VBox*       axis_spinners_box   = manage(new Gtk::VBox());
  Gtk::HButtonBox* controls_button_bar = manage(new Gtk::HButtonBox());
                   activate_button     = manage(new Gtk::Button("On"));
                   recentre_button     = manage(new Gtk::Button("Recentre"));
                   origin_button       = manage(new Gtk::Button("Set Origin"));
		   vergeleft_spinner   = manage(new Gtk::SpinButton());
		   vergeright_spinner  = manage(new Gtk::SpinButton());
		   elevation_spinner   = manage(new Gtk::SpinButton());
		   pan_spinner         = manage(new Gtk::SpinButton());
		   drag_pad            = manage(new VW::YorickDragPad(parent));

  // Build the axes box
  axes_box->set_border_width(3);
  
  axis_labels_box->pack_start(*manage(new Gtk::Label("Verge left")));
  axis_labels_box->pack_start(*manage(new Gtk::Label("Verge right")));
  axis_labels_box->pack_start(*manage(new Gtk::Label("Elevation")));
  axis_labels_box->pack_start(*manage(new Gtk::Label("Pan")));

  axis_spinners_box->pack_start(*vergeleft_spinner);
  axis_spinners_box->pack_start(*vergeright_spinner);
  axis_spinners_box->pack_start(*elevation_spinner);
  axis_spinners_box->pack_start(*pan_spinner);

  // Set spinner limits
  Gtk::Adjustment* ad;
  //   - Vergeleft
  ad=vergeleft_spinner->get_adjustment();
  ad->set_lower(-VW::Yorick::VLEFT_LIMIT);
  ad->set_upper(VW::Yorick::VLEFT_LIMIT);
  ad->set_value(0);
  ad->set_step_increment(1);
  ad->set_page_increment(5);
  ad->value_changed.connect(slot(parent,&YorickGTK::i_VergeLeft));
  //   - Vergeright
  ad=vergeright_spinner->get_adjustment();
  ad->set_lower(-VW::Yorick::VRIGHT_LIMIT);
  ad->set_upper(VW::Yorick::VRIGHT_LIMIT);
  ad->set_value(0);
  ad->set_step_increment(1);
  ad->set_page_increment(5);
  ad->value_changed.connect(slot(parent,&YorickGTK::i_VergeRight));
  //   - Elevation
  ad=elevation_spinner->get_adjustment();
  ad->set_lower(-VW::Yorick::TILT_LIMIT);
  ad->set_upper(VW::Yorick::TILT_LIMIT);
  ad->set_value(0);
  ad->set_step_increment(1);
  ad->set_page_increment(5);
  ad->value_changed.connect(slot(parent,&YorickGTK::i_Elevate));
  //   - Pan
  ad=pan_spinner->get_adjustment();
  ad->set_lower(-VW::Yorick::PAN_LIMIT);
  ad->set_upper(VW::Yorick::PAN_LIMIT);
  ad->set_value(0);
  ad->set_step_increment(1);
  ad->set_page_increment(5);
  ad->value_changed.connect(slot(parent,&YorickGTK::i_Pan));


  // Set up the drag-pad
  drag_pad->activate ();
  drag_pad->set_usize (_DEFAULT_CONTROL_WIDGET_SIZE_X,
		      _DEFAULT_CONTROL_WIDGET_SIZE_Y);
  drag_frame->add(*drag_pad);

  // Build the HBox
  axes_box->pack_start(*axis_labels_box);
  axes_box->pack_start(*axis_spinners_box);
  axes_box->pack_end(*drag_frame);


  // Build the button bar
  controls_button_bar->pack_end(*activate_button);
  controls_button_bar->pack_end(*recentre_button);
  controls_button_bar->pack_end(*origin_button);

  // Base class provides the toolbar
  CreateButtonBar<VW::YorickGTKWidget>(parent,false);

  // Stick things on
  axes_box->set_border_width(3);
  axes_frame->set_border_width(3);
  axes_frame->add(*axes_box);
  main_vbox->pack_start(*button_bar);
  main_vbox->pack_start(*axes_frame);
  main_vbox->pack_start(*controls_button_bar);

  activate_button->clicked.connect(SigC::slot(parent,&VW::YorickGTK::i_Activate));
  recentre_button->clicked.connect(SigC::slot(parent,&VW::YorickGTK::i_Recentre));
  origin_button->clicked.connect(SigC::slot(parent,&VW::YorickGTK::i_SetOrigin));
  this->add(*main_vbox);

  // Set activation by calling the activate function
  //DisableForm();

  show();

}

// }}}

// {{{ void VW::YorickGTKWidget::EnableForm()

void VW::YorickGTKWidget::EnableForm() {
  // Make sure all buttons are enabled
  recentre_button->set_sensitive(true);
  origin_button->set_sensitive(true);
  vergeleft_spinner->set_sensitive(true);
  vergeright_spinner->set_sensitive(true);
  elevation_spinner->set_sensitive(true);
  pan_spinner->set_sensitive(true);
  drag_pad->set_sensitive(true);
}

// }}}

// {{{ void VW::YorickGTKWidget::DisableForm()

void VW::YorickGTKWidget::DisableForm() {
  // Make sure all buttons are enabled
  recentre_button->set_sensitive(false);
  origin_button->set_sensitive(false);
  vergeleft_spinner->set_sensitive(false);
  vergeright_spinner->set_sensitive(false);
  elevation_spinner->set_sensitive(false);
  pan_spinner->set_sensitive(false);
  drag_pad->set_sensitive(false);
}

// }}}

//******* Main class functions *******//



VW::YorickGTK::YorickGTK(VW::InterfaceType t) {
  my_type=t;
  my_name="Yorick";
  if (t==INTERFACE_AS_BUTTONWINDOW) {
    InitButtonWindow();
  } else {
    InitWidget();
  }
  my_widget->InitForm(this);
  // Set callbacks to active
  allow_callbacks=true;

}
VW::YorickGTK::~YorickGTK() {
  cerr << "YorickGtk: Goodbye." << endl;
}



void VW::YorickGTK::i_Activate() {
  if (GetActive()) {
    // Currently active, so switch off
    cerr << "Off!" << endl;
    SetActive(false);
    my_widget->DisableForm();
  } else {
    // Currently inactive, so switch on
    cerr << "On!" << endl;
    SetActive(true);
    my_widget->EnableForm();
    
  }

}
void VW::YorickGTK::UpdateForm() {
  // Stop the callbacks during the update
  allow_callbacks=false;
  // Set spinners based on odometry
  my_widget->vergeleft_spinner->set_value(GetVergeLeft());
  my_widget->vergeright_spinner->set_value(GetVergeRight());
  my_widget->elevation_spinner->set_value(GetTilt());
  my_widget->pan_spinner->set_value(GetPan());
  // Redraw the drag-pad
  my_widget->drag_pad->Redraw();
  // Enable the callbacks
  allow_callbacks=true;
}
void VW::YorickGTK::i_Recentre() {
  cerr << "Recentre" << endl;
  Yorick::SlowRecentre();
  UpdateForm();
}
void VW::YorickGTK::i_SetOrigin() {
  cerr << "Set origin" << endl;
  Yorick::Homez();
  UpdateForm();
}
void VW::YorickGTK::i_VergeLeft() {
  if (allow_callbacks) {
  cerr << "Verge left" << endl;
  Yorick::DemandVergeLeft(my_widget->vergeleft_spinner->get_value_as_float());
  my_widget->drag_pad->Redraw();
  }
}
void VW::YorickGTK::i_VergeRight() {
  if (allow_callbacks) {
  cerr << "Verge right" << endl;
  Yorick::DemandVergeRight(my_widget->vergeright_spinner->get_value_as_float());
  my_widget->drag_pad->Redraw();
}
}
void VW::YorickGTK::i_Elevate() {
  if (allow_callbacks) {
  cerr << "Elevate" << endl;
  Yorick::DemandTilt(my_widget->elevation_spinner->get_value_as_float());
  my_widget->drag_pad->Redraw();
}
}
void VW::YorickGTK::i_Pan() {
  if (allow_callbacks) {
  cerr << "Pan" << endl;
  Yorick::DemandPan(my_widget->pan_spinner->get_value_as_float());
  my_widget->drag_pad->Redraw();
}
}


// Send a complete set of demands (in degrees)
// {{{ int Demand(new_pan, new_tilt, new_vergeleft, new_vergeright)

int VW::YorickGTK::Demand(float new_pan, float new_tilt, float new_vergeleft, float new_vergeright) {
  int status=Yorick::Demand(new_pan, new_tilt, 
			    new_vergeleft, new_vergeright);
  cerr << "YorickGTK::Demand - Update widgets. " << endl;
//   allow_callbacks=false;
//   my_widget->elevation_spinner->set_value(new_tilt);
//   my_widget->pan_spinner->set_value(new_pan);
//   my_widget->vergeleft_spinner->set_value(new_vergeleft);
//   my_widget->vergeright_spinner->set_value(new_vergeright);
//   allow_callbacks=true;
//   my_widget->drag_pad->Redraw();
  cerr << "YorickGTK::Demand - Done" << endl;
  return(status);
}

// }}}

// Demand one or more axes individually (in degrees)
// {{{ int VW::YorickGTK::DemandPanTilt(float new_pan, float new_tilt)

int VW::YorickGTK::DemandPanTilt(float new_pan, float new_tilt) {
  int status=Yorick::DemandPanTilt(new_pan, new_tilt);
  allow_callbacks=false;
  my_widget->elevation_spinner->set_value(new_tilt);
  my_widget->pan_spinner->set_value(new_pan);
  allow_callbacks=true;
  my_widget->drag_pad->Redraw();
  return(status);
}

// }}}
// {{{ int VW::YorickGTK::DemandTilt(float new_tilt)

int VW::YorickGTK::DemandTilt(float new_tilt) {
  int status=Yorick::DemandTilt(new_tilt);
  allow_callbacks=false;
  my_widget->elevation_spinner->set_value(new_tilt);
  allow_callbacks=true;
  my_widget->drag_pad->Redraw();
  return(status);
}

// }}}
// {{{ int VW::YorickGTK::DemandPan(float new_pan)

int VW::YorickGTK::DemandPan(float new_pan) {
  int status=Yorick::DemandPan(new_pan);
  allow_callbacks=false;
  my_widget->pan_spinner->set_value(new_pan);
  allow_callbacks=true;
  my_widget->drag_pad->Redraw();
  return(status);
}

// }}}
// {{{ int VW::YorickGTK::DemandVerge(float new_vergeleft, float new_vergeright)

int VW::YorickGTK::DemandVerge(float new_vergeleft, float new_vergeright)
{
  int status=Yorick::DemandVerge(new_vergeleft,new_vergeright);
  allow_callbacks=false;
  my_widget->vergeleft_spinner->set_value(new_vergeleft);
  my_widget->vergeright_spinner->set_value(new_vergeright);
  allow_callbacks=true;
  my_widget->drag_pad->Redraw();
  return(status);
}

// }}}
// {{{ int VW::YorickGTK::DemandVergeLeft(float new_vergeleft)

int VW::YorickGTK::DemandVergeLeft(float new_vergeleft) {
  int status=Yorick::DemandVergeLeft(new_vergeleft);
//   allow_callbacks=false;
//   my_widget->vergeleft_spinner->set_value(new_vergeleft);
//   allow_callbacks=true;
//   my_widget->drag_pad->Redraw();
  return(status);
}

// }}}
// {{{ int VW::YorickGTK::DemandVergeRight(float new_vergeright)

int VW::YorickGTK::DemandVergeRight(float new_vergeright) {
  int status=Yorick::DemandVergeRight(new_vergeright);
  allow_callbacks=false;
  my_widget->vergeright_spinner->set_value(new_vergeright);
  allow_callbacks=true;
  my_widget->drag_pad->Redraw();
  return(status);
}

// }}}
