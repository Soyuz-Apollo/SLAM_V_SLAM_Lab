/** @file imageconversions.cc
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
 
#include <iostream>

#include "imageconversions.h"


#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false

using namespace std;
using namespace VW;

















// void
// ImageConversions::uyvy2yuy2 (unsigned char *src, unsigned char *dest, int NumPixels) {
// 	swab(src, dest, NumPixels << 1);
// }

// void
// ImageConversions::yuv2yuy2 (unsigned char *src, unsigned char *dest, int NumPixels) {
// 	swab(src, dest, NumPixels << 1);
// }

// {{{ ImageConversions::y41p2yuyv (unsigned char *src, unsigned char *dest, int NumPixels)

void
ImageConversions::y41p2yuyv (unsigned char *src, unsigned char *dest, int NumPixels) {
  register int i=0,j=0;
  register int y0, y1, y2, y3, y4, y5, y6, y7, u0, u4, v0, v4;
  while (i < (NumPixels << 1) )
    {
      u0 = src[i++];
      y0 = src[i++];
      v0 = src[i++];
      y1 = src[i++];
      
      u4 = src[i++];
      y2 = src[i++];
      v4 = src[i++];
      y3 = src[i++];

      y4 = src[i++];
      y5 = src[i++];
      y6 = src[i++];
      y7 = src[i++];

      dest[j++] = y0;
      dest[j++] = u0;
      dest[j++] = y1;
      dest[j++] = v0;

      dest[j++] = y2;
      dest[j++] = u0;
      dest[j++] = y3;
      dest[j++] = v0;

      dest[j++] = y4;
      dest[j++] = u4;
      dest[j++] = y5;
      dest[j++] = v4;

      dest[j++] = y6;
      dest[j++] = u4;
      dest[j++] = y7;
      dest[j++] = v4;

    }
}

// }}}
// {{{ ImageConversions::iyu12yuy2 (unsigned char *src, unsigned char *dest, int NumPixels)

void
ImageConversions::iyu12yuy2 (unsigned char *src, unsigned char *dest, int NumPixels) {
  register int i=0,j=0;
  register int y0, y1, y2, y3, u, v;
  while (i < (NumPixels + (NumPixels >> 1)))
    {
      u = src[i++];
      y0 = src[i++];
      y1 = src[i++];
      v = src[i++];
      y2 = src[i++];
      y3 = src[i++];

      dest[j++] = y0;
      dest[j++] = u;
      dest[j++] = y1;
      dest[j++] = v;

      dest[j++] = y2;
      dest[j++] = u;
      dest[j++] = y3;
      dest[j++] = v;
    }
}

// }}}
// {{{ ImageConversions::iyu22yuy2 (unsigned char *src, unsigned char *dest, int NumPixels)

void
ImageConversions::iyu22yuy2 (unsigned char *src, unsigned char *dest, int NumPixels) {
  int i=0,j=0;
  register int y0, y1, u0, u1, v0, v1;
  while (i < (NumPixels + (NumPixels << 1)))
    {
      u0 = src[i++];
      y0 = src[i++];
      v0 = src[i++];
      u1 = src[i++];
      y1 = src[i++];
      v1 = src[i++];

      dest[j++] = y0;
      dest[j++] = (u0+u1)/2;
      dest[j++] = y1;
      dest[j++] = (v0+v1)/2;
    }
}

// }}}
// {{{ ImageConversions::y2yuy2 (unsigned char *src, unsigned char *dest, int NumPixels)

void
ImageConversions::y2yuy2 (unsigned char *src, unsigned char *dest, int NumPixels) {
  int i=0,j=0;
  register int y0, y1;
  while (i < NumPixels)
    {
      y0 = src[i++];
      y1 = src[i++];

      dest[j++] = y0;
      dest[j++] = 128;
      dest[j++] = y1;
      dest[j++] = 128;
    }
}

// }}}

/*macro used to convert a YUV pixel to RGB format
  from Bart Nabbe, optimized by Damien Douxchamps:
  y + v*1434 / 2048
  y - (u*406 - v*595)/2048
  y + u*2078 / 2048
*/

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//		if s == 0, then h = -1 (undefined)



/**** OLD FUNCTIONS ****/

// {{{ ImageConversions::rgb2yuy2 (char *RGB, char *YUV, int NumPixels)

void
ImageConversions::rgb2yuy2 (char *RGB, char *YUV, int NumPixels) {
  int i, j;
  register unsigned char y0, y1, u0, u1, v0, v1 ;
  register unsigned char r, g, b;

  for (i = 0, j = 0; i < (NumPixels + (NumPixels << 1)); i += 6, j += 4)
    {
      r = (unsigned char) RGB[i + 0];
      g = (unsigned char) RGB[i + 1];
      b = (unsigned char) RGB[i + 2];
      RGB2YUV (r, g, b, y0, u0 , v0);
      r = (unsigned char) RGB[i + 3];
      g = (unsigned char) RGB[i + 4];
      b = (unsigned char) RGB[i + 5];
      RGB2YUV (r, g, b, y1, u1 , v1);
      YUV[j + 0] = y0;
      YUV[j + 1] = (u0+u1) >> 1;
      YUV[j + 2] = y1;
      YUV[j + 3] = (v0+v1) >> 1;
    }
}

// }}}

/*routine to convert an array of YUV data to RGB format
  from Bart Nabbe

  Often called YUV 422 the data is packed as UYVY (ie. twice as
  many Y bits).
*/
// {{{ ImageConversions::uyvy2rgb (char *YUV, char *RGB, int NumPixels)

void
ImageConversions::uyvy2rgb (char *YUV, char *RGB, int NumPixels) {
  int i, j;
  register unsigned char y0, y1, u, v;
  register unsigned char r, g, b;
  //printf("Numpixels=%d\n",NumPixels);
  for (i = 0, j = 0; i < (NumPixels << 1); i += 4, j += 6)
    {
      //printf("%d,%d\n",i,j);
      u = (unsigned char) YUV[i + 0];
      y0 = (unsigned char) YUV[i + 1];
      v = (unsigned char) YUV[i + 2];
      y1 = (unsigned char) YUV[i + 3];
      YUV2RGB (y0, u, v, r, g, b);
      RGB[j + 0] = r;
      RGB[j + 1] = g;
      RGB[j + 2] = b;
      YUV2RGB (y1, u, v, r, g, b);
      RGB[j + 3] = r;
      RGB[j + 4] = g;
      RGB[j + 5] = b;
    }
  //  printf("Done.\n");
}

// }}}
// {{{ ImageConversions::yuy22rgb (char *YUV, char *RGB, int NumPixels)

