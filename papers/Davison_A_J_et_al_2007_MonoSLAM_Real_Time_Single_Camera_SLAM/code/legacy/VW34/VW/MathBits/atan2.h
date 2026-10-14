/** @file atan2.h 
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


#ifndef _atan2_h_
#define _atan2_h_

namespace VW {

  /**
   * @author  DW Murray
   * @ingroup MathsStuff
   * @date    02/08/02
   * @brief   Fast approximation to arc-tangent over full range.
   *
   * This version of atan2 calculates an approximation to atan(y/x)
   * using a linear method. As x,y are passed seperately, their signs
   * are used to determine the quadrant of the result, allowing full
   * range recovery of the result.
   */
  double LinearAtan2(double y, double x);

}; // End namespace VW

#endif /* _atan2_h_ */
