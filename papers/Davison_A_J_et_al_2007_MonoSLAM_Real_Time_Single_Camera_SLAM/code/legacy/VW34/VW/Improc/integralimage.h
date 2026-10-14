/** 
 * @file   integralimage.h
 * @author ID Reid
 * @date   06/08/2002
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
 * @class   VW::IntegralImage
 * @ingroup ImageProcessing
 * @author  ID Reid
 * @date    06/08/2002
 * @brief   Implementation of integral image as described in Viola's 
 *          CVPR2001 paper on face detection via boosting.
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
 *    
 */

#ifndef _integralimage_h_
#define _integralimage_h_

#include <VW/Image/imagemono.h>
#include <VW/Image/imageyuv.h>

namespace VW {

template <class DataType>
class IntegralImage {

 public:
  IntegralImage() : _alloced(false) {};

  ~IntegralImage();

  /** Construct and compute from source image im.
   */
  template <class InType>
  IntegralImage(const ImageMono<InType> &im);

  /** Construct and compute from the Y channel of source image im.
   */
  template <class InType>
  IntegralImage(const ImageYUV<InType> &im);

  /** Construct and compute from the Y channel of source image im in the region specified by ROI.
   */
  template <class InType>
  IntegralImage(const ImageYUV<InType> &im,
		const ROI& roi);

  /** Compute the integral image from source image im. */
  template <class InType>
    void Compute(const ImageMono<InType> &im);

  /** Compute the integral image from the Y channel of source image im. */
  template <class InType>
  void Compute(const ImageYUV<InType> &im);

  /** Compute the integral image from the Y channel of source image im in the region specified by ROI. */
  template <class InType>
  void Compute(const ImageYUV<InType> &im,
	       const ROI& roi);

  /** Compute the sum over a region (x0,y0)-(x1,y1) inclusive. */
  DataType GetRegionSum(int x0, int y0,
			int x1, int y1) const;

  unsigned int GetWidth() const;
  unsigned int GetHeight() const;

  /** Get a pointer to the start of a row. Note that the -1th row and column are
   * zeros to make the region-summing faster, so to skip a row, add GetWidth()+1.
   */
  DataType* operator[](int row);

  operator bool () const;

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
   inline IntegralImage<DataType>::~IntegralImage()
 {
   if (_alloced) {
     _rows--;
     delete[] _rows;
     delete[] _data;
   }
 }
 
 template <class DataType> template <class InType>
   inline IntegralImage<DataType>::IntegralImage(const ImageMono<InType> &im)
   : _alloced(false)
 {
   Compute(im);
 }
  
  template <class DataType> template <class InType>
  inline IntegralImage<DataType>::IntegralImage(const ImageYUV<InType> &im,
						const ROI& roi)
    : _alloced(false)
  {
    Compute(im,roi);
  }
  
  template <class DataType> template <class InType>
  inline void IntegralImage<DataType>::Compute(const ImageYUV<InType> &im)
  {
    Compute(im,im.ROI());
  }
  
  template <class DataType> inline unsigned int 
  IntegralImage<DataType>::GetWidth() const 
  { 
    return _width; 
  }

  template <class DataType> inline unsigned int 
  IntegralImage<DataType>::GetHeight() const 
  { 
    return _height; 
  }

  template <class DataType> inline DataType 
  IntegralImage<DataType>::GetRegionSum(int x0, int y0,
					int x1, int y1) const
  {
    DataType result = _rows[y1][x1]; 
    result -= _rows[y1][x0-1]; 
    result += _rows[y0-1][x0-1];       
    result -= _rows[y0-1][x1]; 
    return result;
  }
  
  template <class DataType> inline DataType* 
  IntegralImage<DataType>::operator[](int row)
  {
    return _rows[row];
  }
  
  template <class DataType> inline 
  IntegralImage<DataType>::operator bool ()  const
  { 
    return _alloced; 
  }

}; // end namespace VW;

#endif // _integralimage_h_
