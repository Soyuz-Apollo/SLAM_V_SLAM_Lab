/** 
 * @file   improc_misc.h
 * @author JGH Knight
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


#ifndef _improc_misc_h_
#define _improc_misc_h_

namespace VW {

/** Part of the VW::Misc namespace
 *
 * Additions specifically related to image processing
 */

namespace Misc {

  template <class T>
  double CorrelationScoreNMSD (T* ppatch,
			       unsigned patch_width, unsigned patch_height,
			       T* pimage,
			       unsigned image_width, unsigned image_height,
			       int x, int y);

  template <class T>
  double CorrelationScoreAJD (T* ppatch,
			      unsigned patch_width, unsigned patch_height,
			      T* pimage,
			      unsigned image_width, unsigned image_height,
			      int x, int y);

}; // end of namespace Misc

}; // end of namespace VW


#endif // #ifndef _improc_misc_h_
