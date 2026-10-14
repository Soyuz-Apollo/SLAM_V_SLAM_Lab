/** 
 * @file   pyramidcorrelate.h
 * @author ID Reid, JGH Knight
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
 * @class   PyramidCorrelate
 * @ingroup ImageProcessing
 * @author  ID Reid, JGH Knight
 * @date    02/08/2002
 *
 * Takes two ImageMonos and calculates the translation, in image coordinates,
 * from one to the other using simple correlation matching in each of the
 * pyramid images calculated using the class Pyramid.  Obviously only works
 * well for either pure rotational or small translational motion.
 */


#ifndef _pyramidcorrelate_h
#define _pyramidcorrelate_h

#include <VW/image.h>
#include "pyramid.h"

namespace VW {


class PyramidCorrelate
{
 public:
  // Constructors, Destructors
  template <class T>
  PyramidCorrelate (const ImageMono<T> &image1, const ImageMono<T> &image2);
  ~PyramidCorrelate ();

  // Computation
  bool Compute ();

  // Data access
  void SetCorrelThresh (double cth) {_cthresh = cth;}
  void Gett (double& x, double& y) const {x = _x; y = _y;}
  double GetX () const {return _x;}
  double GetY () const {return _y;}

 protected:
  Pyramid<float>* _ppyr1;
  Pyramid<float>* _ppyr2;
  double _x, _y;
  double _cthresh;
};


}; // end of namespace VW


#endif