void
ImageConversions::yuy22rgb (char *YUV, char *RGB, int NumPixels) {
  int i, j;
  register unsigned char y0, y1, u, v;
  register unsigned char r, g, b;

  for (i = 0, j = 0; i < (NumPixels << 1); i += 4, j += 6)
    {
      y0 = (unsigned char) YUV[i + 0];
      u = (unsigned char) YUV[i + 1];
      y1 = (unsigned char) YUV[i + 2];
      v = (unsigned char) YUV[i + 3];
      YUV2RGB (y0, u, v, r, g, b);
      RGB[j + 0] = r;
      RGB[j + 1] = g;
      RGB[j + 2] = b;
      YUV2RGB (y1, u, v, r, g, b);
      RGB[j + 3] = r;
      RGB[j + 4] = g;
      RGB[j + 5] = b;
    }
}

// }}}
// {{{ ImageConversions::iyu12rgb (char *YUV, char *RGB, int NumPixels)

void
ImageConversions::iyu12rgb (char *YUV, char *RGB, int NumPixels) {
  int i, j;
  register unsigned char y0, y1, y2, y3, u, v;
  register unsigned char r, g, b;

  for (i = 0, j = 0; i < (NumPixels + (NumPixels >> 1)); i += 6, j += 12)
    {
      u  = (unsigned char) YUV[i + 0];
      y0 = (unsigned char) YUV[i + 1];
      y1 = (unsigned char) YUV[i + 2];
      v  = (unsigned char) YUV[i + 3];
      y2 = (unsigned char) YUV[i + 4];
      y3 = (unsigned char) YUV[i + 5];
      YUV2RGB (y0, u, v, r, g, b);
      RGB[j + 0] = r;
      RGB[j + 1] = g;
      RGB[j + 2] = b;
      YUV2RGB (y1, u, v, r, g, b);
      RGB[j + 3] = r;
      RGB[j + 4] = g;
      RGB[j + 5] = b;
      YUV2RGB (y2, u, v, r, g, b);
      RGB[j + 6] = r;
      RGB[j + 7] = g;
      RGB[j + 8] = b;
      YUV2RGB (y3, u, v, r, g, b);
      RGB[j + 9] = r;
      RGB[j + 10] = g;
      RGB[j + 11] = b;
    }
}

// }}}
// {{{ ImageConversions::iyu22rgb (char *YUV, char *RGB, int NumPixels)

void
ImageConversions::iyu22rgb (char *YUV, char *RGB, int NumPixels) {
  int i, j;
  register unsigned char y0, y1, u0, u1, v0, v1;
  register unsigned char r, g, b;

  for (i = 0, j = 0; i < (NumPixels + (NumPixels << 1)); i += 6, j += 6)
    {
      u0 = (unsigned char) YUV[i + 0];
      y0 = (unsigned char) YUV[i + 1];
      v0 = (unsigned char) YUV[i + 2];
      u1 = (unsigned char) YUV[i + 3];
      y1 = (unsigned char) YUV[i + 4];
      v1 = (unsigned char) YUV[i + 5];
      YUV2RGB (y0, u0, v0, r, g, b);
      RGB[j + 0] = r;
      RGB[j + 1] = g;
      RGB[j + 2] = b;
      YUV2RGB (y1, u1, v1, r, g, b);
      RGB[j + 3] = r;
      RGB[j + 4] = g;
      RGB[j + 5] = b;
    }
}

// }}}
// {{{ ImageConversions::y2rgb (char *YUV, char *RGB, int NumPixels) 

void
ImageConversions::y2rgb (char *YUV, char *RGB, int NumPixels) {
  int i, j;
  register unsigned char y;
  register unsigned char r, g, b;

  for (i = 0, j = 0; i < NumPixels; i++, j += 3)
    {
      y = (unsigned char) YUV[i];
      YUV2RGB (y, 0, 0, r, g, b);
      RGB[j + 0] = r;
      RGB[j + 1] = g;
      RGB[j + 2] = b;
    }
}

// }}}



/**** NEW FUNCTIONS ****/

// {{{ void ResizeVWImage()

template <class T>
void ResizeVWImage(T *image, 
		   const int unsigned new_width, const unsigned int new_height) {
   // Clear the output image if it's the wrong size
  if ((image->IsAlloced()) && ((image->GetWidth()!=new_width) || 
      (image->GetHeight()!=new_height))) {
    cerr << "Changing image size" << endl;
    image->FreeImageData();
  }
  // Alloc the right amount of storage
  if (!image->IsAlloced()) image->AllocImageData(new_width,new_height);
 
}

// }}}

// {{{ Convert a 4-way interlaced mono buffer to a VWImage

void ImageConversions::Interlaced4Buffer_to_VWImage(unsigned char *input_buffer, 
						    ImageBase<unsigned char, unsigned char*> *output_image,
						    unsigned int width, unsigned int height,
						    unsigned int xstep, unsigned int ystep,
						    unsigned int offset) {
   using namespace ImageConversions;
   if (dynamic_cast<ImageYUV<unsigned char>* >(output_image) )
     Interlaced4Buffer_to_VWImage(input_buffer,
				  dynamic_cast<ImageYUV<unsigned char>* >(output_image),
				  width, height, xstep, ystep, offset);
   if (dynamic_cast<ImageRGB<unsigned char>* >(output_image) )
     Interlaced4Buffer_to_VWImage(input_buffer,
				  dynamic_cast<ImageRGB<unsigned char>* >(output_image),
				  width, height, xstep, ystep, offset);
   if (dynamic_cast<ImageMono<unsigned char>* >(output_image) )
     Interlaced4Buffer_to_VWImage(input_buffer,
				  dynamic_cast<ImageMono<unsigned char>* >(output_image),
				  width, height, xstep, ystep, offset);
}

// }}}
// {{{ Convert a 4-way interlaced mono buffer to an RGB VWImage

void ImageConversions::Interlaced4Buffer_to_VWImage (unsigned char *input_buffer, 
						     ImageRGB<unsigned char> *output_image,
						     unsigned int width, unsigned int height,
						    unsigned int xstep, unsigned int ystep,
						     unsigned int offset) {
  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  cerr << "Interlaced4Buffer_to_VWImage(): Conversion to RGB not implemented yet." << endl;

}

// }}}
// {{{ Convert a 4-way interlaced mono buffer to a YUV VWImage

void ImageConversions::Interlaced4Buffer_to_VWImage (unsigned char *input_buffer, 
						     ImageYUV<unsigned char> *output_image,
						     unsigned int width, unsigned int height,
						    unsigned int xstep, unsigned int ystep,
						     unsigned int offset) {
  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);
  
  cerr << "Interlaced4Buffer_to_VWImage(): Conversion to YUV not implemented yet." << endl;

}

// }}}
// {{{ Convert a 4-way interlaced mono buffer to a mono VWImage

void ImageConversions::Interlaced4Buffer_to_VWImage (unsigned char *input_buffer, 
						     ImageMono<unsigned char> *output_image,
						     unsigned int width, unsigned int height,
						    unsigned int xstep, unsigned int ystep,
						     unsigned int offset) {
  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  // Destination is also mono, so no conversion, just a copy
  unsigned char* source=input_buffer;
  unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
  unsigned char y;
  const unsigned int rowend=4*(width - xstep*new_width); // compensate for odd subsampling
  const unsigned int rowskip=4*(ystep-1)*width + rowend;
  for (unsigned int row=0;row<new_height;row++) {
    for (unsigned int column=0;column<new_width;column++) {
      // read the raw buffer
      source+=offset;
      y = *source;
      source+=(4*xstep-offset);

      // Store in the image
      *dest++=static_cast<unsigned char>(y);
    }
    source+=rowskip;
  }

}

