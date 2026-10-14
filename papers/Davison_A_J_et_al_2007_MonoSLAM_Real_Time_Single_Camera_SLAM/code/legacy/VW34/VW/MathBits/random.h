/** @file random.h 
 * @ingroup MathsStuff
 * @author KR Connor, BJ Tordoff
 * @date   02/08/02
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
 * @brief Functions to provide random number generation.
 * Part of this uses C-style calls, 
 * and part as in Stroustrup, p685.
 * The first (Randint) gives random integers in the entire range of valid 
 * integers. The second (Urand) gives integers in the range 0<->n. 
 */

#ifndef _random_h
#define _random_h

#include <stdlib.h>

#include <time.h>

namespace VW {

  /** Seed the random number generator with a time-dependent seed 
   * (comes from clock)
   */
  void SeedRand ();


  /** Get a random number in the range 0 <= x < upper */
  double Rand(double upper);

  /** Get a random integer number in the range 0 <= x < upper */
  int RandInt(int upper);


class Randint 
{
  unsigned long randx;
public:
  Randint(long s=0) : randx(s) {}

  void Seed(long s);
  /*# Magic numbers chosen to use 31 bits of a 32-bit long #*/

  int Abs(int x);
  static double RandMax(); //note: a double
  int Draw();
  
  double FDraw();

  int operator() ();
};

class Urand : public Randint 
{
  int n;
public:
  Urand(int nn) : n(nn) {}

  int operator() ();
};

inline void Randint::Seed(long s) {randx = s;}
inline int Randint::Abs(int x) {return x&0x7fffffff;}
inline double Randint::RandMax() {return 2147483648.0;} //note: a double
inline int Randint::Draw() {return randx = randx*1103515245 + 12345;}
inline double Randint::FDraw() {return Abs(Draw())/RandMax();}
inline int Randint::operator() () {return Abs(Draw());}

inline int Urand::operator() () {int r = static_cast<int>(n*FDraw()); return (r==n) ? n-1 : r;}

}; // end namespace VW


// INLINES

inline 
void VW::SeedRand ()
{
  time_t thetime;
  thetime = time(0);
  // Any int on any machine we're interested in will easily go up to 1e8
  int seed = int(thetime) & 100000000;
  srand (seed);
}

inline
double VW::Rand(double upper)
{
  return (upper * double (rand()) / (double(RAND_MAX) + 1.0));
}

inline
int VW::RandInt(int upper)
{
  return int(Rand(upper));
}

#endif



