/** 
 * @file   distortimage.h
 * @author BJ Tordoff after E Hayman
 * @date   19/08/2003
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


#ifndef _distortimage_h_
#define _distortimage_h_

// VW stuff
#include <VW/image.h>

namespace VW {

  class Point2D;

  namespace RadialDistortion {

    void DistortImage(const double kappa,
		      const VW::ImageMono<unsigned char>& input_image,
		      VW::ImageMono<unsigned char>& result,
		      const VW::Point2D& distortion_centre);

    void DistortImage(const double kappa,
		      const VW::ImageYUV<unsigned char>& input_image,
		      VW::ImageYUV<unsigned char>& result,
		      const VW::Point2D& distortion_centre);

    void UndistortImage(const double kappa,
			const VW::ImageMono<unsigned char>& input_image,
			VW::ImageMono<unsigned char>& result,
			const VW::Point2D& distortion_centre);


  }; // End namespace RadialDistortion
}; // End namespace VW





#endif
