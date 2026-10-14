/** @file moments.h
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
 *
 * @class   VW::Moments 
 * @ingroup Improc
 * @author  ID Reid
 * @date    18/12/04
 * @brief   Compute moments of a monochrome or binary image
 *
 * Determine the 0th, 1st and 2nd moments of a binary or monochrome image
 * and provide various access methods to compute orientation, etc.
 *
 */


#ifndef _moments_h_
#define _moments_h_

#include <math.h>
#include<VW/GeomObjects/point2d.h>

namespace VW {

class Moments {

 public:

  // constructors
  template <class ImType>
    Moments(const ImageMono<ImType>& im);
  template <class ImType>
    Moments(const ImageMono<ImType>& im, const ImageMono<float>& weight);
  template <class ImType>
    Moments(const ImageMono<ImType>& im, const ROI roi);
  template <class ImType>
    Moments(const ImageMono<ImType> &im, const ROI roi, 
	    const ImageMono<float>& weight);

  // compute methods
  template <class ImType>
    void Compute(const ImageMono<ImType>& im, const ROI roi);
  template <class ImType>
    void Compute(const ImageMono<ImType>& im, const ROI roi,
		 const ImageMono<float>& weights);

  // accessors
  VW::Point2D GetCentreOfMass(void) { return VW::Point2D(sumx/sum, sumy/sum); }
  double GetOrientation() { return theta; }
  double GetCentreOfMassX() { return sumx/sum; }
  double GetCentreOfMassY() { return sumx/sum; }
  double GetMass() { return sum; }

 protected:
  double sum, sumx, sumy, sumxx, sumxy, sumyy;
  double a, b, c;
  double theta;
};


template <class ImType>
inline Moments::Moments(const ImageMono<ImType>& im) {
  ROI roi(0, 0, im.GetWidth(), im.GetHeight());
  Compute(im, roi);
};

template <class ImType>
inline Moments::Moments(const ImageMono<ImType>& im, const ROI roi) {
  Compute(im, roi);
};

template <class ImType>
inline Moments::Moments(const ImageMono<ImType>& im, 
			const ImageMono<float>& weights) {
  ROI roi(0, 0, im.GetWidth(), im.GetHeight());
  Compute(im, roi, weights);
};

template <class ImType>
inline Moments::Moments(const ImageMono<ImType>& im, 
			const ROI roi,
			const ImageMono<float>& weights) {
  Compute(im, roi, weights);
};

};

#endif // _moments_h_
