/** 
 * @file   firewirecameragtk.cpp
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


#include "firewirecameragtk.h"
#include <sstream>
#include <gtk--.h>

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false


using namespace std;



//******* Widget functions *******//

// {{{ FirewireCameraGTKWidget::InitForm()

void VW::FirewireCameraGTKWidget::InitForm(FirewireCameraGTK* parent) {
  if (VERBOSE) cerr << "Create camera form" << endl;
  unsigned int row=0;

  this->set_label("Firewire Camera");

  // Global table
  Gtk::Table* main_table = manage(new Gtk::Table(3,1, true));
  main_table->set_homogeneous(false);
  main_table->set_row_spacings(5);

  // Create notebook to hold pages
                   tabbed_pages  = manage(new Gtk::Notebook());

  /***** Page 1 *****/
  if (VERBOSE) cerr << "   Page 1" << endl;
                   about_text    = manage(new Gtk::Text());
		   about_text->set_line_wrap(false);
		   tooltips.set_tip(*about_text, "Details of the camera and settings.", "");
  /***** Page 2 *****/
  if (VERBOSE) cerr << "   Page 2" << endl;

  // Table for page 2
  Gtk::Table*      layout_table2 = manage(new Gtk::Table(3,3));

  // Create page 2 widgets (spinners defined in header)
  zoom_spinner     = manage(new Gtk::SpinButton(1,0));
  zoom_label       = manage(new Gtk::Label("Zoom:"));
  tooltips.set_tip(*zoom_spinner, "Change the zoom setting.", "");

  focus_spinner    = manage(new Gtk::SpinButton(1,0));
  focus_label      = manage(new Gtk::Label("Focus:"));
  tooltips.set_tip(*focus_spinner, "Change the focus setting.", "");

  autofocus_button  = manage(new Gtk::CheckButton("Auto"));
  autofocus_button->set_active(true);
  tooltips.set_tip(*autofocus_button, "Get the camera to automatically set the focus using the image. This can seriously screw up some computer vision algorithms.", "");

  // Buttons to set all options to manual/auto
  Gtk::Button*     auto_button      = manage(new Gtk::Button("All Auto"));
  tooltips.set_tip(*auto_button, "Change all settings that can be automatic to automatic.", "");
  Gtk::Button*     manual_button    = manage(new Gtk::Button("All Manual"));
  tooltips.set_tip(*manual_button, "Change all settings to manual.", "");
  Gtk::HButtonBox* automanual_bar   = manage(new Gtk::HButtonBox());

  automanual_bar->pack_end(*auto_button);
  automanual_bar->pack_end(*manual_button);

 // Add to page 2 table
  row=0;
  if (parent->HasZoom()) {
    layout_table2->attach(*zoom_label,          0,1,row,row+1);
    layout_table2->attach(*zoom_spinner,        1,2,row,row+1);
    row++;
  }
  if (parent->HasFocus()) {
    layout_table2->attach(*focus_label,          0,1,row,row+1);
    layout_table2->attach(*focus_spinner,        1,2,row,row+1);
    if (parent->HasAutoFocus())
      layout_table2->attach(*autofocus_button,   2,3,row,row+1);
    row++;
  }
  layout_table2->attach(*automanual_bar,      0,2,row,row+1);




  /***** Page 3 *****/
  if (VERBOSE) cerr << "   Page 3" << endl;

  // Table for page 3
  Gtk::Table*      layout_table3 = manage(new Gtk::Table(3,3));

  // Create page 3 widgets
  iris_spinner     = manage(new Gtk::SpinButton(1,0));
  iris_label       = manage(new Gtk::Label("Iris:"));
  tooltips.set_tip(*iris_spinner, "Change the iris setting. Use this, the iris and gain settings together to control image brightness.", "");

  autoiris_button  = manage(new Gtk::CheckButton("Auto"));
  autoiris_button->set_active(true);
  tooltips.set_tip(*autoiris_button, "Get the camera to automatically set the iris according to image brightness. This can seriously screw up some computer vision algorithms.", "");

  brightness_spinner     = manage(new Gtk::SpinButton(1,0));
  brightness_label       = manage(new Gtk::Label("Brightness:"));
  tooltips.set_tip(*brightness_spinner, "Change the brightness setting. Use this, the iris and brightness settings together to control image brightness.", "");

  autobrightness_button  = manage(new Gtk::CheckButton("Auto"));
  autobrightness_button->set_active(true);
  tooltips.set_tip(*autobrightness_button, "Get the camera to automatically set the brightness according to image brightness. This can seriously screw up some computer vision algorithms.", "");
  
  gain_spinner     = manage(new Gtk::SpinButton(1,0));
  gain_label       = manage(new Gtk::Label("Gain:"));
  tooltips.set_tip(*gain_spinner, "Change the gain setting. Use this, the iris and gain settings together to control image brightness.", "");

  autogain_button  = manage(new Gtk::CheckButton("Auto"));
  autogain_button->set_active(true);
  tooltips.set_tip(*autogain_button, "Get the camera to automatically set the gain according to image brightness. This can seriously screw up some computer vision algorithms.", "");
  
  exposure_spinner = manage(new Gtk::SpinButton(1,0));
  exposure_label   = manage(new Gtk::Label("Exposure:"));
  tooltips.set_tip(*exposure_spinner, "Change the exposure setting. Use this, the iris and gain settings together to control image brightness.", "");
  
  autoexposure_button  = manage(new Gtk::CheckButton("Auto"));
  tooltips.set_tip(*autoexposure_button, "Get the camera to automatically set the exposure according to image brightness. This can seriously screw up some computer vision algorithms.", "");

  shutter_spinner = manage(new Gtk::SpinButton(1,0));
  shutter_label   = manage(new Gtk::Label("Shutter:"));
  tooltips.set_tip(*shutter_spinner, "Change the shutter setting. Use this, the iris and gain settings together to control image brightness.", "");
  
  autoshutter_button  = manage(new Gtk::CheckButton("Auto"));
  tooltips.set_tip(*autoshutter_button, "Get the camera to automatically set the shutter according to image brightness. This can seriously screw up some computer vision algorithms.", "");

 // Add to page 3 table
  row=0;
  if (parent->HasIris()) {
    layout_table3->attach(*iris_label,          0,1,row,row+1);
    layout_table3->attach(*iris_spinner,        1,2,row,row+1);
    if (parent->HasAutoIris())
      layout_table3->attach(*autoiris_button,   2,3,row,row+1);
    row++;
  }
  if (parent->HasBrightness()) {
    layout_table3->attach(*brightness_label,          0,1,row,row+1);
    layout_table3->attach(*brightness_spinner,        1,2,row,row+1);
    if (parent->HasAutoBrightness())
      layout_table3->attach(*autobrightness_button,   2,3,row,row+1);
    row++;
  }
  if (parent->HasGain()) {
    layout_table3->attach(*gain_label,          0,1,row,row+1);
    layout_table3->attach(*gain_spinner,        1,2,row,row+1);
    if (parent->HasAutoGain())
      layout_table3->attach(*autogain_button,   2,3,row,row+1);
    row++;
  }
  if (parent->HasExposure()) {
    layout_table3->attach(*exposure_label,          0,1,row,row+1);
    layout_table3->attach(*exposure_spinner,        1,2,row,row+1);
    if (parent->HasAutoExposure())
      layout_table3->attach(*autoexposure_button,   2,3,row,row+1);
    row++;
  }
  if (parent->HasShutter()) {
    layout_table3->attach(*shutter_label,          0,1,row,row+1);
    layout_table3->attach(*shutter_spinner,        1,2,row,row+1);
    if (parent->HasAutoShutter())
      layout_table3->attach(*autoshutter_button,   2,3,row,row+1);
    row++;
  }



  /***** Page 4 *****/
  if (VERBOSE) cerr << "   Page 4" << endl;

  // Table for page 4
  Gtk::Table*      layout_table4 = manage(new Gtk::Table(1,3));

  // Create page 4 widgets

  gamma_spinner = manage(new Gtk::SpinButton(1,0));
  gamma_label   = manage(new Gtk::Label("Gamma:"));
  tooltips.set_tip(*gain_spinner, "Change the gamma setting.", "");
  
  autogamma_button  = manage(new Gtk::CheckButton("Auto"));
  tooltips.set_tip(*autogamma_button, "Get the camera to automatically set the gamma according to image colour. This can seriously screw up some computer vision algorithms.", "");

  sharpness_spinner = manage(new Gtk::SpinButton(1,0));
  sharpness_label   = manage(new Gtk::Label("Sharpness:"));
  tooltips.set_tip(*gain_spinner, "Change the sharpness setting.", "");
  
  autosharpness_button  = manage(new Gtk::CheckButton("Auto"));
  tooltips.set_tip(*autosharpness_button, "Get the camera to automatically set the sharpness according to image colour. This can seriously screw up some computer vision algorithms.", "");

  hue_spinner = manage(new Gtk::SpinButton(1,0));
  hue_label   = manage(new Gtk::Label("Hue:"));
  tooltips.set_tip(*gain_spinner, "Change the hue setting. Use this, the iris and gain settings together to control image brightness.", "");
  
  autohue_button  = manage(new Gtk::CheckButton("Auto"));
  tooltips.set_tip(*autohue_button, "Get the camera to automatically set the hue according to image colour. This can seriously screw up some computer vision algorithms.", "");

  saturation_spinner = manage(new Gtk::SpinButton(1,0));
  saturation_label   = manage(new Gtk::Label("Saturation:"));
  tooltips.set_tip(*saturation_spinner, "Change the saturation setting. Use this, the iris and gain settings together to control image brightness.", "");
  
  autosaturation_button  = manage(new Gtk::CheckButton("Auto"));
  tooltips.set_tip(*autosaturation_button, "Get the camera to automatically set the saturation according to image colour. This can seriously screw up some computer vision algorithms.", "");

  whitebalanceU_spinner     = manage(new Gtk::SpinButton(1,0));
  tooltips.set_tip(*whitebalanceU_spinner, "Change the whitebalance U (redness) setting.", "");
  whitebalanceV_spinner     = manage(new Gtk::SpinButton(1,0));
  tooltips.set_tip(*whitebalanceU_spinner, "Change the whitebalance V (blueness) setting.", "");

  whitebalance_label   = manage(new Gtk::Label("White balance:"));
  whitebalanceU_label   = manage(new Gtk::Label("U"));
  whitebalanceV_label   = manage(new Gtk::Label("V"));
  autowhitebalance_button  = manage(new Gtk::CheckButton("Auto"));
  tooltips.set_tip(*autowhitebalance_button, "Get the camera to automaticaal set the white-balance according to image colour. This can seriously screw up some computer vision algorithms.", "");

 // Add to page 4 table
  row=0;
  if (parent->HasGamma()) {
    layout_table4->attach(*gamma_label,          0,1,row,row+1);
    layout_table4->attach(*gamma_spinner,        1,2,row,row+1);
    if (parent->HasAutoGamma())
      layout_table4->attach(*autogamma_button,   2,3,row,row+1);
    row++;
  }

  if (parent->HasSharpness()) {
    layout_table4->attach(*sharpness_label,          0,1,row,row+1);
    layout_table4->attach(*sharpness_spinner,        1,2,row,row+1);
    if (parent->HasAutoSharpness())
      layout_table4->attach(*autosharpness_button,   2,3,row,row+1);
    row++;
  }

  if (parent->HasHue()) {
    layout_table4->attach(*hue_label,          0,1,row,row+1);
    layout_table4->attach(*hue_spinner,        1,2,row,row+1);
    if (parent->HasAutoHue())
      layout_table4->attach(*autohue_button,   2,3,row,row+1);
    row++;
  }

  if (parent->HasSaturation()) {
    layout_table4->attach(*saturation_label,          0,1,row,row+1);
    layout_table4->attach(*saturation_spinner,        1,2,row,row+1);
    if (parent->HasAutoSaturation())
      layout_table4->attach(*autosaturation_button,   2,3,row,row+1);
    row++;
  }

  if (parent->HasWhiteBalance()) {
    layout_table4->attach(*whitebalance_label,          0,1,row,row+1);
    Gtk::Table* whitebalance_spinner_box = manage(new Gtk::Table(2,2));
    whitebalance_spinner_box->attach(*whitebalanceU_label,0,1,0,1);
    whitebalance_spinner_box->attach(*whitebalanceV_label,0,1,1,2);
    whitebalance_spinner_box->attach(*whitebalanceU_spinner,1,2,0,1);
    whitebalance_spinner_box->attach(*whitebalanceV_spinner,1,2,1,2);
    layout_table4->attach(*whitebalance_spinner_box,    1,2,row,row+1);
    if (parent->HasAutoWhiteBalance())
      layout_table4->attach(*autowhitebalance_button,   2,3,row,row+1);
    row++;
  }

  /***** Main *****/
  if (VERBOSE) cerr << "   Add it all together" << endl;

  // Add pages to notebook
  tabbed_pages->pages().push_back(Gtk::Notebook_Helpers::TabElem(*about_text,"About"));
  tabbed_pages->pages().push_back(Gtk::Notebook_Helpers::TabElem(*layout_table2,"General"));
  tabbed_pages->pages().push_back(Gtk::Notebook_Helpers::TabElem(*layout_table3,"Brightness"));
  tabbed_pages->pages().push_back(Gtk::Notebook_Helpers::TabElem(*layout_table4,"Colour"));


  // Base class provides the toolbar
  CreateButtonBar<VW::FirewireCameraGTKWidget>(parent);

  Gtk::VBox* main_vbox=manage(new Gtk::VBox());
  main_vbox->pack_start(*button_bar);
  main_vbox->pack_start(*tabbed_pages);

  this->add(*main_vbox);

  //  if (verbose) cerr << "Attach some callbacks" << endl;

  // Attach callbacks
  tabbed_pages->switch_page.connect(SigC::slot(parent,&FirewireCameraGTK::PageChange));
  autoiris_button->clicked.connect(SigC::slot(parent,&FirewireCameraGTK::ChangeAutoIris));
  autoexposure_button->clicked.connect(SigC::slot(parent,&FirewireCameraGTK::ChangeAutoExposure));
  autofocus_button->clicked.connect(SigC::slot(parent,&FirewireCameraGTK::ChangeAutoFocus));
  autoshutter_button->clicked.connect(SigC::slot(parent,&FirewireCameraGTK::ChangeAutoShutter));
  autobrightness_button->clicked.connect(SigC::slot(parent,&FirewireCameraGTK::ChangeAutoBrightness));
  autogain_button->clicked.connect(SigC::slot(parent,&FirewireCameraGTK::ChangeAutoGain));
  autogamma_button->clicked.connect(SigC::slot(parent,&FirewireCameraGTK::ChangeAutoGamma));
  autosharpness_button->clicked.connect(SigC::slot(parent,&FirewireCameraGTK::ChangeAutoSharpness));
  autohue_button->clicked.connect(SigC::slot(parent,&FirewireCameraGTK::ChangeAutoHue));
  autosaturation_button->clicked.connect(SigC::slot(parent,&FirewireCameraGTK::ChangeAutoSaturation));
  autowhitebalance_button->clicked.connect(SigC::slot(parent,&FirewireCameraGTK::ChangeAutoWhiteBalance));
  auto_button->clicked.connect(SigC::slot(parent,&FirewireCameraGTK::UseAutoSettings));
  manual_button->clicked.connect(SigC::slot(parent,&FirewireCameraGTK::UseManualSettings));
  Gtk::Adjustment* ad;
  ad=zoom_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&FirewireCameraGTK::ChangeZoom));
  ad=focus_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&FirewireCameraGTK::ChangeFocus));
  ad=iris_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&FirewireCameraGTK::ChangeIris));
  ad=brightness_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&FirewireCameraGTK::ChangeBrightness));
  ad=gain_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&FirewireCameraGTK::ChangeGain));
  ad=exposure_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&FirewireCameraGTK::ChangeExposure));
  ad=shutter_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&FirewireCameraGTK::ChangeShutter));
  ad=gamma_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&FirewireCameraGTK::ChangeGamma));
  ad=sharpness_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&FirewireCameraGTK::ChangeSharpness));
  ad=hue_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&FirewireCameraGTK::ChangeHue));
  ad=saturation_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&FirewireCameraGTK::ChangeSaturation));
  ad=whitebalanceU_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&FirewireCameraGTK::ChangeWhiteBalance));
  ad=whitebalanceV_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&FirewireCameraGTK::ChangeWhiteBalance));

  // By default show the second page
  tabbed_pages->next_page();

  //  if (verbose) cerr << "Done camera form: my node="<<camera_info.id << endl;


}

