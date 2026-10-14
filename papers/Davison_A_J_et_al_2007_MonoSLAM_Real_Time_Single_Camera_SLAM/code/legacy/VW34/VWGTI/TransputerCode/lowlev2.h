/** 
 * @file   lowlev2.h
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


extern void lowlevel_init (Channel *out, Channel *in);
extern BYTE output (BYTE device, BYTE data);
extern BYTE input (BYTE device,BYTE *fl);
extern BYTE init (BYTE device);
extern BYTE idle (BYTE device);
extern BYTE filter (BYTE device, BYTE k, BYTE a, BYTE b);
extern BYTE sampleTime (BYTE device, BYTE internalVal);
extern INT32 actPos (BYTE device, BYTE *fl);
extern BYTE setPos (BYTE device, INT32 pos);
extern BYTE accel (BYTE device, INT16 acc);
extern BYTE intgMode (BYTE device);
extern BYTE comdVel (BYTE device, BYTE vel);
extern BYTE propMode (BYTE device);
extern BYTE propVel (BYTE device, INT16 vel);
extern BYTE posMode (BYTE device);
extern BYTE comPos (BYTE device, INT32 pos);
extern int PowerOutput (BYTE device, BYTE *fl);
extern BYTE SetPWM (BYTE device, BYTE level);

/* and in zero.c the check_zero function */
extern BYTE check_zero(Channel *out, Channel *in, float correction_angle);

