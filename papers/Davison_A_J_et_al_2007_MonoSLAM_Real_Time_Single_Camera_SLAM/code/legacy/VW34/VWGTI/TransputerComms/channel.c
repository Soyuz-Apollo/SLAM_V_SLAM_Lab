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
int ChanInInt(Chan Thelink)
{
    int i;
    if (ReadLink(Thelink, &i, 4, TIMEOUT) < 4) {
	fprintf(stderr, "problem reading int on %d\n", Thelink);
    }
    return i;
}


void ChanOutInt(Chan Thelink, int i)
{
    if (WriteLink(Thelink, &i, 4, TIMEOUT) < 4) {
	fprintf(stderr, "problem writing on link %d\n", Thelink);
    }
    return;
}


char ChanInChar(Chan Thelink)
{
    char c;
    int n;
    if ((n=ReadLink(Thelink, &c, 1, TIMEOUT)) != 1) {
	fprintf(stderr, "problem reading char on %d (n==%d)\n", Thelink, n);
    }
    return c;
}


void ChanOutChar(Chan Thelink, char c)
{
    WriteLink(Thelink, &c, 1, TIMEOUT);
    return;
}


void ChanIn(Chan Thelink, char *buf, int count)
{
    ReadLink(Thelink, buf, count, TIMEOUT);
    return;
}


void ChanOut(Chan Thelink, char *buf, int count)
{
    WriteLink(Thelink, buf, count, TIMEOUT);
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