// }}}
// {{{ Convert a 4-way interlaced mono buffer to a HSV VWImage

void ImageConversions::Interlaced4Buffer_to_VWImage (unsigned char *input_buffer, 
						     ImageHSV<unsigned char> *output_image,
						     unsigned int width, unsigned int height,
						    unsigned int xstep, unsigned int ystep,
						     unsigned int offset) {
  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);
  
  cerr << "Interlaced4Buffer_to_VWImage(): Conversion to YUV not implemented yet." << endl;

}

// }}}

// {{{ Convert a BGRA buffer into a VWImage

 void ImageConversions::BGRABuffer_to_VWImage(unsigned char *input_buffer, 
					      ImageBase<unsigned char,unsigned char*> *output_image,
					      unsigned int width, unsigned int height,
					      unsigned int xstep, unsigned int ystep) {
   using namespace ImageConversions;
   if (dynamic_cast<ImageYUV<unsigned char>* >(output_image) )
     BGRABuffer_to_VWImage(input_buffer,
 			    dynamic_cast<ImageYUV<unsigned char>* >(output_image),
			     width, height, xstep, ystep);
   if (dynamic_cast<ImageRGB<unsigned char>* >(output_image) )
     BGRABuffer_to_VWImage(input_buffer,
			   dynamic_cast<ImageRGB<unsigned char>* >(output_image),
			   width, height, xstep, ystep);
   if (dynamic_cast<ImageMono<unsigned char>* >(output_image) )
     BGRABuffer_to_VWImage(input_buffer,
 			    dynamic_cast<ImageMono<unsigned char>* >(output_image),
			     width, height, xstep, ystep);
   if (dynamic_cast<ImageHSV<unsigned char>* >(output_image) )
     BGRABuffer_to_VWImage(input_buffer,
 			    dynamic_cast<ImageHSV<unsigned char>* >(output_image),
			     width, height, xstep, ystep);
 }

// }}}
// {{{ Convert a BGRA buffer into a YUV VWImage

void ImageConversions::BGRABuffer_to_VWImage(unsigned char *input_buffer, 
					     ImageYUV<unsigned char> *output_image,
					     unsigned int width, unsigned int height,
					     unsigned int xstep, unsigned int ystep) {

  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  // Need to resample
  unsigned char* source=input_buffer;
  unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
  register unsigned char r,g,b,y,u,v,alpha;
  const unsigned int rowskip=4*(ystep*width - xstep*new_width); // compensate for odd subsampling
  for (unsigned int row=0;row<new_height;row++) {
    for (unsigned int column=0;column<new_width;column++) {
      // read the raw buffer
      b = (*source++);
      g = (*source++);
      r = (*source++);
      alpha = (*source++);
      source+=4*(xstep-1);
      
      RGB2YUV(r,g,b,y,u,v);


      //cerr << "RGB->YUV: ("<<int(r)<<","<<int(g)<<","<<int(b)<<") ("<<int(y)<<","<<int(u)<<","<<int(v)<<")"<<endl;

      // Store in the image
      *dest++=y;
      *dest++=u;
      *dest++=v;
      *dest++=alpha;
    }
    source+=rowskip;
  } 

}

// }}}
// {{{ Convert a BGRA buffer into an RGB VWImage

void ImageConversions::BGRABuffer_to_VWImage(unsigned char *input_buffer, 
					     ImageRGB<unsigned char> *output_image,
					     unsigned int width,unsigned int height, 
					     unsigned int xstep, unsigned int ystep) {

  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  if ((xstep==1) && (ystep==1)) {
    // Do a direct copy
    unsigned char* source=input_buffer;
    unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
    unsigned int size=4*width*height;
    if (!memcpy(dest,source,size)) {
      cerr << "ImageConversions::BGRABuffer_to_VWImage: Memory copy failed." << endl;
    } 
  } else {
    // Need to resample
    unsigned char* source=input_buffer;
    unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
    unsigned char r,g,b,s;
    const unsigned int rowskip=4*(ystep*width - xstep*new_width); // compensate for odd subsampling
    for (unsigned int row=0;row<new_height;row++) {
      for (unsigned int column=0;column<new_width;column++) {
	// read the raw buffer
	b = *source++;
	g = *source++;
	r = *source++;
	s = *source++;
	source+=4*(xstep-1);
      
	// Store in the image
	*dest++=static_cast<unsigned char>(b);
	*dest++=static_cast<unsigned char>(g);
	*dest++=static_cast<unsigned char>(r);
	*dest++=static_cast<unsigned char>(s);
      }
      source+=rowskip;
    } 
  }
  
}

// }}}
// {{{ Convert a BGRA buffer into a Mono VWImage

void ImageConversions::BGRABuffer_to_VWImage(unsigned char *input_buffer, 
					     ImageMono<unsigned char> *output_image,
					     unsigned int width, unsigned int height, 
					     unsigned int xstep, unsigned int ystep) {

  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  // Need to resample
  unsigned char* source=input_buffer;
  unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
  register unsigned char r,g,b,s,y,u,v;
  const unsigned int rowskip=4*(ystep*width - xstep*new_width); // compensate for odd subsampling
  for (unsigned int row=0;row<new_height;row++) {
    for (unsigned int column=0;column<new_width;column++) {
      // read the raw buffer
      b = int(*source++);
      g = int(*source++);
      r = int(*source++);
      s = int(*source++);
      source+=4*(xstep-1);
      
      RGB2YUV(r,g,b,y,u,v);


      // Store in the image
      *dest++=static_cast<unsigned char>(y);
      }
    source+=rowskip;
  } 
  
}

// }}}
// {{{ Convert a BGRA buffer into a HSV VWImage

void ImageConversions::BGRABuffer_to_VWImage(unsigned char *input_buffer, 
					     ImageHSV<unsigned char> *output_image,
					     unsigned int width, unsigned int height,
					     unsigned int xstep, unsigned int ystep) {

  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  // Need to resample
  unsigned char* source=input_buffer;
  unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
  register unsigned char r,g,b,a,h,s,v;
  const unsigned int rowskip=4*(ystep*width - xstep*new_width); // compensate for odd subsampling
  for (unsigned int row=0;row<new_height;row++) {
    for (unsigned int column=0;column<new_width;column++) {
      // read the raw buffer
      b = (*source++);
      g = (*source++);
      r = (*source++);
      a = (*source++);
      source+=4*(xstep-1);
      
      RGB2HSV(r,g,b,h,s,v);

      // Store in the image
      *dest++=static_cast<unsigned char>(h);
      *dest++=static_cast<unsigned char>(s);
      *dest++=static_cast<unsigned char>(v);
      *dest++=static_cast<unsigned char>(a);
    }
    source+=rowskip;
  } 

}

