/**
 * @file   imageconversions.h
 * @author BJ Tordoff, Dan Dennedy
 * @date   02/08/02
 *
 * Copyright (C) 2000-2001 Dan Dennedy  <dan@dennedy.org>
 * and
 * Copyright (C) 2001-2003 Oxford University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 

#ifndef _PixelConversions_h_
#define _PixelConversions_h_

//#include <iostream>

namespace VW {

namespace ImageConversions {

  /** @name Value conversions. */
  //@{
// These are defined in the cpp. Shame they have to be public, but windows won't have it any other way 8-(
extern const unsigned char uchar_clipping_table[];
extern const int clipping_table_offset;

  inline unsigned char
    CLIPVALUE(int val);
  //@}


  /** @name Type conversions. */
  //@{


  // UCHAR

  inline void 
  YUV2RGB(const unsigned char y, const unsigned char u, const unsigned char v,
	  unsigned char& r,      unsigned char& g,      unsigned char& b);

  inline void 
  RGB2YUV(const unsigned char r, const unsigned char g, const unsigned char b,
	  unsigned char& y,      unsigned char& u,      unsigned char& v);

  inline void 
  RGB2Y(const unsigned char r, const unsigned char g, const unsigned char b, 
	unsigned char& y);

  inline void 
  RGB2HSV(const unsigned char r, const unsigned char g, const unsigned char b, 
	   unsigned char& h,     unsigned char& s,      unsigned char& v );

  inline void 
  HSV2RGB(const unsigned char h, const unsigned char s, const unsigned char v, 
	  unsigned char& r,      unsigned char& g,      unsigned char& b );

  // FLOAT

  inline void 
  YUV2RGB(const float y, const float u, const float v,
	  float& r,      float& g,      float& b);

  inline void 
  RGB2YUV(const float r, const float g, const float b,
	  float& y,      float& u,      float& v);

  inline void 
  RGB2Y(const float r, const float g, const float b, 
	float& y);

  inline void 
  RGB2HSV(const float r, const float g, const float b, 
	   float& h,     float& s,      float& v );

  inline void 
  HSV2RGB(const float h, const float s, const float v, 
	  float& r,      float& g,      float& b );

  //@}






}; // End namespace ImageConversions

}; // End namespace VW

















/** 
 * Conversion from YUV to RGB.
 * The normal conversion matrix is due to Julien (surname unknown):
 * 
 * [ R ]   [  1.0   0.0     1.403 ] [ Y ]
 * [ G ] = [  1.0  -0.344  -0.714 ] [ U ]
 * [ B ]   [  1.0   1.770   0.0   ] [ V ]
 * 
 * and the firewire one is similar:
 *
 * [ R ]   [  1.0   0.0     0.700 ] [ Y ]
 * [ G ] = [  1.0  -0.198  -0.291 ] [ U ]
 * [ B ]   [  1.0   1.015   0.0   ] [ V ]
 *
 * Corrected by BJT (coriander's transforms RGB->YUV and YUV->RGB
 *                   do not get you back to the same RGB!)
 * [ R ]   [  1.0   0.0     1.136 ] [ Y ]
 * [ G ] = [  1.0  -0.396  -0.578 ] [ U ]
 * [ B ]   [  1.0   2.041   0.002 ] [ V ]
 * 
 */
inline void 
VW::ImageConversions::YUV2RGB(const unsigned char y, 
			      const unsigned char u, 
			      const unsigned char v,
			      unsigned char& r, 
			      unsigned char& g, 
			      unsigned char& b) {
  const int y2=int(y);
  const int u2=int(u)-128;
  const int v2=int(v)-128;
  //std::cerr << "YUV=("<<y2<<","<<u2<<","<<v2<<")"<<std::endl;


  // This is the normal YUV conversion, but
  // appears to be incorrect for the firewire cameras
  //   int r2 = y2 + ( (v2*91947) >> 16);
  //   int g2 = y2 - ( ((u2*22544) + (v2*46793)) >> 16 );
  //   int b2 = y2 + ( (u2*115999) >> 16);
  // This is an adjusted version (UV spread out a bit)
  int r2 = y2 + ( (v2*37221) >> 15);
  int g2 = y2 - ( ((u2*12975) + (v2*18949)) >> 15 );
  int b2 = y2 + ( (u2*66883) >> 15);
  //std::cerr << "   RGB=("<<r2<<","<<g2<<","<<b2<<")"<<std::endl;


  // Cap the values.
  r=CLIPVALUE(r2);
  g=CLIPVALUE(g2);
  b=CLIPVALUE(b2);
}
/** 
 * Conversion from RGB to YUV.
 * The conversion matrix is due to Julien (surname unknown):
 * 
 * [ Y ]   [  0.299   0.587   0.114 ] [ R ]
 * [ U ] = [ -0.169  -0.332   0.501 ] [ G ]
 * [ V ]   [  0.701  -0.419  -0.081 ] [ B ]
 * 
 * and the firewire one is similar (from coriander):
 *
 * [ Y ]   [  0.258   0.621   0.121 ] [ R ]
 * [ U ] = [ -0.254  -0.612  -0.866 ] [ G ]
 * [ V ]   [  1.060  -0.887  -0.173 ] [ B ]
 * 
 * [ Y ]   [  0.299   0.587   0.114 ] [ R ]
 * [ U ] = [ -0.145  -0.288   0.434 ] [ G ]
 * [ V ]   [  0.617  -0.517  -0.100 ] [ B ]
 * 
 */
