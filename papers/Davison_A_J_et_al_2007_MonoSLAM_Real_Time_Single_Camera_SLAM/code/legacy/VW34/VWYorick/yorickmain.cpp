/** 
 * @file   yorickmain.cc
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


/**********************************************/
/*                                            */
/*       Yorick Class - Hacked to Bits        */
/*                                            */
/*  This yorick class is derrived from        */
/*  module.h, via some Hayman Hacks. It       */
/*  uses the pmac.cc class.                   */
/*                                            */
/*  NB: the //{{{ and //}}} things are fold   */
/*  marks for use with 'folding.el' - the     */
/*  emacs fold extension. Try it!             */
/*                                            */
/**********************************************/




#include <iostream>
#include <algorithm>
#include <math.h>

extern "C" {
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
}

#include "VW/Utils/timeops.h"
#include "yorickpmac.h"
#include "yorickmain.h"

// Axis limits (software)
const float VW::Yorick::PAN_LIMIT    = 90.0;
const float VW::Yorick::TILT_LIMIT   = 60.0;
const float VW::Yorick::VLEFT_LIMIT  = 45.0;
const float VW::Yorick::VRIGHT_LIMIT = 45.0;
#define DEMAND_DELAY 12   // delay before demand is actioned (in ms)

#define VERBOSE false

using namespace std;

// NB Inline functions given in .h, not in the .c file

// (Except this one)
inline float abs(float A) {; return (A>0)?A:-A;}; 

// {{{ VW::Yorick::Yorick(void)

VW::Yorick::Yorick(void)
{
  // Initialise class data members
  seq_no = 0;
  pan=0.0; 
  tilt=0.0;
  vleft=0.0;
  vright=0.0;
  active = false;
  active_vergeleft=true;
  active_vergeright=true;
  active_tilt=true;
  active_pan=true;

  // Initialize the time and history counter
  gettimeofday(&init_time, NULL);
  for (int i=0;i<YORICK_ODOMETRY_HISTORY_LENGTH;i++) {
    pan_history[i]=0;
    tilt_history[i]=0;
    vleft_history[i]=0;
    vright_history[i]=0;
    time_history[i]=init_time;
  }
  history_pointer=0;

  return;
}

// }}}
// {{{ VW::Yorick::~Yorick(void)

VW::Yorick::~Yorick(void)
{
  // Just close the PMAC
  pmac.Close();
  return;
}

// }}}

// {{{ int VW::Yorick::IsPMACOpen()

int VW::Yorick::IsPMACOpen() {
  return (pmac.GetMode()==YorickPMAC::PMAC_ON);
} 

// }}}

//   Open pmac and enable head movements, or close it
// {{{ bool VW::Yorick::SetActive(bool state)

bool VW::Yorick::SetActive(bool state) {
  bool operation_success=false;
  if (state) {
    if (pmac.Init()) {
      //      SlowRecentre();
      // Flush odometry stacks
      FlushHistory();
      active=true;
      cerr << "VW::Yorick::SetActive(true) - PMAC opened." << endl;
      operation_success=true;

      // Start the odom thread
      // Must lock before accessing 'thread_running', which is shared

    } else {
      cerr << "VW::Yorick::SetActive(true) - Failed to open PMAC." << endl;
    }
  } else {
    if (pmac.GetMode() == YorickPMAC::PMAC_ON) {
     // Now close pmac
      pmac.SetModePending();
      pmac.Close();
      cerr << "VW::Yorick::SetActive(false) - PMAC closed OK." << endl;
      operation_success=true;
    } else {
      cerr << "VW::Yorick::SetActive(false) - PMAC not open!" << endl;
    }
    active=false;
  }
  return(operation_success);
}

// }}}


//   Read the program state
// {{{ int VW::Yorick::ReadState(void)

int VW::Yorick::ReadState(void)
{
  return (pmac.GetState());
}

// }}}


//   Send command to yorick get current odometry (in degrees)
// {{{ int VW::Yorick::GetPosition(float *p, float *t, float *vl, float *vr)

int VW::Yorick::GetPosition(float *p, float *t, float *vl, float *vr)
{
    char status=0;
    int time;
    float vln,vrn;
    float pn,tn;

    if (pmac.GetMode()==YorickPMAC::PMAC_ON) {
	pmac.GetOdometry(seq_no, &time, &pn, &tn, &vln, &vrn);
	if (vl) *vl = vln;
	if (vr) *vr = vrn;
	if (p) *p = pn;
	if (t) *t = tn;
    }

    return (int)status;
}

