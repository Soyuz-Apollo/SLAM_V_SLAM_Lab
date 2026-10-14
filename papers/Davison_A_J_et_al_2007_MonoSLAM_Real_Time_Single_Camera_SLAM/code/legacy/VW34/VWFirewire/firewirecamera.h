/** 
 * @file   firewirecamera.h
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
 * @class   VW::FirewireCamera
 * @ingroup Hardware
 * @author  BJ Tordoff
 * @date    02/08/2002
 * @brief   This class is just a convenience wrapper for the various structs 
 *          which hold info for the cameras.
 */


#ifndef _FirewireCamera_H
#define _FirewireCamera_H


#include <iostream>
#include <string>
#include <vector>
#include <libdc1394/dc1394_control.h>
#include "sys/time.h"

// VW includes
#include <VW/iobasics.h>

const int CAMERA_ODOMETRY_HISTORY_LENGTH = 50000;


namespace VW {

  /** 
   *These are proper C++ enums of things that are enumerated in dc1394_control.h
   */
// 24 May 2001 torfi Initialized enums to dc1394_control.h constants
  namespace FirewireParams {


    /* Enumeration of camera modes */
    enum CameraMode {
      M_160x120_YUV444 = MODE_160x120_YUV444,  // Format 0 modes
      M_320x240_YUV422,
      M_640x480_YUV411,
      M_640x480_YUV422,
      M_640x480_RGB,
      M_640x480_MONO,
      M_640x480_MONO16,
      M_800x600_YUV422 = MODE_800x600_YUV422,  // Format 1 modes
      M_800x600_RGB,
      M_800x600_MONO,
      M_1024x768_YUV422,
      M_1024x768_RGB,
      M_1024x768_MONO,
      M_800x600_MONO16,
      M_1024x768_MONO16,
      M_1280x960_YUV422 = MODE_1280x960_YUV422,  // Format 2 modes
      M_1280x960_RGB,
      M_1280x960_MONO,
      M_1600x1200_YUV422,
      M_1600x1200_RGB,
      M_1600x1200_MONO,
      M_1280x960_MONO16,
      M_1600x1200_MONO16
    };
    
    /* Enumeration of camera image formats */
    enum CameraImageFormat {
      F_VGA_NONCOMPRESSED = FORMAT_VGA_NONCOMPRESSED,
      F_SVGA_NONCOMPRESSED_1,
      F_SVGA_NONCOMPRESSED_2,
      F_RESERVED_1,
      F_RESERVED_2,
      F_RESERVED_3,
      F_STILL_IMAGE,
      F_SCALABLE_IMAGE_SIZE
    };
    
    /* Enumeration of camera framerates */
    enum CameraFramerate
      {
	FPS_1_875 = FRAMERATE_1_875,
	FPS_3_75,
	FPS_7_5,
	FPS_15,
	FPS_30,
	FPS_60
      };

  } // End namespace FirewireParams
    
class FirewireCamera : public VW::IOObject {
 public:

  // General stuff
                         FirewireCamera() {};
                         FirewireCamera(bool display_messages);
  virtual bool           SetUpCamera(raw1394handle_t port_handle);
  void                   SetVerbosity(bool display_messages);


  // ISO Transmission stuff
  bool                   StartISOTransmission();
  bool                   StopISOTransmission();
  bool                   IsISOTransmitting() const;
  bool                   SetupDMACapture(dc1394_cameracapture *capture);
  bool                   ReleaseDMACapture(dc1394_cameracapture *capture);
  // Note that most of the camera control functionality is
  // represented by the three C-structs misc_info, feature_set
  // and camera_info. However, these provide public access to 
  // both data and functions, so are wrapped by inliners here.
  // I know this is pointless duplication, but I like it...
  //
  // The access functions are provided in order according to which
  // struct they are duplicating!


  //
  // Access functions (Camera Info)
  //

  void                   SetNodeNumber(nodeid_t nn);
  nodeid_t               GetNodeNumber(void) const;
  int                    GetISOChannel(void) const;


