/** 
 * @file   firewirecameragtk.h
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
 * @class   VW::FirewireCameraGTK
 * @ingroup UserInterface
 * @author  BJ Tordoff
 * @date    02/08/2002
 * @brief   An interface to a firewire camera, allowing GUI control of 
 *          any/all available camera parameters (zoom, iris, focus etc).
 *
 * *** Insert Long Description Here ***
 */


// This class defines a GTK interface for the firewire cameras
#ifndef _FirewireCameraGTK_H
#define _FirewireCameraGTK_H

#include <gtk--.h>
#include <VWFirewire/firewirecamera.h>
#include "interfacegtk.h"

namespace VW {

  class FirewireCameraGTK;
  /** 
   * The firewire-camera interface widget.
   * This widget contains all the controls for the
   * firewire cameras. Note however that it should not be used directly.
   */
  class FirewireCameraGTKWidget : public VW::InterfaceGTKWidget {
  public:
    void InitForm(FirewireCameraGTK* parent);

    // These widgets are required throughout the class
    Gtk::Notebook*     tabbed_pages;

    // General control widgets
    Gtk::Label*        zoom_label; 
    Gtk::SpinButton*   zoom_spinner; 
    Gtk::Label*        focus_label; 
    Gtk::SpinButton*   focus_spinner;
    Gtk::CheckButton*  autofocus_button; 
 
    // Brightness widgets
    Gtk::SpinButton*   iris_spinner; 
    Gtk::CheckButton*  autoiris_button; 
    Gtk::Label*        iris_label; 
    Gtk::Label*        brightness_label; 
    Gtk::SpinButton*   brightness_spinner; 
    Gtk::CheckButton*  autobrightness_button; 
    Gtk::Label*        gain_label; 
    Gtk::SpinButton*   gain_spinner; 
    Gtk::CheckButton*  autogain_button; 
    Gtk::Label*        exposure_label; 
    Gtk::SpinButton*   exposure_spinner; 
    Gtk::CheckButton*  autoexposure_button; 
    Gtk::Label*        shutter_label; 
    Gtk::SpinButton*   shutter_spinner; 
    Gtk::CheckButton*  autoshutter_button; 

    // Colour widgets
    Gtk::Label*        gamma_label; 
    Gtk::SpinButton*   gamma_spinner; 
    Gtk::CheckButton*  autogamma_button; 
    Gtk::Label*        sharpness_label; 
    Gtk::SpinButton*   sharpness_spinner; 
    Gtk::CheckButton*  autosharpness_button; 
    Gtk::Label*        hue_label; 
    Gtk::SpinButton*   hue_spinner; 
    Gtk::CheckButton*  autohue_button; 
    Gtk::Label*        saturation_label; 
    Gtk::SpinButton*   saturation_spinner; 
    Gtk::CheckButton*  autosaturation_button; 
    Gtk::Label*        whitebalance_label; 
    Gtk::Label*        whitebalanceU_label; 
    Gtk::Label*        whitebalanceV_label; 
    Gtk::SpinButton*   whitebalanceU_spinner; 
    Gtk::SpinButton*   whitebalanceV_spinner; 
    Gtk::CheckButton*  autowhitebalance_button; 
    Gtk::Text*         about_text;
  };

