/** 
 * @file   lowlev2.c
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
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <process.h>
#include <channel.h>
#include <semaphor.h>
#include "lowlev2.h"
#include "flag.h"

#define USEDIFF 0

#if USEDIFF
#include <xchannel.h>
#endif

PRIVATE Channel *edgeout, *edgein;
/*
 * Clock tick rates in microsec.
 */
#define LOWTICK 64
#define HIGHTICK 1
/*
 *  INTERFACE CONTROL CODES
 */
#define ADDR 0xC0
#define WRITE 0xE0
#define READ 0xA0
#define OUTPUT 0x80
#define DUMMY 0x00
#define SYNC 0x20          /* add SYNC and RESET */
#define RESET 0x40
#define TESTPAT 0x5A
extern int node_id;
/*
**       -------------------------------------
**       -- HCTL-1100 high level operations --
**       -------- LowLevel Commands ----------
**       -------------------------------------
*/
/*
** Setup a Semaphore to protect shared coms to hctl1100
** As there is only one link
*/
PRIVATE Semaphore *sem;

PUBLIC void lowlevel_init(Channel *out, Channel *in)
/* Initialise lowlevel, call once only before any use is made
   of hctl1100 coms. */
{
  edgeout = out;
  edgein = in;
  sem = SemAlloc (1);
}
/*
 *  Link communications with fixed timeout
 */
#if USEDIFF
PRIVATE void timePling (Channel* out, BYTE buf[], size_t no)
{
  ChanVOut(out, buf, no);
}
PRIVATE void timePlingB (Channel* out, BYTE b)
{
  ChanVOut(out, &b, sizeof(b));
}
PRIVATE BYTE timeQueryB (Channel* in)
{
  BYTE b;
  ChanVIn (in, &b, sizeof(b));
  return b;
}

#else

PRIVATE BYTE timePling (Channel* out, BYTE buf[], size_t no)
{
  const timeout = 500000 / LOWTICK;
  int end;
  BOOL incomp;

  end = ProcTimePlus (ProcTime(), timeout);
  incomp = ChanOutTimeFail (out, buf, no, end);
  if (incomp) 
    {
    ChanReset (out);
    return FAIL;
     /* clear the channel for further transmission and return
        a transmission error signal */
    }
  else
    return OK;
}
  
PRIVATE BYTE timePlingB (Channel* out, BYTE b)
{
    const timeout = 500000 / LOWTICK;
    int end;
    BOOL incomp;

    end = ProcTimePlus (ProcTime(), timeout);
    incomp = ChanOutTimeFail (out, &b, 1, end);
    if (incomp) 
      {
      ChanReset (out);
      return FAIL;
     /* clear the channel for further transmission and return
        a transmission error signal */
      }
    else
      return OK;
}
  
PRIVATE BYTE timeQueryB (Channel* in, BYTE *fl)
{
    const timeout = 500000 / LOWTICK;
    int end;
    BOOL incomp;
    BYTE b;

    end = ProcTimePlus (ProcTime(), timeout);
    incomp = ChanInTimeFail (in, &b, 1, end);
    if (incomp) 
      {
      ChanReset (in);
      *fl = FAIL;
     /* clear the channel for further transmission and set flag to
        a transmission error signal */
      }
    else
      *fl = OK;
    return b;
} 

#endif

PRIVATE void wait (int us)
/* Wait for us micro seconds for low priority process */
{
  ProcWait (us / LOWTICK);
}

/*
 * HCTL-1100 register read/write board control
 */
PRIVATE BYTE write (BYTE device, int reg, int data)
/* write cycle with timeout */
{
    BYTE fl;
    BYTE writeCycle[4];

    writeCycle[0] = ADDR+device;
    writeCycle[1] = reg;
    writeCycle[2] = WRITE+device;
    writeCycle[3] = data;
    fl = timePling (edgeout, writeCycle, sizeof(writeCycle));

    return fl;
}

PRIVATE BYTE read (BYTE device, BYTE reg, BYTE *fl)
/* read cycle with timeout */
{
    BYTE readCycle[6];

    readCycle[0] = ADDR+device;
    readCycle[1] = reg;
    readCycle[2] = READ+device;
    readCycle[3] = DUMMY;
    readCycle[4] = DUMMY;
    readCycle[5] = OUTPUT+device;
    *fl = timePling (edgeout, readCycle, sizeof(readCycle));
    if (*fl == OK)
      return timeQueryB (edgein,fl);
    else
      return ANYTHING; /* what is returned is unimportant since 
                          a fail is signalled */
}