  /** Set the Raw1394 handle for sending commands. 
   * Note that receiveing images and sending commands use 
   * different handles to avoid conflicts. 
   */
  void                   SetFirewireCommandHandle(raw1394handle_t command_handle);
  /** Set the Raw1394 handle for receiving images. 
   * Note that receiveing images and sending commands use 
   * different handles to avoid conflicts. 
   */
  void                   SetFirewireReceiveHandle(raw1394handle_t receive_handle);
  /** Get the Raw1394 handle for sending commands. */ 
  raw1394handle_t        GetFirewireCommandHandle(void) const;
  /** Get the Raw1394 handle for receiving images. */ 
  raw1394handle_t        GetFirewireReceiveHandle(void) const;

  /** Get the Global Unique Identifier (GUID) that uniquely specifies this device. */
  std::string GetGUID() const;

  // For the following functions, note that the maximum allowed
  // vendor and model names are 32 characters, so the strings will
  // be up to this length.
  std::string            GetVendorString(void) const;
  std::string            GetModelString(void) const;

  //
  // Access functions (Misc Info)
  //

  FirewireParams::CameraMode      GetImageMode() const;
  void                   SetImageMode(FirewireParams::CameraMode new_mode);
  // Possibilities are     MODE_160x120_YUV444 = 0,
  //                       MODE_320x240_YUV422,
  //                       MODE_640x480_YUV411,
  //                       MODE_640x480_YUV422,
  //                       MODE_640x480_RGB,
  //                       MODE_640x480_MONO
  // The following more intuitive functions get parts of the mode string
  int                      GetImageWidth() const;
  int                      GetImageHeight() const;
  FirewireParams::CameraFramerate   GetFramerate(void) const;
  void                     SetFramerate(FirewireParams::CameraFramerate new_framerate);
  FirewireParams::CameraImageFormat GetImageFormat(void) const;
  void                     SetImageFormat(FirewireParams::CameraImageFormat new_image_format);

  // Many settings can be manual and auto, so we provide a quick way to set all
  void                   UseAutoSettings();
  void                   UseManualSettings();


  // What features are available
  bool                   HasZoom() const;
  bool                   HasFocus() const;
  bool                   HasAutoFocus() const;
  bool                   HasIris() const;
  bool                   HasAutoIris() const;
  bool                   HasBrightness() const;
  bool                   HasAutoBrightness() const;
  bool                   HasGain() const;
  bool                   HasAutoGain() const;
  bool                   HasExposure() const;
  bool                   HasAutoExposure() const;
  bool                   HasShutter() const;
  bool                   HasAutoShutter() const;
  bool                   HasGamma() const;
  bool                   HasAutoGamma() const;
  bool                   HasSharpness() const;
  bool                   HasAutoSharpness() const;
  bool                   HasHue() const;
  bool                   HasAutoHue() const;
  bool                   HasSaturation() const;
  bool                   HasAutoSaturation() const;
  bool                   HasWhiteBalance() const;
  bool                   HasAutoWhiteBalance() const;

  // Get functions
  int                    GetZoom() const;
  void                   GetZoomLimits(int* min_zoom, int* max_zoom) const;

  int                    GetFocus() const;
  void                   GetFocusLimits(int* min_focus, int* max_focus) const;
  bool                   GetAutoFocus() const;

  int                    GetIris() const;
  void                   GetIrisLimits(int* min_iris, int* max_iris) const;
  bool                   GetAutoIris() const;

  int                    GetBrightness() const;
  void                   GetBrightnessLimits(int* min_gain, int* max_gain) const;
  bool                   GetAutoBrightness() const;

  int                    GetGain() const;
  void                   GetGainLimits(int* min_gain, int* max_gain) const;
  bool                   GetAutoGain() const;

  int                    GetExposure() const;
  void                   GetExposureLimits(int* min_exposure, int* max_exposure) const;
  bool                   GetAutoExposure() const;

  int                    GetShutter() const;
  void                   GetShutterLimits(int* min_shutter, int* max_shutter) const;
  bool                   GetAutoShutter() const;

