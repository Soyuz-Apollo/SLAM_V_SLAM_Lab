/** @file pixelhsv.h 
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
/*
 * @class   VW::PixelHSV
 * @ingroup Image
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/02
 * @brief   A class for storing HSV (Hue, Saturation, Value) pixels.
 *
 */


#ifndef _pixelhsv_h_
#define _pixelhsv_h_


namespace VW {
  // Forward declare some other pixel types
  template <class T> class PixelMono;
  template <class T> class PixelRGB;
  template <class T> class PixelYUV;

template <class Type>
class PixelHSV {
 public:
  PixelHSV();
  PixelHSV(Type H, Type S, Type V, Type Alpha=0);

  void Set(Type val);
  void Set(Type H, Type S, Type V, Type Alpha=0);
  Type Luminance() const;
  bool operator==(const PixelHSV<Type> that);

   /** Convert a single value to a pixel. */
  PixelHSV<Type>& operator=(Type v);
    
   /** Convert an RGB pixel to a YUV one */
  PixelHSV<Type>& operator=(const VW::PixelRGB<Type>& p);
	
  /** Convert a mono pixel to a YUV one */
  PixelHSV<Type>& operator=(const VW::PixelMono<Type>& p);

  /** This templated copy conversion allows conversion between different 
   * numeric formats. See below for type conversions (eg RGB->YUV)
   */
  template <class inType>
  PixelHSV<Type>& operator=(const PixelHSV<inType>& p)
  { // code must be here for WIN32
    h = static_cast<Type>(p.h);
    s = static_cast<Type>(p.s);
    v = static_cast<Type>(p.v);
    alpha = static_cast<Type>(p.alpha);
    return(*this);
  }

  /** For converting to built-in types
   */
  template <class T>
  operator T() {
    return(T(v));
  };

 public:
  Type h, s, v, alpha;    // H, S, V and unused components
};
  
}; // end namespace VW










// INLINES

template <class T> inline 
VW::PixelHSV<T>::PixelHSV()
  : h(127), s(127), v(0), alpha(0)
{
}
template <class T> inline 
VW::PixelHSV<T>::PixelHSV(T H, T S, T V, T Alpha) 
  : h(H), s(S), v(V), alpha(Alpha)
{
}

template <class T> inline void
VW::PixelHSV<T>::Set(T H, T S, T V, T Alpha) 
{
  h = H; 
  s = S;
  v = V;
  alpha = Alpha;
}

template <class T> inline void
VW::PixelHSV<T>::Set(T val) 
{
  h=127;
  s=127;
  v = val; 
  alpha = 0;
}

template <class T> inline T
VW::PixelHSV<T>::Luminance() const 
{ 
  return v; 
};

template <class T> inline bool 
VW::PixelHSV<T>::operator==(const VW::PixelHSV<T> that) 
{
  return(   (this->h==that.h)
	 && (this->s==that.s)
	 && (this->v==that.v)  );
}



// Type conversions need the actual class definitions
#include "pixelmono.h"
#include "pixelrgb.h"


template <class T> inline VW::PixelHSV<T>& 
VW::PixelHSV<T>::operator=(T v)
{
  this->h = 127;
  this->s = 127;
  this->v = v;
  this->alpha = 0;
  return(*this);
}

template <class T> inline VW::PixelHSV<T>& 
VW::PixelHSV<T>::operator=(const VW::PixelRGB<T>& p)
{

  ImageConversions::RGB2HSV(p.r,p.g,p.b,this->h,this->s,this->v);
  this->alpha = p.alpha;
  return(*this);
}

template <class T> inline VW::PixelHSV<T>& 
VW::PixelHSV<T>::operator=(const VW::PixelMono<T>& p)
{
  this->h=127;
  this->s=127;
  this->v = p.y;
  this->alpha = 0;
  return(*this);
}


#endif /* _pixelhsv_h_ */
