/** 
 * @file   glob.h
 * @author ID Reid
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


#include "defs.h"


#define TWO_PI 6.283185307
#define PI 3.141592654
#define PI_F 3.141592654f
#define CO 3.141592654f/180.00f
#define TPI 6.283185307f

/*************************** DEFINE TO DO WITH TIMINGS ********************** */
/* define what one second is */
#define CLOCKS_PER_SEC 15625

/* Define how often to output to ekf */
#define OUTPUT_TO_EKF_SECS 1.0f
#define OUTPUT_TO_EKF_TICKS (OUTPUT_TO_EKF_SECS * CLOCKS_PER_SEC)

/* Define the lkf loop - reading from the encoders */
#define LKF_TIME_INT_SECS 0.005f
#define LKF_TIME_INT_TICKS (LKF_TIME_INT_SECS * CLOCKS_PER_SEC)

/* Define how often the i/o buffer control sends observations to the other nodes */
#define Z_12_DELAY_SECS 0.01f
#define Z_12_DELAY_TICKS (Z_12_DELAY_SECS * CLOCKS_PER_SEC)

/* Define how often the observation collator collects observations from all the in buffers */
#define Z_COLLATE_DELAY_SECS 0.01f
#define Z_COLLATE_DELAY_TICKS (Z_COLLATE_DELAY_SECS * CLOCKS_PER_SEC)

/* Define the Control Loop Time */
#define DEMAND_TIME_DELAY_SECS 0.1f
#define DEMAND_TIME_DELAY_TICKS (DEMAND_TIME_DELAY_SECS * CLOCKS_PER_SEC)

/* Define the Tracker Loop Time */
#define TRACK_SAM_DELAY_SECS 0.05f
#define TRACK_SAM_DELAY_TICKS (TRACK_SAM_DELAY_SECS * CLOCKS_PER_SEC)

/* define how long the tracker should let the vehicle slow down for before ending */
#define TRACK_END_DELAY_SECS 2.0f
#define TRACK_END_DELAY_TICKS (TRACK_END_DELAY_SECS * CLOCKS_PER_SEC);

/* ************************************ DEFINES TO DO WITH ARRAY SIZE */

/* this is the structure for the time velocity generated array of body */
#define MAX_SIZE 400

#define OC 180.00/3.141592654

#define STIFFNESS 0.5

#define NUM_SPANS 40

#define link_chan1(A, C) {A.C = C;}
#define link_chan2(A, B, C) {A.C = B.C = C;}

/* set the weighted gains */
#define STEER_W1 0.833f
#define STEER_W2 0.485f
#define DRIVE_W1 0.833f
#define DRIVE_W2 0.485f


/* define two pi to convert from rads per sec to rev per sec */
#define TWOPI 6.283185308
#define F_TWOPI 6.283185308f

#define PI 3.141592654
#define F_PI 3.141592654f

/* Define certain constants */
#define DRIVE_RATIO 25
#define STEER_RATIO 40
#define F_DRIVE_RATIO 25.0f
#define F_STEER_RATIO 40.0f

#define STEER_MOTOR_DEV_NO 1
#define DRIVE_MOTOR_DEV_NO 0

#define F_QUADS_PER_REV 2000.0f
#define QUADS_PER_REV 2000

#define drive_encoder_start_pos 0
#define steer_encoder_start_pos 0

#define VALUE_OF_REG_ROFH 255
#define VALUE_OF_REG_ROFHF 255.0f

#define K_S_GAIN 64
#define A_S_GAIN 229
#define B_S_GAIN 64
#define K_D_GAIN 64
#define A_D_GAIN 229
#define B_D_GAIN 64

#define R 0.08



#define MAX_NAME	10

/* *********************************** */
/* globals for the NLLSE */
#define R_VEL_NOISE 0.0015
#define R_GAM_NOISE 0.0003

#define XDIM 3
#define ZDIM 6

#define NUM_WHEELS 2 /* this is starting count at 0 1 2 etc */

#define WHEEL_RADIUS 0.08