// }}}
// {{{ float VW::Yorick::GetInstant[Position] ()

float VW::Yorick::GetInstantVergeLeft ()
{
  float out;
  GetPosition (0, 0, &out, 0);
  return out;
}

float VW::Yorick::GetInstantVergeRight ()
{
  float out;
  GetPosition (0, 0, 0, &out);
  return out;
}

float VW::Yorick::GetInstantTilt ()
{
  float out;
  GetPosition (0, &out, 0, 0);
  return out;
}

float VW::Yorick::GetInstantPan ()
{
  float out;
  GetPosition (&out, 0, 0, 0);
  return out;
}

// }}}
// {{{ int VW::Yorick::GetVelocity(float *vp, float *vt, float *vvl, float *vvr)

int VW::Yorick::GetVelocity(float *vp, float *vt, float *vvl, float *vvr)
{
    char status=0;
    int time;
    float vel_vl,vel_vr;
    float vel_pan;
    float vel_tilt;

    if (pmac.GetMode()==YorickPMAC::PMAC_ON) {
	pmac.GetVelocity(seq_no, &time, &vel_pan, &vel_tilt, &vel_vl, &vel_vr);
	*vvl = vel_vl;
	*vvr = vel_vl;
	*vt = vel_tilt;
	*vp = vel_pan;
    }

    return (int(status));
}

// }}}

//   Send command to yorick get current odometry (in degrees) and store it
//   on the history lists
// {{{ int VW::Yorick::GetPositionAndStore(float *p, float *t, float *vl, float *vr)

int VW::Yorick::GetPositionAndStore(float *p, float *t, float *vl, float *vr)
{
    char status=0;
    int time;
    float vln,vrn;
    float pn,tn;

    if (pmac.GetMode()==YorickPMAC::PMAC_ON) {
	pmac.GetOdometry(seq_no, &time, &pn, &tn, &vln, &vrn);
	*vl = vln;
	*vr = vrn;
	*p = pn;
	*t = tn;
	struct timeval now;
	gettimeofday(&now, NULL);
	StoreOdometryHistory(now,*p,*t,*vl,*vr);
    }

    return (int)status;
}

// }}}





//   Wait until yorick is stationary && close to demanded position
//     I changed this to allow different values to the default to 
//     be input by the user (EH)
// {{{ int VW::Yorick::WaitForCompletion(int max_counts, float odometry_accept)

int VW::Yorick::WaitForCompletion(int max_counts,
				  float odometry_accept)
{
  // Function will exit after max_counts iterations
  // whatever to stop it ever getting stuck
  // odometry_accept determines the threshold for how close the 
  // head has to be to demand before exit

    char status=0;
    int counter = 0;    


    float current_pan, current_e, current_vl, current_vr,
          current_pan_velocity, current_e_velocity, 
          current_vl_velocity, current_vr_velocity;

    /* Demand sent --- now wait until Yorick is stationary */
    do
    {
      GetPosition(&current_pan, &current_e, 
		    &current_vl, &current_vr );
      GetVelocity(&current_pan_velocity, &current_e_velocity, 
		    &current_vl_velocity, &current_vr_velocity );
      counter++;
    } 
    while( counter < max_counts && (
           (  (current_pan - pan) * (current_pan - pan)
            + (current_e - tilt) * (current_e - tilt)
            + (current_vl - vleft) * (current_vl - vleft)
            + (current_vr - vright) * (current_vr - vright)
               > odometry_accept * odometry_accept )
	|| ( current_pan_velocity != 0.0 || current_e_velocity != 0.0 
          || current_vl_velocity != 0.0 || current_vl_velocity != 0.0 ) ) );

    return (int(status));
}

// }}}



//Set current position to zero on all axes
// {{{ int VW::Yorick::Homez(void)

int VW::Yorick::Homez(void)
{
    char status=0;
    float p,t,vl,vr;

    if (pmac.GetMode()==YorickPMAC::PMAC_ON) {
	vl = vleft = 0.0;
	vr = vright = 0.0;
	t = tilt = 0.0;
	p = pan = 0.0;
	// Tell the pmac to reset axes at this position
	pmac.Demand(++seq_no, 0,
		    YorickPMAC::PMAC_HOMEZ, YorickPMAC::PMAC_ALL, 
		    p,t,vl,vr);
	// Odometry is quick, so store current value
	ReadAndStoreOdometry();
    }
    
    return (int)status;
}

