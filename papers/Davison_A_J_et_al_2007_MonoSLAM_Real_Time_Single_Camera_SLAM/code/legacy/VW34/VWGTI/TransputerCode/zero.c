/** 
 * @file   zero.c
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


#include "newdef.h"
#include <stdio.h>
#include <process.h>
#include <misc.h>

#include "glob.h"
#include "str.h"
#include "utils.h"

#include "channel.h"
#include "lowlev2.h"
#include "zero.h"
#include "flag.h"


 /* #define ZEROOFF */

static const SEARCHSTEP = 3;
static const OPTO_DEV_NO = 2;
static const MIN_POS = (int)(F_QUADS_PER_REV * F_STEER_RATIO * -180.0f/ 360.0f);
static const MAX_POS = (int)(F_QUADS_PER_REV * F_STEER_RATIO * 180.0f/ 360.0f);

int node_id = 0;

BOOL covered (BYTE *fl)
/* Return true if steer opto is covered */
{
	return !(input(OPTO_DEV_NO,fl) & 1);
}

BOOL zeroSteer(float correction_angle)
/* Zero steering motor by searching for center position of opto.
   correction_angle (degrees) is the difference between the true zero
   and the opto's zero.
  Return TRUE if successful */
{
	BOOL found;
	BOOL state;
	BOOL cwFound = FALSE, ccwFound = FALSE;
	INT32 pos, center, cwEdge, ccwEdge;
        BYTE fl;

	/* Assume center to be true zero correction */
	center = (int)(F_QUADS_PER_REV * F_STEER_RATIO * correction_angle/ 360.0f);
 
	/* Initialise */
	fl = setPos (STEER_MOTOR_DEV_NO, 0);
	if (fl == OK)
          fl = posMode (STEER_MOTOR_DEV_NO);

	if (fl == OK)
          found = covered(&fl);

		/* Search cw for edges */
        if (fl == OK)
		for (pos=0; pos < MAX_POS; pos += SEARCHSTEP) {
			if ((fl = comPos (STEER_MOTOR_DEV_NO, pos)) != OK)
                          break;
			ProcWait(8); /* wait half a millisecond */
			state = covered(&fl);
                        if (fl != OK)
			  break;
			else if (found) { /* already found */
				if (!state) {	/* far edge */
					cwEdge = actPos(STEER_MOTOR_DEV_NO,&fl);
					cwFound = TRUE;
					break;
				}
			}
			else {	/* Not yet found */
				if (state) {	/* Near edge */
					ccwEdge = actPos(STEER_MOTOR_DEV_NO,&fl);
					if (fl != OK)
					  break;
                                        else
					  ccwFound = TRUE;
				}
				found = state;
			}
		}

		found = state;

		if ((fl == OK)&&(!ccwFound || !cwFound)) {
			/* Search for ccw for edges */
			for (;pos > MIN_POS; pos -= SEARCHSTEP) {
				if ((fl = comPos (STEER_MOTOR_DEV_NO, pos)) != OK)
                                  break;
				ProcWait(8);  /* wait half a millisecond */
				state = covered(&fl);
				if (fl != OK)
				  break;
				else if (found) { /* already found */
					if (!state) {	/* far edge */
						ccwEdge = actPos(STEER_MOTOR_DEV_NO,&fl);
						ccwFound = TRUE;
						break;
					}
				}
				else {	/* Not yet found */
					if (state) {
						cwEdge = actPos(STEER_MOTOR_DEV_NO,&fl);
						if (fl != OK)
						  break;
						else
						  cwFound = TRUE;
					}
				found = state;
				}
			}
		}

				/* Center is correction plus true zero */
		if ((fl == OK)&&(cwFound && ccwFound))
			center = center + (cwEdge + ccwEdge) / 2;

				/* Go to center position */
		while ((fl == OK)&&(pos < center-SEARCHSTEP || pos > center+SEARCHSTEP)) {
			fl = comPos (STEER_MOTOR_DEV_NO, pos);
			if (pos > center)
				pos -= SEARCHSTEP;
			else
				pos += SEARCHSTEP;
		}
				/* Set stopped position to 0, including
				remaining error */
	if (fl == OK)
	  fl = idle (STEER_MOTOR_DEV_NO);

        ProcWait(31); /* wait 2 millisecond */

	if (fl == OK)
          pos = actPos(STEER_MOTOR_DEV_NO,&fl);
        if (fl == OK)
	  fl = setPos (STEER_MOTOR_DEV_NO, steer_encoder_start_pos +
				pos - center);

	if (cwFound && ccwFound)
	  return fl;
        else
          return FAIL;
}




BYTE check_zero(Channel *edge_out, Channel *edge_in, float correction_angle)
{
/* setup the device for the steer motor */
	int d0;
	int d1;
        BYTE fl;
	int err=0;

/* set the steer and drive motor device number */
	d0 = STEER_MOTOR_DEV_NO;
	d1 = DRIVE_MOTOR_DEV_NO;


/*
** Setup hctl1100 before we use it	
** Setup the lowlevel functions for the HCTL1100
** this has to happen here due to the semaphore action
*/

/* send the data needed to operate lowlevel functions correctly */
	lowlevel_init(edge_out, edge_in);

/* initialise the two motors */
	if ((fl = init(STEER_MOTOR_DEV_NO)) != OK) err=1;
		else if ((fl = init(DRIVE_MOTOR_DEV_NO)) != OK) err=2;

/*
** set the sample time register to give the correct read rate
** a 99 give a sample time of 800 microseconds - basically 32 steps
** a 199 give a sample time of 0.0016 seconds - basically steps
** a 255 give a sample time of 0.002048 seconds - basically 100 steps
*/

	else if ((fl = sampleTime(STEER_MOTOR_DEV_NO, reg_rofh)) != OK) err=3;
	else if ((fl = sampleTime(DRIVE_MOTOR_DEV_NO, reg_rofh)) != OK) err=4;

	/* set the start postition on the encoder to */
	else if ((fl = setPos(DRIVE_MOTOR_DEV_NO, drive_encoder_start_pos)) != OK) err=5;

#ifndef ZEROOFF
	else if ((fl = zeroSteer(correction_angle)) != OK) err=6; 
#endif

	return fl;
}

