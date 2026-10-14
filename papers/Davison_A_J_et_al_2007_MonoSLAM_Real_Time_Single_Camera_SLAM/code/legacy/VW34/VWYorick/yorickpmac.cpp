/** 
 * @file   yorickpmac.cc
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


/*
 * pmac.cc
 *
 * Routines for reading and writing to PMAC registers
 *
 */


/* SYSTEM INCLUDES */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/mman.h> // for memory mapping and stuff

/* LOCAL INCLUDES */
#include "yorickpmac.h"
#include "yorickpmacaddress.h" // List of M-Variable locations




/* send/reply buffer lengths */
#define BUFLEN          256

// For zeroing odometry
#define YZER_CHAR 'Z'

// Initialize the static members
const char *const YorickPMAC::PMAC_DEVICE_NAME = "/dev/pmac0";
const char *const YorickPMAC::PMAC_PROG_NAME = "/usr/lav/gonad/src/yorick.pmc";
const char *const YorickPMAC::PMAC_RUN_NAME = "/usr/lav/gonad/src/run.pmc";

// {{{ YorickPMAC::YorickPMAC(void)

YorickPMAC::YorickPMAC(void)
{
  // Constructor does nothing useful. Don't want to initialize at
  // this point, as class may be constructed long before the PMAC
  // actually needs to be used.

  // Set Ram address to NULL, so that nothing tries to access it
  DPRAMbase=NULL;
  pmac_fd=-1;
}

// }}}
// {{{ YorickPMAC::~YorickPMAC(void)

YorickPMAC::~YorickPMAC(void)
{
  // Destructor is similarly useless, although we should make sure the
  // device is closed...
  Close();
}

// }}}

// Initialize PMAC and return true for success
// {{{ bool YorickPMAC::Init(void) 

bool YorickPMAC::Init(void) {
    if (pmac_fd >= 0) {
	fprintf(stderr, "PMAC already open\n");
	SetModeOn();
	return (true);
    }

    fprintf(stderr,"Opening PMAC\n");
    if ((pmac_fd = open(PMAC_DEVICE_NAME, O_RDWR)) < 0) {
	perror("open failed");
	fprintf(stderr, "device %s\n", PMAC_DEVICE_NAME);
	return (false);
    }

    if ((DPRAMbase = (char *)mmap((caddr_t)0, 0x2000, 
				  PROT_READ|PROT_WRITE,
				  MAP_FILE|MAP_SHARED, 
				  pmac_fd, (off_t)0)) == (char *)-1) {
	perror("Couldn't mmap PMAC dual-ported ram");
	close(pmac_fd);
	return (false);
    }

    SetAddresses();

    SetModeOn();
    return (true);
}

// }}}
// {{{ void YorickPMAC::Close(void)

void YorickPMAC::Close(void)
{
    if (pmac_fd >= 0) {
      SetModePending();
      if (munmap(DPRAMbase, 0x2000)!=0) {
	perror("Couldn't munmap pmac: ");
      }
      close(pmac_fd);
      pmac_fd = -1;
    }
    return;
}

// }}}

// {{{ void     YorickPMAC::SetMode (YorickPMAC::PMACMode mode )

void YorickPMAC::SetMode ( PMACMode mode )
{
    *MODE_FLAG = int(mode);
}

// }}}
// {{{ PMACMode YorickPMAC::GetMode (void)

YorickPMAC::PMACMode YorickPMAC::GetMode(void)
{
  if (DPRAMbase==NULL) {
   return(PMAC_UNINITIALIZED);
  } else {
   return(PMACMode(*MODE_FLAG));
  }
}

// }}}
// {{{ void     YorickPMAC::SetModePending(void)

void YorickPMAC::SetModePending(void)
{       
    SetMode ( PMAC_PENDING );
    sleep(1);
}

// }}}
// {{{ void     YorickPMAC::SetModeOn(void)

void YorickPMAC::SetModeOn(void)
{       
    SetMode ( PMAC_ON );
    sleep(1);
}

// }}}
// {{{ void     YorickPMAC::SetModeOff(void)

void YorickPMAC::SetModeOff(void)
{       
    SetMode ( PMAC_OFF );
    sleep(1);
}

// }}}

// {{{ int YorickPMAC::GetState(void)

int YorickPMAC::GetState(void) {
  /* read the progress variable */
  return (*PROGRAM_STATE);
}

// }}}
// {{{ void YorickPMAC::GetOdometry(int seq_no, int *time, float *pan, float *tilt, float *vergeleft float* vergeright)

void YorickPMAC::GetOdometry(int seq_no, int *time, float *pan, float *tilt, float *vergeleft, float* vergeright)
{
    /* ask for odometry for seq_no */

    *ODOM_FRAME = seq_no;
    //    *(int *)ODOM_DATA = 0;

    /* wait for odometry valid flag */
    while (!(*ODOM_VALID));

    /* now grab the data */
    *time = *ODOM_TIME;
    *pan = *ODOM_PAN;
    *tilt = *ODOM_ELEV;
    *vergeleft = *ODOM_VERGE_L;
    *vergeright = *ODOM_VERGE_R;
    //printf("PMAC Odometry: (%f,%f,%f,%f)\n",*pan, *tilt, *vergeleft, *vergeright);

    return;
}

// }}}
// {{{ void YorickPMAC::GetVelocity(int seq_no, int *time, float *vpan, float *vtilt, float *vverge)

void YorickPMAC::GetVelocity(int seq_no, int *time, float *vpan, float *vtilt, float *vvergeleft, float *vvergeright)
{
    /* ask for odometry for seq_no */
    *ODOM_FRAME = seq_no;
    *ODOM_DATA = 0;

    /* wait for odometry valid flag */
    while (!(*ODOM_VALID));

    /* now grab the data */
    *time        = *ODOM_TIME;
    *vpan        = *ODOM_PAN_VEL;
    *vtilt       = *ODOM_ELEV_VEL;
    *vvergeleft  = *ODOM_VERGE_L_VEL;
    *vvergeright = *ODOM_VERGE_R_VEL;

    return;
}

