/** 
 * @file   gti.c
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


#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "horatio/global.h"
#include "horatio/image.h"

#include "iserv.h"
#include "channel.h"
#include "flag.h"
#include "pmac.h"
#include "module.h"

#define DEFAULT_BOOT "gonad.btl"

struct termios OrigMode, CurMode;

int main(int argc, char *argv[])
{
    char  *bootable = DEFAULT_BOOT;
    char   c, replybuf[256];
    int    len, status, *ires = (int *)&replybuf[1];
    float  verge[2], elev, pan;

    if (--argc > 0)
	bootable = *++argv;

    ioctl(0, TCGETS, &OrigMode);
    CurMode = OrigMode;
    CurMode.c_iflag &= ~ICRNL;
    CurMode.c_lflag &= ~(ICANON | ECHO);
    CurMode.c_cc[VTIME] = 0;
    CurMode.c_cc[VMIN] = 1;
    ioctl( 0, TCSETS, &CurMode );

    open_module(bootable);

    fprintf(stderr, "Protocol:\n");
    fprintf(stderr, "\tKey l : move steering to left by small amount.\n");
    fprintf(stderr, "\tKey r : move steering to right by small amount.\n");
    fprintf(stderr, "\tKey u : speed up the drive unit (in int vel mode).\n");
    fprintf(stderr, "\tKey d : slow down the drive unit (in int vel mode).\n");
    fprintf(stderr, "\tKey f : move robot forward by small amount (in pos mode).\n");
    fprintf(stderr, "\tKey b : move robot backward by small amount (in pos mode).\n");
    fprintf(stderr, "\tKey 0 : print contents of steer unit position register.\n");
    fprintf(stderr, "\tKey 1 : print contents of drive unit position register.\n");
    fprintf(stderr, "\tKey o : print power output of steer device.\n");
    fprintf(stderr, "\tKey p : print power output of drive device.\n");
    fprintf(stderr, "\tKey y : yorick position demand.\n");
    fprintf(stderr, "\tKey z : yorick odometry.\n");
    fprintf(stderr, "\tKey h : home yorick odometry.\n");
    fprintf(stderr, "\tKey Q : quit.\n \n");

    status = OK;
    while ((c=getchar())!='Q' && c!=0x3 && status==OK) {

	switch (c) {
	case 'l':
	case 'r':
	case 'u':
	case 'd':
	case 'f':
	case 'b':
	case '0':
	case '1':
	case 'o':
	case 'p':
	    fprintf(stderr, "%c: ", c);
	    if (TheLink >= 0) {
		ChanOutChar(TheLink, c);
		len = ChanInMsg(TheLink, replybuf);
		if ((status=replybuf[0]) != OK) {
		    fprintf(stderr, "Error: Status=%d\n", status);
		} else if (len > 1) {
		    fprintf(stderr, "%d\n", *ires);
		}
	    } else {
		fprintf(stderr, "link=-1%c\n", 0x7);
	    }
	    break;

	case 'y':
	    if (pmac >= 0) {
		ioctl( 0, TCSETS, &OrigMode );
		printf("Verge left: "); scanf("%f", &verge[LEFT]);
		printf("Verge right: "); scanf("%f", &verge[RIGHT]);
		printf("Elevation: "); scanf("%f", &elev);
		printf("Pan: "); scanf("%f", &pan);
		ioctl( 0, TCSETS, &CurMode );
		if (yorick_demand(pan, elev, verge[LEFT], verge[RIGHT]) != OK) {
		    fprintf(stderr, "Error: Status=%d\n", status);
		}
	    } else {
		fprintf(stderr, "pmac=-1%c\n", 0x7);
	    }
	    break;

	case 'z':
	    if (pmac >= 0) {
		if (whereis_yorick(&pan, &elev, &verge[LEFT], &verge[RIGHT]) != OK) {
		    fprintf(stderr, "Yorick: Error: Status=%d\n", status);
		} else {
		    printf("Yorick:   VL      VR     ELEV      PAN\n");
		    printf("       %8.3f %8.3f %8.3f %8.3f\n", 
			   verge[LEFT], verge[RIGHT], elev, pan);
		}
	    } else {
		fprintf(stderr, "pmac=-1%c\n", 0x7);
	    }
	    break;

	case 'h':
	    if (pmac >= 0) {
		homez_yorick();
		fprintf(stderr, "Yorick axes now zero\n");
	    } else {
		fprintf(stderr, "pmac=-1%c\n", 0x7);
	    }
	    break;


	case 10:
	case 13:
	    /* do nothing for newline */
	    break;

	default:
	    /* sound bell */
	    fprintf(stderr, "%c", 0x7);
	    break;
	}
    }

    fprintf(stderr, "Q\n");
    ioctl( 0, TCSETS, &OrigMode );
    close_module();
    exit(0);
}
