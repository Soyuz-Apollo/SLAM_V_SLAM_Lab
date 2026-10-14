/** 
 * @file   channel.h
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


#ifdef TRANSPUTER
typedef Channel *Chan;
#else
typedef int Chan;
#endif

#ifndef TRANSPUTER
int ChanInInt(Chan chan);
void ChanOutInt(Chan chan, int i);
char ChanInChar(Chan chan);
void ChanOutChar(Chan chan, char c);
void ChanIn(Chan chan, char *buf, int count);
void ChanOut(Chan chan, char *buf, int count);
#endif

int ChanInMsg(Chan chan, char *buf);
void ChanOutMsg(Chan chan, char *buf, int len);
