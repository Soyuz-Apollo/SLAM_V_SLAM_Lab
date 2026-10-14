/** 
 * @file   iserv.h
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
 *
 * Include file for iserver to include it as part of another program
 *
 */

#include "iserver.h"

extern int  ReadLink();
extern int  WriteLink();
extern int  ResetLink();
extern int  AnalyseLink();
extern int  TestError();
extern int  TestRead();
extern int  TestWrite();

//int SingleServe(int *Result);
//int iserver_open(char *BootName);
//int iserver_close(void);

extern int TheLink;
