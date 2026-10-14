/** 
 * @file   timer.cpp
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

#ifdef _WIN32
#  include <windows.h> // For QueryPerformanceCounter etc
#else
#  include <sys/time.h> // for gettimeofday etc
#endif

#include "timer.h"

static void _GetCurrentTime(long int& secs, long int& microsecs)
{
  secs=0;
  microsecs=0;

#ifdef _WIN32
  // Use the high precision CPU clock (this might be intel specific, but then so is windows!)
	LARGE_INTEGER tim, freq;
     QueryPerformanceCounter(&tim);
     QueryPerformanceFrequency(&freq);
	 const long double time_in_secs = (long double)tim.QuadPart / (long double)freq.QuadPart;
     secs = long int(time_in_secs);
	 microsecs = long int(1000000.0*(time_in_secs - secs));
#else
  struct timeval t;
  if( gettimeofday(&t,0) == -1 ) {
    // Error
    return;
  } else {
    secs = t.tv_sec;
    microsecs = t.tv_usec;
  }
#endif

  return;
}

static double GetTimeDiff(long int start_secs, long int start_microsecs,
			  long int stop_secs, long int stop_microsecs)
{
  return (stop_secs - start_secs) + double(stop_microsecs - start_microsecs) / 1000000.0;
}		  


VW::Timer::Timer()
  : _is_running(false)
{
  Start();
}

void 
VW::Timer::Start()
{
  _GetCurrentTime(_start_secs, _start_microsecs); 
  _is_running = true;
}

void 
VW::Timer::Stop()
{
  _GetCurrentTime(_stop_secs, _stop_microsecs); 
  _is_running = false;
}

/** Return the time elapsed with units of seconds (precision is still 
 * microsecs). 
 */
double VW::Timer::GetAsSeconds() const
{
  if (_is_running) {
    long int tsecs, tmicrosecs;
    _GetCurrentTime(tsecs, tmicrosecs); 
    return (GetTimeDiff(_start_secs, _start_microsecs,
			tsecs, tmicrosecs));
  } else {
    return (GetTimeDiff(_start_secs, _start_microsecs,
			_stop_secs, _stop_microsecs));
  }
}


std::ostream& VW::operator<<(std::ostream& stream, const VW::Timer& timer)
{
  const double t = timer.GetAsSeconds();
  if (t<0.001) {
    stream << t*1000000.0 << "us ";
  } else if (t<1.0) {
    stream << t*1000.0 << "ms ";
  } else if (t<60.0) {
    stream << t << "s ";
  } else if (t<3600.0) {
    const int mins = int(t/60);
    const double secs = t - (60*mins);
    stream << mins << " min " << secs << "s ";
  } else {
    const int hours = int(t/3600);
    const int mins = int((t - 3600*hours)/60);
    const double secs = t - (60*(mins+60*hours));
    stream << hours << " hours " << mins << " min " << secs << "s ";
  }
  return stream;
}
