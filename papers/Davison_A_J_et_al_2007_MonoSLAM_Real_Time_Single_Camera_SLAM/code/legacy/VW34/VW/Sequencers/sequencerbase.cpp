/** 
 * @file   sequencerbase.cpp
 * @author ID Reid, BJ Tordoff
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

#include <iostream>
#include <assert.h>
#include <signal.h>

#include "sequencerbase.h"
#include <VW/Utils/timeops.h> // For mSleep()

using namespace std;
using namespace VW;


// constructor
SequencerBase::SequencerBase() 
  : delay(1), 
    cont(false),
    _initialised(false),
    user_finished(true)
{
}

void SequencerBase::_FrameArrived() 
{
    if (VERBOSE) std::cerr << "New frame signal"<<std::endl;
    user_finished=false;
    NewFrameEvent(0l); 
    user_finished=true;
  }
void *SequencerBase::Main(void *) 
{
    bool ok=true;
    thread_running=true;
    while (ok) { 
      if (VERBOSE) std::cerr << "Sequencer frame "<<frame_number<<std::endl;
      if (_Step()) {
	_FrameArrived();
	access_lock.Lock();
	ok=cont;
	access_lock.Unlock();
	if (delay>0) msleep(delay);
      } else {
	if (VERBOSE) std::cerr << "Something wrong! "<<std::endl;
	ok=false;
      }
      if (VERBOSE) std::cerr << "Sequencer frame finished"<<std::endl;
    }
    if (VERBOSE) std::cerr << "Terminating thread"<<std::endl;
	access_lock.Lock();
    thread_running=false;
    access_lock.Unlock();
    return(0);
  }
void VW::SequencerBase::_CheckInitialised()
  {
    if (!_initialised) {
      std::cerr << "*** ERROR - trying to use a sequencer before being initialised - did you call Init()? ***"<<std::endl;
      exit(0);
    }
  }

bool SequencerBase::Init() 
{
  _initialised=true;
  return(true);
}
void SequencerBase::Continuous() 
{
  // Must lock before accessing 'cont', which is shared
  _CheckInitialised();
  access_lock.Lock();
  if (!cont) {
    cont=true;
    Start();
  }; 
  access_lock.Unlock();
}
void SequencerBase::Next() 
{
  bool is_running;
  _CheckInitialised();
  // Must lock before accessing 'cont', which is shared
  access_lock.Lock();
  is_running=cont;
  access_lock.Unlock();

  if (is_running) 
    Stop(); 
  if (_Step()) 
    _FrameArrived(); 
}

void SequencerBase::Stop() {
  if (VERBOSE) std::cerr << "VW::SequencerBase::Stop" << std::endl;
  _CheckInitialised();
  // Must lock before accessing 'cont', which is shared
    access_lock.Lock();
  if (cont) {
    cont=false; 
   access_lock.Unlock();
   _WaitForThread(); 
    Detach(); 
  } else {
   access_lock.Unlock();
  }
  if (VERBOSE) std::cerr << "VW::SequencerBase::Stop - done" << std::endl;
}
void SequencerBase::_WaitForThread() 
{
  if (VERBOSE) std::cerr << "Wait for thread"<<std::endl;
  bool still_going=true;
   access_lock.Lock();
   still_going = thread_running;
   access_lock.Unlock();
  while (still_going) {
    if (VERBOSE) std::cerr << "Still waiting..." << std::endl;
    msleep(1);
   access_lock.Lock();
    still_going = thread_running;
   access_lock.Unlock();
  }
  if (VERBOSE) std::cerr << "Wait for thread - thread has exited"<<std::endl;
}