// }}}
// {{{ int VW::Yorick::HomezOneAxis(YorickAxis axis)

int VW::Yorick::HomezOneAxis(YorickAxis axis) {
    char status=0;

    switch (axis) {
    case YORICK_PAN:
      pan = 0.0;
      break;
    case YORICK_TILT:
      tilt = 0.0;
      break;
    case YORICK_VERGELEFT:
      vleft = 0.0;
      break;
    case YORICK_VERGERIGHT:
      vright = 0.0;
      break;
    }
    if (pmac.GetMode()==YorickPMAC::PMAC_ON) {
       pmac.Demand(++seq_no, 0, YorickPMAC::PMAC_HOMEZ, axis, 
		   pan,tilt,vleft,vright);
	// Odometry is quick, so store current value
	ReadAndStoreOdometry();
    }

    return (int(status));
}

// }}}

// Send a complete set of demands (in degrees)
// {{{ int VW::Yorick::Demand(float new_pan, float new_tilt, float new_vergeleft, float new_vergeright)

int VW::Yorick::Demand(float new_pan, float new_tilt, float new_vergeleft, float new_vergeright)
{
    char status=0;

    if ((new_pan>PAN_LIMIT) || (new_pan<-PAN_LIMIT) 
	|| (new_tilt>TILT_LIMIT) || (new_tilt<-TILT_LIMIT) 
	|| (new_vergeleft>VLEFT_LIMIT) || (new_vergeleft<-VLEFT_LIMIT) 
	|| (new_vergeright>VRIGHT_LIMIT) || (new_vergeright<-VRIGHT_LIMIT)) {
      // These are nasty, horrible values, so don't do it...
      perror("Ridiculous values demanded.");
    } else if (pmac.GetMode()==YorickPMAC::PMAC_ON) {
	vleft  = new_vergeleft;
	vright = new_vergeright;
	tilt   = new_tilt;
	pan    = new_pan;
	cerr << "   Make demand" << endl;
	pmac.Demand(++seq_no, 0,
		    YorickPMAC::PMAC_DEMAND, YorickPMAC::PMAC_ALL, 
		    pan,tilt,vleft,vright);
	// Odometry is quick, so store current value
	cerr << "   Store odom" << endl;
	ReadAndStoreOdometry();
    }
      cerr << "   Done making demand" << endl;
 
    return (int)status;
}

// }}}


// Demand one or more axes individually (in degrees)
// {{{ int VW::Yorick::DemandPanTilt(float new_pan, float new_tilt)

int VW::Yorick::DemandPanTilt(float new_pan, float new_tilt)
{
    char status=0;
    // Do some sanity checking to prevent wild and crazy goings on
    if ((new_pan>PAN_LIMIT) || (new_pan<-PAN_LIMIT) || 
	(new_tilt>TILT_LIMIT) || (new_tilt<-TILT_LIMIT)) {
      // These are nasty, horrible values, so don't do it...
      perror("Ridiculous values given for pan or tilt (or both).");

    } else if (pmac.GetMode()==YorickPMAC::PMAC_ON) {
	tilt = new_tilt;
	pan = new_pan;
	pmac.Demand(++seq_no, 0,
		    YorickPMAC::PMAC_DEMAND, YorickPMAC::PMAC_ALL, 
		    pan,tilt,vleft,vright);
	// Odometry is quick, so store current value
	ReadAndStoreOdometry();
    }


    return (int)status;
}

// }}}
// {{{ int VW::Yorick::DemandTilt(float new_tilt)

int VW::Yorick::DemandTilt(float new_tilt)
{
    char status=0;

    // Do some sanity checking to prevent wild and crazy goings on
    if ((new_tilt>TILT_LIMIT) || (new_tilt<-TILT_LIMIT)) {
      // These are nasty, horrible values, so don't do it...
      perror("Ridiculous values given for pan or tilt (or both).");

    } else if (pmac.GetMode()==YorickPMAC::PMAC_ON) {
	tilt = new_tilt;
	pmac.Demand(++seq_no, 0,
		    YorickPMAC::PMAC_DEMAND, YorickPMAC::PMAC_ALL, 
		    pan,tilt,vleft,vright);
	// Odometry is quick, so store current value
	ReadAndStoreOdometry();
    }


    return (int)status;
}