inline void 
VW::ImageConversions::RGB2YUV(const unsigned char r, 
			      const unsigned char g, 
			      const unsigned char b,
			      unsigned char& y, 
			      unsigned char& u, 
			      unsigned char& v) 
{
  const int r2=int(r);
  const int g2=int(g);
  const int b2=int(b);

  // This is the normal YUV conversion, but
  // appears to be incorrect for the firewire cameras
  //      int y2 = (19595*r2 + 38470*g2 + 7471*b2)  >> 16;
  //      int u2 = ((-11076*r2 - 21758*g2 + 32834*b2) >> 16) + 128;
  //      int v2 = ((45941*r2 - 27460*g2 - 5308*b2) >> 16) + 128;
  // This is a conversion with UV spread out a bit
  int y2 = (9798*r2 + 19235*g2 + 3736*b2)  >> 15;
  int u2 = ((-4784*r2 - 9437*g2 + 14221*b2) >> 15) + 128;
  int v2 = ((20218*r2 - 16941*g2 - 3277*b2) >> 15) + 128;

  // Cap the values.
  y = CLIPVALUE(y2);
  u = CLIPVALUE(u2);
  v = CLIPVALUE(v2);
}

/** 
 * Conversion from YUV to RGB.
 * The normal conversion matrix is due to Julien (surname unknown):
 * 
 * [ R ]   [  1.0   0.0     1.403 ] [ Y ]
 * [ G ] = [  1.0  -0.344  -0.714 ] [ U ]
 * [ B ]   [  1.0   1.770   0.0   ] [ V ]
 * 
 * and the firewire one is similar:
 *
 * [ R ]   [  1.0   0.0     0.700 ] [ Y ]
 * [ G ] = [  1.0  -0.198  -0.291 ] [ U ]
 * [ B ]   [  1.0   1.015   0.0   ] [ V ]
 *
 * Corrected by BJT (coriander's transforms RGB->YUV and YUV->RGB
 *                   do not get you back to the same RGB!)
 * [ R ]   [  1.0   0.0     1.136 ] [ Y ]
 * [ G ] = [  1.0  -0.396  -0.578 ] [ U ]
 * [ B ]   [  1.0   2.041   0.002 ] [ V ]
 * 
 */
inline void 
VW::ImageConversions::YUV2RGB(const float y, 
			      const float u, 
			      const float v,
			      float& r, 
			      float& g, 
			      float& b) {
  const float u2=u-128.0f;
  const float v2=v-128.0f;


  // This is the normal YUV conversion, but
  // appears to be incorrect for the firewire cameras
  //   int r2 = y2 + ( (v2*91947) >> 16);
  //   int g2 = y2 - ( ((u2*22544) + (v2*46793)) >> 16 );
  //   int b2 = y2 + ( (u2*115999) >> 16);
  // This is an adjusted version (UV spread out a bit)
  r = y + ( (v2*37221.0f) * 0.00003051757f);
  g = y - ( ((u2*12975.0f) + (v2*18949.0f)) * 0.00003051757f);
  b = y + ( (u2*66883.0f) * 0.00003051757f);

}
/** 
 * Conversion from RGB to YUV.
 * The conversion matrix is due to Julien (surname unknown):
 * 
 * [ Y ]   [  0.299   0.587   0.114 ] [ R ]
 * [ U ] = [ -0.169  -0.332   0.501 ] [ G ]
 * [ V ]   [  0.701  -0.419  -0.081 ] [ B ]
 * 
 * and the firewire one is similar (from coriander):
 *
 * [ Y ]   [  0.258   0.621   0.121 ] [ R ]
 * [ U ] = [ -0.254  -0.612  -0.866 ] [ G ]
 * [ V ]   [  1.060  -0.887  -0.173 ] [ B ]
 * 
 * [ Y ]   [  0.299   0.587   0.114 ] [ R ]
 * [ U ] = [ -0.145  -0.288   0.434 ] [ G ]
 * [ V ]   [  0.617  -0.517  -0.100 ] [ B ]
 * 
 */