// }}}


//******* Main class functions *******//

// {{{ FirewireCameraGTK::FirewireCameraGTK(bool verbosity)

VW::FirewireCameraGTK::FirewireCameraGTK(VW::InterfaceType t,
					 bool verbosity)
  : VW::FirewireCamera(verbosity) 
{
  my_type=t;
  my_name="Firewire camera";
  if (t==INTERFACE_AS_BUTTONWINDOW) {
    InitButtonWindow();
  } else {
    InitWidget();
  }
}

// }}}


// {{{ bool FirewireCameraGTK::SetUpCamera(raw1394handle_t port_handle)

bool VW::FirewireCameraGTK::SetUpCamera(raw1394handle_t port_handle) {
  VW::FirewireCamera::SetUpCamera(port_handle);
  my_widget->InitForm(this);
  UpdateForm();
  return(true);
}

// }}}

// {{{ void FirewireCameraGTK::SetAboutText(void)

void VW::FirewireCameraGTK::SetAboutText(void) {
  ostringstream camera_description;

  // Set about text
  camera_description << (*this);
  Gtk::Text_Helpers::Context c = my_widget->about_text->get_context();
  c.set_font(Gdk_Font("-bitstream-courier-medium-r-normal-*-*-80-*-*-m-*-iso8859-9"));
  // delete the current contents
  int l=my_widget->about_text->get_length();
  my_widget->about_text->set_point(0);
  my_widget->about_text->forward_delete(l);
  // insert the new
  my_widget->about_text->insert(c,camera_description.str());
}

