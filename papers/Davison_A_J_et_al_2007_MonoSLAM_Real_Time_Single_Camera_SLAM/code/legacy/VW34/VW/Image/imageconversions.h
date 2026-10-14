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
 

#ifndef _ImageConversions_h_
#define _ImageConversions_h_

#include "pixelconversions.h"
#include "imagebase.h"
#include "imagemono.h"
#include "imagergb.h"
#include "imageyuv.h"
#include "imagehsv.h"


namespace VW {







  /**
   * @ingroup Image
   * @brief   Whole buffer (or image) conversions
   */
namespace ImageConversions {


  /** @name Buffer to image conversions. */
  //@{


  /** Convert a 4-way interlaced buffer to a VWImage. 
   * This just calls the overloaded functions below according to the type 
   * of the VWImage. 
   */
  void Interlaced4Buffer_to_VWImage(unsigned char *input_buffer, 
	  VW::ImageBase<unsigned char, unsigned char*> *output_image,
				    unsigned int width, unsigned int height,
				    unsigned int xstep=1, unsigned int ystep=1,
				    unsigned int offset=0);
  void Interlaced4Buffer_to_VWImage (unsigned char *input_buffer, 
				     ImageRGB<unsigned char> *output_image,
				     unsigned int width, unsigned int height,
				    unsigned int xstep=1, unsigned int ystep=1,
				    unsigned int offset=0);
  void Interlaced4Buffer_to_VWImage (unsigned char *input_buffer, 
				     ImageYUV<unsigned char> *output_image,
				     unsigned int width, unsigned int height,
				    unsigned int xstep=1, unsigned int ystep=1,
				    unsigned int offset=0);
  void Interlaced4Buffer_to_VWImage (unsigned char *input_buffer, 
				     ImageMono<unsigned char> *output_image,
				     unsigned int width, unsigned int height,
				    unsigned int xstep=1, unsigned int ystep=1,
				    unsigned int offset=0);
  void Interlaced4Buffer_to_VWImage (unsigned char *input_buffer, 
				     ImageHSV<unsigned char> *output_image,
				     unsigned int width, unsigned int height,
				    unsigned int xstep=1, unsigned int ystep=1,
				    unsigned int offset=0);

  /** Convert an BGRA buffer (eg. Meteor) to VWImage. 
   * This just calls the overloaded functions below according to the type 
   * of the VWImage. 
   */
  void BGRABuffer_to_VWImage(unsigned char *input_buffer, 
			     ImageBase<unsigned char, unsigned char*> *output_image,
			     unsigned int width,unsigned int height, 
			     unsigned int xstep=1, unsigned int ystep=1);
  void BGRABuffer_to_VWImage(unsigned char *input_buffer, 
			     ImageYUV<unsigned char> *output_image,
			     unsigned int width,unsigned int height, 
			     unsigned int xstep=1, unsigned int ystep=1);
  void BGRABuffer_to_VWImage(unsigned char *input_buffer, 
			     ImageRGB<unsigned char> *output_image,
			     unsigned int width,unsigned int height, 
			     unsigned int xstep=1, unsigned int ystep=1);
  void BGRABuffer_to_VWImage(unsigned char *input_buffer, 
			     ImageMono<unsigned char> *output_image,
			     unsigned int width,unsigned int height, 
			     unsigned int xstep=1, unsigned int ystep=1);
  void BGRABuffer_to_VWImage(unsigned char *input_buffer, 
			     ImageHSV<unsigned char> *output_image,
			     unsigned int width,unsigned int height, 
			     unsigned int xstep=1, unsigned int ystep=1);

  /** Convert a YUV444 image to VWImage. 
   * This just calls the overloaded functions below according to the type 
   * of the VWImage. 
   */
 void YUV444Buffer_to_VWImage(char *input_buffer, 
			      ImageBase<unsigned char, unsigned char*> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);
 void YUV444Buffer_to_VWImage(char *input_buffer, 
			      ImageYUV<unsigned char> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);
 void YUV444Buffer_to_VWImage(char *input_buffer, 
			      ImageRGB<unsigned char> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);
 void YUV444Buffer_to_VWImage(char *input_buffer, 
			      ImageMono<unsigned char> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);

