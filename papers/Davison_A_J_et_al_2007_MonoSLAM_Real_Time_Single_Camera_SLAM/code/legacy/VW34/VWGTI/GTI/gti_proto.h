/** 
 * @file   gti_proto.h
 * @author ID Reid
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
 *
 * Definitions for client to root transputer protocol and other
 * goodies
 *
 */

#define GONAD_START_STRING "gonad_start"
#define GONAD_STRING_LEN 11

// Communications protocol
#define GRAB_CHAR 'i'        /* grab stereo pair */
#define TOGS_CHAR '='        /* toggle left/right stick control */
#define STCK_CHAR '.'        /* set stick position */
#define STKL_CHAR '<'        /* bump stick left a bit */
#define STKR_CHAR '>'        /* bump stick right a bit */
#define BINC_CHAR '+'        /* increase bump size */
#define BDEC_CHAR '-'        /* decrease bump size */
#define QUIT_CHAR 'q'        /* terminate */
#define VROT_CHAR 'R'        /* rotate vehicle relative angle (degrees) */
#define VDRV_CHAR 'D'        /* drive vehicle relative distance (metres) */
#define YPAN_CHAR 'y'        /* send demand to PMAC */
#define YZER_CHAR 'Z'        /* zero PMAC axes */
#define HEAD_CHAR 'h'        /* enable head tracking */
#define HDIS_CHAR 'H'        /* disable head tracking */
#define NAVI_CHAR 'n'        /* enable navigation */
#define NDIS_CHAR 'N'        /* disable navigation */
#define ODOM_CHAR 'z'        /* get PMAC odometry */
#define LTRN_CHAR 'l'        /* steer left a bit */
#define RTRN_CHAR 'r'        /* steer right a bit */
#define SREL_CHAR 's'        /* steer to relative angle (degrees) */
#define SABS_CHAR 'S'        /* steer to absolute angle (degrees) */
#define VABS_CHAR 'v'        /* set absolute vehicle velocity (+-40) */
#define VINC_CHAR 'u'        /* increase velocity */ 
#define VDEC_CHAR 'd'        /* decrease velocity */ 
#define FOWD_CHAR 'f'        /* bump vehicle forwards */
#define BACK_CHAR 'b'        /* bump vehicle backwards */
#define GETS_CHAR '0'        /* Get steer position */
#define GETD_CHAR '1'        /* Get drive position */
#define POWS_CHAR 'o'        /* Get steering power */
#define POWD_CHAR 'p'        /* Get drive power */

// Status
#define OK		0
#define FAIL		1
#define ANYTHING	2
#define STOP		3
#define COLLISION       4

// Errors
#define ERROR_ZERO           0x10
#define ERROR_STEER_TIME     0x11
#define ERROR_DRIVE_TIME     0x12
#define ERROR_STEER_RESET    0x13
#define ERROR_DRIVE_RESET    0x14
#define ERROR_STEER_POSCTRL  0x15
#define ERROR_DRIVE_POSCTRL  0x16
#define ERROR_STEER_ZERO     0x17
#define ERROR_DRIVE_ZERO     0x18
