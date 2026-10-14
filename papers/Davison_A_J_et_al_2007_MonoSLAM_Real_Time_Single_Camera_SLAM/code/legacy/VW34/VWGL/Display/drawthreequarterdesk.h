/** 
 * @file   drawthreequarterdesk.h
 * @author BJ Tordoff
 * @date   06/02/2003
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

#ifndef _drawthreequarterdesk_h_
#define _drawthreequarterdesk_h_

// Local includes
#include "threedtoolglcommon.h"

namespace VW {

/**
 * @author    BJ Tordoff
 * @ingroup   GLTools
 * @date      06/02/2003
 * @brief   Draw a model of a three-quarter corner desk.
 *
 * Draws a model of the three-quarter desks we have in the lab. 
 * By default the origin is on 
 * the desktop, positioned at the (extrapolated) internal corner of 
 * the 'L' shape. This can be changed to the corner leg if desired.
 */
  void DrawThreeQuarterDesk(VW::ThreeDToolGLCommon& display,
			    const VW::Vector3D& position,
			    const VW::Quaternion& orientation,
			    bool origin_on_desk = true);


  // Some constants (in metres)
  static const double ThreeQuarterDeskHeight(0.725);
  static const double ThreeQuarterDeskSideLength(1.8);

}; // end namespace VW

#endif