inline void 
VW::ImageConversions::RGB2YUV(const float r, 
			      const float g, 
			      const float b,
			      float& y, 
			      float& u, 
			      float& v) 
{

  // This is the normal YUV conversion, but
  // appears to be incorrect for the firewire cameras
  //      int y2 = (19595*r2 + 38470*g2 + 7471*b2)  >> 16;
  //      int u2 = ((-11076*r2 - 21758*g2 + 32834*b2) >> 16) + 128;
  //      int v2 = ((45941*r2 - 27460*g2 - 5308*b2) >> 16) + 128;
  // This is a conversion with UV spread out a bit
  y = (9798.0f*r + 19235.0f*g + 3736.0f*b) * 0.00003051757f; //>>15
  u = (-4784.0f*r - 9437.0f*g + 14221.0f*b) * 0.00003051757f + 128.0f;
  v = (20218.0f*r - 16941.0f*g - 3277.0f*b) * 0.00003051757f + 128.0f;


}

/** Convert RGB to luminance only (floating point version).
 * This one is just a cut-down version of the full YUV, but seems to be 
 * a common operation
 */
inline void VW::ImageConversions::RGB2Y(const float r, 
					const float g, 
					const float b, 
					float& y) {
  // This is the normal YUV conversion, but
  // appears to be incorrect for the firewire cameras
  //      int y2 = (19595*r2 + 38470*g2 + 7471*b2)  *  0.0000152587890625;
  // This is a conversion with UV spread out a bit
  y = (16914.0f*r + 40686.0f*g + 7937.0f*b)  
                          *  0.0000152587890625f; /*  = x / 65536 */
}

/** Convert RGB to luminance only.
 * This one is just a cut-down version of the full YUV, but seems to be 
 * a common operation
 */
inline void 
VW::ImageConversions::RGB2Y(const unsigned char r, 
			    const unsigned char g, 
			    const unsigned char b, 
			    unsigned char& y) 
{
  const int r2=int(r);
  const int g2=int(g);
  const int b2=int(b);
  
  // This is the normal YUV conversion, but
  // appears to be incorrect for the firewire cameras
  //      int y2 = (19595*r2 + 38470*g2 + 7471*b2)  *  0.0000152587890625;
  // This is a conversion with UV spread out a bit
  int y2 = (16914*r2 + 40686*g2 + 7937*b2) >> 16; /*  = x / 65536 */
  y=CLIPVALUE(y2);
}


/**
 * Convert RGB value into HSV value.
 * Note that as there is no formulaic way to do this so the conversion
 * is relatively costly (involves some 'if's).
 */
inline void 
VW::ImageConversions::RGB2HSV( const unsigned char r, 
			       const unsigned char g, 
			       const unsigned char b, 
			       unsigned char& h, 
			       unsigned char& s, 
			       unsigned char& v ) 
{
 int mn, mx, delta;
  
 mn = (g < b) ? g : b;
 mn = (r < mn) ? r : mn;
 mx = (g > b) ? g : b;
 mx = (r > mx) ? r : mx;

  v = (unsigned char)(mx);			// v
  
  delta = mx - mn;
  
  if( mx == 0 ) {
    // r = g = b = 0		// s = 0, v is undefined
    s = 0;
    h = 0;
 } else {
    int hi=0;
    s = (255*delta) / mx;		// s
    // NB: use 42.5 to squeeze 360 degrees into 0<h<255
    if( r == mx )
      hi = int((42.5*( g - b )) / delta);	// between yellow & magenta
    else if( g == mx )
      hi = int((42.5*(2*delta + ( b - r ))) / delta);	// between cyan & yellow
    else
      hi = int((42.5*(4*delta + ( r - g ))) / delta);	// between magenta & cyan
    
    if( hi < 0 ) hi += 255;
    h=static_cast<unsigned char>(hi);
  }
}