// }}}

// {{{ Convert a YUV444 buffer into a VWImage
 void ImageConversions::YUV444Buffer_to_VWImage(char *input_buffer, 
						ImageBase<unsigned char,unsigned char*> *output_image,
					       unsigned int width, unsigned int height,
			  unsigned int xstep, unsigned int ystep) {
   using namespace ImageConversions;
   if (dynamic_cast<ImageYUV<unsigned char>* >(output_image) )
     YUV444Buffer_to_VWImage(input_buffer,
 			    dynamic_cast<ImageYUV<unsigned char>* >(output_image),
			     width, height,
			     xstep,ystep);
   if (dynamic_cast<ImageRGB<unsigned char>* >(output_image) )
     YUV444Buffer_to_VWImage(input_buffer,
 			    dynamic_cast<ImageRGB<unsigned char>* >(output_image),
			     width, height,
			     xstep,ystep);
   if (dynamic_cast<ImageMono<unsigned char>* >(output_image) )
     YUV444Buffer_to_VWImage(input_buffer,
 			    dynamic_cast<ImageMono<unsigned char>* >(output_image),
			     width, height,
			     xstep,ystep);
 }
// }}}
// {{{ Convert a YUV444 buffer into a YUV VWImage

void ImageConversions::YUV444Buffer_to_VWImage(char *input_buffer, 
					       ImageYUV<unsigned char> *output_image,
					       unsigned int width,unsigned int height,
			  unsigned int xstep, unsigned int ystep) {

  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  //  Data is packed as UYV.
  register unsigned char y,u,v;

  char* source=input_buffer;
  unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
  // compensate for odd subsampling
  const unsigned int rowskip=3*(ystep*width - xstep*new_width);
  for (unsigned int i=0;i<new_height;i++) {
    for (unsigned int j=0;j<new_width;j++) {
      // read the raw buffer
      u = *source++;
      y = *source++;
      v = *source++;
      source+=(xstep-1)*3;
      

      // Store in the image
      *dest++=static_cast<unsigned char>(y);
      *dest++=static_cast<unsigned char>(u);
      *dest++=static_cast<unsigned char>(v);
      *dest++=0; // Set alpha to zero
    }
    source+=rowskip;
  }
}

// }}}
// {{{ Convert a YUV444 buffer into an RGB VWImage

void ImageConversions::YUV444Buffer_to_VWImage(char *input_buffer, 
					       ImageRGB<unsigned char> *output_image,
					       unsigned int width,
					       unsigned int height,
			  unsigned int xstep, unsigned int ystep) {

  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  register unsigned char y,u,v,r, g, b;

  char* source=input_buffer;
  unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
  // compensate for odd subsampling
  const unsigned int rowskip=3*(ystep*width - xstep*new_width);

  for (unsigned int i=0;i<new_height;i++) {
    for (unsigned int j=0;j<new_width;j++) {
      //if (VERBOSE) cerr << "ImageConversions::YUV444Buffer_to_VWImage: ("<<j<<","<<i<<")"<<endl;
      // read the raw buffer
      u = (unsigned char)*source++;
      y = *source++;
      v = (unsigned char)*source++;
      source+=(xstep-1)*3;
      
      YUV2RGB (y, u, v, r, g, b);
 
      // Store in the image
      *dest++=static_cast<unsigned char>(b);
      *dest++=static_cast<unsigned char>(g);
      *dest++=static_cast<unsigned char>(r);
      *dest++=0; // Set alpha to zero
    }
    source+=rowskip;
  }
}

// }}}
// {{{ Convert a YUV444 buffer into a Mono VWImage

void ImageConversions::YUV444Buffer_to_VWImage(char *input_buffer, 
					       ImageMono<unsigned char> *output_image,
					       unsigned int width, unsigned int height,
					       unsigned int xstep, unsigned int ystep) {

  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  //  Data is packed as UYV.
  register unsigned char y,u,v;

  char* source=input_buffer;
  unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
  // compensate for odd subsampling
  const unsigned int rowskip=3*(ystep*width - xstep*new_width);
  for (unsigned int i=0;i<new_height;i++) {
    for (unsigned int j=0;j<new_width;j++) {
      // read the raw buffer
      u = *source++;
      y = *source++;
      v = *source++;
      source+=(xstep-1)*3;
      
      // Store in the image
      *dest++=static_cast<unsigned char>(y);
    }
    source+=rowskip;
  }
}

// }}}

// {{{ Convert a YUV422 buffer into a VWImage
 void ImageConversions::YUV422Buffer_to_VWImage(char *input_buffer, 
						ImageBase<unsigned char,unsigned char*> *output_image,
						unsigned int width, unsigned int height,
			  unsigned int xstep, unsigned int ystep) {
   using namespace ImageConversions;
   if (dynamic_cast<ImageYUV<unsigned char>* >(output_image) )
     YUV422Buffer_to_VWImage(input_buffer,
 			    dynamic_cast<ImageYUV<unsigned char>* >(output_image),
			     width, height,
			     xstep,ystep);
   if (dynamic_cast<ImageRGB<unsigned char>* >(output_image) )
     YUV422Buffer_to_VWImage(input_buffer,
 			    dynamic_cast<ImageRGB<unsigned char>* >(output_image),
			     width, height,
			     xstep,ystep);
   if (dynamic_cast<ImageMono<unsigned char>* >(output_image) )
     YUV422Buffer_to_VWImage(input_buffer,
 			    dynamic_cast<ImageMono<unsigned char>* >(output_image),
			     width, height,
			     xstep,ystep);
 }
// }}}
// {{{ Convert a YUV422 buffer into a YUV VWImage

