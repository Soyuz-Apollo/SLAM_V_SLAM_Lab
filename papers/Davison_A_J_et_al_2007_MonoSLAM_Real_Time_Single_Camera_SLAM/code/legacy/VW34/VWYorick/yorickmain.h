/** 
 * @file   yorickmain.h
 * @author ID Reid, E Hayman, BJ Tordoff
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



#ifndef _yorick_class_
#define _yorick_class_

#include "sys/time.h"
#include <vector>
#include <signal.h>
#include "yorickpmac.h"



// default values
const int MAX_COUNTS = 500;
const float ODOMETRY_ACCEPT = 0.12;  

// odometry history stuff
const int ODOMETRY_CAPTURE_DELAY = 1; // (in millisecs - minimum is probably about 20ms on Annie)
const int YORICK_ODOMETRY_HISTORY_LENGTH = 500;

namespace VW {

class Yorick {

public:
  // Creation and Destruction
  Yorick(void);
  virtual ~Yorick();

  // Axis enumeration - use these to refer to individual axes
  enum YorickAxis {
    YORICK_PAN        =YorickPMAC::PMAC_PAN,
    YORICK_TILT       =YorickPMAC::PMAC_TILT,
    YORICK_VERGELEFT  =YorickPMAC::PMAC_VERGELEFT,
    YORICK_VERGERIGHT =YorickPMAC::PMAC_VERGERIGHT 
  };


  // Switching things on and off
  bool SetActive(bool state);
  void SetVergeLeftActive(bool state);    // Inline (see below)
  void SetVergeRightActive(bool state);   // Inline
  void SetTiltActive(bool state);         // Inline
  void SetPanActive(bool state);          // Inline
  bool GetActive(void);                   // Inline
  bool GetVergeLeftActive(void);          // Inline
  bool GetVergeRightActive(void);         // Inline
  bool GetTiltActive(void);               // Inline
  bool GetPanActive(void);                // Inline

  // Axis demands
  virtual int Demand(float p, float t, float vl, float vr);
  virtual int DemandPan(float p);
  virtual int DemandTilt(float t);
  virtual int DemandPanTilt(float p, float t);
  virtual int DemandVergeLeft(float vl);
  virtual int DemandVergeRight(float vr);
  virtual int DemandVerge(float vl, float vr);

  // Slow demands
  int SlowRecentre(void);
  int SlowDemand(float p, float t, float vl, float vr);

  // Wait for Yorick to get to the demanded position
  int WaitForCompletion(int = MAX_COUNTS, float = ODOMETRY_ACCEPT);
  // Reset odometry to show zero at this position
  int Homez(void);
  int HomezOneAxis(YorickAxis axis);

  // Axis Enquiries (read from PMAC)
  int   GetPosition        (float *p, float *t, float *vl, float *vr);
  int   GetPositionAndStore(float *p, float *t, float *vl, float *vr);
  int   GetVelocity        (float *vp, float *vt, float *vvl, float *vvr);

  // Axis values (current position)
  float GetInstantVergeLeft(void);
  float GetInstantVergeRight(void);
  float GetInstantTilt(void);
  float GetInstantPan(void);
  // Axis values (read from local variables - ie. last demand or odometry)
  float GetVergeLeft(void);
  float GetVergeRight(void);
  float GetTilt(void);
  float GetPan(void);


  // Axis limits
  void GetVergeLeftLimits(float* minv, float* maxv); //INLINE
  void GetVergeRightLimits(float* minv, float* maxv); //INLINE
  void GetTiltLimits(float* mint, float* maxt); //INLINE
  void GetPanLimits(float* minp, float* maxp); //INLINE

  // Retreiving interpolated old odometry values
  int  GetHistoricPosition(struct timeval time, 
			   float& p, float& t, 
			   float& vl, float& vr);
  int  GetHistoricPositionAndUncertainty(struct timeval time, 
					 float& p, float& t, 
					 float& vl, float& vr, 
					 float& delta_p,  float& delta_t, 
					 float& delta_vl, float& delta_vr);
  void ShowOdometryHistory(void);
  void GetOdometryHistory(std::vector<timeval>& time,
			  std::vector<double>& pan_history,
			  std::vector<double>& tilt_history,
			  std::vector<double>& vergeleft_history,
			  std::vector<double>& vergeright_history);
  void FlushHistory(void);

  // Other Enquiries
  int IsPMACOpen();
  int ReadState(void);
  

  // Axis limits
  static const float PAN_LIMIT;
  static const float TILT_LIMIT;
  static const float VLEFT_LIMIT;
  static const float VRIGHT_LIMIT;

protected:

  int seq_no;
  float pan, tilt, vleft, vright;
                // these store the most recent head demands 
  // The PMAC
  YorickPMAC pmac;


private:
  // Functions
  void ReadAndStoreOdometry();
  void StoreOdometryHistory(struct timeval& time,
			    float pan,float tilt,
			    float vleft,float vright);


  // Data
  bool active; // Is Yorick actually on


  bool active_vergeleft; // Is axis verge left in use
  bool active_vergeright; // Is axis verge right in use
  bool active_tilt; // Is axis tilt in use
  bool active_pan; // Is axis pan in use
  
  // Stuff for the odometry history
  struct timeval  init_time;
  float pan_history[YORICK_ODOMETRY_HISTORY_LENGTH];
  float tilt_history[YORICK_ODOMETRY_HISTORY_LENGTH];
  float vleft_history[YORICK_ODOMETRY_HISTORY_LENGTH];
  float vright_history[YORICK_ODOMETRY_HISTORY_LENGTH];
  struct timeval time_history[YORICK_ODOMETRY_HISTORY_LENGTH];
  int history_pointer;

};

}; // End namespace Yorick

// Inline functions given here, not in the .c file
inline void VW::Yorick::SetVergeLeftActive(bool state) {active_vergeleft = state;}
inline void VW::Yorick::SetVergeRightActive(bool state) {active_vergeright = state;}
inline void VW::Yorick::SetTiltActive(bool state) {active_tilt = state;}
inline void VW::Yorick::SetPanActive(bool state) {active_pan = state;}
inline bool VW::Yorick::GetActive(void) {return active;}
inline bool VW::Yorick::GetVergeLeftActive(void) {return active_vergeleft;}
inline bool VW::Yorick::GetVergeRightActive(void) {return active_vergeright;}
inline bool VW::Yorick::GetTiltActive(void) {return active_tilt;}
inline bool VW::Yorick::GetPanActive(void) {return active_pan;}
inline float VW::Yorick::GetVergeLeft(void) {return vleft;}
inline float VW::Yorick::GetVergeRight(void) {return vright;}
inline float VW::Yorick::GetTilt(void) {return tilt;}
inline float VW::Yorick::GetPan(void) {return pan;}
//inline int VW::Yorick::ZeroAxes(void) {return pmac.ZeroOdometry ();}
  // Axis limits
inline void VW::Yorick::GetVergeLeftLimits(float* minv, float* maxv) {
  *minv=-VLEFT_LIMIT;
  *maxv= VLEFT_LIMIT;
}
inline void VW::Yorick::GetVergeRightLimits(float* minv, float* maxv) {
  *minv=-VRIGHT_LIMIT;
  *maxv= VRIGHT_LIMIT;
}
inline void VW::Yorick::GetTiltLimits(float* mint, float* maxt) {
  *mint=-TILT_LIMIT;
  *maxt= TILT_LIMIT;
}
inline void VW::Yorick::GetPanLimits(float* minp, float* maxp) {
  *minp=-PAN_LIMIT;
  *maxp= PAN_LIMIT;
}

inline void VW::Yorick::ReadAndStoreOdometry()
{
  float pn,tn,vln,vrn;
  struct timeval now;
  int t;
  // Get the odom
  pmac.GetOdometry(seq_no, &t, &pn, &tn, &vln, &vrn);
  gettimeofday(&now, NULL);
  StoreOdometryHistory(now,pn,tn,vln,vrn);
}

inline void VW::Yorick::StoreOdometryHistory(struct timeval& time,
					     float pan,float tilt,
					     float vleft,float vright)
{
  //cerr << "** Storing odometry" << endl;
      // Move to the next entry and store
      history_pointer++;
      if (history_pointer==YORICK_ODOMETRY_HISTORY_LENGTH) history_pointer=0;
      
      pan_history[history_pointer]=pan;
      tilt_history[history_pointer]=tilt;
      vleft_history[history_pointer]=vleft;
      vright_history[history_pointer]=vright;
      time_history[history_pointer]=time;
      
 
}

///////////////////////////////////////////////////////////

#endif












