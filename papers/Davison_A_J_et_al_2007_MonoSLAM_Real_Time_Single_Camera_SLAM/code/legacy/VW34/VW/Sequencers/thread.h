/** @file thread.h 
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

#ifndef _thread_h_
#define _thread_h_


#ifdef _WIN32
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0400	// Allow use of features specific to Windows NT 4 or later.
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif
 
namespace VW
{
 
// We need OS specific types for the C threads themselves, mutexes etc
#ifdef _WIN32
  typedef unsigned ThreadType; 
  typedef HANDLE MutexType; 
  typedef void FuncRetType;
#else
  typedef pthread_t ThreadType;
  typedef pthread_mutex_t MutexType;
  typedef void* FuncRetType;
#endif

  /** Base class from which a multi-threaded app can derrive.
   * @ingroup CoreSequencers
   */
  class Thread
  {
  public:
    /** Construct, with optional attributes. */
    Thread();
    virtual ~Thread();
    
    operator ThreadType*() {return &_thread;}
    
    virtual void* Main(void*)=0;
    int Detach();
    
     // arg is for passing extra data to main, but never pass a
     // local variable or address of local variable.  Arg must
     // be available throughout life of program.
    int Start(void* arg=0);
    
  protected:
    ThreadType _thread;
    void*     _arg;
    
    static FuncRetType _CallMain(void* obj);
    
  };
  
  /** Simple class to represent a mutex (mutual exclusion). */
  class Mutex {
  public:
	  Mutex();
	  ~Mutex();

	  /** Attempt to lock the mutex. */
	  bool Lock();
	  bool Unlock();
  private:
	  MutexType _mutex;
  };

} /* namespace VW */

#endif