// }}}
// {{{ int VW::Yorick::DemandPan(float new_pan)

int VW::Yorick::DemandPan(float new_pan)
{
    char status=0;

    // Do some sanity checking to prevent wild and crazy goings on
    if ((new_pan>PAN_LIMIT) || (new_pan<-PAN_LIMIT)) {
      // These are nasty, horrible values, so don't do it...
      perror("VW::Yorick::DemandPan - Ridiculous values given for pan.");

    } else if ((pmac.GetMode()==YorickPMAC::PMAC_ON) && (active_pan)) {
	pan = new_pan;
	pmac.Demand(++seq_no, 0,
		    YorickPMAC::PMAC_DEMAND, YorickPMAC::PMAC_ALL, 
		    pan,tilt,vleft,vright);
	// Odometry is quick, so store current value
	ReadAndStoreOdometry();
    }


    return (int)status;
}

// }}}
// {{{ int VW::Yorick::DemandVerge(float new_vergeleft, float new_vergeright)

int VW::Yorick::DemandVerge(float new_vergeleft, float new_vergeright)
{
    char status=0;

    if ((new_vergeleft>VLEFT_LIMIT) || (new_vergeleft<-VLEFT_LIMIT) || 
	(new_vergeright>VRIGHT_LIMIT) || (new_vergeright<-VRIGHT_LIMIT)) {
      // These are nasty, horrible values, so don't do it...
      perror("VW::Yorick::DemandVerge - Ridiculous values given for vergence.");
    } else if (pmac.GetMode()==YorickPMAC::PMAC_ON) {
	vleft = new_vergeleft;vright = new_vergeright;
	pmac.Demand(++seq_no, 0,
		    YorickPMAC::PMAC_DEMAND, YorickPMAC::PMAC_ALL, 
		    pan,tilt,vleft,vright);
	// Odometry is quick, so store current value
	ReadAndStoreOdometry();
    }

    return (int)status;
}

// }}}
// {{{ int VW::Yorick::DemandVergeLeft(float new_vergeleft)

int VW::Yorick::DemandVergeLeft(float new_vergeleft)
{
    char status=0;

    if ((new_vergeleft>VLEFT_LIMIT) || (new_vergeleft<-VLEFT_LIMIT)) {
      // These are nasty, horrible values, so don't do it...
      perror("Ridiculous values given for verge-left.");
    } else if (pmac.GetMode()==YorickPMAC::PMAC_ON) {
	vleft = new_vergeleft;
	pmac.Demand(++seq_no, 0,
		    YorickPMAC::PMAC_DEMAND, YorickPMAC::PMAC_ALL, 
		    pan,tilt,vleft,vright);
	// Odometry is quick, so store current value
	ReadAndStoreOdometry();
 
    }

    return (int)status;
}

// }}}
// {{{ int VW::Yorick::DemandVergeRight(float new_vergeright)

int VW::Yorick::DemandVergeRight(float new_vergeright) {
    char status=0;

    if (pmac.GetMode()==YorickPMAC::PMAC_ON) {

      if ((new_vergeright>VRIGHT_LIMIT) || (new_vergeright<-VRIGHT_LIMIT)) {
	// These are nasty, horrible values, so don't do it...
	perror("Ridiculous values given for verge-left.");
      } else {
	vright = new_vergeright;
	pmac.Demand(++seq_no, 0,
		    YorickPMAC::PMAC_DEMAND, YorickPMAC::PMAC_ALL, 
		    pan,tilt,vleft,vright);
	// Odometry is quick, so store current value
	ReadAndStoreOdometry();
 
      }
    }

    return (int)status;
}

// }}}


// Some Slow stuff for smooth movements

//   Send yorick demand (same as above, but move slowly)
// {{{ int VW::Yorick::SlowDemand(float new_pan, float new_tilt, float new_vergeleft, float new_vergeright)