  int                    GetGamma() const;
  void                   GetGammaLimits(int* min_gamma, int* max_gamma) const;
  bool                   GetAutoGamma() const;

  int                    GetSharpness() const;
  void                   GetSharpnessLimits(int* min_sharpness, int* max_sharpness) const;
  bool                   GetAutoSharpness() const;

  int                    GetHue() const;
  void                   GetHueLimits(int* min_hue, int* max_hue) const;
  bool                   GetAutoHue() const;

  int                    GetSaturation() const;
  void                   GetSaturationLimits(int* min_saturation, int* max_saturation) const;
  bool                   GetAutoSaturation() const;

  int                    GetWhiteBalanceU() const;
  int                    GetWhiteBalanceV() const;
  void                   GetWhiteBalanceULimits(int* min_wbu, int* max_wbu) const;
  void                   GetWhiteBalanceVLimits(int* min_wbv, int* max_wbv) const;
  bool                   GetAutoWhiteBalance() const;



  // Set functions can be overloaded by interfaces
  virtual void           SetZoom(int new_zoom);
  virtual void           SetFocus(int new_focus);
  virtual void           SetAutoFocus(bool new_state);
  virtual void           SetIris(int new_iris);
  virtual void           SetAutoIris(bool new_state);
  virtual void           SetGain(int new_gain);
  virtual void           SetAutoGain(bool new_state);
  virtual void           SetBrightness(int new_brightness);
  virtual void           SetAutoBrightness(bool new_state);
  virtual void           SetExposure(int new_exposure);
  virtual void           SetAutoExposure(bool new_state);
  virtual void           SetShutter(int new_shutter);
  virtual void           SetAutoShutter(bool new_state);
  virtual void           SetGamma(int new_gamma);
  virtual void           SetAutoGamma(bool new_state);
  virtual void           SetSharpness(int new_sharpness);
  virtual void           SetAutoSharpness(bool new_state);
  virtual void           SetHue(int new_hue);
  virtual void           SetAutoHue(bool new_state);
  virtual void           SetSaturation(int new_saturation);
  virtual void           SetAutoSaturation(bool new_state);
  virtual void           SetWhiteBalance(int new_U,int new_V);
  virtual void           SetAutoWhiteBalance(bool new_state);

  // (de-) Activate the camera
  bool                   SwitchOn(void);
  bool                   SwitchOff(void);

  // Set a feature using its name as a string (eg "Zoom", "Gain" etc)
  // Note that value2 is needed only for white balance
  void UpdateFeatureByName(const std::string& name,
			   bool automatic,
			   int value,
			   int value2=0);

  // Retreiving interpolated old odometry values
  int  GetHistoricPosition(struct timeval time, 
			   double *zoom);
  void GetOdometryHistory(std::vector<timeval>& time,
			  std::vector<double>& zoom_history);
  void FlushHistory(void);



  // Streaming funcs
  virtual 
  bool WriteASCII(std::ostream& stream) const;
  virtual 
  bool WriteXML(std::ostream& stream,
		const std::string& name="") const;
  virtual 
  bool WriteBinary(std::ostream& stream) const;


  virtual 
  bool ReadASCII(std::istream& stream);
  virtual 
  bool ReadXML(std::istream& stream, 
	       const std::string& name="");
  virtual 
  bool ReadBinary(std::istream& stream);


protected:
  dc1394_camerainfo camera_info;
  dc1394_feature_set feature_set;
  dc1394_miscinfo misc_info;
  bool verbose;
  // Easy access to mode specs
  int image_width;
  int image_height;
  // handle for image transfer
  raw1394handle_t receive_handle;

  // For odometry history
  void StoreOdometryHistory(struct timeval& time,
			    int zoom);
  int zoom_history[CAMERA_ODOMETRY_HISTORY_LENGTH];
  struct timeval time_history[CAMERA_ODOMETRY_HISTORY_LENGTH];
  int history_pointer;
};

}; // End-namespace


/***************** INLINES *****************/

