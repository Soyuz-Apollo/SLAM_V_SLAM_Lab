/** @file pixelyuv.h 
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
 * @class   VW::PixelYUV
 * @ingroup Image
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/02
 * @brief   A class for storing YUV (YCrCb?) pixels.
 *
 */


#ifndef _pixelyuv_h_
#define _pixelyuv_h_

#include "pixelconversions.h"


namespace VW {

  // Forward declare some other pixel types
  template <class T> class PixelMono;
  template <class T> class PixelRGB;
  template <class T> class PixelHSV;
  
  
template <class Type>
class PixelYUV {
 public:
  PixelYUV();
  PixelYUV(Type Y, Type U, Type V, Type Alpha=0);

  void Set(Type val);
  void Set(Type Y, Type U, Type V, Type Alpha=0);


  /** Scale the intensity (Y) value in place.
   * Use the operator* functions if you want the result to be floating point.
   */
  void ScaleIntenisty(double scale_factor);

  Type Luminance() const { return y; };
  bool operator==(const PixelYUV<Type> that);

   /** Convert a single value to a pixel. */
    PixelYUV<Type>& operator=(Type v);
    
   /** Convert an RGB pixel to a YUV one */
  PixelYUV<Type>& operator=(const VW::PixelRGB<Type>& p);
	
  /** Convert a mono pixel to a YUV one */
  PixelYUV<Type>& operator=(const VW::PixelMono<Type>& p);
	

  
  /** This templated copy conversion allows conversion between different 
   * numeric formats. See below for type conversions (eg RGB->YUV)
   */
  template <class inType>
  PixelYUV<Type>& operator=(const PixelYUV<inType>& p)
  { // code must be here for WIN32
    y = static_cast<Type>(p.y);
    u = static_cast<Type>(p.u);
    v = static_cast<Type>(p.v);
    alpha = static_cast<Type>(p.alpha);
    return(*this);
  }

  /** Scale the pixel around the central value (0,127,127). */
  PixelYUV<Type>& operator*=(double scale_factor);
  /** Accumulate around the central value (0,127,127) (ie.
   * (10,127,127)+(10,127,127)=(20,127,127) ). */
  PixelYUV<Type>& operator+=(const PixelYUV<Type> &p);
  /** Subtract around the central value (0,127,127) (ie.
   * (10,127,127)-(6,127,127)=(4,127,127) ). */
  PixelYUV<Type>& operator-=(const PixelYUV<Type> &p);


  /** For converting to built-in types
   */
  template <class T>
  operator T() {
    return(T(y));
  };



public: // data is puiblic too!
  Type y, u, v, alpha;    // Y, U, V and alpha components


};
  

// Other operators

template <class Type> inline VW::PixelYUV<double>
operator*(const VW::PixelYUV<Type> &p, const double a) 
{
  // Note that the alpha isn't scaled, and the colour is scaled towards gray
  return VW::PixelYUV<double>(a*p.y,
			      127.0 + a*(p.u-127.0),
			      127.0 + a*(p.v-127.0));
}

template <class Type> inline VW::PixelYUV<double> 
operator*(const double a, const VW::PixelYUV<Type> &p) 
{
  // Note that the alpha isn't scaled, and the colour is scaled towards gray
  return VW::PixelYUV<double>(a*p.y,
			      127.0 + a*(p.u-127.0),
			      127.0 + a*(p.v-127.0));
}

template <class Type> inline VW::PixelYUV<float>
operator*(const VW::PixelYUV<Type> &p, const float a) 
{
  // Note that the alpha isn't scaled, and the colour is scaled towards gray
  return VW::PixelYUV<float>(a*p.y,
			     127.0f + a*(p.u-127.0f),
			     127.0f + a*(p.v-127.0f));
}

template <class Type> inline VW::PixelYUV<float> 
operator*(const float a, const VW::PixelYUV<Type> &p) 
{
  // Note that the alpha isn't scaled, and the colour is scaled towards gray
  return VW::PixelYUV<float>(a*p.y,
			     127.0f + a*(p.u-127.0f),
			     127.0f + a*(p.v-127.0f));
}

template <class Type> inline VW::PixelYUV<Type> 
operator+(const VW::PixelYUV<Type> &p1, const VW::PixelYUV<Type> &p2) 
{
  // More efficeint to use the += operator!
  VW::PixelYUV<Type> r(p1);
  return (r += p2);
}

template <class Type> inline VW::PixelYUV<Type> 
operator-(const VW::PixelYUV<Type> &p1, const VW::PixelYUV<Type> &p2) 
{
  // More efficeint to use the -= operator!
  VW::PixelYUV<Type> r(p1);
  return (r -= p2);
}




}; // end namespace VW

//////////////////////////////////////////////////////////
template <class T> inline void 
VW::PixelYUV<T>::ScaleIntenisty(double scale_factor)
{
  y = scale_factor * y;
}

template <class T> inline bool 
VW::PixelYUV<T>::operator==(const VW::PixelYUV<T> that) {
  return(   (this->y==that.y)
	 && (this->u==that.u)
	 && (this->v==that.v)  );
}

template <class T> inline 
VW::PixelYUV<T>::PixelYUV()
  : y(0), u(127), v(127), alpha(0)
{
}
template <class T> inline 
VW::PixelYUV<T>::PixelYUV(T Y, T U, T V, T Alpha) 
  : y(Y), u(U), v(V), alpha(Alpha)
{
}

template <class T> inline void
VW::PixelYUV<T>::Set(T Y, T U, T V, T Alpha) 
{
  y = Y; 
  u = U;
  v = V;
  alpha = Alpha;
}

template <class T> inline void
VW::PixelYUV<T>::Set(T val) 
{
  y = val; 
  u = 127;
  v = 127;
  alpha = 0;
}

template <class Type> VW::PixelYUV<Type>& 
VW::PixelYUV<Type>::operator*=(const double scale_factor)
{
  y *= scale_factor;
  u = Type(127.0 + scale_factor*(u - 127.0));
  v = Type(127.0 + scale_factor*(v - 127.0));
  return( *this );
}

template <class Type> VW::PixelYUV<Type>& 
VW::PixelYUV<Type>::operator+=(const PixelYUV<Type> &p)
{
  y += p.y;
  u += (p.u - 127);
  v += (p.v - 127);
  return( *this );
}

template <class Type> VW::PixelYUV<Type>& 
VW::PixelYUV<Type>::operator-=(const PixelYUV<Type> &p)
{
  y -= p.y;
  u -= (p.u - 127);
  v -= (p.v - 127);
  return( *this );
}




// Type conversions need the actual class definitions
#include "pixelmono.h"
#include "pixelrgb.h"


template <class T> inline VW::PixelYUV<T>& 
VW::PixelYUV<T>::operator=(T v)
{
    this->y = v;
    this->u = 127;
    this->v = 127;
    this->alpha = 0;
    return(*this);
}

template <class T> inline VW::PixelYUV<T>& 
VW::PixelYUV<T>::operator=(const VW::PixelRGB<T>& p)
{

	ImageConversions::RGB2YUV(p.r,p.g,p.b,this->y,this->u,this->v);
	this->alpha = p.alpha;
    return(*this);
}

template <class T> inline VW::PixelYUV<T>& 
VW::PixelYUV<T>::operator=(const VW::PixelMono<T>& p)
{
this->y = p.y;
	this->u=127;
	this->v=127;
	this->alpha = 0;
    return(*this);
}


#endif /* _pixelyuv_h_ */