PRIVATE BYTE boardControl (BOOL reset, BOOL sync)
/* Assert Reset and Sync signals on both HCTL-1100
   signals also cleared by next dummy == boardAssert(FALSE,FALSE) */
{
    BYTE control;

    if (reset)
        control = RESET;
    else
        control = DUMMY;
    if (sync)
        control = control + SYNC;
    return timePlingB (edgeout, control);
}
  
PUBLIC BYTE output (BYTE device, BYTE data)
/* Parallel output cycle with timeout*/
{
    BYTE outputCycle[2];
    outputCycle[0] = WRITE+device;
    outputCycle[1] = data;
    return timePling (edgeout, outputCycle, sizeof(outputCycle));
}

PUBLIC BYTE input (BYTE device, BYTE *fl)
/* Parallel input cycle with timeout */
{
    BYTE inputCycle;

    inputCycle = OUTPUT+device;
    *fl = timePlingB (edgeout, inputCycle);
    if (*fl == OK)
      return timeQueryB (edgein,fl);
    else
      return ANYTHING; /* what is returned is unimportant since
                          an error has been signalled */
}
  
/*
**       -------------------------------------
**       -- HCTL-1100 high level operations --
**       ------------- General ---------------
**       -------------------------------------
*/

/*
** Mode Control  
*/

PUBLIC BYTE init (BYTE device)
/* Reset the a Hctl-1100 leaving it in idle mode. With sign reversal inhinit */
{
    const resetDelay = 1000;   /* 1ms */
    BYTE fl;
#if USEDIFF
	;
#endif
SemWait(sem);
    fl = write (device, 0x05, 0);   /* Reset */
    wait (resetDelay);
    if (fl == OK)
      fl = write (device, 0x07, 1);   /* Turn on sign reversal inhibit */
SemSignal(sem);
#if USEDIFF
	;
#endif
return fl;
}

PUBLIC BYTE idle (BYTE device)
/* Idle Mode */
{
 BYTE fl;

 SemWait(sem);
 fl = write (device, 0x05, 1);
 SemSignal(sem);
 return fl;
}

/*
 * General data commands
 */
PUBLIC BYTE filter (BYTE device, BYTE k, BYTE a, BYTE b)
/* filter */
{
  BYTE fl;

  SemWait(sem);
  fl = write (device, 0x22, k);
  if (fl == OK)
    fl = write (device, 0x20, a);
  if (fl == OK)
    fl = write (device, 0x21, b);
  SemSignal(sem);
  return fl;
}

PUBLIC BYTE sampleTime (BYTE device, BYTE internalVal)
/* sample timer,  No conversion made from microseconds */
{
  BYTE fl;

  SemWait(sem);
  fl = write (device, 0x0F, internalVal);
  SemSignal(sem);

  return fl;
}

PUBLIC BYTE setPos (BYTE device, INT32 pos)
/* Preset actual position */
{
  BYTE fl;

  SemWait(sem);
  if ((fl = write (device, 0x15, (pos >> 16) & 0xFF)) == OK)   /* MSB */
    if ((fl = write (device, 0x16, (pos >> 8) & 0xFF)) == OK)
      fl = write (device, 0x17, pos & 0xFF);      /* write LSB latches rest */
  SemSignal(sem);

  return fl;
}

PUBLIC INT32 actPos (BYTE device, BYTE *fl)
/* get actual position */
{
    	BYTE l,c,m;
    	/* sign extension */
	unsigned int high_byte = 0;
/*	unsigned int temp1, temp2;
	signed long int final;*/
SemWait(sem);
    	l = read (device, 0x14, fl);    /* read LSB latches rest */
    	if (*fl == OK)
          c = read (device, 0x13, fl);
    	if (*fl == OK)
          m = read (device, 0x12, fl);
SemSignal(sem);    /* if *fl  == FAIL we do not care what is returned */
/*	temp1 = ((c << 8) + l);
	temp2 = m;
	if((temp2 & 0x80) != 0)
		temp2 = (temp2 | 0xFF00);
	final = ((long)temp2 << 16) + (long)temp1;
	return (final);*/
	if((m & 0x80) != 0)
		high_byte =0xFF;
     return ((high_byte << 24) + (m <<16) + (c <<8) + l);
}
 
