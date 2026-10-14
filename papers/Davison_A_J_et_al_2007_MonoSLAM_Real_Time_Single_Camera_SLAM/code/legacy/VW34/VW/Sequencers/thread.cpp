/** @file thread.cpp 
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
#include "thread.h"

#define VERBOSE false

using namespace std;

namespace VW
{


FuncRetType Thread::_CallMain(void* obj)
{
  if (VERBOSE) cerr << "Thread::_CallMain" << endl;
  Thread *thread=(Thread*)obj;
  thread->Main(thread->_arg);
#ifndef _WIN32
  return 0;
#endif
}

Thread::Thread()
{
}

Thread::~Thread() 
{
}

int Thread::Detach()
{
#ifdef _WIN32
	// No such thing!
	return 0;
#else
  return pthread_detach(_thread);
#endif
}

int Thread::Start(void* arg)
{
  if (VERBOSE) cerr << "Start thread" << endl;
  Thread *t=this;
  _arg=arg;
#ifdef _WIN32
  return _beginthread(Thread::_CallMain,0,t);
#else
  // NB: pass 0 as attribute forces defaults
  return pthread_create(&_thread,0,Thread::_CallMain,t);
#endif
  if (VERBOSE) cerr << "      started" << endl;
}


Mutex::Mutex()
{
  if (VERBOSE) cerr << "Create mutex" << endl;
#ifdef _WIN32
	_mutex = CreateMutex( NULL, FALSE, NULL );
#else
	pthread_mutex_init(&_mutex, NULL);
#endif
}

  Mutex::~Mutex()
  {
  if (VERBOSE) cerr << "Destroy mutex" << endl;
#ifdef _WIN32
	//DestoryMutex(_mutex);
#else
	pthread_mutex_destroy(&_mutex);
#endif
  }

  bool Mutex::Lock()
  {
  if (VERBOSE) cerr << "Mutex lock" << endl;
		bool success=true;
#ifdef _WIN32
	success = (WaitForSingleObject( _mutex, INFINITE ) != WAIT_FAILED);
#else
	success = (pthread_mutex_lock(&_mutex) == 0);
#endif
	if (VERBOSE) cerr << "      locked" << endl;
	return success;
  }

  bool Mutex::Unlock()
  {
  if (VERBOSE) cerr << "Mutex unlock" << endl;
		bool success=true;
#ifdef _WIN32
		success = (ReleaseMutex(_mutex) == TRUE);
#else
	success = (pthread_mutex_unlock(&_mutex) == 0);
#endif
  if (VERBOSE) cerr << "      unlocked" << endl;
	return success;
  }



}; // End namespace VW