// }}}

// {{{ void FirewireCameraGTK::UpdateForm(void)

void VW::FirewireCameraGTK::UpdateForm(void) {
  int min_val=0,max_val=0,_val=0,_val2=0;
  Gtk::Adjustment* adj;

  // Set spinner limits
  // Zoom
  adj=my_widget->zoom_spinner->get_adjustment();
  this->GetZoomLimits(&min_val, &max_val);
  _val=this->GetZoom();
  adj->set_lower(min_val);
  adj->set_upper(max_val);
  adj->set_value(_val);
  adj->set_step_increment(1.0);
  adj->set_page_increment(25.0);

  // Focus
  adj=my_widget->focus_spinner->get_adjustment();
  this->GetFocusLimits(&min_val, &max_val);
  _val=this->GetFocus();
  adj->set_lower(min_val);
  adj->set_upper(max_val);
  adj->set_value(_val);
  adj->set_step_increment(1.0);
  adj->set_page_increment(25.0);

  my_widget->autofocus_button->set_active(this->GetAutoFocus());
  SetAutoFocus(GetAutoFocus());

  // Iris
  adj=my_widget->iris_spinner->get_adjustment();
  this->GetIrisLimits(&min_val, &max_val);
  _val=this->GetIris();
  adj->set_lower(min_val);
  adj->set_upper(max_val);
  adj->set_value(_val);  
  adj->set_step_increment(1.0);
  adj->set_page_increment(25.0);

  my_widget->autoiris_button->set_active(this->GetAutoIris());
  SetAutoIris(GetAutoIris());

  // Brightness
  adj=my_widget->brightness_spinner->get_adjustment();
  this->GetBrightnessLimits(&min_val, &max_val);
  _val=this->GetBrightness();
  adj->set_lower(min_val);
  adj->set_upper(max_val);
  adj->set_value(_val);  
  adj->set_step_increment(1.0);
  adj->set_page_increment(5.0);

  my_widget->autobrightness_button->set_active(this->GetAutoBrightness());
  SetAutoBrightness(GetAutoBrightness());

  // Gain
  adj=my_widget->gain_spinner->get_adjustment();
  this->GetGainLimits(&min_val, &max_val);
  _val=this->GetGain();
  adj->set_lower(min_val);
  adj->set_upper(max_val);
  adj->set_value(_val);  
  adj->set_step_increment(1.0);
  adj->set_page_increment(5.0);

  my_widget->autogain_button->set_active(this->GetAutoGain());
  SetAutoGain(GetAutoGain());

  // Exposure
  adj=my_widget->exposure_spinner->get_adjustment();
  this->GetExposureLimits(&min_val, &max_val);
  _val=this->GetExposure();
  adj->set_lower(min_val);
  adj->set_upper(max_val);
  adj->set_value(_val);  
  adj->set_step_increment(1.0);
  adj->set_page_increment(5.0);

  my_widget->autoexposure_button->set_active(this->GetAutoExposure());
  SetAutoExposure(GetAutoExposure());

  // Shutter
  adj=my_widget->shutter_spinner->get_adjustment();
  this->GetShutterLimits(&min_val, &max_val);
  _val=this->GetShutter();
  adj->set_lower(min_val);
  adj->set_upper(max_val);
  adj->set_value(_val);  
  adj->set_step_increment(1.0);
  adj->set_page_increment(5.0);

  my_widget->autoshutter_button->set_active(this->GetAutoShutter());
  SetAutoShutter(GetAutoShutter());

  // Gamma
  adj=my_widget->gamma_spinner->get_adjustment();
  this->GetGammaLimits(&min_val, &max_val);
  _val=this->GetGamma();
  adj->set_lower(min_val);
  adj->set_upper(max_val);
  adj->set_value(_val);  
  adj->set_step_increment(1.0);
  adj->set_page_increment(5.0);

  my_widget->autogamma_button->set_active(this->GetAutoGamma());
  SetAutoGamma(GetAutoGamma());

  // Sharpness
  adj=my_widget->sharpness_spinner->get_adjustment();
  this->GetSharpnessLimits(&min_val, &max_val);
  _val=this->GetSharpness();
  adj->set_lower(min_val);
  adj->set_upper(max_val);
  adj->set_value(_val);  
  adj->set_step_increment(1.0);
  adj->set_page_increment(5.0);

  my_widget->autosharpness_button->set_active(this->GetAutoSharpness());
  SetAutoSharpness(GetAutoSharpness());

  // Hue
  adj=my_widget->hue_spinner->get_adjustment();
  this->GetHueLimits(&min_val, &max_val);
  _val=this->GetHue();
  adj->set_lower(min_val);
  adj->set_upper(max_val);
  adj->set_value(_val);  
  adj->set_step_increment(1.0);
  adj->set_page_increment(5.0);

  my_widget->autohue_button->set_active(this->GetAutoHue());
  SetAutoHue(GetAutoHue());

  // Saturation
  adj=my_widget->saturation_spinner->get_adjustment();
  this->GetSaturationLimits(&min_val, &max_val);
  _val=this->GetSaturation();
  adj->set_lower(min_val);
  adj->set_upper(max_val);
  adj->set_value(_val);  
  adj->set_step_increment(1.0);
  adj->set_page_increment(5.0);

  my_widget->autosaturation_button->set_active(this->GetAutoSaturation());
  SetAutoSaturation(GetAutoSaturation());

  // White Balance
  adj=my_widget->whitebalanceU_spinner->get_adjustment();
  this->GetWhiteBalanceULimits(&min_val, &max_val);
  _val=this->GetWhiteBalanceU();
  _val2=this->GetWhiteBalanceV();
  adj->set_lower(min_val);
  adj->set_upper(max_val);
  adj->set_value(_val);  
  adj->set_step_increment(1.0);
  adj->set_page_increment(5.0);

  adj=my_widget->whitebalanceV_spinner->get_adjustment();
  this->GetWhiteBalanceVLimits(&min_val, &max_val);
  adj->set_lower(min_val);
  adj->set_upper(max_val);
  adj->set_value(_val2);  
  adj->set_step_increment(1.0);
  adj->set_page_increment(5.0);

  my_widget->autowhitebalance_button->set_active(this->GetAutoWhiteBalance());
  SetAutoWhiteBalance(GetAutoWhiteBalance());

}