inline void VW::FirewireCamera::SetVerbosity(bool display_messages)
{
  verbose=display_messages;
};

inline std::string VW::FirewireCamera::GetGUID() const
{
  quadlet_t full_guid = camera_info.euid_64 & 0xffffffff;
  quadlet_t device_guid = (camera_info.euid_64 >>32) & 0x000000ff;
  quadlet_t vendor_guid = (camera_info.euid_64 >>40) & 0xfffff;
  char temp[19];
  sprintf(temp,"0x%06x-%02x%08x", vendor_guid, device_guid, full_guid);
  return (std::string(temp));
};

inline void VW::FirewireCamera::SetNodeNumber(nodeid_t nn)
{
  camera_info.id=nn;
};

inline nodeid_t VW::FirewireCamera::GetNodeNumber(void) const
{
  return(camera_info.id);
};

inline int VW::FirewireCamera::GetISOChannel(void) const
{
  return(misc_info.iso_channel);
}


inline void 
VW::FirewireCamera::SetFirewireCommandHandle(raw1394handle_t command_handle)
{
  camera_info.handle=command_handle;
};

inline raw1394handle_t VW::FirewireCamera::GetFirewireCommandHandle(void) const
{
  return(camera_info.handle);
};

inline void 
VW::FirewireCamera::SetFirewireReceiveHandle(raw1394handle_t receive_handle)
{
  this->receive_handle=receive_handle;
};

inline raw1394handle_t VW::FirewireCamera::GetFirewireReceiveHandle(void) const
{
  return(receive_handle);
};

// Misc data
inline VW::FirewireParams::CameraMode VW::FirewireCamera::GetImageMode() const
{
  return(FirewireParams::CameraMode(misc_info.mode));
};

inline int VW::FirewireCamera::GetImageWidth(void) const
{
  return(image_width);
};

inline int VW::FirewireCamera::GetImageHeight(void) const
{
  return(image_height);
};

inline VW::FirewireParams::CameraFramerate 
VW::FirewireCamera::GetFramerate(void) const
{
  return(FirewireParams::CameraFramerate(misc_info.framerate));
};

inline void 
VW::FirewireCamera::SetFramerate(FirewireParams::CameraFramerate new_framerate)
{
  if (!dc1394_set_video_framerate(camera_info.handle,
			     camera_info.id,
				  new_framerate))
    std::cerr << "VW::FirewireCamera::SetFramerate - "
	 << "could not set framerate!" << std::endl;
  else
    misc_info.framerate=new_framerate;
};

inline VW::FirewireParams::CameraImageFormat 
VW::FirewireCamera::GetImageFormat(void) const
{
  return(FirewireParams::CameraImageFormat(misc_info.format));
};

inline void VW::FirewireCamera::SetImageFormat(FirewireParams::CameraImageFormat new_image_format) 
{
  if (!dc1394_set_video_format(camera_info.handle,
			       camera_info.id,
			       new_image_format))
    std::cerr << "VW::FirewireCamera::SetImageFormat - "
	 << "could not set new image format!" << std::endl;
  else
    misc_info.format=new_image_format;
};




/**********************/
/*      Features      */
/**********************/

// {{{ Zoom
inline bool VW::FirewireCamera::HasZoom() const 
{
  if(feature_set.feature[FEATURE_ZOOM-FEATURE_MIN].available) return (true);
  else return(false);
};

inline int VW::FirewireCamera::GetZoom() const
{
  unsigned int z;
  if(dc1394_get_zoom(camera_info.handle,camera_info.id,&z)) {
    return (int(z));
  }
  else return(0);
};

inline void VW::FirewireCamera::GetZoomLimits(int* min_zoom, int* max_zoom) const 
{
  unsigned int mi,ma;
  dc1394_get_min_value(camera_info.handle,camera_info.id,FEATURE_ZOOM,&mi);
  dc1394_get_max_value(camera_info.handle,camera_info.id,FEATURE_ZOOM,&ma);
  *min_zoom=int(mi);
  *max_zoom=int(ma);
};
// }}}