int VW::Yorick::SlowDemand(float new_pan, float new_tilt, float new_vergeleft, float new_vergeright)
{
    char status=0;
      float p,t,vl,vr;
    float ratio;
    float *currp,*currt,*currvl,*currvr;
    int time;

    currp = new float(0);
    currt = new float(0);
    currvl = new float(0);
    currvr = new float(0);
    if ((new_pan>90) || (new_pan<-90) || 
	(new_tilt>60) || (new_tilt<-60) || 
	(new_vergeleft>45) || (new_vergeleft<-45) || 
	(new_vergeright>45) || (new_vergeright<-45)) {
      // These are nasty, horrible values, so don't do it...
      perror("Ridiculous values demanded.");
    } else if (pmac.GetMode()==YorickPMAC::PMAC_ON) {
      // work out the biggest motion to determine number of steps
      float MaxMotion = max(float(0.0),float(fabs(vleft-new_vergeleft)));
      MaxMotion = max(MaxMotion,float(fabs(vright-new_vergeright)));
      MaxMotion = max(MaxMotion,float(fabs(tilt-new_tilt)));
      MaxMotion = max(MaxMotion,float(fabs(pan-new_pan)));
      int NumSteps = max(1,static_cast<int>(MaxMotion*4));
    
      //cerr << "MaxMotion=" << MaxMotion << "     Number of steps=" << NumSteps << endl;
      // Recentre sinusoidally
      pmac.GetOdometry(seq_no, &time, currp, currt, currvl, currvr);
      vleft = *currvl;
      vright = *currvr;
      tilt=*currt;
      pan=*currp;
      for (int i=0; i<(NumSteps+1);i++) {
	ratio = 0.5*(1+cos(i*3.1415926/NumSteps));
	vl = new_vergeleft + (vleft - new_vergeleft)*ratio;
	vr = new_vergeright + (vright - new_vergeright)*ratio;
	t = new_tilt + (tilt - new_tilt)*ratio;
	p = new_pan + (pan - new_pan)*ratio;

	// Use the demand function so that the interface (if any)
	// also gets updated
	Demand(p,t,vl,vr);
// 	pmac.Demand(++seq_no, 0,
// 		    YorickPMAC::PMAC_DEMAND, YorickPMAC::PMAC_ALL, 
// 		    p,t,vl,vr);
// 	// Indicate new demand to history lists
// 	StoreDemandHistory();
	usleep(5000); // Wait one 200th of a second
      }
      vleft=0;vright=0;tilt=0;pan=0;


      vleft=new_vergeleft;
      vright=new_vergeright;
      pan=new_pan;
      tilt=new_tilt;
	vl = new_vergeleft;
	vr = new_vergeright;
	t = new_tilt;
	p = new_pan;
	Demand(p,t,vl,vr);
// 	pmac.Demand(++seq_no, 0,
// 		    YorickPMAC::PMAC_DEMAND, YorickPMAC::PMAC_ALL, 
// 		    p,t,vl,vr);
// 	// Indicate new demand to history lists
// 	StoreDemandHistory();
    }

    return (int(status));
}

// }}}

//   Send yorick back to centre in a slow fashion
// {{{ int VW::Yorick::SlowRecentre(void)

