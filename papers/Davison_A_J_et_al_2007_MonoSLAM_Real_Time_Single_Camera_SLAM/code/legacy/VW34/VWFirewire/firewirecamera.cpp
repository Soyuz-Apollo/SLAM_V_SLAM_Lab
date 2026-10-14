/** 
 * @file   firewirecamera.cpp
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


#include <libraw1394/raw1394.h>
#include <iostream>
#include <math.h>

// VW includes
#include "VW/Utils/timeops.h"

// Class definition
#include "firewirecamera.h"

#define VERBOSE true
#define FIREWIRE_VIDEO_DEVICE "/dev/video1394/0"
#define NUM_DMA_BUFFERS 4


using namespace std;








// {{{ FirewireCamera::FirewireCamera(bool display_messages)
VW::FirewireCamera::FirewireCamera(bool display_messages) : 
  verbose(display_messages)
{
  cerr << "FirewireCamera::FirewireCamera: Constructed!" << endl;
}
// }}}

// {{{ bool FirewireCamera::SetUpCamera(raw1394handle_t port_handle)
bool VW::FirewireCamera::SetUpCamera(raw1394handle_t port_handle) {
  bool success=true;

  // Store the port handle for future use
  camera_info.handle=port_handle;

  if (VERBOSE) cerr << "FirewireCamera::SetUpCamera: Get misc info"<<endl;
  if (dc1394_get_camera_misc_info(port_handle, camera_info.id, &misc_info)==0) {
    if (verbose) cerr << "FirewireCamera::SetUpCamera() - Failed to get misc info." << endl;
    return (false);
  }

  // Get the camera info stuff
   if (VERBOSE) cerr << "FirewireCamera::SetUpCamera: Get camera info"<<endl;
 if (dc1394_get_camera_info(port_handle, camera_info.id, &camera_info)==0) {
    if (verbose) cerr << "FirewireCamera::SetUpCamera() - Failed to get camera info." << endl;
    return (false);
  }

  // Get the feature set
   if (VERBOSE) cerr << "FirewireCamera::SetUpCamera: Get feature set"<<endl;
  if (dc1394_get_camera_feature_set(port_handle, camera_info.id, &feature_set)==0) {
    if (verbose) cerr << "FirewireCamera::SetUpCamera() - Failed to get feature set." << endl;
    return (false);
  }
  cerr << "Available features: "
       <<int(feature_set.feature[FEATURE_IRIS-FEATURE_MIN].available)
       <<int(feature_set.feature[FEATURE_EXPOSURE-FEATURE_MIN].available)
       <<int(feature_set.feature[FEATURE_BRIGHTNESS-FEATURE_MIN].available)
       <<int(feature_set.feature[FEATURE_GAIN-FEATURE_MIN].available)
       <<int(feature_set.feature[FEATURE_FOCUS-FEATURE_MIN].available)
       <<int(feature_set.feature[FEATURE_SHUTTER-FEATURE_MIN].available)
       <<int(feature_set.feature[FEATURE_GAMMA-FEATURE_MIN].available)
       <<int(feature_set.feature[FEATURE_HUE-FEATURE_MIN].available)
       <<int(feature_set.feature[FEATURE_SATURATION-FEATURE_MIN].available)
       <<int(feature_set.feature[FEATURE_WHITE_BALANCE-FEATURE_MIN].available)
       <<endl; 
  cerr << "Auto features:      "
       <<int(feature_set.feature[FEATURE_IRIS-FEATURE_MIN].auto_capable)
       <<int(feature_set.feature[FEATURE_EXPOSURE-FEATURE_MIN].auto_capable)
       <<int(feature_set.feature[FEATURE_BRIGHTNESS-FEATURE_MIN].available)
       <<int(feature_set.feature[FEATURE_GAIN-FEATURE_MIN].auto_capable)
       <<int(feature_set.feature[FEATURE_FOCUS-FEATURE_MIN].auto_capable)
       <<int(feature_set.feature[FEATURE_SHUTTER-FEATURE_MIN].auto_capable)
       <<int(feature_set.feature[FEATURE_GAMMA-FEATURE_MIN].auto_capable)
       <<int(feature_set.feature[FEATURE_HUE-FEATURE_MIN].auto_capable)
       <<int(feature_set.feature[FEATURE_SATURATION-FEATURE_MIN].auto_capable)
       <<int(feature_set.feature[FEATURE_WHITE_BALANCE-FEATURE_MIN].auto_capable)
       <<endl; 


  // Initialize
   if (VERBOSE) cerr << "FirewireCamera::SetUpCamera: Init camera"<<endl;
  if (dc1394_init_camera(port_handle, camera_info.id)==0) {
    if (verbose) cerr << "FirewireCamera::SetUpCamera() - Failed to initialise." << endl;
    return (false);
  }

  // *** Hack to get the cameras onto differnt ISO channels ***
   if (VERBOSE) cerr << "FirewireCamera::SetUpCamera: Set channel"<<endl;
  misc_info.iso_channel=int((camera_info.id));
  if (VERBOSE) cerr << "FirewireCamera::SetUpCamera: Done"<<endl;

 return(success);

}
// }}}
// {{{ bool FirewireCamera::StartISOTransmission(void)
bool VW::FirewireCamera::StartISOTransmission(void) {
  bool success=true;
  unsigned int channel,speed;
  if (dc1394_get_iso_channel_and_speed(camera_info.handle, 
				       camera_info.id, 
				       &channel, 
				       &speed)==0) {
    if (verbose) cerr << "FirewireCamera::StartISOTransmission - Could not get channel." << endl;
    return (false);
  }
  if (dc1394_start_iso_transmission(camera_info.handle,camera_info.id)==0) {
    if (verbose) cerr << "FirewireCamera::StartISOTransmission - Could not start transmission." << endl;
    return (false);
}
  // The receiver gets its own handle
  receive_handle = dc1394_create_handle(0);

  // If we got here, then transmission should be started
  misc_info.is_iso_on=DC1394_TRUE;
  return(success);
}
// }}}
// {{{ bool FirewireCamera::StopISOTransmission(void)
bool VW::FirewireCamera::StopISOTransmission(void) {
  bool success=true;
  if (dc1394_stop_iso_transmission(camera_info.handle,camera_info.id)==0) {
    if (verbose) 
      cerr << "FirewireCamera::StopISOTransmission - "
	   << "Could not stop transmission!" << endl;
    return (false);
  }
  if (verbose) 
    cerr << "FirewireCamera::StopISOTransmission - "
	 << "Transmission stopped" << endl;

  // If we got here, then transmission should be stopped
  misc_info.is_iso_on=DC1394_FALSE;

  return(success);
}
// }}}
// {{{ bool FirewireCamera::SetupDMACapture(dc1394_cameracapture *capture)

bool VW::FirewireCamera::SetupDMACapture(dc1394_cameracapture *capture) {
  bool success=false;

    if (VERBOSE) cerr << "FirewireCamera::SetupDMACapture - Attempting to set up DMA for "
		      << "\n\tnode "<<camera_info.id
		      << "\n\ton channel "<<misc_info.iso_channel
		      << "\n\tat "<<pow(2.0,(misc_info.framerate-FirewireParams::FPS_1_875))*1.875<<" fps."
		      << "\n\tin format "<<misc_info.format<<" mode " << misc_info.mode << endl;


    // This function call changed in latest libdc
//   if (dc1394_dma_setup_capture( camera_info.handle, 
// 				camera_info.id, 
// 				misc_info.iso_channel, 
// 				misc_info.format, 
// 				misc_info.mode, 
// 				misc_info.iso_speed, 
// 				misc_info.framerate,
// 				NUM_DMA_BUFFERS,
// 				capture)


  if (dc1394_dma_setup_capture( camera_info.handle, 
				camera_info.id, 
				misc_info.iso_channel, 
				misc_info.format, 
				misc_info.mode, 
				misc_info.iso_speed, 
				misc_info.framerate,
				NUM_DMA_BUFFERS,
				0, // indicate to drop frames
				FIREWIRE_VIDEO_DEVICE,
				capture)
      == DC1394_SUCCESS) {
    success=true;
  } else {
    return (false);
  }
  return(success);
}

// }}}
// {{{ bool FirewireCamera::ReleaseDMACapture(dc1394_cameracapture *capture)

bool VW::FirewireCamera::ReleaseDMACapture(dc1394_cameracapture *capture) {
  bool success=false;

  if (verbose) 
    cerr << "FirewireCamera::ReleaseDMACapture - "
	 << "Attempting to release DMA for node "<<camera_info.id
	 <<" on channel "<<misc_info.iso_channel
	 <<" at "<<pow(2.0,(misc_info.framerate-FirewireParams::FPS_1_875))*1.875
	 <<" fps."<<endl;
  
  if (dc1394_dma_release_camera(camera_info.handle, capture)==0) {
    cerr << "FirewireCamera::ReleaseDMACapture - "
	 << "failed to release camera." << endl;
    success=false;
  } else if (dc1394_dma_unlisten(camera_info.handle, capture)==0) {
    cerr << "FirewireCamera::ReleaseDMACapture - "
	 << "failed to stop listening." << endl;
    success=false;
  }

  //  dc1394_destroy_handle(receive_handle);
   success=true;
  return(success);
}

// }}}
// {{{ void FirewireCamera::SetImageMode(FirewireParams::CameraMode new_mode)

void VW::FirewireCamera::SetImageMode(FirewireParams::CameraMode new_mode)
{
  if (!dc1394_set_video_mode(camera_info.handle,
			     camera_info.id,
			     new_mode)) {
    cerr << "VW::FirewireCamera::SetImageMode - "
	 << "could not set new mode!" << endl;
  } else {
    misc_info.mode=new_mode;
    // Also set the format
    SetImageFormat(FirewireParams::F_VGA_NONCOMPRESSED);
  }
  
  // Store width and height for later
  switch(new_mode) {
  case FirewireParams::M_160x120_YUV444:
    image_width=160;image_height=120;
    break;
  case FirewireParams::M_320x240_YUV422:
   image_width=320;image_height=240;
   break;
  case FirewireParams::M_640x480_YUV422:
  case FirewireParams::M_640x480_YUV411:
  case FirewireParams::M_640x480_RGB:
  case FirewireParams::M_640x480_MONO:
    image_width=640;image_height=480;
    break;
  default:
    image_width=0;image_height=0;
  };
};

// }}}

// {{{ Set functions
void VW::FirewireCamera::SetZoom(int new_zoom) {
  unsigned int z=static_cast<unsigned int>(new_zoom);
  //  struct timeval s,e;
  //  gettimeofday(&s,NULL);
  dc1394_set_zoom(camera_info.handle,camera_info.id,z);
  //  gettimeofday(&e,NULL);
  //  cerr << "Demand time="<<e-s<<endl;
  // Also get zoom for odometry history
  //  gettimeofday(&s,NULL);
  //  GetZoom();
  //  gettimeofday(&e,NULL);
  //  cerr << "Odom time="<<e-s<<endl;
};
void VW::FirewireCamera::SetFocus(int new_focus) {
  unsigned int f=static_cast<unsigned int>(new_focus);
  dc1394_set_focus(camera_info.handle,camera_info.id,f);
};
void VW::FirewireCamera::SetAutoFocus(bool new_state) {
  if (dc1394_auto_on_off(camera_info.handle, camera_info.id, 
			 FEATURE_FOCUS, new_state))
    feature_set.feature[FEATURE_FOCUS-FEATURE_MIN].auto_active=dc1394bool_t(new_state);
};
void VW::FirewireCamera::SetIris(int new_iris) {
  unsigned int i=static_cast<unsigned int>(new_iris);
  dc1394_set_iris(camera_info.handle,camera_info.id,i);
};
void VW::FirewireCamera::SetAutoIris(bool new_state) {
  if (dc1394_auto_on_off(camera_info.handle, camera_info.id, 
		     FEATURE_IRIS, new_state))
    feature_set.feature[FEATURE_IRIS-FEATURE_MIN].auto_active=dc1394bool_t(new_state);
};
void VW::FirewireCamera::SetBrightness(int new_brightness) {
  unsigned int i=static_cast<unsigned int>(new_brightness);
  dc1394_set_brightness(camera_info.handle,camera_info.id,i);
};
void VW::FirewireCamera::SetAutoBrightness(bool new_state) {
  if (dc1394_auto_on_off(camera_info.handle, camera_info.id, 
		     FEATURE_BRIGHTNESS, new_state))
    feature_set.feature[FEATURE_BRIGHTNESS-FEATURE_MIN].auto_active=dc1394bool_t(new_state);
};
void VW::FirewireCamera::SetGain(int new_gain) {
  unsigned int i=static_cast<unsigned int>(new_gain);
  dc1394_set_gain(camera_info.handle,camera_info.id,i);
};
void VW::FirewireCamera::SetAutoGain(bool new_state) {
  if (dc1394_auto_on_off(camera_info.handle, camera_info.id, 
		     FEATURE_GAIN, new_state))
    feature_set.feature[FEATURE_GAIN-FEATURE_MIN].auto_active=dc1394bool_t(new_state);
};
void VW::FirewireCamera::SetExposure(int new_exposure) {
  unsigned int i=static_cast<unsigned int>(new_exposure);
  dc1394_set_exposure(camera_info.handle,camera_info.id,i);
};
void VW::FirewireCamera::SetAutoExposure(bool new_state) {
  if (dc1394_auto_on_off(camera_info.handle, camera_info.id, 
		     FEATURE_EXPOSURE, new_state))
    feature_set.feature[FEATURE_EXPOSURE-FEATURE_MIN].auto_active=dc1394bool_t(new_state);
};
void VW::FirewireCamera::SetShutter(int new_shutter) {
  unsigned int i=static_cast<unsigned int>(new_shutter);
  dc1394_set_shutter(camera_info.handle,camera_info.id,i);
};
void VW::FirewireCamera::SetAutoShutter(bool new_state) {
  if (dc1394_auto_on_off(camera_info.handle, camera_info.id, 
		     FEATURE_SHUTTER, new_state))
    feature_set.feature[FEATURE_SHUTTER-FEATURE_MIN].auto_active=dc1394bool_t(new_state);
};
void VW::FirewireCamera::SetGamma(int new_gamma) {
  unsigned int i=static_cast<unsigned int>(new_gamma);
  dc1394_set_gamma(camera_info.handle,camera_info.id,i);
};
void VW::FirewireCamera::SetAutoGamma(bool new_state) {
  if (dc1394_auto_on_off(camera_info.handle, camera_info.id, 
		     FEATURE_GAMMA, new_state))
    feature_set.feature[FEATURE_GAMMA-FEATURE_MIN].auto_active=dc1394bool_t(new_state);
};
void VW::FirewireCamera::SetSharpness(int new_sharpness) {
  unsigned int i=static_cast<unsigned int>(new_sharpness);
  dc1394_set_sharpness(camera_info.handle,camera_info.id,i);
};
void VW::FirewireCamera::SetAutoSharpness(bool new_state) {
  if (dc1394_auto_on_off(camera_info.handle, camera_info.id, 
		     FEATURE_SHARPNESS, new_state))
    feature_set.feature[FEATURE_SHARPNESS-FEATURE_MIN].auto_active=dc1394bool_t(new_state);
};
void VW::FirewireCamera::SetHue(int new_hue) {
  unsigned int i=static_cast<unsigned int>(new_hue);
  dc1394_set_hue(camera_info.handle,camera_info.id,i);
};
void VW::FirewireCamera::SetAutoHue(bool new_state) {
  if (dc1394_auto_on_off(camera_info.handle, camera_info.id, 
		     FEATURE_HUE, new_state))
    feature_set.feature[FEATURE_HUE-FEATURE_MIN].auto_active=dc1394bool_t(new_state);
};
void VW::FirewireCamera::SetSaturation(int new_saturation) {
  unsigned int i=static_cast<unsigned int>(new_saturation);
  dc1394_set_saturation(camera_info.handle,camera_info.id,i);
};
void VW::FirewireCamera::SetAutoSaturation(bool new_state) {
  if (dc1394_auto_on_off(camera_info.handle, camera_info.id, 
		     FEATURE_SATURATION, new_state))
    feature_set.feature[FEATURE_SATURATION-FEATURE_MIN].auto_active=dc1394bool_t(new_state);
};
void VW::FirewireCamera::SetWhiteBalance(int new_U,int new_V) {
  unsigned int u=static_cast<unsigned int>(new_U);
  unsigned int v=static_cast<unsigned int>(new_V);
  dc1394_set_white_balance(camera_info.handle,camera_info.id,u,v);
};
void VW::FirewireCamera::SetAutoWhiteBalance(bool new_state) {
  if (dc1394_auto_on_off(camera_info.handle, camera_info.id, 
		     FEATURE_WHITE_BALANCE, new_state))
    feature_set.feature[FEATURE_WHITE_BALANCE-FEATURE_MIN].auto_active=dc1394bool_t(new_state);
};
// }}}

// {{{ void VW::FirewireCamera::UseAutoSettings()

void VW::FirewireCamera::UseAutoSettings()
{
  // Go through any setting that can be auto and set it so
  if (HasAutoFocus()) SetAutoFocus(true);
  if (HasAutoBrightness()) SetAutoBrightness(true);
  if (HasAutoGain()) SetAutoGain(true);
  if (HasAutoExposure()) SetAutoExposure(true);
  if (HasAutoShutter()) SetAutoShutter(true);
  if (HasAutoGamma()) SetAutoGamma(true);
  if (HasAutoSharpness()) SetAutoSharpness(true);
  if (HasAutoHue()) SetAutoHue(true);
  if (HasAutoSaturation()) SetAutoSaturation(true);
  if (HasAutoIris()) SetAutoIris(true);
  if (HasAutoWhiteBalance()) SetAutoWhiteBalance(true);
}

// }}}
// {{{ void VW::FirewireCamera::UseManualSettings()

void VW::FirewireCamera::UseManualSettings()
{
  // Go through all settings that can be auto and set them to manual
  SetAutoFocus(false);
  SetAutoBrightness(false);
  SetAutoGain(false);
  SetAutoExposure(false);
  SetAutoShutter(false);
  SetAutoGamma(false);
  SetAutoSharpness(false);
  SetAutoHue(false);
  SetAutoSaturation(false);
  SetAutoIris(false);
  SetAutoWhiteBalance(false);

}

// }}}

// {{{ int VW::FirewireCamera::GetHistoricPosition()

int VW::FirewireCamera::GetHistoricPosition(struct timeval time, 
					    double *zoom) {

  int z;
  char status=0;

  // If 'time' is more recent than anything
  // on the odometry stacks, get current odometry (it's cheap anyway)
  if (time>time_history[history_pointer]) {
    struct timeval now;
    z=GetZoom();
    gettimeofday(&now, NULL);
    // Convert time to ms
    StoreOdometryHistory(now,z);
  }


  // Search back through list
  int before_pos=history_pointer;
  int after_pos=history_pointer;
  bool still_searching=true;
  for (int i=0;(i<CAMERA_ODOMETRY_HISTORY_LENGTH) && (still_searching);i++) {
    before_pos=history_pointer-i;
    if (before_pos<0) before_pos+=CAMERA_ODOMETRY_HISTORY_LENGTH;

    if (time>time_history[before_pos]) {
      still_searching=false;
    }
  }
  if (still_searching==true) {
    // This means that we're looking for something older than anything stored
    // on the list - let's return the oldest value
    (*zoom)= double(zoom_history[before_pos]);
    
  } else {
    if (before_pos==history_pointer) {
      // OK, this is more recent than anything stored - send latest value
      (*zoom)=  double(zoom_history[history_pointer]);
    } else {
      // Found it, so interpolate
      after_pos=before_pos+1;
      if (after_pos>=CAMERA_ODOMETRY_HISTORY_LENGTH) after_pos=0;
      double fractime = (time.tv_sec*1000000 + time.tv_usec)
	- (time_history[before_pos].tv_sec*1000000 
	   + time_history[before_pos].tv_usec);
      double totaltime = (time_history[after_pos].tv_sec*1000000 
			 + time_history[after_pos].tv_usec)
	- (time_history[before_pos].tv_sec*1000000 
	   + time_history[before_pos].tv_usec);
      double frac = fractime / totaltime;
      (*zoom) = frac     * double(zoom_history[after_pos]) 
	   + (1-frac) * double(zoom_history[before_pos]);
      }
    }
  return int(status);
}

// }}}

// {{{ void VW::FirewireCamera::GetOdometryHistory()

void VW::FirewireCamera::GetOdometryHistory(vector<timeval>& time,
					    vector<double>& zoom)
{
  for (int i=0;i<CAMERA_ODOMETRY_HISTORY_LENGTH;i++) {
    int j=history_pointer+i+1;
    if (j>=CAMERA_ODOMETRY_HISTORY_LENGTH) j-=CAMERA_ODOMETRY_HISTORY_LENGTH;
    if (time_history[j].tv_sec>0) { // only display the meaningful bits
      time.push_back(time_history[j]);
      zoom.push_back(double(zoom_history[j]));
    }
  }
}

// }}}

// {{{ void VW::FirewireCamera::FlushHistory(void)

void VW::FirewireCamera::FlushHistory(void) {
  int z;
  struct timeval init_time;
  init_time.tv_sec=0;init_time.tv_usec=0;
  
  z=GetZoom();
  for (int i=0;i<CAMERA_ODOMETRY_HISTORY_LENGTH;i++) {
    zoom_history[i]=z;
    time_history[i]=init_time;
  }
  history_pointer=0;

}

// }}}

// {{{ bool VW::FirewireCamera::WriteASCII(std::ostream& stream) const

bool VW::FirewireCamera::WriteASCII(std::ostream& stream) const
{
  using namespace VW;
  stream<<"  Vendor:  "<<GetVendorString()<<endl;
  stream<<"   Model:  "<<GetModelString()<<endl;
  stream<<"    GUID:  "<<GetGUID()<<endl;
  stream<<"    Node:  " << GetNodeNumber() << "     ISO:  " << GetISOChannel() << endl;
  switch (GetImageMode()) {
  case FirewireParams::M_160x120_YUV444:
    stream<<"    Mode:  160x120 YUV 4:4:4"<<endl;
    break;
  case FirewireParams::M_320x240_YUV422:
    stream<<"    Mode:  320x240 YUV 4:2:2"<<endl;
    break;
  case FirewireParams::M_640x480_YUV411:
    stream<<"    Mode:  640x480 YUV 4:1:1"<<endl;
    break;
  case FirewireParams::M_640x480_YUV422:
    stream<<"    Mode:  640x480 YUV 4:2:2"<<endl;
    break;
  case FirewireParams::M_640x480_RGB:
    stream<<"    Mode:  640x480 RGB"<<endl;
    break;
  case FirewireParams::M_640x480_MONO:
    stream<<"    Mode:  640x480 MONO"<<endl;
    break;
  default:
    stream<<"  ** UNKNOWN MODE NUMBER **"<<endl;
  }

  switch (GetFramerate()) {
  case FirewireParams::FPS_1_875:
    stream<<"    Rate:  1.875 fps"<<endl;
    break;
  case FirewireParams::FPS_3_75:
    stream<<"    Rate:  3.75 fps"<<endl;
    break;
  case FirewireParams::FPS_7_5:
    stream<<"    Rate:  7.5 fps"<<endl;
    break;
  case FirewireParams::FPS_15:
    stream<<"    Rate:  15 fps"<<endl;
    break;
  case FirewireParams::FPS_30:
    stream<<"    Rate:  30 fps"<<endl;
    break;
  case FirewireParams::FPS_60:
    stream<<"    Rate:  60 fps"<<endl;
    break;
  default:
    stream<<"  ** UNKNOWN FRAME RATE **"<<endl;
  }


  // Now list the features
  int mx=0,mn=0;

  stream<<"Features:"<<endl;
  
  
  if (HasZoom()) {
    GetZoomLimits(&mn, &mx);
    stream<<"        Zoom: "<<GetZoom()<<" ("<<mn<<"-"<<mx<<")"<<endl;
  }  
  if (HasFocus()) {
    GetFocusLimits(&mn, &mx);
    stream<<"       Focus: "<<GetFocus()<<" ("<<mn<<"-"<<mx<<")"<<endl;   
  }    
  if (HasIris()) {
    GetIrisLimits(&mn, &mx);
    stream<<"        Iris: "<<GetIris()<<" ("<<mn<<"-"<<mx<<")"<<endl;  
  }     
  if (HasBrightness()) {
    GetBrightnessLimits(&mn, &mx);
    stream<<"  Brightness: "<<GetBrightness()<<" ("<<mn<<"-"<<mx<<")"<<endl;
  }   
  if (HasGain()) {
    GetGainLimits(&mn, &mx);
    stream<<"        Gain: "<<GetGain()<<" ("<<mn<<"-"<<mx<<")"<<endl;
  }   
  if (HasExposure()) {
    GetExposureLimits(&mn, &mx);
    stream<<"    Exposure: "<<GetExposure()<<" ("<<mn<<"-"<<mx<<")"<<endl;
  }   
  if (HasShutter()) {
    GetShutterLimits(&mn, &mx);
    stream<<"     Shutter: "<<GetShutter()<<" ("<<mn<<"-"<<mx<<")"<<endl;
  }   
  if (HasGamma()) {
    GetGammaLimits(&mn, &mx);
    stream<<"       Gamma: "<<GetGamma()<<" ("<<mn<<"-"<<mx<<")"<<endl;
  }   
  if (HasSharpness()) {
    GetSharpnessLimits(&mn, &mx);
    stream<<"   Sharpness: "<<GetSharpness()<<" ("<<mn<<"-"<<mx<<")"<<endl;
  }   
  if (HasHue()) {
    GetHueLimits(&mn, &mx);
    stream<<"         Hue: "<<GetHue()<<" ("<<mn<<"-"<<mx<<")"<<endl;
  }   
  if (HasSaturation()) {
    GetSaturationLimits(&mn, &mx);
    stream<<"  Saturation: "<<GetSaturation()<<" ("<<mn<<"-"<<mx<<")"<<endl;
  }   
  if (HasWhiteBalance()) {
    GetWhiteBalanceULimits(&mn, &mx);
    stream<<"WhiteBalance: U "<<GetWhiteBalanceU()<<" ("<<mn<<"-"<<mx<<")"<<endl;
    GetWhiteBalanceVLimits(&mn, &mx);
    stream<<"              V "<<GetWhiteBalanceV()<<" ("<<mn<<"-"<<mx<<")"<<endl;
  }   
  return(true);
}

// }}}

// A local func
void WriteFeatureXML(std::ostream& stream,
		     const string& name,
		     bool available,
		     bool automatic,
		     double value,
		     double max, double min)
{
  stream << "  <Feature NAME=\"" << name << "\"> " << endl;
  stream << "    <Available> " << available << " </Available>" << endl;
  if (available) {
    stream << "    <Automatic> " << automatic << " </Automatic>" << endl;
    stream << "    <Value> " << value << " </Value>" << endl;
    stream << "    <Max> " << max << " </Max>" << endl;
    stream << "    <Min> " << min << " </Min>" << endl;
  }
  stream << "  </Feature> " << endl << endl;

}
void WriteWhiteBalanceXML(std::ostream& stream,
			  bool automatic,
			  double uvalue, double umax, double umin,
			  double vvalue, double vmax, double vmin)
{
  stream << "  <Feature NAME=\"WhiteBalance\"> " << endl;
  stream << "    <Available> 1 </Available>" << endl;
  stream << "    <Automatic> " << automatic << " </Automatic>" << endl;
  stream << "    <UValue> " << uvalue << " </UValue>" << endl;
  stream << "    <UMax> " << umax << " </UMax>" << endl;
  stream << "    <UMin> " << umin << " </UMin>" << endl;
  stream << "    <VValue> " << vvalue << " </VValue>" << endl;
  stream << "    <VMax> " << vmax << " </VMax>" << endl;
  stream << "    <VMin> " << vmin << " </VMin>" << endl;
  stream << "  </Feature> " << endl << endl;

}
		     

bool VW::FirewireCamera::WriteXML(std::ostream& stream,
				  const std::string& name) const
{
  /* Write an XML style heirarchical string, eg.
   *
   * <FirewireCamera NAME="MyCam">
   *
   *   <Feature NAME="Zoom">
   *     <Available> 0 </Available>
   *   </Feature>
   *
   *   <Feature NAME="Gain">
   *     <Available> 1 </Available>
   *     <Automatic> 0 </Automatic>
   *     <Value> 127 </Value>
   *     <Max> 0 </Max>
   *     <Min> 255 </Min>
   *   </Feature>
   *
   * </FirewireCamera>
   *
   */
  
  // Start tag
  if (name.size()>0)
    stream << "<FirewireCamera NAME=\"" << name << "\"> " << endl << endl;
  else
    stream << "<FirewireCamera> " << endl << endl;
  
  // Contents

  // Zoom
  if (HasZoom()) {
    int mn=0,mx=0;
    GetZoomLimits(&mn,&mx);
    WriteFeatureXML(stream,
		    "Zoom",
		    true,false,GetZoom(),mn,mx);
  } else {
    WriteFeatureXML(stream,
		    "Zoom",
		    false,false,0,0,0);
  }
  
  // Focus
  if (HasFocus()) {
    int mn=0,mx=0;
    GetFocusLimits(&mn,&mx);
    WriteFeatureXML(stream,
		    "Focus",
		    true,GetAutoFocus(),GetFocus(),mn,mx);
  } else {
    WriteFeatureXML(stream,
		    "Focus",
		    false,false,0,0,0);
  }
  
  // Iris
  if (HasIris()) {
    int mn=0,mx=0;
    GetIrisLimits(&mn,&mx);
    WriteFeatureXML(stream,
		    "Iris",
		    true,GetAutoIris(),GetIris(),mn,mx);
  } else {
    WriteFeatureXML(stream,
		    "Iris",
		    false,false,0,0,0);
  }
  
  // Brightness
  if (HasBrightness()) {
    int mn=0,mx=0;
    GetBrightnessLimits(&mn,&mx);
    WriteFeatureXML(stream,
		    "Brightness",
		    true,GetAutoBrightness(),GetBrightness(),mn,mx);
  } else {
    WriteFeatureXML(stream,
		    "Brightness",
		    false,false,0,0,0);
  }
  
   // Gain
  if (HasGain()) {
    int mn=0,mx=0;
    GetGainLimits(&mn,&mx);
    WriteFeatureXML(stream,
		    "Gain",
		    true,GetAutoGain(),GetGain(),mn,mx);
  } else {
    WriteFeatureXML(stream,
		    "Gain",
		    false,false,0,0,0);
  }
  
  // Exposure
  if (HasExposure()) {
    int mn=0,mx=0;
    GetExposureLimits(&mn,&mx);
    WriteFeatureXML(stream,
		    "Exposure",
		    true,GetAutoExposure(),GetExposure(),mn,mx);
  } else {
    WriteFeatureXML(stream,
		    "Exposure",
		    false,false,0,0,0);
  }
  
  // Shutter
  if (HasShutter()) {
    int mn=0,mx=0;
    GetShutterLimits(&mn,&mx);
    WriteFeatureXML(stream,
		    "Shutter",
		    true,GetAutoShutter(),GetShutter(),mn,mx);
  } else {
    WriteFeatureXML(stream,
		    "Shutter",
		    false,false,0,0,0);
  }
  
  // Gamma
  if (HasGamma()) {
    int mn=0,mx=0;
    GetGammaLimits(&mn,&mx);
    WriteFeatureXML(stream,
		    "Gamma",
		    true,GetAutoGamma(),GetGamma(),mn,mx);
  } else {
    WriteFeatureXML(stream,
		    "Gamma",
		    false,false,0,0,0);
  }
  
  // Sharpness
  if (HasSharpness()) {
    int mn=0,mx=0;
    GetSharpnessLimits(&mn,&mx);
    WriteFeatureXML(stream,
		    "Sharpness",
		    true,GetAutoSharpness(),GetSharpness(),mn,mx);
  } else {
    WriteFeatureXML(stream,
		    "Sharpness",
		    false,false,0,0,0);
  }
  
  // Hue
  if (HasHue()) {
    int mn=0,mx=0;
    GetHueLimits(&mn,&mx);
    WriteFeatureXML(stream,
		    "Hue",
		    true,GetAutoHue(),GetHue(),mn,mx);
  } else {
    WriteFeatureXML(stream,
		    "Hue",
		    false,false,0,0,0);
  }
  
  // Saturation
  if (HasSaturation()) {
    int mn=0,mx=0;
    GetSaturationLimits(&mn,&mx);
    WriteFeatureXML(stream,
		    "Saturation",
		    true,GetAutoSaturation(),GetSaturation(),mn,mx);
  } else {
    WriteFeatureXML(stream,
		    "Saturation",
		    false,false,0,0,0);
  }
  
  // Whitebalance
  if (HasWhiteBalance()) {
    int umn=0,umx=0,vmn=0,vmx=0;
    GetWhiteBalanceULimits(&umn,&umx);
    GetWhiteBalanceVLimits(&vmn,&vmx);
    WriteWhiteBalanceXML(stream,GetAutoWhiteBalance(),
			 GetWhiteBalanceU(),umn,umx,
			 GetWhiteBalanceV(),vmn,vmx);
  } else {
    WriteFeatureXML(stream,
		    "WhiteBalance",
		    false,false,0,0,0);
  }



   // End tag
  stream << "</FirewireCamera>" << endl;


  return(true);
}

