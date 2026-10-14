/** 
 * @file   yorickpmac.h
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


#ifndef _yorick_pmac_class_h_
#define _yorick_pmac_class_h_

class YorickPMAC {
 public:


  YorickPMAC(void);
  ~YorickPMAC();
  
  // Some enums

  // Mode setting
  enum PMACMode {
    PMAC_UNINITIALIZED=-2,
    PMAC_OFF,
    PMAC_PENDING,  
    PMAC_ON
  };
  // Whether to act on the demands
  enum PMACAction {
    PMAC_NODEMAND =0,
    PMAC_DEMAND   =1,
    PMAC_HOMEZ    =2
  };
  // Which axes to action
  enum PMACValidFlags {
    PMAC_PAN        = 1,
    PMAC_TILT       = 2,
    PMAC_VERGELEFT  = 4,
    PMAC_VERGERIGHT = 8,
    PMAC_ALL        =15
  };


  void Close(void);

  bool Init(void);
  int Download(const int pmac, char *filename, int pflag);
  
  void SetMode(const PMACMode mode);
  PMACMode GetMode(void);
  void SetModePending(void);
  void SetModeOn(void);
  void SetModeOff(void);
  int  GetState(void);
  
  void GetOdometry(int seq_no, int *time, 
			       float *pan, float *tilt, float *vergeleft, float* vergeright);
  void GetVelocity(int seq_no, int *time, float *vpan, float *vtilt, float *vvergeleft, float *vvergeright);

  int ZeroOdometry (void);
  void Demand (int seq_no, int time, int action, int valid_flags, 
	       float pan, float elev, float vergeleft, float vergeright);
 private:
  // Function to load the M-value interpretations
  void SetAddresses(void);

  char *DPRAMbase;
  int pmac_fd; // file descriptor for communications

  // Some consts (initialized in the .cc file)
  static const char *const PMAC_DEVICE_NAME;
  static const char *const PMAC_PROG_NAME;
  static const char *const PMAC_RUN_NAME;
  /* PMAC M-variable interpretations */
  int*   MODE_FLAG;
  int*   PROGRAM_STATE;
  int*   VISION_TIME;
  int*   VISION_FRAME;
  float* VISION_DATA;
  float* VISION_PAN;
  float* VISION_ELEV_R;
  float* VISION_ELEV_L;
  float* VISION_VERGE_L;
  float* VISION_VERGE_R;
  int*   VISION_ACTION;
  int*   VISION_VALID_FLAGS;
  volatile int*   ODOM_FRAME;
  volatile float* ODOM_DATA;
  volatile int*   ODOM_VALID;
  volatile int*   ODOM_TIME;
  volatile float* ODOM_PAN;
  volatile float* ODOM_ELEV;
  volatile float* ODOM_VERGE_L;
  volatile float* ODOM_VERGE_R;
  volatile float* ODOM_PAN_VEL;
  volatile float* ODOM_ELEV_VEL;
  volatile float* ODOM_VERGE_L_VEL;
  volatile float* ODOM_VERGE_R_VEL;
  
};

///////////////////////////////////////////////////////////

#endif