int VW::Yorick::SlowRecentre(void)
{
  cerr << "Slow Recentre"<<endl;
//     char status=0;
//     float vl,vr,p,t;
//     float ratio;
//     float p2,t2,vl2,vr2;
//     int time;

//      if (pmac.GetMode()==YorickPMAC::PMAC_ON) {
//       pmac.GetOdometry(seq_no, &time, &p2, &t2, &vl2, &vr2);
//       vleft = vl2;
//       vright = vr2;
//       tilt=t2;
//       pan=p2;
//       // work out the biggest motion to determine number of steps
//       float MaxMotion = max(float(0.0),abs(vleft));
//       MaxMotion = max(MaxMotion,abs(vright));
//       MaxMotion = max(MaxMotion,abs(tilt));
//       MaxMotion = max(MaxMotion,abs(pan));
//       int NumSteps = max(1,static_cast<int>(MaxMotion*2));
      
//       //cerr << "MaxMotion=" << MaxMotion << "     Number of steps=" << NumSteps << endl;

//       // Recentre sinusoidally
//       for (int i=0; i<(NumSteps+1);i++) {
// 	ratio = 0.5*(1+cos(i*3.1415926/NumSteps));
// 	vl = vleft*ratio;
// 	vr = vright*ratio;
// 	t = tilt*ratio;
// 	p = pan*ratio;
// 	//cerr << "Yorick Demand: P="<<e[RIGHT]<<"  T="<<e[LEFT]<<"   V="<<v[LEFT]<<","<<v[RIGHT]<<endl;
// 	pmac.Demand(++seq_no, 0,
// 		    YorickPMAC::PMAC_DEMAND, YorickPMAC::PMAC_ALL, 
// 		    p,t,vl,vr);
// 	// Indicate new demand to history lists
// 	StoreDemandHistory();
// 	usleep(5000); // Wait one 200th of a second
//       }
//       vl = float(0.0); // Zero a few times to make sure
//       vr = float(0.0);
//       t = float(0.0);
//       p = float(0.0);
//       pmac.Demand(++seq_no, 0,
// 		  YorickPMAC::PMAC_DEMAND, YorickPMAC::PMAC_ALL, 
// 		  p,t,vl,vr);
//       // Indicate new demand to history lists
//       StoreDemandHistory();
//       usleep(5000); // Wait one 200th of a second
//       pmac.Demand(++seq_no, 0,
// 		  YorickPMAC::PMAC_DEMAND, YorickPMAC::PMAC_ALL, 
// 		  p,t,vl,vr);
//       // Indicate new demand to history lists
//       StoreDemandHistory();
//       usleep(5000); // Wait one 200th of a second
//       vleft=0;vright=0;tilt=0;pan=0;
//     }

  int status=SlowDemand(0,0,0,0);

    return(status);
}

// }}}




// Some odometry history stuff (a vain attempt to lookup odometry at the time
// of image capture)



//   Get odometry from history lists (in degrees)
// {{{ int VW::Yorick::GetHistoricPosition()

int VW::Yorick::GetHistoricPosition(struct timeval time, 
				   float& p, float& t, 
				   float& vl, float& vr) {

  // These just get discarded
  float delta_p, delta_t, delta_vl, delta_vr;
  int status = GetHistoricPositionAndUncertainty(time,
						 p, t,
						 vl, vr,
						 delta_p, delta_t, 
						 delta_vl, delta_vr);
  return int(status);
}

// }}}
// {{{ int VW::Yorick::GetHistoricUncertainty()

int VW::Yorick::GetHistoricPositionAndUncertainty(struct timeval time, 
						  float& p, float& t, 
						  float& vl, float& vr, 
						  float& delta_p,  
						  float& delta_t, 
						  float& delta_vl, 
						  float& delta_vr)
{
  int tm;
  float vln, vrn;
  float pn,tn;
  char status=0;

  if (pmac.GetMode()==YorickPMAC::PMAC_ON) {


    // If 'time' is more recent than anything
    // on the odometry stacks, get current odometry (it's cheap anyway)
    if (time>time_history[history_pointer]) {
      struct timeval now;
      pmac.GetOdometry(seq_no, &tm, &pn, &tn, &vln, &vrn);
      gettimeofday(&now, NULL);
      // Convert time to ms
      StoreOdometryHistory(now,pn,tn,vln,vrn);
    }


    // Search back through list
    int before_pos=history_pointer;
    int after_pos=history_pointer;
    bool still_searching=true;
    for (int i=0;(i<YORICK_ODOMETRY_HISTORY_LENGTH) && (still_searching);i++) {
      before_pos=history_pointer-i;
      if (before_pos<0) before_pos+=YORICK_ODOMETRY_HISTORY_LENGTH;

      //      cerr << "   odometry ("<<time_history[before_pos]<<"): " << pan_history[before_pos] << "," << tilt_history[before_pos] << "," << vleft_history[before_pos] << "," << vright_history[before_pos] << endl; 
      if (time>time_history[before_pos]) {
	still_searching=false;
      }
    }
    if (still_searching==true) {
      // This means that we're looking for something older than anything stored
      // on the list - let's return the oldest value
      p =  pan_history[before_pos];
      t =  tilt_history[before_pos];
      vl= vleft_history[before_pos];
      vr= vright_history[before_pos];

      // This is wrong, but can't be arsed to fix it
      delta_p = 0;
      delta_t = 0;
      delta_vl = 0;
      delta_vr = 0;
    
    } else {
      if (before_pos==history_pointer) {
	// OK, this is more recent than anything stored - send latest value
	p =  pan_history[history_pointer];
	t =  tilt_history[history_pointer];
	vl= vleft_history[history_pointer];
	vr= vright_history[history_pointer];
       // This is wrong, but can't be arsed to fix it
      delta_p = 0;
      delta_t = 0;
      delta_vl = 0;
      delta_vr = 0;
    
     } else {
	// Found it, so interpolate
	after_pos=before_pos+1;
	if (after_pos>=YORICK_ODOMETRY_HISTORY_LENGTH) after_pos=0;
	float fractime = (time.tv_sec*1000000 + time.tv_usec)
	  - (time_history[before_pos].tv_sec*1000000 
	     + time_history[before_pos].tv_usec);
	float totaltime = (time_history[after_pos].tv_sec*1000000 
	     + time_history[after_pos].tv_usec)
	  - (time_history[before_pos].tv_sec*1000000 
	     + time_history[before_pos].tv_usec);
	float frac = fractime / totaltime;
	p =  frac*pan_history[after_pos] + (1-frac)*pan_history[before_pos];
	t =  frac*tilt_history[after_pos] + (1-frac)*tilt_history[before_pos];
	vl= frac*vleft_history[after_pos] + (1-frac)*vleft_history[before_pos];
	vr= frac*vright_history[after_pos] + (1-frac)*vright_history[before_pos];
	// Uncertainty is +/- 2 * (v2-v1) * [ frac*(1-frac) ]
	float delta = 2*frac*(1-frac);
	delta_p  = delta * (pan_history[after_pos] - pan_history[before_pos]);
 	delta_t  = delta * (tilt_history[after_pos] - tilt_history[before_pos]);
 	delta_vl = delta * (vleft_history[after_pos] - vleft_history[before_pos]);
 	delta_vr = delta * (vright_history[after_pos] - vright_history[before_pos]);
	//cerr << "frac="<<frac<<"   v1="<<vleft_history[before_pos]<<"   v2="<<vleft_history[after_pos]<<"  delta-v="<<delta_vl<<endl;

      }
    }
  }
  return int(status);
}


