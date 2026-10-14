/** 
 * @file    getsiftdescriptor.h
 * @author  BJ Tordoff
 * @date    09/11/2004
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
 * 
 */
/**
 * @class   VW::GetSIFTDescriptor
 * @ingroup ImageProcessing
 * @author  BJ Tordoff after ND Molton
 * @date    09/11/2004
 * @brief   Interest point description using the method of Lowe.
 *
 * #include <VW/Improc/getsiftdescriptor.h>
 *
 * Functions for adding a SIFT descriptor to a point set based on 
 * NDM's SIFT code.
 *
 * Input is a list of location as output by VW::Plessey or VW::DOGDetector and
 * the source image or image-pyramid. Output is a list of SIFTFeatures where
 * each location generates one SIFTFeature per dominant orientation (typically
 * either 1 or 2).
 *
 * NB: Currently turning off orientation speeds things up a lot, but the
 * weighting flag isn't used as yet.
 */

#include <vector>
#include <VW/Improc/siftfeature.h>


namespace VW {

  template <class T> class Pyramid;

  class GetSIFTDescriptor {
  public:
    GetSIFTDescriptor(const bool weighted=true,
		      const bool oriented=true);
    GetSIFTDescriptor(const std::vector<VW::CornerFeature>& locations,
		      const VW::Pyramid<float>& image,
		      std::vector<VW::SIFTFeature>& resulting_features,
		      const bool weighted=true,
		      const bool oriented=true);

   bool Compute(const std::vector<VW::CornerFeature>& locations,
		const VW::Pyramid<float>& pyramid,
		std::vector<VW::SIFTFeature>& resulting_features);

  private:
    unsigned int _patchsize; // Unused (yet)
    bool _weighted, _oriented;

  }; // end class GetSIFTDescriptor



}; // End namespace VW





inline VW::GetSIFTDescriptor::
GetSIFTDescriptor(const bool weighted,
		   const bool oriented)
  : _weighted(weighted),
    _oriented(oriented)
{
}

inline VW::GetSIFTDescriptor::
GetSIFTDescriptor(const std::vector<VW::CornerFeature>& locations,
		   const VW::Pyramid<float>& pyramid,
		   std::vector<VW::SIFTFeature>& resulting_features,
		   const bool weighted,
		   const bool oriented)
  : _weighted(weighted),
    _oriented(oriented)
{
  Compute(locations,pyramid,resulting_features);
}
