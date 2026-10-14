/** 
 * @file   flag.c
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



char *status_msg[0x19] = {"OK",
			      "FAIL",
			      "ANYTHING",
			      "STOP",
			      "COLLISION",
			      NULL,
			      NULL,
			      NULL,
			      NULL,
			      NULL,
			      NULL,
			      NULL,
			      NULL,
			      NULL,
			      NULL,
			      NULL,
			      "ERROR_ZERO",
			      "ERROR_STEER_TIME",
			      "ERROR_DRIVE_TIME",
			      "ERROR_STEER_RESET",
			      "ERROR_DRIVE_RESET",
			      "ERROR_STEER_POSCTRL",
			      "ERROR_DRIVE_POSCTRL",
			      "ERROR_STEER_ZERO",
			      "ERROR_DRIVE_ZERO" };
