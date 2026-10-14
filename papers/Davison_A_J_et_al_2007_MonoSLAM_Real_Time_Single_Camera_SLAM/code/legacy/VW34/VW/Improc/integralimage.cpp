/** 
 * @file   integralimage.cpp
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


#include <cstdlib>
#include <cmath>

#include <VW/Image/imagemono.h>
#include "integralimage.h"

using namespace VW;
using namespace std;

template <class DataType>
void IntegralImage<DataType>::_DestroyData()
{
  _width = 0;
  _height = 0;
  delete[] _data;
  delete[] _rows;
  _alloced = false;
}

template <class DataType>
void IntegralImage<DataType>::_CreateData(unsigned int w, unsigned int h)
{
  _width = w;
  _height = h;
  _data = new DataType[(w+1)*(h+1)];
  _rows = new DataType*[h+1];
  _rows++;
  _rows[-1] = _data+1;
  for (int r=0;r<int(h);r++) {
    _rows[r] = _rows[r-1] + (w+1);
  }
  _alloced = true;
}

template <class DataType> template <class InType>
void IntegralImage<DataType>::Compute(const ImageMono<InType> &im)
{
  int col, row; 
  int r0, c0, rl, cl;
  DataType sumr;

  if (_alloced) {
    if ((_width != im.GetWidth()) || (_height != im.GetHeight())) {
      _DestroyData();
      _CreateData(im.GetWidth(), im.GetHeight());
    }
  } else {
    _CreateData(im.GetWidth(), im.GetHeight());
  }


  im.ROI().Get(c0,r0,cl,rl);

  typename ImageMono<InType>::PixelPtr  im_ptr;
  DataType *result_ptr, *result_p_ptr;

  // When padding, insert a zero row and column
  result_ptr = _rows[r0-1] + c0;
  for (col=c0; col<cl; col++) {
    *result_ptr++ = 0;
  }
  result_ptr = _rows[r0-1] + c0-1;
  for (row=r0; row<(rl+1); row++) {
      *result_ptr = 0; 
      result_ptr+=(_width+1);
  }
  
  // first row
  im_ptr = im.GetROIRowStart(r0);
  result_ptr = _rows[r0] + c0;
  for (col=c0,sumr=0; col<cl; col++) {
    sumr += im_ptr++->y;
    *result_ptr++ = sumr;
  }

  // rest of image
  for (row=r0+1; row<rl; row++) {
    result_p_ptr = _rows[row-1] + c0;
    result_ptr = _rows[row] + c0;
    im_ptr = im.GetROIRowStart(row);
    for (col=c0,sumr=0; col<cl; col++) {
      sumr += im_ptr++->y;
      *result_ptr++ = (sumr + *result_p_ptr++);
    }
  }

  return;
}

// YUV version pretty much same as Mono - perhaps should be combined somehow?
template <class DataType> template <class InType>
void IntegralImage<DataType>::Compute(const ImageYUV<InType> &im,
				      const ROI& roi)
{
  int col, row; 
  int r0, c0, rl, cl;
  DataType sumr;

  if (_alloced) {
    if ((_width != im.GetWidth()) || (_height != im.GetHeight())) {
      _DestroyData();
      _CreateData(im.GetWidth(), im.GetHeight());
    }
  } else {
    _CreateData(im.GetWidth(), im.GetHeight());
  }


  roi.Get(c0,r0,cl,rl);

  typename ImageYUV<InType>::PixelPtr  im_ptr;
  DataType *result_ptr, *result_p_ptr;

  // When padding, insert a zero row and column
  result_ptr = _rows[r0-1] + c0;
  for (col=c0; col<cl; col++) {
    *result_ptr++ = 0;
  }
  result_ptr = _rows[r0-1] + c0-1;
  for (row=r0; row<(rl+1); row++) {
      *result_ptr = 0; 
      result_ptr+=(_width+1);
  }
  
  // first row
  im_ptr = im.GetRowStart(r0) + c0;
  result_ptr = _rows[r0] + c0;
  for (col=c0,sumr=0; col<cl; col++) {
    sumr += im_ptr++->y;
    *result_ptr++ = sumr;
  }

  // rest of image
  for (row=r0+1; row<rl; row++) {
    result_p_ptr = _rows[row-1] + c0;
    result_ptr = _rows[row] + c0;
    im_ptr = im.GetRowStart(row) + c0;
    for (col=c0,sumr=0; col<cl; col++) {
      sumr += im_ptr++->y;
      *result_ptr++ = (sumr + *result_p_ptr++);
    }
  }

  return;
}


#ifdef INSTANTIATE
#  undef INSTANTIATE
#endif
# define INSTANTIATE(M, N) \
  template void IntegralImage<M>::Compute<N>(const ImageMono<N> &); \
  template void IntegralImage<M>::Compute<N>(const ImageYUV<N> &);

// Storage type (first param) should be at least as big as the input type
INSTANTIATE(unsigned int,      unsigned char); 
INSTANTIATE(long unsigned int, unsigned char); 
INSTANTIATE(double,            unsigned char);

INSTANTIATE(int,               char); 
INSTANTIATE(long int,          char); 
INSTANTIATE(double,            char);

INSTANTIATE(float,             float); 
INSTANTIATE(double,            float); 

INSTANTIATE(double,            double); 
INSTANTIATE(long double,       double); 

#undef INSTANTIATE