PUBLIC BYTE accel (BYTE device, INT16 acc)
/* acceleration for Trapezoidal and Integral Velocity Mode */
{
        BYTE fl;

	SemWait(sem);
    	if ((fl = write (device, 0x27, (acc >> 8 ) & 0x7F)) == OK)   /* Integer only 7 bits */
    	  fl = write (device, 0x26, acc & 0xFF);                       /* Fractional */
	SemSignal(sem);

        return fl;
}
  
/*
**       -------------------------------------
**       -- HCTL-1100 high level operations --
**       ------ Integral Velocity Mode -------
**       -------------------------------------
*/

PUBLIC BYTE intgMode (BYTE device)
/* Set to Integral Velocity */
{
BYTE fl;

SemWait(sem);
    if ((fl = write (device, 0x00, 0x0)) == OK)  /* Clear F0 Trapezoidal Profile */
      if ((fl = write (device, 0x00, 0x3)) == OK)  /* Clear F3 Proportional Velocity */
        if ((fl = write (device, 0x00, 0xD)) == OK)  /* Set F5 Integral Velocity */
          fl = write (device, 0x05, 3);    /* Go */
SemSignal(sem);

return fl;
}

PUBLIC BYTE comdVel (BYTE device, BYTE vel)
/* set command velocity, for integral velocity mode
   velocity is in quadrature count per sample time */
{
BYTE fl;

SemWait(sem);
    fl = write (device, 0x3C, vel);
SemSignal(sem);

return fl;
}
/*
**       -----------------------------------------
**       -- HCTL-1100 high level operations ------
**       ------ Proportional Velocity Mode -------
**       -----------------------------------------
*/
PUBLIC BYTE propMode (BYTE device)
/* Set to proportional velocity mode */
{
BYTE fl;

SemWait(sem);
    if ((fl = write (device, 0x00, 0x0)) == OK)  /* Clear F0 Trapeziodal Profile */
      if ((fl = write (device, 0x00, 0xB)) == OK)  /* Set F3 Proportional Velocity */
        if ((fl = write (device, 0x00, 0x5)) == OK)  /* Clear F5 Integral Velocity */
          fl = write (device, 0x05, 3);    /* Go */
SemSignal(sem);

return fl;
}

PUBLIC BYTE propVel (BYTE device, INT16 vel)
/* set command velocity for proportional velocity mode
   velocity is in quadrature count per sample time * 16, as
   defined by the HCTL-1100's fractional format */
{
BYTE fl;

SemWait(sem);
    if ((fl = write (device, 0x23, vel)) == OK)
      fl = write (device, 0x24, vel>>8);
SemSignal(sem);

return fl;
}
/*
**       -------------------------------------
**       -- HCTL-1100 high level operations --
**       ------ Position Control Mode --------
**       -------------------------------------
*/
PUBLIC BYTE posMode (BYTE device)
/* Set into Position Control Mode */
{
BYTE fl;
SemWait(sem);
    if ((fl = write (device, 0x00, 0x0)) == OK)  /* Clear F0 Trapeziodal Profile */
      if ((fl = write (device, 0x00, 0x3)) == OK)  /* Clear F3 Proportional Velocity */
        if ((fl = write (device, 0x00, 0x5)) == OK)  /* Clear F5 Integral Velocity */
          fl = write (device, 0x05, 3);    /* Go */
SemSignal(sem);
return fl;
}

PUBLIC BYTE comPos (BYTE device, INT32 pos)
/* set command position,  For Position Mode */
{
BYTE fl;
SemWait(sem);
    if ((fl = write (device, 0x0C, (pos >> 16) & 0xFF)) == OK)   /* MSB */
      if ((fl = write (device, 0x0D, (pos >> 8) & 0xFF)) == OK)
        fl = write (device, 0x0E, pos & 0xFF);           /* write LSB latches rest */
SemSignal(sem);
return fl;
}

PUBLIC int PowerOutput (BYTE device, BYTE *fl)
/* reads the power output to the PWM port */
{
BYTE l;
int i;

SemWait(sem);
l = read (device, 0x09, fl);
SemSignal(sem);
if ((l & 0x80) == 0)
  i = (int)l;
else
  i = ((int)l) - 256;
return i;
}

PUBLIC BYTE SetPWM (BYTE device, BYTE level)
/* sets the power output to the PWM port */
{
BYTE l;

SemWait(sem);
l = write (device, 0x09, level);
SemSignal(sem);
return l ;
}