// {{{ Focus
inline bool VW::FirewireCamera::HasFocus() const 
{
  if(feature_set.feature[FEATURE_FOCUS-FEATURE_MIN].available) return (true);
  else return(false);
};

inline bool VW::FirewireCamera::HasAutoFocus() const 
{
  if(feature_set.feature[FEATURE_FOCUS-FEATURE_MIN].auto_capable) return (true);
  else return(false);
};

inline int VW::FirewireCamera::GetFocus() const 
{
  unsigned int f;
  if(dc1394_get_focus(camera_info.handle,camera_info.id,&f)) return (int(f));
  else return(0);
};

inline void VW::FirewireCamera::GetFocusLimits(int* min_focus, int* max_focus) const 
{
  unsigned int mi,ma;
  dc1394_get_min_value(camera_info.handle,camera_info.id,FEATURE_FOCUS,&mi);
  dc1394_get_max_value(camera_info.handle,camera_info.id,FEATURE_FOCUS,&ma);
  *min_focus=int(mi);
  *max_focus=int(ma);
};
inline bool VW::FirewireCamera::GetAutoFocus() const 
{
  return(feature_set.feature[FEATURE_FOCUS-FEATURE_MIN].auto_active);
}
// }}}

// {{{ Iris
inline bool VW::FirewireCamera::HasIris() const 
{
  if(feature_set.feature[FEATURE_IRIS-FEATURE_MIN].available) return (true);
  else return(false);
};

inline bool VW::FirewireCamera::HasAutoIris() const 
{
  if(feature_set.feature[FEATURE_IRIS-FEATURE_MIN].auto_capable) return (true);
  else return(false);
};

inline int VW::FirewireCamera::GetIris() const 
{
  unsigned int i;
  if(dc1394_get_iris(camera_info.handle,camera_info.id,&i)) return (int(i));
  else return(0);
};

inline void VW::FirewireCamera::GetIrisLimits(int* min_iris, int* max_iris) const 
{
  unsigned int mi,ma;
  dc1394_get_min_value(camera_info.handle,camera_info.id,FEATURE_IRIS,&mi);
  dc1394_get_max_value(camera_info.handle,camera_info.id,FEATURE_IRIS,&ma);
  *min_iris=int(mi);
  *max_iris=int(ma);
};

inline bool VW::FirewireCamera::GetAutoIris() const 
{
  return(feature_set.feature[FEATURE_IRIS-FEATURE_MIN].auto_active);
}

// }}}

// {{{ Brightness
inline bool VW::FirewireCamera::HasBrightness() const 
{
  if(feature_set.feature[FEATURE_GAIN-FEATURE_MIN].available) return (true);
  else return(false);
};

inline bool VW::FirewireCamera::HasAutoBrightness() const 
{
  if(feature_set.feature[FEATURE_BRIGHTNESS-FEATURE_MIN].auto_capable) return (true);
  else return(false);
};

inline int VW::FirewireCamera::GetBrightness() const 
{
  unsigned int i;
  if(dc1394_get_brightness(camera_info.handle,camera_info.id,&i)) return (int(i));
  else return(0);
};

inline bool VW::FirewireCamera::GetAutoBrightness() const 
{
  return(feature_set.feature[FEATURE_BRIGHTNESS-FEATURE_MIN].auto_active);
}

inline void VW::FirewireCamera::GetBrightnessLimits(int* min_gain, int* max_gain) const 
{
  unsigned int mi,ma;
  dc1394_get_min_value(camera_info.handle,camera_info.id,FEATURE_BRIGHTNESS,&mi);
  dc1394_get_max_value(camera_info.handle,camera_info.id,FEATURE_BRIGHTNESS,&ma);
  *min_gain=int(mi);
  *max_gain=int(ma);
};
// }}}

// {{{ Gain
inline bool VW::FirewireCamera::HasGain() const 
{
  if(feature_set.feature[FEATURE_GAIN-FEATURE_MIN].available) return (true);
  else return(false);
};

inline bool VW::FirewireCamera::HasAutoGain() const 
{
  if(feature_set.feature[FEATURE_GAIN-FEATURE_MIN].auto_capable) return (true);
  else return(false);
};

