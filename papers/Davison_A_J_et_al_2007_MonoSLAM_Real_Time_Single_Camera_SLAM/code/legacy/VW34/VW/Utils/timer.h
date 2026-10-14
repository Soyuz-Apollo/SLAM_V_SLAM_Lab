/** 
 * @file   timer.h
 * @author BJ Tordoff
 * @date   25/03/2004		   		
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
/**
 * @class   VW::Timer
 * @ingroup Utils
 * @author BJ Tordoff
 * @date   25/03/2004		   		
 * @brief   Simple timer class for measuring elapsed time, with precision
 * down to microseconds.
 *
 * There are several ways to use this class, but some general rules:
 * - The timer starts when the class is created
 * - Calling Start() will reset the timer to zero
 * - Calling Stop() stores the end time
 * - Calling GetAsXXX() with the timer running returns the elapsed time but
 * leaves the timer running
 *
 * Some example uses:
 *
 *   VW::Timer timer;
 *   t.Start();
 *   SomeFunc();
 *   t.Stop();
 *   cout << "SomeFunc took " << timer << endl;
 *
 *
 *   VW::Timer timer;
 *   SomeCode();
 *   cout << "Time since start is " << timer << endl;
 *   SomeMoreCode();
 *   cout << "Time since start is " << timer << endl;
 *
 *
 *   VW::Timer timer;
 *   double secs1,secs2;
 *   timer.Start();
 *   SomeCode();
 *   secs1 = timer.GetAsSeconds();
 *   timer.Start(); // Restart the timer
 *   SomeMoreCode();
 *   secs2 = timer.GetAsSeconds();
 *
 *   cout << "Code took " << secs1 << " + " << secs2 << " seconds." << endl;
 */

#ifndef _vw_timer_h_
#define _vw_timer_h_

#include <iostream>

namespace VW {

class Timer {
public:
  Timer();

  /** Start, or restart (if already going) the timer. */
  void Start();
  /** Stop the timer, storing the time elapsed. */
  void Stop();

  double GetAsSeconds() const;
  double GetAsMilliseconds() const;
  double GetAsMicroseconds() const;
  double GetAsNanoseconds() const;

protected:
  bool _is_running;
  long int _start_secs;
  long int _start_microsecs;
  long int _stop_secs;
  long int _stop_microsecs;
};

  std::ostream& operator<<(std::ostream& stream, const Timer& timer);

}; // End namespace VW







// Inlines //

/** Return the time elapsed with units of milli-seconds (precision is still
 * microsecs). 
 */
inline double 
VW::Timer::GetAsMilliseconds() const
{
  return (GetAsSeconds()*1000.0);
}


/** Return the time elapsed with units of micro-seconds (precision is  
 * microsecs). 
 */
inline double 
VW::Timer::GetAsMicroseconds() const
{
  return (GetAsSeconds()*1000000.0);
}

/** Return the time elapsed with units of micro-seconds (precision is  
 * still microsecs).
 */
inline double 
VW::Timer::GetAsNanoseconds() const
{
  return (GetAsSeconds()*1000000000.0);
}

#endif
