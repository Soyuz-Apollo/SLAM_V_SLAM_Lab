/** 
 * @file   interpolationmethod.h
 * @author BJ Tordoff
 * @date   05/07/2004
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
 * @ingroup ImageProcessing
 * @author  BJ Tordoff
 * @date    05/07/2004
 * @brief   method type for scaling, warping etc.
 */

#ifndef _interpolationmethod_h_
#define _interpolationmethod_h_

namespace VW {

  enum InterpolationMethod {
    NearestNeighbour, // Quick and dirty
    Bilinear,
    Biquadratic,
    Bicubic           // Sexy but slow
  };

};

#endif
