/** 
 * @file   gti_main.cc
 * @author ID Reid, BJ Tordoff
 * @date   09/09/2002
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

/******** System Includes ********/
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
/*********************************/


/******** Transputer specific stuff ********/
extern "C" {
#include "VW/gtitransputer.h"
}
/*******************************************/


/******** GTI specific stuff ********/
#include "gti_main.h"
#include "gti_proto.h" // comms protocol and errors
/************************************/

#define BOOTABLE "gti.btl"

using namespace std;
using namespace VW;

const char *GTI::status_msg[] = {"OK",
				 "FAIL",
				 "ANYTHING",
				 "STOP",
				 "COLLISION",
				 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				 "ERROR_ZERO",
				 "ERROR_STEER_TIME",
				 "ERROR_DRIVE_TIME",
				 "ERROR_STEER_RESET",
				 "ERROR_DRIVE_RESET",
				 "ERROR_STEER_POSCTRL",
				 "ERROR_DRIVE_POSCTRL",
				 "ERROR_STEER_ZERO",
				 "ERROR_DRIVE_ZERO" };
  
// {{{ int GTI::InitLink(string boot_file)

int GTI::InitLink(string boot_file)
{
    int i, status, len;
    char s[256];

    /* boot tranputer */
    if ((TheLink=iserver_open(const_cast<char *>(boot_file.c_str ()))) < 0) {
	fprintf(stderr, "Error booting transputers\n");
	return -1;
    }

    /* transputer preamble */
    for (i=0; i<3; i++) {
	if (SingleServe(&status) < 0) {
	    fprintf(stderr, "Transputer error\n");
	    return -1;
	}
    }

    /* Handshake with transputer code */
    ChanOutInt(TheLink, 0);
    len = ChanInMsg(TheLink, s);
    fprintf(stderr, "GONAD: %s\n", s);

    // fprintf(stderr, "Test char = %x\n", ChanInChar(TheLink));
    fprintf(stderr, "Press <RETURN> when steering centred: "); getchar();

    status = ChanInChar(TheLink);
    fprintf(stderr, "Status=%x [%s]\n", status, status_msg[status]);
    len = ChanInMsg(TheLink, s);
    fprintf(stderr, "GONAD (%d): %s\n", len, s);

    if (status != OK) {
	iserver_close();
	return -1;
    }

    return TheLink;
}

// }}}

/*************
*   int SendChar(char c)
*
*   write a given char to transputer
**************/
// {{{ int GTI::SendChar(char c)

int GTI::SendChar(char c)
{
    if (TheLink < 0) {
	fprintf(stderr, "link not open\n");
	return -1;
    }

    ChanOutChar(TheLink, c);
    return 0;
}

// }}}


/******************
*
* void rotate_vehicle(float angle)
*
* Rotate vehicle through angle (degrees) in position mode
*
*                                                    -90
*                                                     ^
*                  _____                              |
*                 ||o-o||   <--- Yorick        180 <--|--> 0
*  Gonad --->     |     |                             |
*                 |__|__|   <--- Steering end         v
*                                                     90 
*******************/                   
// {{{ int GTI::Rotate(float angle)

int GTI::Rotate(float angle)
{
    char status=0;

    if (TheLink < 0) {
	fprintf(stderr, "Link not open.\n");
	return -1;
    }

    ChanOutChar(TheLink, VROT_CHAR);
    ChanOut(TheLink, (char *)(&angle), sizeof(float));
    status = ChanInChar(TheLink);

    return (int)status;
}

// }}}


/******************
*
* void drive_vehicle(float dist)
*                          
* Drive vehicle dist (metres) in position mode
*
*                    ^ -ve
*                    |
*                  _____ 
*                 ||o-o||
*  Gonad --->     |     |
*                 |__|__|   <--- Steering wheel
*
*                    |
*                    v +ve
*
********************/                   
// {{{ int GTI::Drive(float dist)

int GTI::Drive(float dist)
{
    char status=0;

    if (TheLink < 0) {
	fprintf(stderr, "Link not open.\n");
	return -1;
    }

    ChanOutChar(TheLink, VDRV_CHAR);
    ChanOut(TheLink, (char *)(&dist), sizeof(float));
    status = ChanInChar(TheLink);
    return (int)status;
}

// }}}


/******************
*
* void set_vehicle_velocity(float velocity)
*
* Set the vehicle velocity in velocity mode
*
*******************/
// {{{ int GTI::SetVelocity(float velocity)

int GTI::SetVelocity(float velocity)
{
  int status = OK, len;
  char replybuf[256];

  if (TheLink >= 0) {
    ChanOutChar(TheLink, VABS_CHAR);
    ChanOut(TheLink, (char *) &velocity, sizeof(float));
    len = ChanInMsg(TheLink, replybuf);
    if ((status=replybuf[0]) != OK) {
      fprintf(stderr, "Error: Status=%d\n", status);
    }
  }
  else {
    fprintf(stderr, "Link not open.\n");
    return -1;
  }
  return (int) status;
}

// }}}


/******************
*
* void set_vehicle_steering_angle(float angle)
*
* Set the vehicle steering angle in velocity mode
*
*******************/
// {{{ int GTI::SetSteeringAngle(float angle)

int GTI::SetSteeringAngle(float angle)
{
  int status = OK, len;
  char replybuf[256];

  if (TheLink >= 0) {
    ChanOutChar(TheLink, SABS_CHAR);
    ChanOut(TheLink, (char *) &angle, sizeof(float));
    len = ChanInMsg(TheLink, replybuf);
    if ((status=replybuf[0]) != OK) {
      fprintf(stderr, "Error: Status=%d\n", status);
    }
  }
  else {
    fprintf(stderr, "Link not open.\n");
    return -1;
  }
  return (int) status;
}

// }}}


/******************
*
* int get_vehicle_drive_odometry(int *count)
*
* Get the current odometry count from the vehicle's drive wheel
*
*******************/
// {{{ int GTI::GetDriveOdometry(int *count)

int GTI::GetDriveOdometry(int *count)
{
  int status = OK, len;
  char replybuf[256];

  if (TheLink >= 0) {
    ChanOutChar(TheLink, GETD_CHAR);
    len = ChanInMsg(TheLink, replybuf);
    if ((status=replybuf[0]) != OK) {
      fprintf(stderr, "Error: Status=%d\n", status);
    }
    else if (len > 1) {
      *count = * (int *)&replybuf[1];
    }
  }
  else {
    fprintf(stderr, "Link not open.\n");
    return -1;
  }
  return (int) status;
}

// }}}

