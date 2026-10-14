/** 
 * @file    linesegfitted.h
 * @author  ID Reid and BJ Tordoff
 * @date    02/08/2002
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
 * @class   VW::LineSegFitted
 * @ingroup ImageProcessing
 * @author  ID Reid and BJ Tordoff
 * @date    02/08/2002
 * @brief  A derivative of LineSeg2D that also includes goodness-of-fit data.
 *
 */

#ifndef _linesegfitted_h_
#define _linesegfitted_h_

#include <list>
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/lineseg2d.h>

#include "edgel.h"

namespace VW {

	/** Statistical information about a fitted line segment */
struct LineInfo {
	int n;                 // number of edgels
  double sumx, sumy;     // sum of x and y coords
  double sumxx, sumxy, sumyy;  // second moments (well, sort of)
  double R;              // score for potential merge of this seg with next          
};
	
	
class LineSegFitted : public LineSeg2D {
  
 public:

  LineSegFitted();
  LineSegFitted(Edgel *, Edgel *);
  ~LineSegFitted() { edgels.clear(); };

  /** Remove the end-edgels and update the line. 
   * @returns true if successful, false if this line segment is no longer valid.
   */
  bool CorrectEndpoints();

  // data
  std::list<VW::Edgel *> edgels;
 
  double  orient;   
  double  len;
  VW::Point2D midpoint;
  int status;       // For storing any user state info
  LineInfo info;
};

}; // End namespace VW
#endif
