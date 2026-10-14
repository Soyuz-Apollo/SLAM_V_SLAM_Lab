/** 
 * @file   imagediff.h
 * @author ID Reid
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


#ifndef _imagediff_h_
#define _imagediff_h_

namespace VW {
/** 
 * @class   VW::ImageDiff
 * @ingroup ImageProcessing
 * @author  ID Reid
 * @date    02/08/2002
 * @brief   Calculate the difference between two images.
 *
 * A simple class to comute either the absolute or signed difference between
 * two images.
 */
class ImageDiff {

 public:
  ImageDiff() : success(true) {}

  template <class ImType>
  ImageDiff(const ImType &im1, const ImType &im2, ImType &diff, 
	    bool absolute=true);

  /** Compute the absolute difference abs(im1 - im2) (ie. order
   * doesn't matter).
   */
  template <class ImType>
  void ComputeAbsolute(const ImType &im1, const ImType &im2, ImType &diff);

  /** Compute the signed image difference (im1 - im2) (order does matter,
   * and bad things will happen if your images are an unsigned type).
   */
  template <class ImType>
  void ComputeSigned(const ImType &im1, const ImType &im2, ImType &diff);

  operator bool() const;

 private:

  bool success;

};  





  // INLINES

  template <class ImType> inline 
  ImageDiff::ImageDiff(const ImType &im1, 
		       const ImType &im2, 
		       ImType &diff,
		       bool absolute) 
    : success(true) 
  { 
    if (absolute) {
      ComputeAbsolute(im1, im2, diff); 
    } else {
      ComputeSigned(im1, im2, diff); 
    }
  }

  inline ImageDiff::operator bool() const
  { 
    return success; 
  }

}; // end namespace VW

#endif // _imagediff_h_
