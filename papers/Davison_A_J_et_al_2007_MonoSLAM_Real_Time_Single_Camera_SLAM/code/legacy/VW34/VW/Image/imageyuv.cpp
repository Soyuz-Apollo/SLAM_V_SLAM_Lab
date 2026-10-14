/** @file imageyuv.cpp
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


#include <string>

#include "imageyuv.h"
#include "imageio.h"

using namespace std;
using namespace VW;



template <class Type>
bool VW::ImageYUV<Type>::ReadImage(const string& filename)
{
  return VW::ReadImage(filename,*this);
}

template <class Type>
bool VW::ImageYUV<Type>::WriteImage(const string& filename) const
{
  return VW::WriteImage(filename,*this);
}



template <class T> VW::PixelYUV<double> 
VW::ImageYUV<T>::GetInterpolatedPixel(double x,double y) const
{
  // Find colour by bi-linear interpolation
  //cerr << "("<<x<<","<<y<<")"<<endl;
  int x0,y0;
  double xfrac,yfrac;
  x0=int(x);
  y0=int(y);
  xfrac=x-x0;
  yfrac=y-y0;

  double mm_frac = (1-xfrac) * (1-yfrac);
  double mp_frac = (1-xfrac) * (yfrac);
  double pm_frac = (xfrac)   * (1-yfrac);
  double pp_frac = (xfrac)   * (yfrac);

  const VW::PixelYUV<T>& mm = Base::pixel[y0][x0];
  const VW::PixelYUV<T>& pm = Base::pixel[y0][x0+1];
  const VW::PixelYUV<T>& mp = Base::pixel[y0+1][x0];
  const VW::PixelYUV<T>& pp = Base::pixel[y0+1][x0+1];

  VW::PixelYUV<double> result;
  result.y = mm_frac*double(mm.y) 
    + mp_frac*double(mp.y)
    + pm_frac*double(pm.y)
    + pp_frac*double(pp.y);
  result.u = mm_frac*double(mm.u) 
    + mp_frac*double(mp.u)
    + pm_frac*double(pm.u)
    + pp_frac*double(pp.u);
  result.v = mm_frac*double(mm.v) 
    + mp_frac*double(mp.v)
    + pm_frac*double(pm.v)
    + pp_frac*double(pp.v);
  return(result);
}
//////////////////////////////////////////////////////

template class VW::ImageYUV <unsigned char>;
template class VW::ImageYUV <double>;
template class VW::ImageYUV <float>;
//template class VW::ImageYUV <int>;
//template class VW::ImageYUV <short int>;