  /** 
   * The main firewire-camera interface class.
   * This interface contains a widget which can either be attached
   * directly to the user window, or attaches a button which pops
   * up the widget.
   */
class FirewireCameraGTK : public VW::InterfaceGTK<FirewireCameraGTKWidget>, 
			  public VW::FirewireCamera {

  friend class FirewireCameraGTKWidget;

public:
  FirewireCameraGTK(VW::InterfaceType t=VW::INTERFACE_AS_BUTTONWINDOW,
		    bool verbosity=false);
  ~FirewireCameraGTK(void) {};
  virtual bool SetUpCamera(raw1394handle_t port_handle);

  // Set lots of things to auto/manual
  void                   UseAutoSettings();
  void                   UseManualSettings();

  // Set functions are overloaded
  virtual void SetZoom(int new_zoom);
  virtual void SetFocus(int new_focus);
  virtual void SetAutoFocus(bool new_state);
  virtual void SetIris(int new_iris);
  virtual void SetAutoIris(bool new_state);
  virtual void SetBrightness(int new_brightness);
  virtual void SetAutoBrightness(bool new_state);
  virtual void SetGain(int new_gain);
  virtual void SetAutoGain(bool new_state);
  virtual void SetExposure(int new_exposure);
  virtual void SetAutoExposure(bool new_state);
  virtual void SetShutter(int new_shutter);
  virtual void SetAutoShutter(bool new_state);
  virtual void SetGamma(int new_gamma);
  virtual void SetAutoGamma(bool new_state);
  virtual void SetSharpness(int new_sharpness);
  virtual void SetAutoSharpness(bool new_state);
  virtual void SetHue(int new_hue);
  virtual void SetAutoHue(bool new_state);
  virtual void SetSaturation(int new_saturation);
  virtual void SetAutoSaturation(bool new_state);
  virtual void SetWhiteBalance(int new_U, int new_V);
  virtual void SetAutoWhiteBalance(bool new_state);

  // Must have a load and save function even if they do nothing
  void LoadParameterFile(std::string paramfile=".param");
  void SaveParameterFile(std::string paramfile=".param");

protected:

  // Interface stuff
  void CreateForm();
  inline void PageChange(Gtk::Notebook_Helpers::Page* p0,guint p1);
  inline void ChangeZoom();
  inline void ChangeFocus();
  inline void ChangeAutoFocus();
  inline void ChangeIris();
  inline void ChangeAutoIris();
  inline void ChangeBrightness();
  inline void ChangeAutoBrightness();
  inline void ChangeGain();
  inline void ChangeAutoGain();
  inline void ChangeExposure();
  inline void ChangeAutoExposure();
  inline void ChangeShutter();
  inline void ChangeAutoShutter();
  inline void ChangeGamma();
  inline void ChangeAutoGamma();
  inline void ChangeSharpness();
  inline void ChangeAutoSharpness();
  inline void ChangeHue();
  inline void ChangeAutoHue();
  inline void ChangeSaturation();
  inline void ChangeAutoSaturation();
  inline void ChangeWhiteBalance();
  inline void ChangeAutoWhiteBalance();
private:

  void SetAboutText(void);
  void UpdateForm(void);
 
};

}; // end namespace

inline void VW::FirewireCameraGTK::PageChange(Gtk::Notebook_Helpers::Page* p0,guint p1) {
  if (p1==0) {
    // Re-write the summary
    SetAboutText();
  }
}

inline void VW::FirewireCameraGTK::ChangeZoom() 
{
  int v=my_widget->zoom_spinner->get_value_as_int();
  VW::FirewireCamera::SetZoom(v);
}

inline void VW::FirewireCameraGTK::ChangeFocus() 
{
  int v=my_widget->focus_spinner->get_value_as_int();
  VW::FirewireCamera::SetFocus(v);
}

inline void VW::FirewireCameraGTK::ChangeAutoFocus() 
{
  bool v=my_widget->autofocus_button->get_active();
  VW::FirewireCamera::SetAutoFocus(v);
  // (De) activate the spinner
  my_widget->focus_spinner->set_sensitive(!v);
  my_widget->focus_label->set_sensitive(!v);
}

inline void VW::FirewireCameraGTK::ChangeIris() 
{
  int v=my_widget->iris_spinner->get_value_as_int();
  VW::FirewireCamera::SetIris(v);
}

inline void VW::FirewireCameraGTK::ChangeAutoIris() 
{
  bool v=my_widget->autoiris_button->get_active();
  VW::FirewireCamera::SetAutoIris(v);
  // (De) activate the spinner
  my_widget->iris_spinner->set_sensitive(!v);
  my_widget->iris_label->set_sensitive(!v);
}


inline void VW::FirewireCameraGTK::ChangeBrightness() 
{
  int v=my_widget->brightness_spinner->get_value_as_int();
  VW::FirewireCamera::SetBrightness(v);
}

inline void VW::FirewireCameraGTK::ChangeAutoBrightness() 
{
  bool v=my_widget->autobrightness_button->get_active();
  VW::FirewireCamera::SetAutoBrightness(v);
  // (De) activate the spinner
  my_widget->brightness_spinner->set_sensitive(!v);
  my_widget->brightness_label->set_sensitive(!v);
}

inline void VW::FirewireCameraGTK::ChangeGain() 
{
  int v=my_widget->gain_spinner->get_value_as_int();
  VW::FirewireCamera::SetGain(v);
}

inline void VW::FirewireCameraGTK::ChangeAutoGain() 
{
  bool v=my_widget->autogain_button->get_active();
  VW::FirewireCamera::SetAutoGain(v);
  // (De) activate the spinner
  my_widget->gain_spinner->set_sensitive(!v);
  my_widget->gain_label->set_sensitive(!v);
}

inline void VW::FirewireCameraGTK::ChangeExposure() 
{
  int v=my_widget->exposure_spinner->get_value_as_int();
  VW::FirewireCamera::SetExposure(v);
}

