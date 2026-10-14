/** 
 * @file   channel.c
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


#include <stdio.h>
#ifndef TRANSPUTER
#include "iserv.h"
#else
#include <channel.h>
#endif
#include "channel.h"

#ifndef TRANSPUTER
int ChanInInt(Chan TheLink)
{
    int i;
    if (ReadLink(TheLink, &i, 4, TIMEOUT) < 4) {
	fprintf(stderr, "problem reading int on %d\n", TheLink);
    }
    return i;
}


void ChanOutInt(Chan TheLink, int i)
{
    if (WriteLink(TheLink, &i, 4, TIMEOUT) < 4) {
	fprintf(stderr, "problem writing on link %d\n", TheLink);
    }
    return;
}


char ChanInChar(Chan TheLink)
{
    char c;
    int n;
    if ((n=ReadLink(TheLink, &c, 1, TIMEOUT)) != 1) {
	fprintf(stderr, "problem reading char on %d (n==%d)\n", TheLink, n);
    }
    return c;
}


void ChanOutChar(Chan TheLink, char c)
{
    WriteLink(TheLink, &c, 1, TIMEOUT);
    return;
}


void ChanIn(Chan TheLink, char *buf, int count)
{
    ReadLink(TheLink, buf, count, TIMEOUT);
    return;
}


void ChanOut(Chan TheLink, char *buf, int count)
{
    WriteLink(TheLink, buf, count, TIMEOUT);
    return;
}
#endif

int ChanInMsg(Chan chan, char *buf)
{
    int len;
    len = ChanInInt(chan);
    ChanIn(chan, buf, len);
    buf[len] = '\0';
    return len;
}

void ChanOutMsg(Chan chan, char *buf, int len)
{
    ChanOutInt(chan, len);
    ChanOut(chan, buf, len);
    return;
}