// }}}

// {{{ Set functions can be overloaded by interfaces

void VW::FirewireCameraGTK::SetZoom(int new_zoom) {
  VW::FirewireCamera::SetZoom(new_zoom);
  Gtk::Adjustment* g=my_widget->zoom_spinner->get_adjustment();
  g->set_value(new_zoom);
}
void VW::FirewireCameraGTK::SetFocus(int new_focus) {
  VW::FirewireCamera::SetFocus(new_focus);
  Gtk::Adjustment* g=my_widget->focus_spinner->get_adjustment();
  g->set_value(new_focus);
}
void VW::FirewireCameraGTK::SetIris(int new_iris) {
  VW::FirewireCamera::SetIris(new_iris);
  Gtk::Adjustment* g=my_widget->iris_spinner->get_adjustment();
  g->set_value(new_iris);
}
void VW::FirewireCameraGTK::SetGain(int new_gain) {
  VW::FirewireCamera::SetGain(new_gain);
  Gtk::Adjustment* g=my_widget->gain_spinner->get_adjustment();
  g->set_value(new_gain);
}
void VW::FirewireCameraGTK::SetBrightness(int new_brightness) {
  VW::FirewireCamera::SetBrightness(new_brightness);
  Gtk::Adjustment* g=my_widget->brightness_spinner->get_adjustment();
  g->set_value(new_brightness);
}
void VW::FirewireCameraGTK::SetExposure(int new_exposure) {
  VW::FirewireCamera::SetExposure(new_exposure);
  Gtk::Adjustment* g=my_widget->exposure_spinner->get_adjustment();
  g->set_value(new_exposure);
}
void VW::FirewireCameraGTK::SetShutter(int new_shutter) {
  VW::FirewireCamera::SetShutter(new_shutter);
  Gtk::Adjustment* g=my_widget->shutter_spinner->get_adjustment();
  g->set_value(new_shutter);
}
void VW::FirewireCameraGTK::SetGamma(int new_gamma) {
  VW::FirewireCamera::SetGamma(new_gamma);
  Gtk::Adjustment* g=my_widget->gamma_spinner->get_adjustment();
  g->set_value(new_gamma);
}
void VW::FirewireCameraGTK::SetSharpness(int new_sharpness) {
  VW::FirewireCamera::SetSharpness(new_sharpness);
  Gtk::Adjustment* g=my_widget->sharpness_spinner->get_adjustment();
  g->set_value(new_sharpness);
}
void VW::FirewireCameraGTK::SetHue(int new_hue) {
  VW::FirewireCamera::SetHue(new_hue);
  Gtk::Adjustment* g=my_widget->hue_spinner->get_adjustment();
  g->set_value(new_hue);
}
void VW::FirewireCameraGTK::SetSaturation(int new_saturation) {
  VW::FirewireCamera::SetSaturation(new_saturation);
  Gtk::Adjustment* g=my_widget->saturation_spinner->get_adjustment();
  g->set_value(new_saturation);
}
void VW::FirewireCameraGTK::SetWhiteBalance(int new_U, int new_V)
{
  VW::FirewireCamera::SetWhiteBalance(new_U, new_V);
  Gtk::Adjustment* g=my_widget->whitebalanceU_spinner->get_adjustment();
  g->set_value(new_U);
  g=my_widget->whitebalanceV_spinner->get_adjustment();
  g->set_value(new_V);
}


