/** 
 * @file   integralimagesquared.h
 * @author BJ Tordoff
 * @date   03/04/2004
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
 * @class   VW::IntegralImageSquared
 * @ingroup ImageProcessing
 * @author  BJ Tordoff
 * @date    03/04/2004
 * @brief   Implementation of integral image squared.
 *
 * The class is templated on the internal storage type, which should be at
 * least as big as the data-type of the image being integrated. Usually you
 * should maintain signed-ness, such as:
 *    Image Type            Integral Image Type
 *    "unsigned char"  ->   "unsigned int" or "long unsigned int"
 *    "char"           ->   "int" or "long int"
 *    "unsigned int"   ->   "long unsigned int"
 *    "int"            ->   "long int"
 *    "float"          ->   "float" or "double"
 *    "double"         ->   "double" or "long double"
 */

#ifndef _integralimagesquared_h_
#define _integralimagesquared_h_

#include <VW/Image/imagemono.h>
#include <VW/Image/imageyuv.h>

namespace VW {

template <class DataType>
class IntegralImageSquared {

 public:
  IntegralImageSquared() : _alloced(false) {};

  ~IntegralImageSquared();

  /** Construct and compute from source image im.
   */
  template <class InType>
  IntegralImageSquared(const ImageMono<InType> &im);

  /** Construct and compute from Y channel of source image im.
   */
  template <class InType>
  IntegralImageSquared(const ImageYUV<InType> &im);

  /** Construct and compute from Y channel of source image im.
   */
  template <class InType>
    IntegralImageSquared(const ImageYUV<InType> &im,
			 const ROI& roi);

  /** Compute the integral image squared from source image im. */
  template <class InType>
    void Compute(const ImageMono<InType> &im);

   /** Compute the integral image squared from Y channel of source image im. */
  template <class InType>
    void Compute(const ImageYUV<InType> &im);

   /** Compute the integral image squared from Y channel of source image im. */
  template <class InType>
    void Compute(const ImageYUV<InType> &im,
		 const ROI& roi);

  /** Compute the sum-squared over a region (x0,y0)-(x1,y1) inclusive. */
  DataType GetRegionSumSquared(int x0, int y0,
			       int x1, int y1) const;

  unsigned int GetWidth() const { return _width; }
  unsigned int GetHeight() const { return _height; }

  operator bool () { return _alloced; }

private:
  // Funcs
  void _DestroyData();
  void _CreateData(unsigned int w, unsigned int h);

  // Data
  bool _alloced;
  DataType* _data;
  DataType** _rows;
  unsigned int _width, _height;
};









// INLINES

 template <class DataType> 
   inline IntegralImageSquared<DataType>::~IntegralImageSquared()
 {
   if (_alloced) {
     _rows--;
     delete[] _rows;
     delete[] _data;
   }
 }
   template <class DataType> template <class InType>
  inline IntegralImageSquared<DataType>::IntegralImageSquared(const ImageMono<InType> &im)
    : _alloced(false)
  {
    Compute(im);
  }
  
  template <class DataType> template <class InType>
  inline IntegralImageSquared<DataType>::IntegralImageSquared(const ImageYUV<InType> &im)
    : _alloced(false)
  {
    Compute(im);
  }
  
  template <class DataType> template <class InType>
  inline IntegralImageSquared<DataType>::IntegralImageSquared(const ImageYUV<InType> &im,
						const ROI& roi)
    : _alloced(false)
  {
    Compute(im,roi);
  }
  
  template <class DataType> template <class InType>
  inline void IntegralImageSquared<DataType>::Compute(const ImageYUV<InType> &im)
  {
    Compute(im,im.ROI());
  }
  
template <class DataType> inline DataType 
IntegralImageSquared<DataType>::GetRegionSumSquared(int x0, int y0,
					       int x1, int y1) const
{
  DataType result = _rows[y1][x1]; 
  result -= _rows[y1][x0-1]; 
  result += _rows[y0-1][x0-1];       
  result -= _rows[y0-1][x1]; 
  return result;
}


}; // end namespace VW;

#endif // _integralimagesquared_h_
