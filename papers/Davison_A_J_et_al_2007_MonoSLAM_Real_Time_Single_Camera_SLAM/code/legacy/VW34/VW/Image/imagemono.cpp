/** @file imagemono.cpp
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


// C++ includes
#include <string>

#include "imagemono.h"
#include "imageio.h"

using namespace std;
using namespace VW;


template <class Type>
bool ImageMono<Type>::ReadImage(FILE *file)
{
  return VW::ReadImage(file,*this);
}

template <class Type>
bool ImageMono<Type>::ReadImage(const string& filename)
{
  return VW::ReadImage(filename,*this);
}

template <class Type>
bool ImageMono<Type>::WriteImage(FILE *file) const
{
 return VW::WriteImage(file,*this);
}

template <class Type>
bool ImageMono<Type>::WriteImage(const string& filename) const
{
 return VW::WriteImage(filename,*this);
}


template <class T> VW::PixelMono<double> 
VW::ImageMono<T>::GetInterpolatedPixel(double x,double y) const
{
  // Find colour by bi-linear interpolation
  const int x0=int(x);
  const int y0=int(y);
  const double xfrac=x-x0;
  const double yfrac=y-y0;

  const double thresh=0.001;
  if (xfrac<thresh) {
    // Just use y-interp
    if (yfrac<thresh) {
      // neither x or y are significant
      return(VW::PixelMono<double>(Base::pixel[y0][x0].y));
    } else {
      const VW::PixelMono<double> result((1-yfrac)*double(Base::pixel[y0][x0].y) 
					 + yfrac*double(Base::pixel[y0+1][x0].y));
      return(result);
    }
  } else {
    if (yfrac<thresh) {
      // just use x-interp
       const VW::PixelMono<double> result((1-xfrac)*double(Base::pixel[y0][x0].y) 
					 + xfrac*double(Base::pixel[y0][x0+1].y));
      return(result);
    } else {
      // Both significant
      double mm_frac = (1-xfrac) * (1-yfrac);
      double mp_frac = (1-xfrac) * (yfrac);
      double pm_frac = (xfrac)   * (1-yfrac);
      double pp_frac = (xfrac)   * (yfrac);
      
      const VW::PixelMono<T>& mm = Base::pixel[y0][x0];
      const VW::PixelMono<T>& pm = Base::pixel[y0][x0+1];
      const VW::PixelMono<T>& mp = Base::pixel[y0+1][x0];
      const VW::PixelMono<T>& pp = Base::pixel[y0+1][x0+1];
      
      const VW::PixelMono<double> result(mm_frac*double(mm.y) 
				   + mp_frac*double(mp.y)
				   + pm_frac*double(pm.y)
				   + pp_frac*double(pp.y));
      
      return(result);
    }
  }
  // Never gets here
  assert(false);
  return VW::PixelMono<double>(0);
}


template <class T> VW::PixelMono<float> 
VW::ImageMono<T>::GetInterpolatedPixel(float x,float y) const
{
  // Find colour by bi-linear interpolation
  const int x0=int(x);
  const int y0=int(y);
  const float xfrac=x-x0;
  const float yfrac=y-y0;

  const float thresh=0.001f;
  if (xfrac<thresh) {
    // Just use y-interp
    if (yfrac<thresh) {
      // neither x or y are significant
      const VW::PixelMono<float> result = float(Base::pixel[y0][x0].y);
      return(result);
    } else {
      const VW::PixelMono<float> result((1.0f-yfrac)*float(Base::pixel[y0][x0].y) 
					 + yfrac*float(Base::pixel[y0+1][x0].y));
      return(result);
    }
  } else {
    if (yfrac<thresh) {
      // just use x-interp
       const VW::PixelMono<float> result((1-xfrac)*float(Base::pixel[y0][x0].y) 
					 + xfrac*float(Base::pixel[y0][x0+1].y));
      return(result);
    } else {
      // Both significant
      float mm_frac = (1-xfrac) * (1-yfrac);
      float mp_frac = (1-xfrac) * (yfrac);
      float pm_frac = (xfrac)   * (1-yfrac);
      float pp_frac = (xfrac)   * (yfrac);
      
      const VW::PixelMono<T>& mm = Base::pixel[y0][x0];
      const VW::PixelMono<T>& pm = Base::pixel[y0][x0+1];
      const VW::PixelMono<T>& mp = Base::pixel[y0+1][x0];
      const VW::PixelMono<T>& pp = Base::pixel[y0+1][x0+1];
      
      const VW::PixelMono<float> result(mm_frac*float(mm.y) 
				   + mp_frac*float(mp.y)
				   + pm_frac*float(pm.y)
				   + pp_frac*float(pp.y));
      
      return(result);
    }
  }
  // Never gets here
  assert(false);
  return VW::PixelMono<float>(0);
}


// Instantiate some versions
#ifdef INSTANTIATE
#  undef INSTANTIATE
#endif
# define INSTANTIATE(M) \
  template class VW::ImageMono<M >;

INSTANTIATE(unsigned char); 
INSTANTIATE(float); 
INSTANTIATE(int); 
INSTANTIATE(unsigned int); 

#undef INSTANTIATE

//////////////////////////////////////////////////////////
