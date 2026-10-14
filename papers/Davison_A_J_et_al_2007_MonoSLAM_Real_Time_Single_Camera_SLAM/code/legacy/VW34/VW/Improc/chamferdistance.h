/** 
 * @file   chamferdistance.h
 * @author BJ Tordoff
 * @date   26/06/2003
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
 * @class   VW::ChamferDistance
 * @ingroup ImageProcessing
 * @author  BJ Tordoff
 * @date    26/06/2003
 * @brief   Class for computing a linear distance transfor (aka chamfer distance) image.
 *
 * The distance transform uses a binary image of "features" from which the distance is
 * measured. There are several ways in which it can be used, the easiest of which is
 * to provide a binary image with features having a "true" value and everything else 
 * "false". Typically it is used on a thresholded silhouette or on the output of the
 * Canny edge detector (see VW::Canny), and compute functions for these types of data
 * are also provided.
 */

#ifndef _chamferdistance_h_
#define _chamferdistance_h_

#include <VW/Image/imagemono.h>
#include "edgel.h"

namespace VW {


class ChamferDistance {
  
 public:

  ChamferDistance();
  ChamferDistance(const VW::EdgeData &edges,
		  VW::ImageMono<unsigned int> &result);
  ChamferDistance(const VW::ImageMono<unsigned char> &inputimage,
		  VW::ImageMono<unsigned int> &result,
		  unsigned char threshold = 127);
  ChamferDistance(const VW::ImageMono<bool> &inputimage,
		  VW::ImageMono<unsigned int> &result);


/**
 * Compute the linear distance transform from the edges specified
 * in "edges". Note that as edges don't specify an image size, the result image
 * should be resized to the desired size before use.
 */
void Compute(const VW::EdgeData &edges,
	     VW::ImageMono<unsigned int> &result);

/**
 * Compute the linear distance transform from a mono image
 * where any pixel with a value above the threshold is treated as a "feature".
 */
void Compute(const VW::ImageMono<unsigned char> &inputimage,
	     VW::ImageMono<unsigned int> &result,
	     unsigned char threshold = 127);

/**
 * Compute the linear distance transform from pixels of value
 * 255.
 */
void Compute(const VW::ImageMono<bool> &inputimage,
		     VW::ImageMono<unsigned int> &result);


 private:


};

}; // end namespace VW




























// Inlines

inline VW::ChamferDistance::ChamferDistance(const VW::EdgeData &edges,
					    VW::ImageMono<unsigned int> &result)
{
  Compute(edges,result);
}

inline VW::ChamferDistance::ChamferDistance(const VW::ImageMono<unsigned char> &inputimage,
					    VW::ImageMono<unsigned int> &result,
					    unsigned char threshold)
{
  Compute(inputimage,result,threshold);
}

inline VW::ChamferDistance::ChamferDistance(const VW::ImageMono<bool> &inputimage,
					    VW::ImageMono<unsigned int> &result)
{
  Compute(inputimage,result);
}


#endif /* _chamferdistance_h_ */