// Auto stuff
void VW::FirewireCameraGTK::SetAutoIris(bool new_state) {
  VW::FirewireCamera::SetAutoIris(new_state);
  my_widget->autoiris_button->set_active(new_state);
}
void VW::FirewireCameraGTK::SetAutoBrightness(bool new_state) {
  VW::FirewireCamera::SetAutoBrightness(new_state);
  my_widget->autobrightness_button->set_active(new_state);
}
void VW::FirewireCameraGTK::SetAutoGain(bool new_state) {
  VW::FirewireCamera::SetAutoGain(new_state);
  my_widget->autogain_button->set_active(new_state);
}
void VW::FirewireCameraGTK::SetAutoFocus(bool new_state) {
  VW::FirewireCamera::SetAutoFocus(new_state);
  my_widget->autofocus_button->set_active(new_state);
}
void VW::FirewireCameraGTK::SetAutoExposure(bool new_state) {
  VW::FirewireCamera::SetAutoExposure(new_state);
  my_widget->autoexposure_button->set_active(new_state);
}
void VW::FirewireCameraGTK::SetAutoShutter(bool new_state) {
  VW::FirewireCamera::SetAutoShutter(new_state);
  my_widget->autoshutter_button->set_active(new_state);
}
void VW::FirewireCameraGTK::SetAutoGamma(bool new_state) {
  VW::FirewireCamera::SetAutoGamma(new_state);
  my_widget->autogamma_button->set_active(new_state);
}
void VW::FirewireCameraGTK::SetAutoSharpness(bool new_state) {
  VW::FirewireCamera::SetAutoSharpness(new_state);
  my_widget->autosharpness_button->set_active(new_state);
}
void VW::FirewireCameraGTK::SetAutoHue(bool new_state) {
  VW::FirewireCamera::SetAutoHue(new_state);
  my_widget->autohue_button->set_active(new_state);
}
void VW::FirewireCameraGTK::SetAutoSaturation(bool new_state) {
  VW::FirewireCamera::SetAutoSaturation(new_state);
  my_widget->autosaturation_button->set_active(new_state);
}

void VW::FirewireCameraGTK::SetAutoWhiteBalance(bool new_state) {
  VW::FirewireCamera::SetAutoWhiteBalance(new_state);
  my_widget->autowhitebalance_button->set_active(new_state);
}

// }}}