void ImageConversions::YUV422Buffer_to_VWImage(char *input_buffer, 
					       ImageYUV<unsigned char> *output_image, 
					       unsigned int width, unsigned int height,
					       unsigned int xstep, unsigned int ystep) {

  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  if (VERBOSE) cerr << "YUV422 to ImageYUV<unsigned char>: width="
		    <<output_image->GetWidth()
		    <<" height="<<output_image->GetHeight()<<endl;

  char* source=input_buffer;
  unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
  // compensate for odd subsampling
  const unsigned int rowskip=2*(ystep*width - xstep*new_width);

  // First Y-value is centred on second element (raw data = U Y V Y)
  source++;

  // Because two adjacent pixels share colour information, a
  // seperate subsampling scheme is required for odd and even steps
  if (int(xstep/2)*2!=int(xstep)) {
    // Pick odd-spaced pixels
  if (VERBOSE) cerr << "Odd spaced YUV pixels." << endl;
   //  Data is packed as UYVY (ie. twice as many Y bits).
    register unsigned char y0,y1,u0,u1,v0,v1;
    bool odd=false;
    if (int(new_width)!=int(new_width/2)*2) {
      // Odd number of pixels across
      odd=true;
      new_width--;
    }

    for (unsigned int row=0;row<new_height;row++) {
      if (VERBOSE) cerr << "   Start "<<row<<" ("<<int(source)<<","<<int(dest)<<")." << endl;
      for (unsigned int column=0;column<(new_width/2);column++) { // NB: 2 at a time
	// read the raw buffer
	u0 = static_cast<unsigned char>(*(source-1));
	y0 = static_cast<unsigned char>(*(source));
	v0 = static_cast<unsigned char>(*(source+1));
 	// skip to next pixel
	source += 2*xstep;

	u1 = static_cast<unsigned char>(*(source-3)); // NB: U part is before previous Y value
 	v1 = static_cast<unsigned char>(*(source-1));
	y1 = static_cast<unsigned char>(*(source));
    	// skip to next pixel
	source += 2*xstep;

	// Store in the image
	*dest=y0; dest++;
	*dest=u0; dest++;
	*dest=v0; dest++;
	*dest=0; dest++; // Set alpha to zero
	
	*dest=y1; dest++;
	*dest=u1; dest++;
	*dest=v1; dest++;
	*dest=0; dest++; // Set alpha to zero
      }
      if (odd) {
	// line-length was odd, so need one final value
	// read the raw buffer
	if (VERBOSE) cerr << "Odd line length: final pixel." << endl;
	u0 = static_cast<unsigned char>(*(source-1));
	y0 = static_cast<unsigned char>(*(source));
	v0 = static_cast<unsigned char>(*(source+1));
	// skip to next pixel
	source += 2*xstep;
	// Store in the image
	*dest=y0; dest++;
	*dest=u0; dest++;
	*dest=v0; dest++;
	*dest=0;  dest++;// Set alpha to zero
      }
      source+=rowskip;
      if (VERBOSE) cerr << "   Done "<<row<<" ("<<int(source)<<","<<int(dest)<<")." << endl;
    }
     if (VERBOSE) cerr << "Done copying." << endl;
  } else {
    // Pick even-spaced pixels, so format is always same
  if (VERBOSE) cerr << "Even spaced YUV pixels." << endl;
    register unsigned char y0,u0,v0;
    for (unsigned int row=0;row<new_height;row++) {
      for (unsigned int column=0;column<new_width;column++) {
	// read the raw buffer
	u0 = static_cast<unsigned char>(*(source-1));
	y0 = static_cast<unsigned char>(*(source));
	v0 = static_cast<unsigned char>(*(source+1));
	// skip to next pixel
	source += 2*xstep;

	// Store in the image (doubled because original was 7-bit)
	*dest++=y0;
	*dest++=u0;
	*dest++=v0;
	*dest++=0; // Set alpha to zero
      }
      source+=rowskip;
    }
  }
  if (VERBOSE) cerr << "Done." << endl;
}

// }}}
// {{{ Convert a YUV422 buffer into a RGB VWImage

void ImageConversions::YUV422Buffer_to_VWImage(char *input_buffer, 
					       ImageRGB<unsigned char> *output_image,
					       unsigned int width,
					       unsigned int height,
			  unsigned int xstep, unsigned int ystep) {

  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  char* source=input_buffer;
  unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
  // compensate for odd subsampling
  const unsigned int rowskip=2*(ystep*width - xstep*new_width);

  // First Y-value is ceneterd on second element
  source++;

  // Because two adjacent pixels share colour information, a
  // seperate subsampling scheme is required for odd and even steps
  if (int(xstep/2)*2!=int(xstep)) {
    // Pick odd-spaced pixels
    //  Data is packed as UYVY (ie. twice as many Y bits).
    register unsigned char y0,y1,u0,u1,v0,v1;
    register unsigned char r0,r1,g0,g1,b0,b1;
    bool odd=false;
    if (int(new_width)!=int(new_width/2)*2) {
      // Odd number of pixels across
      odd=true;
      new_width--;
    }
    for (unsigned int i=0;i<new_height;i++) {
      for (unsigned int j=0;j<(new_width/2);j++) { // Note that 2 pixels are handled each time
	// read the raw buffer
	u0 = (unsigned char)source[-1];
	y0 = (unsigned char)*(source);
	v0 = (unsigned char)source[1];
	// skip to next pixel
	source += 2*xstep;

	u1 = static_cast<unsigned char>(*(source-3)); // NB: U part is before previous Y value
 	v1 = static_cast<unsigned char>(*(source-1));
	y1 = static_cast<unsigned char>(*(source));
      	// skip to next pixel
	source += 2*xstep;

	// The conversion
	YUV2RGB(y0,u0,v0,r0,g0,b0);
	YUV2RGB(y1,u1,v1,r1,g1,b1);

	// Store in the image
	*dest++=static_cast<unsigned char>(b0);
	*dest++=static_cast<unsigned char>(g0);
	*dest++=static_cast<unsigned char>(r0);
	*dest++=0; // Set alpha to zero
	
	*dest++=static_cast<unsigned char>(b1);
	*dest++=static_cast<unsigned char>(g1);
	*dest++=static_cast<unsigned char>(r1);
	*dest++=0; // Set alpha to zero
      }
      if (odd) {
	// line-length was odd, so need one final value
	// read the raw buffer
	u0 = (unsigned char)source[-1];
	y0 = (unsigned char)*(source);
	v0 = (unsigned char)source[1];
	// skip to next pixel
	source += 2*xstep;
	// The conversion
	YUV2RGB(y0,u0,v0,r0,g0,b0);
	// Store in the image
	*dest++=static_cast<unsigned char>(b0);
	*dest++=static_cast<unsigned char>(g0);
	*dest++=static_cast<unsigned char>(r0);
	*dest++=0; // Set alpha to zero
      }
      source+=rowskip;
    }
  } else {
    // Pick even-spaced pixels, so format is always same
    register unsigned char y,u,v,r,g,b;
    for (unsigned int i=0;i<new_height;i++) {
      for (unsigned int j=0;j<new_width;j++) {
	// read the raw buffer
	u = (unsigned char)source[-1];
	y = (unsigned char)*(source);
	v = (unsigned char)source[1];
	// skip to next pixel
	source += 2*xstep;

	// The conversion
	YUV2RGB(y,u,v,r,g,b);

	// Store in the image
	*dest++=static_cast<unsigned char>(b);
	*dest++=static_cast<unsigned char>(g);
	*dest++=static_cast<unsigned char>(r);
	*dest++=0; // Set alpha to zero
      }
      source+=rowskip;
    }
  }
}

// }}}
// {{{ Convert a YUV422 buffer into a mono VWImage