  /** Convert a YUV422 image to VWImage. 
   * This just calls the overloaded functions below according to the type 
   * of the VWImage. 
   */
 void YUV422Buffer_to_VWImage(char *input_buffer, 
			      ImageBase<unsigned char,unsigned char*> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);
 void YUV422Buffer_to_VWImage(char *input_buffer, 
			      ImageYUV<unsigned char> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);
 void YUV422Buffer_to_VWImage(char *input_buffer, 
			      ImageRGB<unsigned char> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);
 void YUV422Buffer_to_VWImage(char *input_buffer, 
			      ImageMono<unsigned char> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);
  /** Convert a YUV411 image to VWImage. 
   * This just calls the overloaded functions below according to the type 
   * of the VWImage. 
   */
 void YUV411Buffer_to_VWImage(char *input_buffer, 
			      ImageBase<unsigned char,unsigned char*> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);
 void YUV411Buffer_to_VWImage(char *input_buffer, 
			      ImageYUV<unsigned char> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);
 void YUV411Buffer_to_VWImage(char *input_buffer, 
			      ImageRGB<unsigned char> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);
 void YUV411Buffer_to_VWImage(char *input_buffer, 
			      ImageMono<unsigned char> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);

  /** Convert a RGB image to VWImage. 
   * This just calls the overloaded functions below according to the type 
   * of the VWImage. 
   */
 void RGBBuffer_to_VWImage(char *input_buffer, 
			      ImageBase<unsigned char,unsigned char*> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);
 void RGBBuffer_to_VWImage(char *input_buffer, 
			      ImageYUV<unsigned char> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);
 void RGBBuffer_to_VWImage(char *input_buffer, 
			      ImageRGB<unsigned char> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);
 void RGBBuffer_to_VWImage(char *input_buffer, 
			      ImageMono<unsigned char> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);

  /** Convert a mono buffer to VWImage. 
   * This just calls the overloaded functions below according to the type 
   * of the VWImage. 
   */
 void MonoBuffer_to_VWImage(char *input_buffer, 
			    ImageBase<unsigned char, unsigned char*> *output_image,
			    unsigned int width, unsigned int height,
			    unsigned int xstep=1, unsigned int ystep=1);
 void MonoBuffer_to_VWImage(char *input_buffer, 
			      ImageYUV<unsigned char> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);
 void MonoBuffer_to_VWImage(char *input_buffer, 
			      ImageRGB<unsigned char> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);
 void MonoBuffer_to_VWImage(char *input_buffer, 
			      ImageMono<unsigned char> *output_image,
			      unsigned int width, unsigned int height,
			      unsigned int xstep=1, unsigned int ystep=1);


  //@}

  /** @name Image to buffer conversions. */
  //@{

  // The following convert to a packed RGB buffer, as required by GDK.
  // Note that the buffer is not re-sized, so make sure the data will fit
  // before calling!
  void VWImage_to_RGBBuffer(const ImageBase<unsigned char,unsigned char*> *input_image,
			    unsigned char *output_buffer);
  void VWImage_to_RGBBuffer(const ImageMono<unsigned char> *input_image,
			    unsigned char *output_buffer);
  void VWImage_to_RGBBuffer(const ImageRGB<unsigned char> *input_image,
			    unsigned char *output_buffer);
  void VWImage_to_RGBBuffer(const ImageYUV<unsigned char> *input_image,
			    unsigned char *output_buffer);
			    
  // The following convert to a packed RGBA buffer, as required by GDK.
  // Note that the buffer is not re-sized, so make sure the data will fit
  // before calling!
  void VWImage_to_RGBABuffer(const ImageBase<unsigned char,unsigned char*> *input_image,
			    unsigned char *output_buffer);
  void VWImage_to_RGBABuffer(const ImageMono<unsigned char> *input_image,
			    unsigned char *output_buffer);
  void VWImage_to_RGBABuffer(const ImageRGB<unsigned char> *input_image,
			    unsigned char *output_buffer);
  void VWImage_to_RGBABuffer(const ImageYUV<unsigned char> *input_image,
			    unsigned char *output_buffer);
			    
  //@}

  /** @name Old stuff */
  //@{

void
y41p2yuyv (unsigned char *src, unsigned char *dest, int NumPixels);

void
iyu12yuy2 (unsigned char *src, unsigned char *dest, int NumPixels);

void
iyu22yuy2 (unsigned char *src, unsigned char *dest, int NumPixels);

void
y2yuy2 (unsigned char *src, unsigned char *dest, int NumPixels);

void
rgb2yuy2 (char *RGB, char *YUV, int NumPixels);

void
uyvy2rgb (char *YUV, char *RGB, int NumPixels);

void
yuy22rgb (char *YUV, char *RGB, int NumPixels);

void
iyu12rgb (char *YUV, char *RGB, int NumPixels);

void
iyu22rgb (char *YUV, char *RGB, int NumPixels);

void
y2rgb (char *YUV, char *RGB, int NumPixels);

  //@}
} // End ImageConversions namespace











} // End VW namespace




#endif
