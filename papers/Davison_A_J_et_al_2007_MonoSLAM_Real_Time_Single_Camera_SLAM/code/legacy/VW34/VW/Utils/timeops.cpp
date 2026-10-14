#include "timeops.h"
#include <stdio.h>

#ifdef _WIN32
#else
	#include <unistd.h> // usleep
#endif

void msleep(double millisecs)
{
#ifdef _WIN32
	Sleep(DWORD(millisecs));
#else
	usleep(int(1000.0*millisecs));
#endif
}


bool operator<(const timeval& t1, const timeval& t2)
{
  if (t1.tv_sec < t2.tv_sec) return(true);
  else if (t1.tv_sec == t2.tv_sec) {
    // Secs equal, so check usecs
    if (t1.tv_usec<t2.tv_usec) return(true);
    else return(false);
  } else return(false);
}

bool operator>(const timeval& t1, const timeval& t2)
{
  if (t1.tv_sec > t2.tv_sec) return(true);
  else if (t1.tv_sec == t2.tv_sec) {
    // Secs equal, so check usecs
    if (t1.tv_usec>t2.tv_usec) return(true);
    else return(false);
  } else return(false);
}
std::ostream& operator<<(std::ostream& s, const timeval& t)
{
  s << double(t.tv_sec + double(t.tv_usec)*0.000001);
  return(s);
}
timeval operator-(const timeval& t1, const timeval& t2)
{
  double diff = (t1.tv_sec - t2.tv_sec)*1000000 + (t1.tv_usec - t2.tv_usec);
  timeval d;
  d.tv_sec=int(diff*0.000001);
  d.tv_usec=int(diff - 1000000*d.tv_sec);
  return(d);
}
timeval operator+(const timeval& t1, const timeval& t2)
{
  int s = t1.tv_sec + t2.tv_sec;
  int us = t1.tv_usec + t2.tv_usec;

  // Check for overflow
  if (us>1000000) {
    us-=1000000;
    s++;
  }

  timeval d;
  d.tv_sec=s;
  d.tv_usec=us;
  return(d);
}
timeval operator+(const timeval& t1, const double secs)
{
  int s=int(secs); // integer part is seconds
  int us=int((secs*1000000) - (s*1000000)); // Subtract integer part for micro-secs

  s  += t1.tv_sec;
  us += t1.tv_usec;

  // Check for overflow
  if (us>1000000) {
    us-=1000000;
    s++;
  }

  // Form result
  timeval d;
  d.tv_sec=s;
  d.tv_usec=us;

  return(d);
}
timeval operator-(const timeval& t1, const double secs)
{
  int s=int(secs); // integer part is seconds
  int us=int((secs*1000000) - (s*1000000)); // Subtract integer part for micro-secs

  s  = t1.tv_sec - s;
  us = t1.tv_usec - us;

  // Check for overflow
  if (us<0) {
    us+=1000000;
    s--;
  }

  // Form result
  timeval d;
  d.tv_sec=s;
  d.tv_usec=us;

  return(d);
}

double timeval_to_double(const timeval& t)
{
    return(t.tv_sec + t.tv_usec*0.000001);
}
timeval double_to_timeval(const double secs)
{
  struct timeval t;
  // integer part is seconds
  t.tv_sec=int(secs);
  // Subtract integer part for micro-secs  
  t.tv_usec=int((secs*1000000) - (t.tv_sec*1000000));
  return(t);
}