void ImageConversions::YUV422Buffer_to_VWImage(char *input_buffer, 
					       ImageMono<unsigned char> *output_image,
					       unsigned int width, unsigned int height,
					       unsigned int xstep, unsigned int ystep) {

  // In this case the packing is U-Y-V-Y, and as we only want
  // the Y part, its just every second element.


  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  char* source=input_buffer;
  unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
  // compensate for odd subsampling
  const unsigned int rowskip=2*(ystep*width - xstep*new_width);

  // First Y-value is ceneterd on second element
  source++;

  register unsigned char y0;
  for (unsigned int row=0;row<new_height;row++) {
    for (unsigned int column=0;column<new_width;column++) {
      // read the raw buffer
      y0 = (unsigned char)*(source);
      // skip to next pixel
      source += 2*xstep;
      
      // Store in the image
      *dest++=static_cast<unsigned char>(y0);
    }
    source+=rowskip;
  }
}

// }}}

// {{{ Convert a YUV411 buffer into a VWImage
void ImageConversions::YUV411Buffer_to_VWImage(char *input_buffer, 
					       ImageBase<unsigned char,unsigned char*> *output_image,
					       unsigned int width, unsigned int height,
					       unsigned int xstep, unsigned int ystep) {
   using namespace ImageConversions;
   if (dynamic_cast<ImageYUV<unsigned char>* >(output_image) )
     YUV411Buffer_to_VWImage(input_buffer,
 			    dynamic_cast<ImageYUV<unsigned char>* >(output_image),
			     width, height,
			     xstep,ystep);
   if (dynamic_cast<ImageRGB<unsigned char>* >(output_image) )
     YUV411Buffer_to_VWImage(input_buffer,
 			    dynamic_cast<ImageRGB<unsigned char>* >(output_image),
			     width, height,
			     xstep,ystep);
   if (dynamic_cast<ImageMono<unsigned char>* >(output_image) )
     YUV411Buffer_to_VWImage(input_buffer,
 			    dynamic_cast<ImageMono<unsigned char>* >(output_image),
			     width, height,
			     xstep,ystep);
 }
// }}}
// {{{ Convert a YUV411 buffer into a YUV VWImage

void ImageConversions::YUV411Buffer_to_VWImage(char *input_buffer, 
					       ImageYUV<unsigned char> *output_image, 
					       unsigned int width, unsigned int height,
					       unsigned int xstep, unsigned int ystep) {

  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);


  if ((xstep==1) && (ystep==1)) {
    // Direct copy, so no sampling issues
    if (VERBOSE) cerr << "YUV411 to ImageYUV<unsigned char>: width="
		      <<output_image->GetWidth()
		      <<" height="<<output_image->GetHeight()<<endl;
    
    char* source=input_buffer;
    unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
    
    register unsigned char y0,y1,y2,y3,u,v;
    
    for (unsigned int i=0;i<(width*height/4);i++) {
      u  = static_cast<unsigned char>(*source++);
      y0 = static_cast<unsigned char>(*source++);
      y1 = static_cast<unsigned char>(*source++);
      v  = static_cast<unsigned char>(*source++);
      y2 = static_cast<unsigned char>(*source++);
      y3 = static_cast<unsigned char>(*source++);
      
      // Store pixel 1
      (*dest) = y0;dest++; 
      (*dest) = u;dest++; 
      (*dest) = v;dest+=2; 
      // Store pixel 2
      (*dest) = y1;dest++; 
      (*dest) = u;dest++; 
      (*dest) = v;dest+=2; 
      // Store pixel 3
      (*dest) = y2;dest++; 
      (*dest) = u;dest++; 
      (*dest) = v;dest+=2; 
      // Store pixel 4
      (*dest) = y3;dest++; 
      (*dest) = u;dest++; 
      (*dest) = v;dest+=2; 
    }
  } else {

    // Subsampling a 411 image is bloody complicated, so we'll just create a temp
    // image and work from that
    VW::ImageYUV<unsigned char> temp_image(width,height);

    if (VERBOSE) cerr << "YUV411 to ImageYUV<unsigned char>: width="
		      <<output_image->GetWidth()
		      <<" height="<<output_image->GetHeight()<<endl;

    { // Make a temporary YUV image
      char* source=input_buffer;
      unsigned char* dest=static_cast<unsigned char*>(temp_image.GetRawBuffer());
    
      register unsigned char y0,y1,y2,y3,u,v;
    
      for (unsigned int i=0;i<(width*height/4);i++) {
	u  = static_cast<unsigned char>(*source++);
	y0 = static_cast<unsigned char>(*source++);
	y1 = static_cast<unsigned char>(*source++);
	v  = static_cast<unsigned char>(*source++);
	y2 = static_cast<unsigned char>(*source++);
	y3 = static_cast<unsigned char>(*source++);
    
	// Store pixel 1
	(*dest) = y0;dest++; 
	(*dest) = u;dest++; 
	(*dest) = v;dest+=2; 
	// Store pixel 2
	(*dest) = y1;dest++; 
	(*dest) = u;dest++; 
	(*dest) = v;dest+=2; 
	// Store pixel 3
	(*dest) = y2;dest++; 
	(*dest) = u;dest++; 
	(*dest) = v;dest+=2; 
	// Store pixel 4
	(*dest) = y3;dest++; 
	(*dest) = u;dest++; 
	(*dest) = v;dest+=2; 
      }
    }

    // Now re-sample the temp image
  
    // compensate for odd subsampling
    const unsigned int rowskip=4*(ystep*width - xstep*new_width);

    unsigned char* source=static_cast<unsigned char*>(temp_image.GetRawBuffer());
    unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
    for (unsigned int i=0;i<new_height;i++) {
      for (unsigned int j=0;j<new_width;j++) {
	(*dest) = (*source); dest++; source++;
	(*dest) = (*source); dest++; source++;
	(*dest) = (*source); dest++; source++;
	(*dest) = (*source); dest++; source++;
	source+=(xstep-1)*4;
      }
      source+=rowskip;
    }
  }
  if (VERBOSE) cerr << "Done." << endl;
}

// }}}
// {{{ Convert a YUV411 buffer into a RGB VWImage

void ImageConversions::YUV411Buffer_to_VWImage(char *input_buffer, 
					       ImageRGB<unsigned char> *output_image,
					       unsigned int width,
					       unsigned int height,
					       unsigned int xstep, unsigned int ystep) {
  
  
  cerr << "ImageConversions::YUV411Buffer_to_VWImage: RGB conversion not implemented yet!" << endl;
 
}

// }}}
// {{{ Convert a YUV411 buffer into a mono VWImage

void ImageConversions::YUV411Buffer_to_VWImage(char *input_buffer, 
					       ImageMono<unsigned char> *output_image,
					       unsigned int width, unsigned int height,
					       unsigned int xstep, unsigned int ystep) {

  cerr << "ImageConversions::YUV411Buffer_to_VWImage: Mono conversion not implemented yet!" << endl;
 
}

// }}}

