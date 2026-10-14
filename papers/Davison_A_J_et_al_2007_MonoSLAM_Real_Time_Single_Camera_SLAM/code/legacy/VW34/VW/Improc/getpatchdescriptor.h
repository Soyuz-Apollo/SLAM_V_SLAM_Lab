/** 
 * @file    getpatchdescriptor.h
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
 * @class   VW::GetPatchDescriptor
 * @ingroup ImageProcessing
 * @author  BJ Tordoff
 * @date    25/11/2004
 * @brief   Describe image features by the surrounding patch (with 
 * normalisation).
 *
 * #include <VW/Improc/getpatchdescriptor.h>
 * 
 * The normalisation can optionally be weighted, giving a distribution which
 * is still zero mean and unit variance, but where the central values are
 * given more prominence (at the cost of some initialisation time).
 */

#include <vector>
#include "patchfeature.h"

namespace VW {

  // Forward declare some stuff
  template <class T> class Pyramid;



  class GetPatchDescriptor {
  public:
     /** Construct and set params. Note that weighting and orienting the
      * patches improves matching but makes the aquistion step take longer.
      * @param patch_size the size of the (square) patch - must be odd
      * @param weighted should the patch be centrally weighted?
      * @param weighted should the patch be align to the dominent orientation?
      */
   GetPatchDescriptor(const unsigned int patch_size=7,
		      const bool weighted=false,
		      const bool oriented=false);

    /** Calculate a set of PatchFeatures from a set of locations. Each
     * location can give rise to more than one feature since it may
     * have ambiguous dominant direction. 
     * @param patch_size the size of the (square) patch - must be odd
     */
    GetPatchDescriptor(const std::vector<VW::CornerFeature>& locations,
		       const VW::Pyramid<float>& pyramid,
		       std::vector<VW::PatchFeature>& resulting_features,
		       const unsigned int patch_size=7,
		       const bool weighted=false,
		       const bool oriented=false);
    
    bool Compute(const std::vector<VW::CornerFeature>& locations,
		 const VW::Pyramid<float>& pyramid,
		 std::vector<VW::PatchFeature>& resulting_features) const;
    
  private:
    unsigned int _patchsize;
    bool _weighted, _oriented;
 
  }; // end class GetPatchDescriptor



}; // End namespace VW





inline VW::GetPatchDescriptor::
GetPatchDescriptor(const unsigned int patch_size,
		   const bool weighted,
		   const bool oriented)
  : _patchsize(patch_size),
    _weighted(weighted),
    _oriented(oriented)
{
}

inline VW::GetPatchDescriptor::
GetPatchDescriptor(const std::vector<VW::CornerFeature>& locations,
		   const VW::Pyramid<float>& pyramid,
		   std::vector<VW::PatchFeature>& resulting_features,
		   const unsigned int patch_size,
		   const bool weighted,
		   const bool oriented)
  : _patchsize(patch_size),
    _weighted(weighted),
    _oriented(oriented)
{
  Compute(locations,pyramid,resulting_features);
}
