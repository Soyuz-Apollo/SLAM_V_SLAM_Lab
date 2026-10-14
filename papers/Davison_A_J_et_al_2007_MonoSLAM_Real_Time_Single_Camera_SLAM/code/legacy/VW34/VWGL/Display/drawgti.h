/** 
 * @file   drawgti.h
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

#ifndef _drawgti_h_
#define _drawgti_h_

#include <string>


namespace VW {

  class ThreeDToolGLCommon;
    class Point3D; typedef Point3D Vector3D;
  class Quaternion;

/**
 * @author    BJ Tordoff
 * @ingroup   GLTools
 * @date      02/02/2003
 * @brief   Draw a model of the GTI mobile robot.
 *
 * Draws a model of the GTI "mobile" (well, in theory it is) robot. The origin
 * is the position on the floor below the point of attachment of the Yorick8-11
 * head.
 */  
  void DrawGTI(VW::ThreeDToolGLCommon& display,
		     const VW::Vector3D& position,
		     const VW::Quaternion& orientation);

/**
 * @author    BJ Tordoff
 * @ingroup   GLTools
 * @date      02/08/02
 * @brief   Draw a model of the GTI mobile robot.
 *
 * Draws a model of the GTI "mobile" (well, in theory it is) robot. The origin
 * is the position on the floor below the point of attachment of the Yorick8-11
 * head.
 */  
  void DrawGTIWithYorick(VW::ThreeDToolGLCommon& display,
			 const VW::Vector3D& position,
			 const VW::Quaternion& orientation,
			 const double yorickpan=0, 
			 const double yoricktilt=0, 
			 const double yorickvergeleft=0, 
			 const double yorickvergeright=0, 
			 const std::string& yorickcameratype="VL500");

}; // end namespace VW

#endif
