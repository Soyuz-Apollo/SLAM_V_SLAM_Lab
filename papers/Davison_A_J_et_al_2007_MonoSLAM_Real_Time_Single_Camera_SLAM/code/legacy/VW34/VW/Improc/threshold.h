/** 
 * @file   threshold.h
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
/** 
 * @class   VW::Threshold
 * @ingroup ImageProcessing
 * @author  ID Reid
 * @date    02/08/2002
 * @brief   
 */


#ifndef _threshold_h_
#define _threshold_h_

namespace VW {

template <class Type>
class Threshold {

 public:
  Threshold(const Type thresh=1, const Type l=0, const Type u=1) 
    : threshold(thresh), lower(l), upper(u) {};
  Threshold(const ImageMono<Type> &im, ImageMono<Type> &result, const Type val) 
    : threshold(val), lower(0), upper(1) { 
    Compute(im, result);
  }
  Threshold(const ImageMono<Type> &im, ImageMono<Type> &result, 
	    const Type val, const Type l, const Type u) : threshold(val), lower(l), upper(u) { 
    Compute(im, result);
  }
  Threshold(ImageMono<Type> &im, const Type val, const Type l, const Type u) 
    : threshold(val), lower(l), upper(u) { 
    Compute(im);
  }
  Threshold(ImageMono<Type> &im, const Type val) 
    : threshold(val), lower(0), upper(1) { 
    Compute(im);
  }


  void Compute(const ImageMono<Type> &im, ImageMono<Type> &result);
  void Compute(ImageMono<Type> &im);

  operator bool () { return true; }

  void SetThreshold(Type val) { threshold = val; }
  void SetLower(Type val) { lower = val; }
  void SetUpper(Type val) { upper = val; }

private:

 Type threshold;
 Type lower, upper;

};

}; // end namespace VW;

#endif // _threshold_h_
