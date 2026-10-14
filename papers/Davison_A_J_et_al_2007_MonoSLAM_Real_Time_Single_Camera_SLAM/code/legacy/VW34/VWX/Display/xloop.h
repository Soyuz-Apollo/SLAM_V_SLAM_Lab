/** 
 * @file   xloop.h
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

#ifndef _xloop_h_
#define _xloop_h_


namespace VW {

  // Initialise X
  void XInit(int argc, char** argv);

  // The main loop. After creating all displays and stuff, call this baby to
  // hand over control

  void XLoop();


  // Alternatively, write your own loop, using the following func to process
  // a single event at a time
  void GetAndDispatchXEvent();

}; // End namespace VW

#endif