// }}}
// {{{ int YorickPMAC::ZeroOdometry (void)

int YorickPMAC::ZeroOdometry (void)
{
    char status=0;

    // Very unsure as to whether it's safe to do this JK
    *ODOM_PAN     = 0.0;
    *ODOM_ELEV    = 0.0;
    *ODOM_VERGE_L = 0.0;
    *ODOM_VERGE_R = 0.0;

    return (int)status;
}

// }}}
// {{{ void YorickPMAC::Demand (int seq_no, int time, int action, int valid_flags, float pan, float elev, float vergeleft, float vergeright)

void YorickPMAC::Demand (int seq_no, int time, int action, int valid_flags, 
			 float pan, float elev, float vergeleft, float vergeright)
{
    /* set up new demands */

  //printf("PMAC Demand: (%f,%f,%f,%f)\n",pan, elev, vergeleft, vergeright);

    *VISION_VERGE_L     = vergeleft;
    *VISION_VERGE_R     = vergeright;
    *VISION_PAN         = pan;
    *VISION_ELEV_L      = elev;
    *VISION_ACTION      = action;
    *VISION_VALID_FLAGS = valid_flags;

    /* validate them with time and seqno info */
    *VISION_TIME = time;
    *VISION_FRAME = seq_no;

    return;
}

// }}}
// {{{ int YorickPMAC::Download(int pmac_fd, char *filename, int pflag)

int YorickPMAC::Download(int pmac_fd, char *filename, int pflag)
{
    FILE *fp;
    char  buf[BUFLEN], reply[BUFLEN];
    int   i, nc;

    fp = fopen(filename,"r");
    if(fp==NULL) {
	fprintf(stderr, "Cannot read file [%s]\n", filename);
	return -1;
    }
	
    fprintf(stderr, "Sending file [%s] ...\n", filename);
    while (fgets(buf, BUFLEN-1, fp) != NULL) {
	nc = strlen(buf);
	if (pflag) 
	    printf("%s", buf);
	else
	    for (i=0; i<100000; i++);
	buf[nc-1] = '\r';
	write(pmac_fd, &buf, nc);
	nc = read(pmac_fd, reply, 1);
	if (nc!=1) {
	    fprintf(stderr, "PMAC TIMEOUT%c\n", 0x7);
	    break;
	}
	if (reply[0]!='\n') {
	    nc = read(pmac_fd, reply+1, BUFLEN-2);
	    if (reply[0]==7) {
		fprintf(stderr, "PMAC ERROR%c: %s\n", 0x7, reply);
		break;
	    } else if (pflag)
		printf("PMAC REPLY: %s\n", reply);
	}
    }
    fclose(fp);
    fprintf(stderr, "Done.\n");

    return 0;
}

// }}}

// {{{ void YorickPMAC::SetAddresses(void)

void YorickPMAC::SetAddresses(void) {
  /***********************************************************
   * DPRAMbase+0x00 = M900, with each memory location taking up
   * 4 bytes (ie. DPRAMseg+0x04 = M901). 0x indicates HEX.
   ************************************************************/
  
  // See yorickpmacaddress.h for the M### definitions

  MODE_FLAG        = reinterpret_cast<int*>  (DPRAMbase + M900);
  PROGRAM_STATE    = reinterpret_cast<int*>  (DPRAMbase + M903);
  VISION_TIME      = reinterpret_cast<int*>  (DPRAMbase + M901);
  VISION_FRAME     = reinterpret_cast<int*>  (DPRAMbase + M902);
  VISION_DATA      = reinterpret_cast<float*>(DPRAMbase + M916);
  VISION_PAN       = reinterpret_cast<float*>(DPRAMbase + M916);
  VISION_ELEV_R    = reinterpret_cast<float*>(DPRAMbase + M916);
  VISION_ELEV_L    = reinterpret_cast<float*>(DPRAMbase + M917);
  VISION_VERGE_L   = reinterpret_cast<float*>(DPRAMbase + M918);
  VISION_VERGE_R   = reinterpret_cast<float*>(DPRAMbase + M919);
  VISION_ACTION    = reinterpret_cast<int*>  (DPRAMbase + M920);
  VISION_VALID_FLAGS = reinterpret_cast<int*>(DPRAMbase + M921);
  ODOM_FRAME       = reinterpret_cast<int*>  (DPRAMbase + M932);
  ODOM_DATA        = reinterpret_cast<float*>(DPRAMbase + M933);
  ODOM_VALID       = reinterpret_cast<int*>  (DPRAMbase + M933);
  ODOM_TIME        = reinterpret_cast<int*>  (DPRAMbase + M934);
  ODOM_PAN         = reinterpret_cast<float*>(DPRAMbase + M935);
  ODOM_ELEV        = reinterpret_cast<float*>(DPRAMbase + M936);
  ODOM_VERGE_L     = reinterpret_cast<float*>(DPRAMbase + M937);
  ODOM_VERGE_R     = reinterpret_cast<float*>(DPRAMbase + M938);
  ODOM_PAN_VEL     = reinterpret_cast<float*>(DPRAMbase + M939);
  ODOM_ELEV_VEL    = reinterpret_cast<float*>(DPRAMbase + M940);
  ODOM_VERGE_L_VEL = reinterpret_cast<float*>(DPRAMbase + M941);
  ODOM_VERGE_R_VEL = reinterpret_cast<float*>(DPRAMbase + M942);

}

// }}}