inline int VW::FirewireCamera::GetGain() const 
{
  unsigned int i;
  if(dc1394_get_gain(camera_info.handle,camera_info.id,&i)) return (int(i));
  else return(0);
};

inline bool VW::FirewireCamera::GetAutoGain() const 
{
  return(feature_set.feature[FEATURE_GAIN-FEATURE_MIN].auto_active);
}

inline void VW::FirewireCamera::GetGainLimits(int* min_gain, int* max_gain) const 
{
  unsigned int mi,ma;
  dc1394_get_min_value(camera_info.handle,camera_info.id,FEATURE_GAIN,&mi);
  dc1394_get_max_value(camera_info.handle,camera_info.id,FEATURE_GAIN,&ma);
  *min_gain=int(mi);
  *max_gain=int(ma);
};
// }}}

// {{{ Exposure
inline bool VW::FirewireCamera::HasExposure() const 
{
  if(feature_set.feature[FEATURE_EXPOSURE-FEATURE_MIN].available) return (true);
  else return(false);
};

inline bool VW::FirewireCamera::HasAutoExposure() const 
{
  if(feature_set.feature[FEATURE_EXPOSURE-FEATURE_MIN].auto_capable) return (true);
  else return(false);
};

inline int VW::FirewireCamera::GetExposure() const 
{
  unsigned int i;
  if(dc1394_get_exposure(camera_info.handle,camera_info.id,&i)) return (int(i));
  else return(0);
};

inline bool VW::FirewireCamera::GetAutoExposure() const 
{
  return(feature_set.feature[FEATURE_EXPOSURE-FEATURE_MIN].auto_active);
}

inline void VW::FirewireCamera::GetExposureLimits(int* min_exposure, int* max_exposure) const 
{
  unsigned int mi,ma;
  dc1394_get_min_value(camera_info.handle,camera_info.id,FEATURE_EXPOSURE,&mi);
  dc1394_get_max_value(camera_info.handle,camera_info.id,FEATURE_EXPOSURE,&ma);
  *min_exposure=int(mi);
  *max_exposure=int(ma);
};
// }}}

// {{{ Shutter
inline bool VW::FirewireCamera::HasShutter() const 
{
  if(feature_set.feature[FEATURE_SHUTTER-FEATURE_MIN].available) return (true);
  else return(false);
};

inline bool VW::FirewireCamera::HasAutoShutter() const 
{
  if(feature_set.feature[FEATURE_SHUTTER-FEATURE_MIN].auto_capable) return (true);
  else return(false);
};

inline int VW::FirewireCamera::GetShutter() const 
{
  unsigned int i;
  if(dc1394_get_shutter(camera_info.handle,camera_info.id,&i)) return (int(i));
  else return(0);
};

inline bool VW::FirewireCamera::GetAutoShutter() const 
{
  return(feature_set.feature[FEATURE_SHUTTER-FEATURE_MIN].auto_active);
}

inline void VW::FirewireCamera::GetShutterLimits(int* min_shutter, int* max_shutter) const 
{
  unsigned int mi,ma;
  dc1394_get_min_value(camera_info.handle,camera_info.id,FEATURE_SHUTTER,&mi);
  dc1394_get_max_value(camera_info.handle,camera_info.id,FEATURE_SHUTTER,&ma);
  *min_shutter=int(mi);
  *max_shutter=int(ma);
};
// }}}

// {{{ Gamma
inline bool VW::FirewireCamera::HasGamma() const 
{
  if(feature_set.feature[FEATURE_GAMMA-FEATURE_MIN].available) return (true);
  else return(false);
};

inline bool VW::FirewireCamera::HasAutoGamma() const 
{
  if(feature_set.feature[FEATURE_GAMMA-FEATURE_MIN].auto_capable) return (true);
  else return(false);
};

inline int VW::FirewireCamera::GetGamma() const 
{
  unsigned int i;
  if(dc1394_get_gamma(camera_info.handle,camera_info.id,&i)) return (int(i));
  else return(0);
};