inline void VW::FirewireCameraGTK::ChangeAutoExposure() 
{
  bool v=my_widget->autoexposure_button->get_active();
  VW::FirewireCamera::SetAutoExposure(v);
  // (De) activate the spinner
  my_widget->exposure_spinner->set_sensitive(!v);
  my_widget->exposure_label->set_sensitive(!v);
}
inline void VW::FirewireCameraGTK::ChangeShutter() 
{
  int v=my_widget->shutter_spinner->get_value_as_int();
  VW::FirewireCamera::SetShutter(v);
}

inline void VW::FirewireCameraGTK::ChangeAutoShutter() 
{
  bool v=my_widget->autoshutter_button->get_active();
  VW::FirewireCamera::SetAutoShutter(v);
  // (De) activate the spinner
  my_widget->shutter_spinner->set_sensitive(!v);
  my_widget->shutter_label->set_sensitive(!v);
}
inline void VW::FirewireCameraGTK::ChangeGamma() 
{
  int v=my_widget->gamma_spinner->get_value_as_int();
  VW::FirewireCamera::SetGamma(v);
}
inline void VW::FirewireCameraGTK::ChangeAutoGamma() 
{
  bool v=my_widget->autogamma_button->get_active();
  VW::FirewireCamera::SetAutoGamma(v);
  // (De) activate the spinner
  my_widget->gamma_spinner->set_sensitive(!v);
  my_widget->gamma_label->set_sensitive(!v);
}
inline void VW::FirewireCameraGTK::ChangeSharpness() 
{
  int v=my_widget->sharpness_spinner->get_value_as_int();
  VW::FirewireCamera::SetSharpness(v);
}
inline void VW::FirewireCameraGTK::ChangeAutoSharpness() 
{
  bool v=my_widget->autosharpness_button->get_active();
  VW::FirewireCamera::SetAutoSharpness(v);
  // (De) activate the spinner
  my_widget->sharpness_spinner->set_sensitive(!v);
  my_widget->sharpness_label->set_sensitive(!v);
}
inline void VW::FirewireCameraGTK::ChangeHue() 
{
  int v=my_widget->hue_spinner->get_value_as_int();
  VW::FirewireCamera::SetHue(v);
}
inline void VW::FirewireCameraGTK::ChangeAutoHue() 
{
  bool v=my_widget->autohue_button->get_active();
  VW::FirewireCamera::SetAutoHue(v);
  // (De) activate the spinner
  my_widget->hue_spinner->set_sensitive(!v);
  my_widget->hue_label->set_sensitive(!v);
}
inline void VW::FirewireCameraGTK::ChangeSaturation() 
{
  int v=my_widget->saturation_spinner->get_value_as_int();
  VW::FirewireCamera::SetSaturation(v);
}
inline void VW::FirewireCameraGTK::ChangeAutoSaturation() 
{
  bool v=my_widget->autosaturation_button->get_active();
  VW::FirewireCamera::SetAutoSaturation(v);
  // (De) activate the spinner
  my_widget->saturation_spinner->set_sensitive(!v);
  my_widget->saturation_label->set_sensitive(!v);
}
inline void VW::FirewireCameraGTK::ChangeWhiteBalance() 
{
  int u=my_widget->whitebalanceU_spinner->get_value_as_int();
  int v=my_widget->whitebalanceV_spinner->get_value_as_int();
  VW::FirewireCamera::SetWhiteBalance(u,v);
}

inline void VW::FirewireCameraGTK::ChangeAutoWhiteBalance() 
{
  bool v=my_widget->autowhitebalance_button->get_active();
  VW::FirewireCamera::SetAutoWhiteBalance(v);
  // (De) activate the spinner
  my_widget->whitebalanceU_spinner->set_sensitive(!v);
  my_widget->whitebalanceV_spinner->set_sensitive(!v);
  my_widget->whitebalance_label->set_sensitive(!v);
  my_widget->whitebalanceU_label->set_sensitive(!v);
  my_widget->whitebalanceV_label->set_sensitive(!v);
}

// These next two only need to be here to pass on the message...
inline void VW::FirewireCameraGTK::UseAutoSettings()
{
  FirewireCamera::UseAutoSettings();
  UpdateForm();
}

inline void VW::FirewireCameraGTK::UseManualSettings()
{
  FirewireCamera::UseManualSettings();
  UpdateForm();
}

inline void 
VW::FirewireCameraGTK::LoadParameterFile(std::string paramfile)
{
  this->ReadXMLFile(paramfile);
}

inline void 
VW::FirewireCameraGTK::SaveParameterFile(std::string paramfile)
{
  this->WriteXMLFile(paramfile);
}


#endif
