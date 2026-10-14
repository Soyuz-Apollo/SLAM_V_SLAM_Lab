/** 
 * @file   drawfirei.h
 * @author BJ Tordoff
 * @date   02/02/2003
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

#ifndef _drawfirei_h_
#define _drawfirei_h_

// Local includes
#include "threedtoolglcommon.h"

namespace VW {

  /**
   * @author    BJ Tordoff
   * @ingroup   GLTools
   * @date      02/02/2003
   * @brief   Draw a model of the Unibrain Fire-I camera.
   *
   * Draws a model of the camera with origin at approximately the optic centre
   * of the camera. Assumes that one GL unit is one metre.
   */
  void DrawFireI(VW::ThreeDToolGLCommon& display,
		 const VW::Vector3D& position,
		 const VW::Quaternion& orientation);

}; // end namespace VW

#endif