inline bool VW::FirewireCamera::GetAutoGamma() const 
{
  return(feature_set.feature[FEATURE_GAMMA-FEATURE_MIN].auto_active);
}

inline void VW::FirewireCamera::GetGammaLimits(int* min_gamma, int* max_gamma) const 
{
  unsigned int mi,ma;
  dc1394_get_min_value(camera_info.handle,camera_info.id,FEATURE_GAMMA,&mi);
  dc1394_get_max_value(camera_info.handle,camera_info.id,FEATURE_GAMMA,&ma);
  *min_gamma=int(mi);
  *max_gamma=int(ma);
};
// }}}

// {{{ Sharpness
inline bool VW::FirewireCamera::HasSharpness() const 
{
  if(feature_set.feature[FEATURE_SHARPNESS-FEATURE_MIN].available) return (true);
  else return(false);
};

inline bool VW::FirewireCamera::HasAutoSharpness() const 
{
  if(feature_set.feature[FEATURE_SHARPNESS-FEATURE_MIN].auto_capable) return (true);
  else return(false);
};

inline int VW::FirewireCamera::GetSharpness() const 
{
  unsigned int i;
  if(dc1394_get_sharpness(camera_info.handle,camera_info.id,&i)) return (int(i));
  else return(0);
};

inline bool VW::FirewireCamera::GetAutoSharpness() const 
{
  return(feature_set.feature[FEATURE_SHARPNESS-FEATURE_MIN].auto_active);
}

inline void VW::FirewireCamera::GetSharpnessLimits(int* min_sharpness, int* max_sharpness) const 
{
  unsigned int mi,ma;
  dc1394_get_min_value(camera_info.handle,camera_info.id,FEATURE_SHARPNESS,&mi);
  dc1394_get_max_value(camera_info.handle,camera_info.id,FEATURE_SHARPNESS,&ma);
  *min_sharpness=int(mi);
  *max_sharpness=int(ma);
};
// }}}

// {{{ Hue
inline bool VW::FirewireCamera::HasHue() const 
{
  if(feature_set.feature[FEATURE_HUE-FEATURE_MIN].available) return (true);
  else return(false);
};

inline bool VW::FirewireCamera::HasAutoHue() const 
{
  if(feature_set.feature[FEATURE_HUE-FEATURE_MIN].auto_capable) return (true);
  else return(false);
};

inline int VW::FirewireCamera::GetHue() const 
{
  unsigned int i;
  if(dc1394_get_hue(camera_info.handle,camera_info.id,&i)) return (int(i));
  else return(0);
};

inline bool VW::FirewireCamera::GetAutoHue() const 
{
  return(feature_set.feature[FEATURE_HUE-FEATURE_MIN].auto_active);
}

inline void VW::FirewireCamera::GetHueLimits(int* min_hue, int* max_hue) const 
{
  unsigned int mi,ma;
  dc1394_get_min_value(camera_info.handle,camera_info.id,FEATURE_HUE,&mi);
  dc1394_get_max_value(camera_info.handle,camera_info.id,FEATURE_HUE,&ma);
  *min_hue=int(mi);
  *max_hue=int(ma);
};
// }}}

// {{{ Saturation
inline bool VW::FirewireCamera::HasSaturation() const 
{
  if(feature_set.feature[FEATURE_SATURATION-FEATURE_MIN].available) return (true);
  else return(false);
};

inline bool VW::FirewireCamera::HasAutoSaturation() const 
{
  if(feature_set.feature[FEATURE_SATURATION-FEATURE_MIN].auto_capable) return (true);
  else return(false);
};

inline int VW::FirewireCamera::GetSaturation() const 
{
  unsigned int i;
  if(dc1394_get_saturation(camera_info.handle,camera_info.id,&i)) return (int(i));
  else return(0);
};

inline bool VW::FirewireCamera::GetAutoSaturation() const 
{
  return(feature_set.feature[FEATURE_SATURATION-FEATURE_MIN].auto_active);
}

