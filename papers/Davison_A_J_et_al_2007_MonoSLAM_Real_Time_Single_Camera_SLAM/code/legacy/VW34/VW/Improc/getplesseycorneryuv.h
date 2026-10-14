/** 
 * @file    getplesseycorneryuv.h
 * @author  BJ Tordoff
 * @date    25/11/2004
 *
 *              (c) Copyright University of Cambridge
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
 * @class   VW::GetPlesseyCornerYUV
 * @ingroup ImageProcessing
 * @author  BJ Tordoff
 * @date    25/11/2004
 * @brief   Old PlesseyCorner code
 *
 * Example usage:
 *  std::vector<VW::PlesseyCornerYUV> corners;
 *  {
 *    std::vector<VW::CornerFeature> locations;           // temporary storage
 *    VW::Plessey(image,locations);                       // find locations
 *    VW::GetPlesseyCornerYUV(locations,image,7,corners); // add patches
 *  }
 *
 */

#include <vector>
#include "plesseycorneryuv.h"

namespace VW {

  // Forward declare some stuff
  template <class T> class PyramidYUV;
  template <class T> class ImageYUV;


  class GetPlesseyCornerYUV {
  public:
//    GetPlesseyCorner(const std::vector<VW::CornerFeature>& locations,
// 		      const VW::Pyramid<float>& pyramid,
// 		      const unsigned int patch_size,
// 		      std::vector<VW::PlesseyCorner>& resulting_features);

   GetPlesseyCornerYUV(const std::vector<VW::CornerFeature>& locations,
		      const VW::ImageYUV<unsigned char>& image,
		      const unsigned int patch_size,
		      std::vector<VW::PlesseyCornerYUV>& resulting_features);

  private:
 
  }; // end class GetPlesseyCornerYUV



}; // End namespace VW
