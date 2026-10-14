/** 
 * @file   subsample.h
 * @author ID Reid and BJ Tordoff
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
 * @class   VW::Subsample
 * @ingroup ImageProcessing
 * @author  ID Reid and BJ Tordoff
 * @date    02/08/2002
 * @brief   Subsample an image by an integer factor.
 */


#ifndef _subsample_h_
#define _subsample_h_

#include "assert.h"

namespace VW {

class Subsample {

 public:

  Subsample(int f=2) : factor(f) { assert(f>0); }


  /** Subsample an image by the factor $f$ in-place.
   */
  template <class ImType>
  Subsample(ImType &im, int f=2) 
    : factor(f) 
  { 
    assert(f>0); 
    Compute(im); 
  }

  /** Subsample an image by the factor $f$ storing the result.
   */
  template <class ImType>
  Subsample(const ImType &im, ImType &result, int f=2) 
    : factor(f) 
  { 
    assert(f>0); 
    Compute(im, result); 
  }

  template <class ImType>
  void Compute(ImType &im);

  template <class ImType>
  void Compute(const ImType &im, ImType &result);

  operator bool () { return true; }

  void SetFactor(int f) { assert(f>0); factor=f; }

 private:

  int factor;
};

}; // end namespace VW

#endif // _subsample_h_
