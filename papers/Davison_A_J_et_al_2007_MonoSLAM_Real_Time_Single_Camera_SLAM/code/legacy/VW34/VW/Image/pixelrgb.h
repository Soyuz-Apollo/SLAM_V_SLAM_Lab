/** @file pixelrgb.h 
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
 * @class   VW::PixelRGB
 * @ingroup Image
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/02
 * @brief   A class for storing RGB pixels.
 *
 * This pixel class stores the colour in packed BGRA format, with A specifying the alpha
 * value (if used - which is not common). Note the ordering for little-endian CPU, which
 * corresponds to the order used by the Matrox Meteor capture device and the X-windows
 * display system. The extra alpha channel ensures that data is word-aligned in memory
 * (ie. chunks of 4 bytes of multiples thereof).
 *
 * If not specified, the default template format is 8-bit, ie unsigned char, so that
 * VW::PixelRGB<> p == VW::PixelRGB<unsigned char> p.
 */


#ifndef _pixelrgb_h_
#define _pixelrgb_h_

#include "pixelconversions.h"


namespace VW {

  // Need to know about other pixel types
  template <class T> class PixelMono;
  template <class T> class PixelYUV;
  template <class T> class PixelHSV;


  template <class Type>
  class PixelRGB {
  public:
    PixelRGB();
    PixelRGB(Type val);
    PixelRGB(Type R, Type G, Type B, Type Alpha=0);
    PixelRGB(PixelYUV<Type> yuv);
    
    void Set(Type val);
    void Set(Type R, Type G, Type B, Type Alpha=0);
    
    /** Recover the luminance component */
    Type Luminance() const;
    
    /** Scale the intensity (ie each of RGB) value in place.
     * Use the operator* functions if you want the result to be floating point.
     */
    void ScaleIntensity(double scale_factor);

   /** Convert a single value to a pixel. */
    PixelRGB<Type>& operator=(Type u);
    
    /** Copy a non identical pixel (ie perform type conversion)*/
    template <class inType>
    PixelRGB<Type>& operator=(const PixelRGB<inType>& p)
    { // Code must be here for WIN32
      r = static_cast<Type>(p.r);
      g = static_cast<Type>(p.g);
      b = static_cast<Type>(p.b);
      alpha = static_cast<Type>(p.alpha);
      return(*this);
    }
    
    /** Convert a mono pixel to an RGB one */
    PixelRGB<Type>& operator=(const PixelMono<Type>& p);
    
    /** Convert an YUV pixel to an RGB one */
    PixelRGB<Type>& operator=(const PixelYUV<Type>& p);
	
    /** Convert an YUV pixel to an RGB one */
    PixelRGB<Type>& operator=(const PixelHSV<Type>& p);
	
    /** Is this pixel the same as me? */
    bool operator==(const PixelRGB<Type>& p) const;
    
    /** Is this pixel different to me? */
    bool operator!=(const PixelRGB<Type>& p) const;
    


    // Data is public too...
  public:
    Type b, g, r, alpha;  // Red, Green, Blue and Alpha components
    // Note order for little-endian CPU
    
  };
  
 






// Inlines
template <class Type> inline VW::PixelRGB<double>
operator*(const VW::PixelRGB<Type> &p, const double a) 
{
  // Note that the alpha isn't scaled, only the colour
  return VW::PixelRGB<double>(static_cast<double>(a*p.r), 
			      static_cast<double>(a*p.g), 
			      static_cast<double>(a*p.b), 
			      static_cast<double>(p.alpha));
}

template <class Type> inline VW::PixelRGB<double> 
operator*(const double a, const VW::PixelRGB<Type> &p) 
{
  // Note that the alpha isn't scaled, only the colour
  return VW::PixelRGB<double>(static_cast<double>(a*p.r), 
			      static_cast<double>(a*p.g), 
			      static_cast<double>(a*p.b),
			      static_cast<double>(p.alpha));
}

template <class Type> inline VW::PixelRGB<float>
operator*(const VW::PixelRGB<Type> &p, const float a) 
{
  // Note that the alpha isn't scaled, only the colour
  return VW::PixelRGB<float>(static_cast<float>(a*p.r), 
			     static_cast<float>(a*p.g), 
			     static_cast<float>(a*p.b), 
			     static_cast<float>(p.alpha));
}

template <class Type> inline VW::PixelRGB<float> 
operator*(const float a, const VW::PixelRGB<Type> &p) 
{
  // Note that the alpha isn't scaled, only the colour
  return VW::PixelRGB<float>(static_cast<float>(a*p.r), 
			 static_cast<float>(a*p.g), 
			 static_cast<float>(a*p.b),
			 static_cast<float>(p.alpha));
}

template <class Type> inline VW::PixelRGB<Type> 
operator+(const VW::PixelRGB<Type> &p1, const VW::PixelRGB<Type> &p2) 
{
  return VW::PixelRGB<Type>(p1.r+p2.r, 
			    p1.g+p2.g, 
			    p1.b+p2.b, 
			    p1.alpha+p2.alpha);
}

template <class Type> inline VW::PixelRGB<Type> 
operator-(const VW::PixelRGB<Type> &p1, const VW::PixelRGB<Type> &p2) 
{
  return VW::PixelRGB<Type>(p1.r-p2.r, 
			    p1.g-p2.g, 
			    p1.b-p2.b, 
			    p1.alpha-p2.alpha);
}

}; // end namespace VW