// {{{ Convert a RGB buffer into a VWImage
void ImageConversions::RGBBuffer_to_VWImage(char *input_buffer, 
					       ImageBase<unsigned char,unsigned char*> *output_image,
					       unsigned int width, unsigned int height,
					       unsigned int xstep, unsigned int ystep) {
   using namespace ImageConversions;
   if (dynamic_cast<ImageYUV<unsigned char>* >(output_image) )
     RGBBuffer_to_VWImage(input_buffer,
 			    dynamic_cast<ImageYUV<unsigned char>* >(output_image),
			     width, height,
			     xstep,ystep);
   if (dynamic_cast<ImageRGB<unsigned char>* >(output_image) )
     RGBBuffer_to_VWImage(input_buffer,
 			    dynamic_cast<ImageRGB<unsigned char>* >(output_image),
			     width, height,
			     xstep,ystep);
   if (dynamic_cast<ImageMono<unsigned char>* >(output_image) )
     RGBBuffer_to_VWImage(input_buffer,
 			    dynamic_cast<ImageMono<unsigned char>* >(output_image),
			     width, height,
			     xstep,ystep);
 }
// }}}
// {{{ Convert a RGB buffer into a YUV VWImage

void ImageConversions::RGBBuffer_to_VWImage(char *input_buffer, 
					       ImageYUV<unsigned char> *output_image, 
					       unsigned int width, unsigned int height,
					       unsigned int xstep, unsigned int ystep) {
  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  //  Data is packed as RGB.
  register unsigned char r,g,b,y,u,v;

  char* source=input_buffer;
  unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
  // compensate for odd subsampling
  const unsigned int rowskip=3*(ystep*width - xstep*new_width);
  for (unsigned int i=0;i<new_height;i++) {
    for (unsigned int j=0;j<new_width;j++) {
      // read the raw buffer
      b = *source++;
      g = *source++;
      r = *source++;
      source+=(xstep-1)*3;
     
      // convert 
      RGB2YUV (r, g, b, y, u, v);

      // Store in the image
      *dest++=static_cast<unsigned char>(y);
      *dest++=static_cast<unsigned char>(u);
      *dest++=static_cast<unsigned char>(v);
      *dest++=0; // Set alpha to zero
    }
    source+=rowskip;
  }
}

// }}}
// {{{ Convert a RGB buffer into a RGB VWImage

void ImageConversions::RGBBuffer_to_VWImage(char *input_buffer, 
					       ImageRGB<unsigned char> *output_image,
					       unsigned int width,
					       unsigned int height,
					       unsigned int xstep, unsigned int ystep) {
  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  //  Data is packed as RGB.
  register unsigned char r,g,b;

  char* source=input_buffer;
  unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
  // compensate for odd subsampling
  const unsigned int rowskip=3*(ystep*width - xstep*new_width);
  for (unsigned int i=0;i<new_height;i++) {
    for (unsigned int j=0;j<new_width;j++) {
      // read the raw buffer
      r = *source++;
      g = *source++;
      b = *source++;
      source+=(xstep-1)*3;
     
      // Store in the image
      *dest++=static_cast<unsigned char>(b);
      *dest++=static_cast<unsigned char>(g);
      *dest++=static_cast<unsigned char>(r);
      *dest++=0; // Set alpha to zero
    }
    source+=rowskip;
  }
}

// }}}
// {{{ Convert a RGB buffer into a mono VWImage

void ImageConversions::RGBBuffer_to_VWImage(char *input_buffer, 
					       ImageMono<unsigned char> *output_image,
					       unsigned int width, unsigned int height,
					       unsigned int xstep, unsigned int ystep) {

   // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  //  Data is packed as RGB.
  register unsigned char r,g,b,y;

  char* source=input_buffer;
  unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
  // compensate for odd subsampling
  const unsigned int rowskip=3*(ystep*width - xstep*new_width);
  for (unsigned int i=0;i<new_height;i++) {
    for (unsigned int j=0;j<new_width;j++) {
      // read the raw buffer
      r = *source++;
      g = *source++;
      b = *source++;
      source+=(xstep-1)*3;
     
      // convert 
      RGB2Y (r, g, b, y);

      // Store in the image
      *dest++=static_cast<unsigned char>(y);
    }
    source+=rowskip;
  }
}

// }}}


// {{{ Convert a mono buffer into a VWImage
void ImageConversions::MonoBuffer_to_VWImage(char *input_buffer, 
					     ImageBase<unsigned char, unsigned char*> *output_image,
					     unsigned int width, unsigned int height,
					     unsigned int xstep, unsigned int ystep)
{
  using namespace ImageConversions;
   if (dynamic_cast<ImageYUV<unsigned char>* >(output_image) )
     MonoBuffer_to_VWImage(input_buffer,
			   dynamic_cast<ImageYUV<unsigned char>* >(output_image),
			   width, height,
			   xstep,ystep);
   if (dynamic_cast<ImageRGB<unsigned char>* >(output_image) )
     MonoBuffer_to_VWImage(input_buffer,
			   dynamic_cast<ImageRGB<unsigned char>* >(output_image),
			   width, height,
			   xstep,ystep);
   if (dynamic_cast<ImageMono<unsigned char>* >(output_image) )
     MonoBuffer_to_VWImage(input_buffer,
			   dynamic_cast<ImageMono<unsigned char>* >(output_image),
			   width, height,
			   xstep,ystep);
}
// }}}
// {{{ Convert a mono buffer into a YUV VWImage

void ImageConversions::MonoBuffer_to_VWImage(char *input_buffer, 
					     ImageYUV<unsigned char> *output_image,
					     unsigned int width, unsigned int height,
					     unsigned int xstep, unsigned int ystep)
{
  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

    char* source=input_buffer;
    unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
    // compensate for odd subsampling
    const unsigned int rowskip=(ystep*width - xstep*new_width);
    for (unsigned int i=0;i<new_height;i++) {
      for (unsigned int j=0;j<new_width;j++) {
	// read the raw buffer
	register unsigned char y;
	y = *source++;
	source+=(xstep-1);
	
	
	// Store in the image (colour values at middle)
	*dest++ = static_cast<unsigned char>(y);
	*dest++ = 127;
	*dest++ = 127;
	*dest++ = 0;
      }
      source+=rowskip;
    }
}

// }}}
// {{{ Convert a mono buffer into a RGB VWImage
void ImageConversions::MonoBuffer_to_VWImage(char *input_buffer, 
					     ImageRGB<unsigned char> *output_image,
					     unsigned int width, unsigned int height,
					     unsigned int xstep, unsigned int ystep)
{
  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

    char* source=input_buffer;
    unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
    // compensate for odd subsampling
    const unsigned int rowskip=(ystep*width - xstep*new_width);
    for (unsigned int i=0;i<new_height;i++) {
      for (unsigned int j=0;j<new_width;j++) {
	register unsigned char y;
	// read the raw buffer
	y = *source++;
	source+=(xstep-1);
	
	
	// Store in the image (for grey, R=G=B=Y)
	*dest++ = static_cast<unsigned char>(y);
	*dest++ = static_cast<unsigned char>(y);
	*dest++ = static_cast<unsigned char>(y);
	*dest++ = 0;
      }
      source+=rowskip;
    }
}
// }}}