bool VW::FirewireCamera::WriteBinary(std::ostream& stream) const
{
  cerr << "Sorry, VW::FirewireCamera::WriteBinary not implemented yet." << endl;
  exit(0);
}


// A little func for setting the read param
void VW::FirewireCamera::UpdateFeatureByName(const string& name,
					     bool automatic,
					     int value,
					     int value2)
{
  cerr << "Setting \""<<name<<"\" to "<<value<<endl;

  if (name=="Zoom") {
    SetZoom(value);
  } else if (name=="Focus") {
    SetAutoFocus(automatic);
    SetFocus(value);
  } else if (name=="Iris") {
    SetAutoIris(automatic);
    SetIris(value);
  } else if (name=="Gain") {
    SetAutoGain(automatic);
    SetGain(value);
  } else if (name=="Brightness") {
    SetAutoBrightness(automatic);
    SetBrightness(value);
  } else if (name=="Exposure") {
    SetAutoExposure(automatic);
    SetExposure(value);
  } else if (name=="Shutter") {
    SetAutoShutter(automatic);
    SetShutter(value);
  } else if (name=="Gamma") {
    SetAutoGamma(automatic);
    SetGamma(value);
  } else if (name=="Sharpness") {
    SetAutoSharpness(automatic);
    SetSharpness(value);
  } else if (name=="Hue") {
    SetAutoHue(automatic);
    SetHue(value);
  } else if (name=="Saturation") {
    SetAutoSaturation(automatic);
    SetSaturation(value);
  } else if (name=="WhiteBalance") {
    SetAutoWhiteBalance(automatic);
    SetWhiteBalance(value,value2);
  } else {
    cerr << "VW::FirewireCamera::UpdateFeature: Aaargh, feature \"" << name 
	 << "\" not on my list!" << endl;
    exit(0);
  }
}