template <class Type> inline 
VW::PixelRGB<Type>::PixelRGB(PixelYUV<Type> yuv)
{
  *this = yuv;
}


template <class Type> inline void 
VW::PixelRGB<Type>::ScaleIntensity(double scale_factor)
{
  r = static_cast<Type>(r*scale_factor);
  g = static_cast<Type>(g*scale_factor);
  b = static_cast<Type>(b*scale_factor);
}


template <class Type> inline       
VW::PixelRGB<Type>::PixelRGB() 
{
};

template <class Type> inline       
VW::PixelRGB<Type>::PixelRGB(Type val) 
  : b(val), g(val), r(val), alpha(val)  
{
};

template <class Type> inline       
VW::PixelRGB<Type>::PixelRGB(Type R, Type G, Type B, Type Alpha) 
  : b(B), g(G), r(R), alpha(Alpha) 
{
};
    
template <class Type> inline void
VW::PixelRGB<Type>::Set(Type val) 
{
  b = val;
  g = val;
  r = val;
  alpha = 0; 
};
    
template <class Type> inline void
VW::PixelRGB<Type>::Set(Type R, Type G, Type B, Type Alpha) 
{
  b = B;
  g = G;
  r = R;
  alpha = Alpha; 
};
    
template <class Type> inline Type     
VW::PixelRGB<Type>::Luminance() const 
{
  Type y;
  ImageConversions::RGB2Y(r, g, b, y);
  return(y); 
};




// Need these for type conversions
#include "pixelmono.h"
#include "pixelyuv.h"



template <class T> inline VW::PixelRGB<T>& 
VW::PixelRGB<T>::operator=(T u)
{
    this->r = u;
    this->g = u;
    this->b = u;
    this->alpha = 0;
    return(*this);
}

template <class T> inline VW::PixelRGB<T>& 
VW::PixelRGB<T>::operator=(const VW::PixelMono<T>& p)
{
	this->r = p.y;
	this->g = p.y;
	this->b = p.y;
	this->alpha=0;
    return(*this);
}

template <class T> inline VW::PixelRGB<T>& 
VW::PixelRGB<T>::operator=(const VW::PixelYUV<T>& p)
{
  ImageConversions::YUV2RGB(p.y,p.u,p.v,this->r,this->g,this->b);
  this->alpha = p.alpha;
  return(*this);
}

template <class T> inline VW::PixelRGB<T>& 
VW::PixelRGB<T>::operator=(const VW::PixelHSV<T>& p)
{
  ImageConversions::HSV2RGB(p.h,p.s,p.v,this->r,this->g,this->b);
  this->alpha = p.alpha;
  return(*this);
}

template <class T> inline bool 
VW::PixelRGB<T>::operator==(const VW::PixelRGB<T>& p) const
{
  return ((p.r==this->r)
	  && (p.g==this->g)
	  && (p.b==this->b)
	  && (p.alpha==this->alpha));
}

template <class T> inline bool 
VW::PixelRGB<T>::operator!=(const VW::PixelRGB<T>& p) const
{
  return ((p.r!=this->r)
	  || (p.g!=this->g)
	  || (p.b!=this->b)
	  || (p.alpha!=this->alpha));
}

#endif /* _pixelrgb_h_ */