inline void VW::FirewireCamera::GetSaturationLimits(int* min_saturation, int* max_saturation) const 
{
  unsigned int mi,ma;
  dc1394_get_min_value(camera_info.handle,camera_info.id,FEATURE_SATURATION,&mi);
  dc1394_get_max_value(camera_info.handle,camera_info.id,FEATURE_SATURATION,&ma);
  *min_saturation=int(mi);
  *max_saturation=int(ma);
};
// }}}

// {{{ WhiteBalance
inline bool VW::FirewireCamera::HasWhiteBalance() const 
{
  if(feature_set.feature[FEATURE_WHITE_BALANCE-FEATURE_MIN].available) return (true);
  else return(false);
};

inline int VW::FirewireCamera::GetWhiteBalanceU() const 
{
  unsigned int u,v;
  if(dc1394_get_white_balance(camera_info.handle,camera_info.id,&u,&v)) return (int(u));
  else  {
    std::cerr << "VW::FirewireCamera::GetWhiteBalanceU - failed" << std::endl;
    return(0);
  }
};
inline int VW::FirewireCamera::GetWhiteBalanceV() const 
{
  unsigned int u,v;
  if(dc1394_get_white_balance(camera_info.handle,camera_info.id,&u,&v)) return (int(v));
  else {
    std::cerr << "VW::FirewireCamera::GetWhiteBalanceV - failed" << std::endl;
    return(0);
  }
};

// At the moment both U and V white-balances have the same scale, but
// just incase, we read them seperately
inline void VW::FirewireCamera::GetWhiteBalanceULimits(int* min_wbu, int* max_wbu) const 
{
  unsigned int mi,ma;
  dc1394_get_min_value(camera_info.handle,camera_info.id,FEATURE_WHITE_BALANCE,&mi);
  dc1394_get_max_value(camera_info.handle,camera_info.id,FEATURE_WHITE_BALANCE,&ma);
  *min_wbu=int(mi);
  *max_wbu=int(ma);
};

inline void VW::FirewireCamera::GetWhiteBalanceVLimits(int* min_wbv, int* max_wbv) const 
{
  unsigned int mi,ma;
  dc1394_get_min_value(camera_info.handle,camera_info.id,FEATURE_WHITE_BALANCE,&mi);
  dc1394_get_max_value(camera_info.handle,camera_info.id,FEATURE_WHITE_BALANCE,&ma);
  *min_wbv=int(mi);
  *max_wbv=int(ma);
};

inline bool VW::FirewireCamera::HasAutoWhiteBalance() const 
{
  if(feature_set.feature[FEATURE_WHITE_BALANCE-FEATURE_MIN].auto_capable) return (true);
  else return(false);
};

inline bool VW::FirewireCamera::GetAutoWhiteBalance() const 
{
  return(feature_set.feature[FEATURE_WHITE_BALANCE-FEATURE_MIN].auto_active);
}

// }}}


inline std::string VW::FirewireCamera::GetVendorString(void) const 
{
  return(std::string(camera_info.vendor));
};

inline std::string VW::FirewireCamera::GetModelString(void) const
{
  return(std::string(camera_info.model));
};

inline bool VW::FirewireCamera::SwitchOn(void)
{
  if(dc1394_camera_on(camera_info.handle,camera_info.id)) 
    return(true);
  else 
    return(false);
};

inline bool VW::FirewireCamera::SwitchOff(void)
{
  if(dc1394_camera_off(camera_info.handle,camera_info.id)) 
    return(true);
  else 
    return(false);
};

inline bool VW::FirewireCamera::IsISOTransmitting() const
{
  return(misc_info.is_iso_on!=0);
}

inline void VW::FirewireCamera::StoreOdometryHistory(struct timeval& time,
						     int zoom)
{
      // Move to the next entry and store
      history_pointer++;
      if (history_pointer==CAMERA_ODOMETRY_HISTORY_LENGTH) history_pointer=0;
      
      zoom_history[history_pointer]=zoom;
      time_history[history_pointer]=time;
     
}

#endif 
