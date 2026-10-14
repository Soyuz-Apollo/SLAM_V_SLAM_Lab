
/** @file pixelmono.h 
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
 * @class   VW::PixelMono
 * @ingroup Image
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/02
 * @brief   A class for storing Mono pixels.
 *
 * This pixel class stores luminance data only.
 *
 * If not specified, the default template format is 8-bit, ie unsigned char, so that
 * VW::PixelRGB<> p == VW::PixelRGB<unsigned char> p.
 */


#ifndef _pixelmono_h_
#define _pixelmono_h_

#include "pixelconversions.h"

namespace VW {

  // Forward declare other pixel types
  template <class T> class PixelYUV;
  template <class T> class PixelRGB;


  template <class Type>
  class PixelMono {
  public:
    PixelMono();
    PixelMono(Type Y);
    
    void Set(Type Y);
    
    /** Recover the luminance component */
    Type Luminance() const;
    
    /** Scale the intensity value in place.
     * Use the operator* functions if you want the result to be floating point.
     */
    void ScaleIntensity(const float scale_factor);

    /** Convert a single value to a pixel. */
    PixelMono<Type>& operator=(Type u);
    
    /** Copy a non identical pixel (ie perform type conversion)*/
    template <class inType>
    PixelMono<Type>& operator=(const PixelMono<inType>& p)
    { // Code must be here for WIN32
      y = static_cast<Type>(p.y);
      return(*this);
    }
    
    /** Convert an RGB pixel to a mono one */
    PixelMono<Type>& operator=(const PixelRGB<Type>& p);
    
    /** Convert an YUV pixel to a mono one */
    PixelMono<Type>& operator=(const PixelYUV<Type>& p);
	


    PixelMono<Type>& operator*=(const float scale_factor);
    PixelMono<Type>& operator+=(const PixelMono<Type> &p);
    PixelMono<Type>& operator-=(const PixelMono<Type> &p);
    
    /** For converting to built-in types (int, float, double etc)
     */
    //template <class T>
    //operator T();
    

 

    // Data is public too...
  public:
    Type y;  // Luminance only
    // Note order for little-endian CPU
    
  };
  
 






// Inlines
 
template <class Type> inline VW::PixelMono<double>
operator*(const VW::PixelMono<Type> &p, const double a) 
{
  // Note that the alpha isn't scaled, only the colour
  return VW::PixelMono<double>(static_cast<double>(a*p.y));
}

template <class Type> inline VW::PixelMono<double> 
operator*(const double a, const VW::PixelMono<Type> &p) 
{
  // Note that the alpha isn't scaled, only the colour
  return VW::PixelMono<double>(static_cast<double>(a*p.y));
}

template <class Type> inline VW::PixelMono<float>
operator*(const VW::PixelMono<Type> &p, const float a) 
{
  // Note that the alpha isn't scaled, only the colour
  return VW::PixelMono<float>(static_cast<float>(a*p.y));
}

template <class Type> inline VW::PixelMono<float> 
operator*(const float a, const VW::PixelMono<Type> &p) 
{
  // Note that the alpha isn't scaled, only the colour
  return VW::PixelMono<float>(static_cast<float>(a*p.y));
}

template <class Type> inline VW::PixelMono<Type> 
operator+(const VW::PixelMono<Type> &p1, const VW::PixelMono<Type> &p2) 
{
  // More efficeint to use the += operator!
  VW::PixelMono<Type> r(p1);
  return (r += p2);
}

template <class Type> inline VW::PixelMono<Type> 
operator-(const VW::PixelMono<Type> &p1, const VW::PixelMono<Type> &p2) 
{
  // More efficeint to use the += operator!
  VW::PixelMono<Type> r(p1);
  return (r -= p2);
}

}; // end namespace VW



template <class Type> inline void 
VW::PixelMono<Type>::ScaleIntensity(const float scale_factor)
{
  y *= scale_factor;
}


template <class Type> inline       
VW::PixelMono<Type>::PixelMono()
  : y(0)
{
};

template <class Type> inline       
VW::PixelMono<Type>::PixelMono(Type Y) 
  : y(Y)
{
};

template <class T>
inline VW::PixelMono<T>& VW::PixelMono<T>::operator=(T u)
{
    this->y=u;
    return(*this);
}


template <class Type> inline void
VW::PixelMono<Type>::Set(Type Y) 
{
  y = Y;
};
    
template <class Type> inline Type     
VW::PixelMono<Type>::Luminance() const 
{
  return(y); 
};

/*    
template <class Type> template <class T>
VW::PixelMono<Type>::operator T() 
{
  return(T(y));
};
*/

template <class Type> VW::PixelMono<Type>& 
VW::PixelMono<Type>::operator*=(const float scale_factor)
{
  y *= scale_factor;
  return( *this );
}

template <class Type> VW::PixelMono<Type>& 
VW::PixelMono<Type>::operator+=(const PixelMono<Type> &p)
{
  y += p.y;
  return( *this );
}

template <class Type> VW::PixelMono<Type>& 
VW::PixelMono<Type>::operator-=(const PixelMono<Type> &p)
{
  y -= p.y;
  return( *this );
}





// Need these for type conversions
#include "pixelrgb.h"
#include "pixelyuv.h"



template <class T>
inline VW::PixelMono<T>& VW::PixelMono<T>::operator=(const VW::PixelRGB<T>& p)
{

  ImageConversions::RGB2Y(p.r,p.g,p.b,this->y);
    return(*this);
}

template <class T>
inline VW::PixelMono<T>& VW::PixelMono<T>::operator=(const VW::PixelYUV<T>& p)
{
  this->y = p.y;
  return(*this);
}





#endif /* _pixelrgb_h_ */
