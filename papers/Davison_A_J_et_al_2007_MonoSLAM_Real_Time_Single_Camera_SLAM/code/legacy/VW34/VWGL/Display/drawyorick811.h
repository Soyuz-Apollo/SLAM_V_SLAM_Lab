/** 
 * @file   drawyorick811.h
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

#ifndef _drawyorick811_h_
#define _drawyorick811_h_


namespace VW {

  class ThreeDToolGLCommon;

/**
 * @author    BJ Tordoff
 * @ingroup   GLTools
 * @date      06/02/2003
 * @brief     Draw a model of the Yorick811 active head.
 *
 * Draws a model of medium Yorick, adding two Sony VL500's to the platforms.
 * Note that angles are in DEGREES.
 */  
  void DrawYorick811(VW::ThreeDToolGLCommon& display,
		     const VW::Vector3D& position,
		     const VW::Quaternion& orientation,
		     const double pan=0, const double tilt=0, 
		     const double vergeleft=0, const double vergeright=0, 
		     const std::string& cameratype="VL500");

}; // end namespace VW

#endif
