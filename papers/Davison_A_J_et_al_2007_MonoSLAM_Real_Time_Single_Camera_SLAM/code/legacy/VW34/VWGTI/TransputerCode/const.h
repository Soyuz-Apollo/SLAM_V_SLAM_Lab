/** 
 * @file   const.h
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
 *
 *  This file contains the values of the constants to be used
 *  by the controller board and the simulations: it includes
 *  such things as:
 *    - values for controller board parameters.
 *    - dimensions of vehicle. 
 */

#define PI			3.141592654

#define POSITION_CONTROL        0
#define VELOCITY_CONTROL        1

#define STEER_TIME_UNIT		255
#define DRIVE_TIME_UNIT		255

#define STEER_DEVICE            1
#define DRIVE_DEVICE            0
#define OPTO_DEVICE             2

#define CMASK                   0x80
#define COL_WAIT_TIME           16  /* 1 millisecond */

#define TURN_LIMIT              2

#define HCTL_FREQ		2000000  /* frequency of 2MHz */

#define DRIVE_PERIOD 		(((double)(16*(DRIVE_TIME_UNIT+1)))/HCTL_FREQ)

#define STEER_A		229
#define STEER_B		64
#define STEER_K		64
#define DRIVE_A		229
#define DRIVE_B		64
#define DRIVE_K		64

#define AccFactor		256
#define ACCELERATION            ((INT16)(4*AccFactor))  /* 
   the integral velocity mode limiting acceleration (in counts/(cycle_time)^2) 
                                                          */

#define WHEEL_RADIUS		0.079  /* only approximate */
#define ROT_RADIUS              0.380  /* ditto */

#define ANGLE_ERROR		0.0f   /* the difference between the opto zero and the real straight ahead */

#define ONESEC			15625 /* clock ticks */

#define SAMPLE_INTERVAL		39 /* (int)(ONESEC/400) i.e. 2.5 millisec */

#define ENCODER_COUNTSperCYCLE	2000

#define STEER_GEAR_RATIO	40
#define DRIVE_GEAR_RATIO	25

#define STEER_COUNTperREV	(STEER_GEAR_RATIO*ENCODER_COUNTSperCYCLE)
#define DRIVE_COUNTperREV	(DRIVE_GEAR_RATIO*ENCODER_COUNTSperCYCLE)

#define RADtoPOS                (((double)STEER_COUNTperREV)/(2.*PI))
#define DISTtoPOS		(((double)DRIVE_COUNTperREV)/(2.*WHEEL_RADIUS*PI))
