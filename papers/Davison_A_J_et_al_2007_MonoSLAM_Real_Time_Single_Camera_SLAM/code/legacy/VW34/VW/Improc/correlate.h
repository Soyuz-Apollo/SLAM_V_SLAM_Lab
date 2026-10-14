/** 
 * @file   correlate.h
 * @author BJ Tordoff
 * @date   05/04/2004
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
 * @class   VW::Correlate
 * @author BJ Tordoff
 * @date   05/04/2004
 * @ Brief  A class for computing the zero-normalised cross-correlation (ZNCC) between a 
 * patch and either another patch or an image.
 */

#ifndef _correlate_h_
#define _correlate_h_

#include <VW/image.h>
#include <VW/Improc/imagepatch.h>
#include <VW/Improc/imagepatchyuv.h>

namespace VW {




class Correlate {

public:
  /** Compute the zero-normalised correlation between two image patches. */
  Correlate(const VW::ImagePatch& p1, 
	    const VW::ImagePatch& p2, 
	    float& result)
  {
    Compute(p1,p2,result);
  }

  /** Compute the zero-normalised correlation between two YUV image patches. */
  Correlate(const VW::ImagePatchYUV& p1, 
	    const VW::ImagePatchYUV& p2, 
	    float& result)
  {
    Compute(p1,p2,result);
  }

  /** Compute the zero-normalised correlation between an image patch
   * and every pixel within the ROI of an image, storing the result in
   * a new image. 
   */
  template<class Type>
  Correlate(const VW::ImagePatch& patch, 
	    const VW::ImageMono<Type>& image, 
	    VW::ImageMono<float>& result)
  {
    Compute(patch,image,result);
  }


  /** Compute the zero-normalised correlation between an image patch
   * and every pixel within the ROI of an image, storing the result in
   * a new patch. 
   */
  template<class Type>
  Correlate(const VW::ImagePatch& patch, 
	    const VW::ImageMono<Type>& image, 
	    VW::ImagePatch& result)
  {
    Compute(patch,image,result);
  }

  /** Compute the zero-normalised correlation between an image patch
   * and pixels in the specified ROI, storing the result in
   * a new patch. 
   */
  template<class Type>
  Correlate(const VW::ImagePatch& patch, 
	    const VW::ImageMono<Type>& image,
	    const VW::ROI& roi,
	    VW::ImagePatch& result)
  {
    Compute(patch,image,roi,result);
  }


  /** Compute the zero-normalised correlation between an image patch
   * and pixels in the specified ROI using only the Y channel of the YUV 
   * image, storing the result in a new patch. 
   */
  template<class Type>
  Correlate(const VW::ImagePatch& patch, 
	    const VW::ImageYUV<Type>& image,
	    const VW::ROI& roi,
	    VW::ImagePatch& result)
  {
    Compute(patch,image,roi,result);
  }



  // Equivalent compute functions


  void Compute(const VW::ImagePatch& p1, 
	       const VW::ImagePatch& p2, 
	       float& result) const;

  void Compute(const VW::ImagePatchYUV& p1, 
	       const VW::ImagePatchYUV& p2, 
	       float& result) const;

  template<class Type>
  void Compute(const VW::ImagePatch& patch, 
	       const VW::ImageMono<Type>& image, 
	       VW::ImageMono<float>& result) const;

  template<class Type>
  void Compute(const VW::ImagePatch& patch, 
	       const VW::ImageMono<Type>& image, 
	       VW::ImagePatch& result) const
  {
    Compute(patch,image,image.GetROI(),result);
  }

  template<class Type>
  void Compute(const VW::ImagePatch& patch, 
	       const VW::ImageMono<Type>& image, 
	       const VW::ROI& roi,
	       VW::ImagePatch& result) const;

  template<class Type>
  void Compute(const VW::ImagePatch& patch, 
	       const VW::ImageYUV<Type>& image, 
	       const VW::ROI& roi,
	       VW::ImagePatch& result) const;
};

}; // End namespace VW



// INLINES

inline void 
VW::Correlate::Compute(const VW::ImagePatch& p1, 
		       const VW::ImagePatch& p2, 
		       float& result) const
{
  result = p1.GetZNCC(p2);
}

inline void 
VW::Correlate::Compute(const VW::ImagePatchYUV& p1, 
		       const VW::ImagePatchYUV& p2, 
		       float& result) const
{
  result = p1.GetZNCC(p2);
}

#endif