bool VW::FirewireCamera::ReadASCII(std::istream& stream)
{
  cerr << "Sorry, VW::FirewireCamera::ReadASCII not implemented yet." << endl;
  exit(0);
}

bool VW::FirewireCamera::ReadXML(std::istream& stream, 
				 const std::string& name)
{
  cerr << "VW::FirewireCamera::ReadXML - start" << endl;
  std::string cameraname,featurename;
  if (!VW::FindXMLBlock("FirewireCamera",stream,cameraname)) {

    cerr << "VW::FirewireCamera::ReadXML: Could not find <FirewireCamera> block" << endl;
    return(false);

  } else {

    // Found main block, so get those features!
    // For each feature we only read whether it is automatic and the value, as the limits
    // and other stuff are hardware specified.

  std::istringstream feature_contents(" ");
    while (VW::GetXMLBlock("Feature",stream,feature_contents,featurename)) {
      cerr << "VW::FirewireCamera::ReadXML - found a feature ("<<featurename<<")" << endl;
     // Scan the contents to see if this feature is available
      std::istringstream setting_contents(" ");
      string tmp;
      int isavailable=0;
      cerr << "Feature data="<<feature_contents.str()<<endl;
      if (!VW::GetXMLBlock("Available",feature_contents,setting_contents,tmp))
	cerr << "Errr, no <Available> tag found" << endl;
      cerr << "Setting="<<setting_contents.str()<<endl;
      setting_contents >> isavailable;
      if (isavailable==0) {
	// Don't bother reading any more
	cerr << "Feature not available" << endl;
      } else {
	// Read the auto and value
	int isauto=0;
	VW::GetXMLBlock("Automatic",feature_contents,setting_contents,tmp);
	setting_contents >> isauto;

	// Handle WhiteBalance differently
	if (featurename=="WhiteBalance") {
	  int uvalue=0,vvalue=0;
	  VW::GetXMLBlock("UValue",feature_contents,setting_contents,tmp);
	  setting_contents >> uvalue;
	  VW::GetXMLBlock("VValue",feature_contents,setting_contents,tmp);
	  setting_contents >> vvalue;
	  UpdateFeatureByName(featurename,isauto,uvalue,vvalue);
	} else {
	  int value=0;
	  VW::GetXMLBlock("Value",feature_contents,setting_contents,tmp);
	  setting_contents >> value;
	  UpdateFeatureByName(featurename,isauto,value);
	}
      }
    }
    feature_contents.clear();
  }
  
  return(true);
}

bool VW::FirewireCamera::ReadBinary(std::istream& stream)
{
  cerr << "Sorry, VW::FirewireCamera::ReadBinary not implemented yet." << endl;
  exit(0);
}