// }}}

// {{{ void VW::Yorick::ShowOdometryHistory()

void VW::Yorick::ShowOdometryHistory(void) {
  for (int i=0;i<YORICK_ODOMETRY_HISTORY_LENGTH;i++) {
    int j=history_pointer-i;
    if (j<0) j+=YORICK_ODOMETRY_HISTORY_LENGTH;
    if (time_history[j].tv_sec>0) { // only display the meaningful bits
    cerr << "Item " << j
	 << "   time: " << time_history[j]
	 << "   odometry: " << pan_history[j] << "," << tilt_history[j] << "," << vleft_history[j] << "," << vright_history[j] << endl;
    }
  }
}

// }}}

// {{{ void VW::Yorick::GetOdometryHistory()

void VW::Yorick::GetOdometryHistory(vector<timeval>& time,
				    vector<double>& pan,
				    vector<double>& tilt,
				    vector<double>& vergeleft,
				    vector<double>& vergeright)
{
  for (int i=0;i<YORICK_ODOMETRY_HISTORY_LENGTH;i++) {
    int j=history_pointer+i+1;
    if (j>=YORICK_ODOMETRY_HISTORY_LENGTH) j-=YORICK_ODOMETRY_HISTORY_LENGTH;
    if (time_history[j].tv_sec>0) { // only display the meaningful bits
      time.push_back(time_history[j]);
      pan.push_back(double(pan_history[j]));
      tilt.push_back(double(tilt_history[j]));
      vergeleft.push_back(double(vleft_history[j]));
      vergeright.push_back(double(vright_history[j]));
    }
  }
}

// }}}

// {{{ void VW::Yorick::FlushHistory(void)

void VW::Yorick::FlushHistory(void) {
  int time;
  float vln,vrn;
  float pn,tn;
  struct timeval init_time;
  init_time.tv_sec=0;init_time.tv_usec=0;
  
  if (pmac.GetMode()==YorickPMAC::PMAC_ON) {
    pmac.GetOdometry(seq_no, &time, &pn, &tn, &vln, &vrn);
    for (int i=0;i<YORICK_ODOMETRY_HISTORY_LENGTH;i++) {
      pan_history[i]=pn;
      tilt_history[i]=tn;
      vleft_history[i]=vln;
      vright_history[i]=vrn;
      time_history[i]=init_time;
    }
    history_pointer=0;
  }

}

// }}}




