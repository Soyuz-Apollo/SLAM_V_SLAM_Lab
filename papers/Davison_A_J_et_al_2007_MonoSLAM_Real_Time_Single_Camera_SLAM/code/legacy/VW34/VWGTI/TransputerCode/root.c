/** 
 * @file   root.c
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



#include <process.h>
#include <channel.h>
#include <mathf.h>
#include <misc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iocntrl.h>

#include "channel.h"
#include "newdef.h"
#include "lowlev2.h"
#include "const.h"
#include "flag.h"

#define VERSION "110599gamma"

#define STEER_STEP      ((INT32)(STEER_COUNTperREV/72))
#define DRIVE_STEP      ((INT32)(DRIVE_COUNTperREV/4))
#define SPEED_STEP      4
#define MAX_SPEED       40

#define hostprint(S) ChanOutMsg(to_host, (S), strlen(S));

int main(int argc, char *argv[])
{
    int j = 1;
    Channel *from_host = get_param(j++),
            *to_host = get_param(j++),
            *from_hctl = get_param(j++),
            *to_hctl = get_param(j++);
    char cmd, replybuf[256], msg[256];
    BYTE status;
    int replylen, itemp;
    int steer_pos, drive_pos, stemp;
    int drive_speed;
    int drive_mode;
    float ftemp;

    set_abort_action(ABORT_HALT);

    /* Wait for synchronization signal from host */
    j = ChanInInt(from_host);
    sprintf(msg, "communications ok [Version: %s]", VERSION);
    hostprint(msg);

    /* test char */
    /* ChanOutChar(to_host, OK); */

    if ((status = check_zero(to_hctl,from_hctl,ANGLE_ERROR)) == FAIL) {
	ChanOutChar(to_host, ERROR_ZERO);
	sprintf(msg, "Unable to initialise devices and check for zero (status=%x)!\n", status);
	hostprint(msg);
	} else if ((status = sampleTime(STEER_DEVICE,STEER_TIME_UNIT)) == FAIL) {
	ChanOutChar(to_host, ERROR_STEER_TIME);
	hostprint("Unable to set time unit on steer device!\n");
    } else if ((status = sampleTime(DRIVE_DEVICE,DRIVE_TIME_UNIT)) == FAIL) {
	ChanOutChar(to_host, ERROR_DRIVE_TIME);
	hostprint("Unable to set time unit on drive device!\n");
    } else if ((status = setPos(STEER_DEVICE, 0)) == FAIL) {
	ChanOutChar(to_host, ERROR_STEER_RESET);
	hostprint("Unable to reset position on steer device.\n");
    } else if ((status = setPos(DRIVE_DEVICE, 0)) == FAIL) {
	ChanOutChar(to_host, ERROR_DRIVE_RESET);
	hostprint("Unable to reset position on drive device.\n");
    } else if ((status = posMode(STEER_DEVICE)) == FAIL) {
	ChanOutChar(to_host, ERROR_STEER_POSCTRL);
	hostprint("Unable to put steer device in position control mode!\n");
    } else if ((status = posMode(DRIVE_DEVICE)) == FAIL) {
	ChanOutChar(to_host, ERROR_DRIVE_POSCTRL);
	hostprint("Unable to put drive device in position control mode!\n");
    } else if ((status = comPos(STEER_DEVICE,0)) == FAIL) {
	ChanOutChar(to_host, ERROR_STEER_ZERO);
	hostprint("Unable to set desired steer position to 0.\n");
    } else if ((status = comPos(DRIVE_DEVICE,0)) == FAIL) {
	ChanOutChar(to_host, ERROR_DRIVE_ZERO);
	hostprint("Unable to set desired drive position to 0.\n");
    } else {
	ChanOutChar(to_host, OK);
	sprintf(msg, "All devices initialised! (status=%x)\n", status);
	hostprint(msg);
    }

    status = OK;

    /* initialise state variables */

    drive_mode = POSITION_CONTROL;
    drive_pos = 0;
    steer_pos = 0;
    drive_speed = 0;

    while (status == OK) {
    	cmd = ChanInChar(from_host);
        replylen = 1;
    	switch (cmd) {
        case 'l':
	    if (steer_pos < STEER_COUNTperREV)
		steer_pos += STEER_STEP;
	    status = comPos(STEER_DEVICE,steer_pos);
	    replylen = 1;
	    break;
        case 'r':
	    if (steer_pos > (-1*STEER_COUNTperREV))
		steer_pos -= STEER_STEP;
	    status = comPos(STEER_DEVICE,steer_pos);
	    replylen = 1;
	    break;
	case 'S':
	    /* absolute steer */
	    ChanIn(from_host, &ftemp, sizeof(float));
	    itemp = (int)(RADtoPOS * PI*ftemp/180.0 + .5);
	    if (itemp < -STEER_COUNTperREV || itemp > STEER_COUNTperREV) {
	        break;
	    }
	    steer_pos = itemp;
	    status = comPos(STEER_DEVICE,steer_pos);
	    replylen = 1;
	    break;
	case 's':
	    /* relative steer */
	    ChanIn(from_host, &ftemp, sizeof(float));
	    itemp = (int)(RADtoPOS * PI*ftemp/180.0 + .5);
	    if (itemp+steer_pos < -STEER_COUNTperREV || 
	    	itemp+steer_pos > STEER_COUNTperREV) {
	        break;
	    }
	    steer_pos += itemp;
	    status = comPos(STEER_DEVICE,steer_pos);
	    replylen = 1;
	    break;
	case 'D':
	    ChanIn(from_host, &ftemp, sizeof(float));
	    if (ftemp > 1 || ftemp < -1) {
	    	break;
	    }
	    itemp = (int)(DISTtoPOS*ftemp + .5);
	    if (drive_mode == VELOCITY_CONTROL) {
		drive_mode = POSITION_CONTROL;
		drive_pos = actPos(DRIVE_DEVICE, &status);
		if (status != FAIL)  {
	            if ((status = posMode(DRIVE_DEVICE)) != FAIL) {
		        drive_pos += itemp;
		        status = comPos(DRIVE_DEVICE,drive_pos);
		    }
		}
            } else {
		drive_pos += itemp;
		status = comPos(DRIVE_DEVICE,drive_pos);
            }
	    replylen = 1;
	    break;
	case 'R':
	    replylen = 1;
	    ChanIn(from_host, &ftemp, sizeof(float));
	    itemp = (int)(DISTtoPOS * ROT_RADIUS * PI*ftemp/180.0 + .5);
	    stemp = (int)(RADtoPOS * PI/2.0 + .5);
	    if ((status = comPos(STEER_DEVICE,stemp)) == FAIL) {
	        break;
	    }
	    ProcWait(2*ONESEC);
	    
            if (drive_mode == VELOCITY_CONTROL) {
	        drive_mode = POSITION_CONTROL;
		drive_pos = actPos(DRIVE_DEVICE,&status);
		if (status == FAIL) {
		    break;
	        } else if ((status = posMode(DRIVE_DEVICE)) == FAIL) {
		    break;
	        }
            }
            drive_pos += itemp;
	    if ((status = comPos(DRIVE_DEVICE,drive_pos)) == FAIL) {
	    	break;
	    }
	    ProcWait(5*ONESEC);
	    
	    if ((status = comPos(STEER_DEVICE,steer_pos)) == FAIL) {
	    	break;
	    }
	    break;

        case 'u':
	    if (drive_mode == POSITION_CONTROL) {
		drive_mode = VELOCITY_CONTROL;
		drive_speed = -1*SPEED_STEP;
		if ((status = intgMode(DRIVE_DEVICE)) != FAIL)
		    if ((status = accel(DRIVE_DEVICE,ACCELERATION)) == FAIL)
		        status = comdVel(DRIVE_DEVICE,((BYTE)drive_speed));
            } else if (drive_speed > (-1*MAX_SPEED)) {
		drive_speed -= SPEED_STEP;
		status = comdVel(DRIVE_DEVICE,((BYTE)drive_speed));
            }
	    replylen = 1;
	    break;

        case 'd':
	    if (drive_mode == POSITION_CONTROL) {
		drive_mode = VELOCITY_CONTROL;
		drive_speed = SPEED_STEP;
		if ((status = intgMode(DRIVE_DEVICE)) != FAIL)
		    if ((status = accel(DRIVE_DEVICE,ACCELERATION)) != FAIL)
		        status = comdVel(DRIVE_DEVICE,((BYTE)drive_speed));
	    } else if (drive_speed < MAX_SPEED) {
		drive_speed += SPEED_STEP;
		status = comdVel(DRIVE_DEVICE,((BYTE)drive_speed));
	    }
	    replylen = 1;
	    break;

        case 'v':
	    ChanIn(from_host, &ftemp, sizeof(float));
	    itemp = (int)ftemp;
	    if (itemp > MAX_SPEED || itemp<-MAX_SPEED)
	    	break;

	    if (drive_mode == POSITION_CONTROL) {
		drive_mode = VELOCITY_CONTROL;
		if ((status = intgMode(DRIVE_DEVICE)) != FAIL)
		    status = accel(DRIVE_DEVICE,ACCELERATION);
		{
		    drive_speed = itemp;
		    status = comdVel(DRIVE_DEVICE,((BYTE)drive_speed));
		}
	    } else {
	    	drive_speed = itemp;
		status = comdVel(DRIVE_DEVICE,((BYTE)drive_speed));
	    }
	    replylen = 1;
	    break;
	    
        case 'f':
	    if (drive_mode == VELOCITY_CONTROL) {
		drive_mode = POSITION_CONTROL;
		drive_pos = actPos(DRIVE_DEVICE,&status);
		if (status != FAIL)
		    if ((status = posMode(DRIVE_DEVICE)) != FAIL)
		        status = comPos(DRIVE_DEVICE,drive_pos);
            } else {
		drive_pos-= DRIVE_STEP;
		status = comPos(DRIVE_DEVICE,drive_pos);
            }
	    replylen = 1;
	    break;
        case 'b':
	    if (drive_mode == VELOCITY_CONTROL) {
		drive_mode = POSITION_CONTROL;
		drive_pos = actPos(DRIVE_DEVICE,&status);
		if (status != FAIL)
		    if ((status = posMode(DRIVE_DEVICE)) != FAIL)
		        status = comPos(DRIVE_DEVICE,drive_pos);
            } else {
		drive_pos+= DRIVE_STEP;
		status = comPos(DRIVE_DEVICE,drive_pos);
            }
	    replylen = 1;
	    break;
        case '0':
	    itemp = actPos(STEER_DEVICE,&status);
	    memcpy(replybuf+1, &itemp, sizeof(int));
	    replylen = 1 + sizeof(int);
	    break;
        case '1':
	    itemp = actPos(DRIVE_DEVICE,&status);
	    memcpy(replybuf+1, &itemp, sizeof(int));
	    replylen = 1 + sizeof(int);
	    break;
        case 'o':
	    itemp = PowerOutput(STEER_DEVICE, &status);
	    memcpy(replybuf+1, &itemp, sizeof(int));
	    replylen = 1 + sizeof(int);
	    break;
        case 'p':
	    itemp = PowerOutput(DRIVE_DEVICE, &status);
	    memcpy(replybuf+1, &itemp, sizeof(int));
	    replylen = 1 + sizeof(int);
	    break;
	case 'Q':
	    status = STOP;
    	    replylen = 1;
	    break;
        default:
	    /* do absolutely nothing at all */
	    break;
        }  

	replybuf[0] = status;
	ChanOutMsg(to_host, replybuf, replylen);

    }

    /* In whichever case, an attempt is made to idle both steer and drive */
    status = idle(STEER_DEVICE);
    status = idle(DRIVE_DEVICE);

    exit_terminate(EXIT_SUCCESS);
}
 