void ImageConversions::MonoBuffer_to_VWImage(char *input_buffer, 
					     ImageMono<unsigned char> *output_image,
					     unsigned int width, unsigned int height,
					     unsigned int xstep, unsigned int ystep)
{
  // Calculate the final image size
  unsigned int new_width=width/xstep, new_height=height/ystep;
  ResizeVWImage(output_image,new_width,new_height);

  if ((xstep==1) && (ystep==1)) {
    // If no subsampling, a memcpy suffices
    memcpy(input_buffer,
	   output_image->GetRawBuffer(),
	   width*height);
  } else {
    // When subsampling, must go through image
    char* source=input_buffer;
    unsigned char* dest=static_cast<unsigned char*>(output_image->GetRawBuffer());
    // compensate for odd subsampling
    const unsigned int rowskip=(ystep*width - xstep*new_width);
    for (unsigned int i=0;i<new_height;i++) {
      for (unsigned int j=0;j<new_width;j++) {
	// read the raw buffer
	register unsigned char y;
	y = *source++;
	source+=(xstep-1);
	
	
	// Store in the image
	*dest++=static_cast<unsigned char>(y);
      }
      source+=rowskip;
    }
  }
}


void ImageConversions::VWImage_to_RGBBuffer(const ImageBase<unsigned char,unsigned char*> *input_image,
					    unsigned char *output_buffer) {
   using namespace ImageConversions;
   if (dynamic_cast<const ImageMono<unsigned char>* >(input_image) )
     VWImage_to_RGBBuffer(dynamic_cast<const ImageMono<unsigned char>* >(input_image),
			  output_buffer);
   if (dynamic_cast<const ImageRGB<unsigned char>* >(input_image) )
     VWImage_to_RGBBuffer(dynamic_cast<const ImageRGB<unsigned char>* >(input_image),
			  output_buffer);
   if (dynamic_cast<const ImageYUV<unsigned char>* >(input_image) )
     VWImage_to_RGBBuffer(dynamic_cast<const ImageYUV<unsigned char>* >(input_image),
			  output_buffer);

}


void ImageConversions::VWImage_to_RGBBuffer(const ImageMono<unsigned char> *input_image,
					    unsigned char *output_buffer) {
  unsigned char* dest=output_buffer;
  unsigned char* source=static_cast<unsigned char*>(input_image->GetRawBuffer());
  unsigned char y;
  for (unsigned int i=0;i<(input_image->GetWidth()*input_image->GetHeight());i++) {
    y=*(source++);
    *(dest++)=y;
    *(dest++)=y;
    *(dest++)=y;
  }

}


void ImageConversions::VWImage_to_RGBBuffer(const ImageRGB<unsigned char> *input_image,
					    unsigned char *output_buffer) {
  unsigned char* dest=output_buffer;
  unsigned char* source=static_cast<unsigned char*>(input_image->GetRawBuffer());
  unsigned char r,g,b,a;
  for (unsigned int i=0;i<(input_image->GetWidth()*input_image->GetHeight());i++) {
    b=*(source++);
    g=*(source++);
    r=*(source++);
    a=*(source++);
    // BGRA -> RGB - note the order change
    *(dest++)=r;
    *(dest++)=g;
    *(dest++)=b;
  }
}


void ImageConversions::VWImage_to_RGBBuffer(const ImageYUV<unsigned char> *input_image,
					    unsigned char *output_buffer) {

  //cerr << "Converting YUV to RGB packed" << endl;

  unsigned char* dest=output_buffer;
  unsigned char* source=static_cast<unsigned char*>(input_image->GetRawBuffer());
  unsigned char r,g,b;
  unsigned char y,u,v,a;
  for (unsigned int i=0;i<(input_image->GetWidth()*input_image->GetHeight());i++) {
    y=*(source++);
    u=*(source++);
    v=*(source++);
    a=*(source++);

    // Convert
    YUV2RGB(y,u,v,r,g,b);
    
    *(dest++)=r;
    *(dest++)=g;
    *(dest++)=b;
  }
  //cerr << "  Done" << endl;
}







/**********************************************/
/*             RGBA -> VWImage                */
/**********************************************/


void ImageConversions::VWImage_to_RGBABuffer(const ImageBase<unsigned char,unsigned char*> *input_image,
					    unsigned char *output_buffer) {
   using namespace ImageConversions;
   if (dynamic_cast<const ImageMono<unsigned char>* >(input_image) )
     VWImage_to_RGBABuffer(dynamic_cast<const ImageMono<unsigned char>* >(input_image),
			  output_buffer);
   if (dynamic_cast<const ImageRGB<unsigned char>* >(input_image) )
     VWImage_to_RGBABuffer(dynamic_cast<const ImageRGB<unsigned char>* >(input_image),
			  output_buffer);
   if (dynamic_cast<const ImageYUV<unsigned char>* >(input_image) )
     VWImage_to_RGBABuffer(dynamic_cast<const ImageYUV<unsigned char>* >(input_image),
			  output_buffer);

}


void ImageConversions::VWImage_to_RGBABuffer(const ImageMono<unsigned char> *input_image,
					    unsigned char *output_buffer) {
  unsigned char* dest=output_buffer;
  unsigned char* source=static_cast<unsigned char*>(input_image->GetRawBuffer());
  unsigned char y;
  for (unsigned int i=0;i<(input_image->GetWidth()*input_image->GetHeight());i++) {
    y=*(source++);
    *(dest++)=y;
    *(dest++)=y;
    *(dest++)=y;
    *(dest++)=0;
  }

}


void ImageConversions::VWImage_to_RGBABuffer(const ImageRGB<unsigned char> *input_image,
					    unsigned char *output_buffer) {
  unsigned char* dest=output_buffer;
  unsigned char* source=static_cast<unsigned char*>(input_image->GetRawBuffer());
  unsigned char r,g,b,a;
  for (unsigned int i=0;i<(input_image->GetWidth()*input_image->GetHeight());i++) {
    b=*(source++);
    g=*(source++);
    r=*(source++);
    a=*(source++);
    // BGRA -> RGB - note the order change
    *(dest++)=r;
    *(dest++)=g;
    *(dest++)=b;
    *(dest++)=a;
  }
}


void ImageConversions::VWImage_to_RGBABuffer(const ImageYUV<unsigned char> *input_image,
					    unsigned char *output_buffer) {

  //cerr << "Converting YUV to RGB packed" << endl;

  unsigned char* dest=output_buffer;
  unsigned char* source=static_cast<unsigned char*>(input_image->GetRawBuffer());
  unsigned char r,g,b;
  unsigned char y,u,v,a;
  for (unsigned int i=0;i<(input_image->GetWidth()*input_image->GetHeight());i++) {
    y=*(source++);
    u=*(source++);
    v=*(source++);
    a=*(source++);

    // Convert
    YUV2RGB(y,u,v,r,g,b);
    
    *(dest++)=r;
    *(dest++)=g;
    *(dest++)=b;
    *(dest++)=a;
  }
  //cerr << "  Done" << endl;
}