/**
 * Convert HSV value into RGB value.
 * Note that as there is no formulaic way to do this so the conversion
 * is relatively costly (involves some 'if's).
 */
inline void 
VW::ImageConversions::HSV2RGB( const unsigned char h, 
			       const unsigned char s, 
			       const unsigned char v,
			       unsigned char& r, 
			       unsigned char& g, 
			       unsigned char& b ) 
{
  float h2,f;
  int i;
  unsigned char p, q, t;
  
  if( s == 0 ) {
    // achromatic (grey)
    r = g = b = v;
    return;
  }
  
  h2 = h / 42.5f;		// sector 0 to 5
  i = int( h2 );
  f = (h2 - i);			// factorial part of h
  p = static_cast<unsigned char>(v * ( 255 - s ) / 255);
  q = static_cast<unsigned char>(v * ( 255 - s * f ) / 255);
  t = static_cast<unsigned char>(v * ( 255 - s * ( 1 - f ) ) / 255);
  
  switch( i ) {
  case 0:
    r = v;
    g = t;
    b = p;
    break;
  case 1:
    r = q;
    g = v;
    b = p;
    break;
  case 2:
    r = p;
    g = v;
    b = t;
    break;
  case 3:
    r = p;
    g = q;
    b = v;
    break;
  case 4:
    r = t;
    g = p;
    b = v;
    break;
  default:
  case 5:
    r = v;
    g = p;
    b = q;
    break;
  }
}

/**
 * Convert RGB value into HSV value.
 * Note that as there is no formulaic way to do this so the conversion
 * is relatively costly (involves some 'if's).
 */
inline void 
VW::ImageConversions::RGB2HSV( const float r, 
			       const float g, 
			       const float b, 
			       float& h, 
			       float& s, 
			       float& v ) 
{
  float mn, mx, delta;
  
 mn = (g < b) ? g : b;
 mn = (r < mn) ? r : mn;
 mx = (g > b) ? g : b;
 mx = (r > mx) ? r : mx;

  v = float(mx);			// v
  
  delta = mx - mn;
  
  if( mx == 0 ) {
    // r = g = b = 0		// s = 0, v is undefined
    s = 0;
    h = 0;
 } else {
    float hi=0;
    s = (255*delta) / mx;		// s
    // NB: use 42.5 to squeeze 360 degrees into 0<h<255
    if( r == mx )
      hi = ((42.5f*( g - b )) / delta);	// between yellow & magenta
    else if( g == mx )
      hi = (42.5f*(2*delta + ( b - r ))) / delta;	// between cyan & yellow
    else
      hi = (42.5f*(4*delta + ( r - g ))) / delta;	// between magenta & cyan
    
    if( hi < 0.0f ) hi += 255.0f;
    h=hi;
  }
}

/**
 * Convert HSV value into RGB value.
 * Note that as there is no formulaic way to do this so the conversion
 * is relatively costly (involves some 'if's).
 */
inline void 
VW::ImageConversions::HSV2RGB( const float h, 
			       const float s, 
			       const float v,
			       float& r, 
			       float& g, 
			       float& b ) 
{
  float h2,f;
  int i;
  float p, q, t;
  
  if( s == 0 ) {
    // achromatic (grey)
    r = g = b = v;
    return;
  }
  
  h2 = h / 42.5f;		// sector 0 to 5
  i = int( h2 );
  f = (h2 - i);			// factorial part of h
  p = v * ( 255 - s ) / 255;
  q = v * ( 255 - s * f ) / 255;
  t = v * ( 255 - s * ( 1 - f ) ) / 255;
  
  switch( i ) {
  case 0:
    r = v;
    g = t;
    b = p;
    break;
  case 1:
    r = q;
    g = v;
    b = p;
    break;
  case 2:
    r = p;
    g = v;
    b = t;
    break;
  case 3:
    r = p;
    g = q;
    b = v;
    break;
  case 4:
    r = t;
    g = p;
    b = v;
    break;
  default:
  case 5:
    r = v;
    g = p;
    b = q;
    break;
  }
}



  /** Clip a value to the range 0<val<255. For speed this is done using an 
   * array, so can only cope with numbers in the range -128<val<383.
   */
inline unsigned char
VW::ImageConversions::CLIPVALUE(int val)
{
    // Old method (if)
/*   val = val < 0 ? 0 : val; */
/*   return val > 255 ? 255 : val; */

    // New method (array)
    return uchar_clipping_table[val+clipping_table_offset];
}

#endif
