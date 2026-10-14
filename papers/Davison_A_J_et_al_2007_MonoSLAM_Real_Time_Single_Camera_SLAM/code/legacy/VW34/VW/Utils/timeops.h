/**
 * Some useful inline operations on the standard timeval struct.
 */


#ifndef _TIMEOPS_H_
#define _TIMEOPS_H_

#ifdef _WIN32
	#include "windows.h"
	#include <time.h>
#else
	#include <sys/time.h>
#endif

#include <iostream>

/** Sleep for a given number of millisecs. */
void msleep(double millisecs);


bool operator<(const timeval& t1, const timeval& t2);
bool operator>(const timeval& t1, const timeval& t2);
std::ostream& operator<<(std::ostream& s, const timeval& t);
timeval operator-(const timeval& t1, const timeval& t2);
timeval operator+(const timeval& t1, const timeval& t2);
timeval operator-(const timeval& t1, const double secs);
timeval operator+(const timeval& t1, const double secs);

double timeval_to_double(const timeval& t);
timeval double_to_timeval(const double secs);

#endif
