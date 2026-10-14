/** 
 * @file   linefit.h 
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


#ifndef _linefit_h_
#define _linefit_h_

#include <list>
#include "linesegfitted.h"

namespace VW {

#define LIM_DEFAULT 0.5
#define LEN_DEFAULT 10
#define MIN_CONF_DEFAULT 0


class LenThresh {
 private:
  int thresh;
 public:
  LenThresh(int len) : thresh(len) {};
  bool operator()(LineSegFitted &l) 
    { 
      return l.info.n < thresh; 
    };
};

  /** Fit lines to edge data.
   * @param edat the input edge data (as output from VW::Canny)
   * @param lines the resulting set of straight line segments
   * @param lambda_lim a threshold on line straightness based on ratio of minimum eigenval of scatter matrix to number of points in the line (default 0.5)
   * @param lengththresh the minimum line length to be included in the result
   */

class LineFit {

 public:
  LineFit(const double lim=LIM_DEFAULT, const int len=LEN_DEFAULT);

  LineFit(const EdgeData& edat, 
	  std::list<LineSegFitted>& lines,
	  const double lim=LIM_DEFAULT, const int len=LEN_DEFAULT);
  
  LineFit(const EdgeString *s, 
	  std::list<VW::LineSegFitted> &lines, 
	  const double lim=LIM_DEFAULT, const int len=LEN_DEFAULT);

  void Compute(const EdgeData& edat, std::list<LineSegFitted>& lines);
  void Compute(const EdgeString *s, std::list<LineSegFitted> &lines);

  operator bool() { return true; }
 

 private:

  double lambda_lim, min_conf;
  int    lengththresh;

  double CalcMergeScore(LineSegFitted &l1, LineSegFitted &l2, LineInfo& info);
  void StartLines(std::list<LineSegFitted>& lines, const EdgeString *s);
  void MergeLines(std::list<LineSegFitted>& lines);

};

}; // end namespace VW


inline 
VW::LineFit::LineFit(const double lim, 
		     const int len) 
  : lambda_lim(lim), 
    min_conf(MIN_CONF_DEFAULT),
    lengththresh(len+2) // Must add 2 for the ends, which get removed
{
}

inline 
VW::LineFit::LineFit(const EdgeData& edat, 
		     std::list<LineSegFitted>& lines,
		     const double lim, const int len)
  : lambda_lim(lim),
    min_conf(MIN_CONF_DEFAULT),
    lengththresh(len+2) // Must add 2 for the ends, which get removed
{
  Compute(edat, lines);
}
  
inline
VW::LineFit::LineFit(const EdgeString *s, 
		     std::list<VW::LineSegFitted> &lines, 
		     const double lim, const int len)
  : lambda_lim(lim),
    min_conf(MIN_CONF_DEFAULT),
    lengththresh(len+2) // Must add 2 for the ends, which get removed
{
  Compute(s, lines);
}
  



#endif // _linefit_h_
