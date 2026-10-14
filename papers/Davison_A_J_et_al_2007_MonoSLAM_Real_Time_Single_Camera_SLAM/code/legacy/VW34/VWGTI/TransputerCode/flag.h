/** 
 * @file   flag.h
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
 * this file defines the values of useful flags 
 */

#define OK		0
#define FAIL		1
#define ANYTHING	2
#define STOP		3
#define COLLISION       4

#define ERROR_ZERO           0x10
#define ERROR_STEER_TIME     0x11
#define ERROR_DRIVE_TIME     0x12
#define ERROR_STEER_RESET    0x13
#define ERROR_DRIVE_RESET    0x14
#define ERROR_STEER_POSCTRL  0x15
#define ERROR_DRIVE_POSCTRL  0x16
#define ERROR_STEER_ZERO     0x17
#define ERROR_DRIVE_ZERO     0x18

extern char *status_msg[0x19];
