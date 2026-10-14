/** @file imageio_ppm.tpp
 *
 *              (c) Copyright University of Cambridge
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
#ifndef _vw_imageio_ppm_tpp_
#define _vw_imageio_ppm_tpp_


#include "pixelconversions.h"
#include "imagemono.h"
#include "imagergb.h"
#include "imageyuv.h"
#include "imagehsv.h"
#include "imagergbplanes.h"

#include "imagecopy.tpp"

#include "imageio_ppm.h"

extern "C" {
#include <stdio.h>
#include <fcntl.h>
}

#include <iostream>

using namespace std;

namespace VW {

//////////////////////////////////////////////////////////

/** Swallow a single white space (ie. space, tab, newline or linefeed) 
 * character. If the next character in the file is not a white space char
 * then the filepointer is left untouched.
 */
static inline void _SwallowWhiteSpace(FILE* filepointer)
{
  char c = fgetc(filepointer);
  const bool iswhite = (   c == ' ' 
			   || c == '\t' 
			   || c == 10 
			   || c == 13);
  if ( !iswhite ) {
    // Rewind by one byte
    fseek(filepointer, -1, SEEK_CUR);
  }
}

/**
Strip leading whitespace and PNM comments from a file. Comments start with
a "#" and finisg with a newline.
*/
static bool _PNMSkipCommentsAndWhitespace(FILE* filepointer)
{
  if (filepointer == NULL)
  {
    cerr << "VW::ReadPPMImage(): file stream NULL\n";
    return false;
  }

  int c;

  while(true)
  {
    c = fgetc(filepointer);

    if(c == EOF)
    {
      cerr <<
        "VW::ReadPPMImage(): unexpected end of file when reading header\n";
      return false;
    }
    else if(c == '#') //It is comment, so read the rest of the line
    {
      do
      {
        c = fgetc(filepointer);
        if(c == EOF) 
        {
          cerr <<
            "VW::ReadPPMImage(): unexpected end of file when reading header\n";
          return false;
        }
      }
      while(c != '\n');
    }
    else if(!isspace(c))
    {
      // Rewind by one byte
      // fseek(filepointer, -1, SEEK_CUR);
      ungetc( c, filepointer );
      return true;
    }
  }
}


/**
Read the header of a PNM file to find its type, size and maximum value.
A PNM file has the following format:
-# A magic number identifying the file type e.g. a PPM is "P6"
-# Whitespace (blanks, tabs, carriage returns and line feeds)
-# A width in ASCII decimal
-# Whitespace
-# A height in ASCII decimal
-# Whitespace
-# The maximum colour value (maxval) in ASCII decimal (can be between 1 and
65535, but we only accept numbers up to 255)
-# Newline or other single whitespace character
-# The image data in raster format.
Comments can also be included in the header: chracters from a '#' until the
next end-of-line that occur before the maxval are comments and are ignored
@param filepointer The file to read
@param ptype The magic number (without the 'P')
@param w The image width
@param h The image height
@param maxval The maximum pixel value (maxval)
@returns <code>true</code> on success, <code>false</code> otherwise
*/
static inline bool _PNMReadHeader(
  FILE* filepointer, unsigned int& ptype, unsigned int& w, unsigned int& h,
  unsigned int& maxval)
{
  if (filepointer == NULL)
  {
    cerr << "VW::ReadPPMImage(): file stream NULL\n";
    return false;
  }
  
  _PNMSkipCommentsAndWhitespace(filepointer);
  
  if (fscanf(filepointer, "P%d", &ptype) != 1)
  {
    cerr << "VW::ReadPPMImage(): Bad magic number for PNM file\n";
    return false;
  }

  _PNMSkipCommentsAndWhitespace(filepointer);
  
  if (fscanf(filepointer, "%d", &w) != 1)
  {
    cerr << "VW::ReadPPMImage(): Could not read width\n";
    return false;
  }
  
  _PNMSkipCommentsAndWhitespace(filepointer);

  if (fscanf(filepointer, "%d", &h) != 1)
  {
    cerr << "VW::ReadPPMImage(): Could not read height\n";
    return false;
  }
  
  _PNMSkipCommentsAndWhitespace(filepointer);
  
  if (fscanf(filepointer, "%d", &maxval) != 1)
  {
    cerr << "VW::ReadPPMImage(): Could not read maxval\n";
    return false;
  }
  
  // Check maxval is as expected
  if (maxval<=0)
  {
    cerr << "VW::ReadPPMImage(): maxval is too low (" << maxval << ")\n";
    return false;
  }
  else if (maxval>255)
  {
    cerr << "VW::ReadPPMImage(): maxval is too high (" << maxval << ")\n";
    return false;
  }

  // Eric's 'white-space bug' fix
  _SwallowWhiteSpace(filepointer);

  return true;
} 


/** Try to open a file pointer from a filename */
static FILE* _ppmio_OpenInputFile(const std::string& filename)
{
  return fopen ( filename.c_str(), "r" );
}

/** Try to open a file pointer from a filename */
static FILE* _ppmio_OpenOutputFile(const std::string& filename)
{
   return fopen ( filename.c_str(), "w" );
}





//////////////////////////////////////////////////////////
// Default versions
  template <class valType>
  bool ReadPPMImage(const std::string& filename,
                    VW::ImageMono<valType>& image)
  {
		FILE* file = _ppmio_OpenInputFile(filename);
		if (file==NULL) return false;
		bool success = VW::ReadPPMImage(file, image);
		fclose(file);
		return success;
  }

  template <class valType>
  bool ReadPPMImage(const std::string& filename,
                    VW::ImageYUV<valType>& image)
  {
		FILE* file = _ppmio_OpenInputFile(filename);
		if (file==NULL) return false;
		bool success = VW::ReadPPMImage(file, image);
		fclose(file);
		return success;
  }

  template <class valType>
  bool ReadPPMImage(const std::string& filename,
                    VW::ImageHSV<valType>& image)
  {
		FILE* file = _ppmio_OpenInputFile(filename);
		if (file==NULL) return false;
		bool success = VW::ReadPPMImage(file, image);
		fclose(file);
		return success;
  }

  template <class valType>
  bool ReadPPMImage(const std::string& filename,
                    VW::ImageRGB<valType>& image)
  {
		FILE* file = _ppmio_OpenInputFile(filename);
		if (file==NULL) return false;
		bool success = VW::ReadPPMImage(file, image);
		fclose(file);
		return success;
  }

  template <class valType>
  bool ReadPPMImage(const std::string& filename,
                    VW::ImageRGBPlanes<valType>& image)
   {
		FILE* file = _ppmio_OpenInputFile(filename);
		if (file==NULL) return false;
		bool success = VW::ReadPPMImage(file, image);
		fclose(file);
		return success;
  }
  
  
    template <class valType>
  bool WritePPMImage(const std::string& filename,
                     const VW::ImageMono<valType>& image)
   {
     FILE* file = _ppmio_OpenOutputFile(filename);
     if (file==NULL) return false;
     bool success = VW::WritePPMImage(file, image);
     fclose(file);
     return success;
  }

  template <class valType>
  bool WritePPMImage(const std::string& filename,
                     const VW::ImageYUV<valType>& image)
  {
		FILE* file = _ppmio_OpenOutputFile(filename);
		if (file==NULL) return false;
		bool success = VW::WritePPMImage(file, image);
		fclose(file);
		return success;
  }

  template <class valType>
  bool WritePPMImage(const std::string& filename,
                     const VW::ImageHSV<valType>& image)
  {
		FILE* file = _ppmio_OpenOutputFile(filename);
		if (file==NULL) return false;
		bool success = VW::WritePPMImage(file, image);
		fclose(file);
		return success;
  }

  template <class valType>
  bool WritePPMImage(const std::string& filename,
                     const VW::ImageRGB<valType>& image)
{
		FILE* file = _ppmio_OpenOutputFile(filename);
		if (file==NULL) return false;
		bool success = VW::WritePPMImage(file, image);
		fclose(file);
		return success;
  }

  template <class valType>
  bool WritePPMImage(const std::string& filename,
                     const VW::ImageRGBPlanes<valType>& image)
     {
		FILE* file = _ppmio_OpenOutputFile(filename);
		if (file==NULL) return false;
		bool success = VW::WritePPMImage(file, image);
		fclose(file);
		return success;
  }

  
  
  
  
  
  
  
  
  
                    
template <class valType>
inline bool ReadPPMImage(FILE* filepointer,
			 VW::ImageMono<valType>& image)
{
  // Just read as uchar then do VW::ImageCopy!
  VW::ImageMono<unsigned char> tmp_img;
  if (ReadPPMImage(filepointer, tmp_img)) {
    VW::ImageCopy(tmp_img,image);
    return true;
  }
  return false;
}

template <class valType>
inline bool ReadPPMImage(FILE* filepointer,
                      VW::ImageRGB<valType>& image)
{
  // Just read as uchar then do VW::ImageCopy!
  VW::ImageRGB<unsigned char> tmp_img;
  if (ReadPPMImage(filepointer, tmp_img)) {
    VW::ImageCopy(tmp_img,image);
    return true;
  }
  return false;
}

template <class valType>
inline bool ReadPPMImage(FILE* filepointer,
                      VW::ImageYUV<valType>& image)
{
  // Just read as uchar then do VW::ImageCopy!
  VW::ImageYUV<unsigned char> tmp_img;
  if (ReadPPMImage(filepointer, tmp_img)) {
    VW::ImageCopy(tmp_img,image);
    return true;
  }
  return false;
}

template <class valType>
inline bool ReadPPMImage(FILE* filepointer,
                      VW::ImageHSV<valType>& image)
{
  // Just read as uchar then do VW::ImageCopy!
  VW::ImageHSV<unsigned char> tmp_img;
  if (ReadPPMImage(filepointer, tmp_img)) {
    VW::ImageCopy(tmp_img,image);
    return true;
  }
  return false;
}


template <class valType>
inline bool WritePPMImage(FILE* filepointer,
			  const VW::ImageMono<valType>& image)
{
  cerr << "WritePPMImage (mono): Warning - floating point data being clipped and written to unsigned char\n";

  // Note that it's faster to convert in one go than to do it pixel by pixel
  VW::ImageMono<unsigned char> tmp;
  VW::ImageCopy(image, tmp);
  VW::WritePPMImage(filepointer,tmp);
  return (true);
}

template <class valType>
inline bool WritePPMImage(FILE* filepointer,
                      const VW::ImageYUV<valType>& image)
{
  cerr << "WritePPMImage (YUV): Warning - floating point data being clipped and written to unsigned char\n";

  // Note that it's faster to convert in one go than to do it pixel by pixel
  VW::ImageYUV<unsigned char> tmp;
  VW::ImageCopy(image, tmp);
  VW::WritePPMImage(filepointer,tmp);
  return (true);
}

template <class valType>
inline bool WritePPMImage(FILE* filepointer,
                      const VW::ImageHSV<valType>& image)
{
  cerr << "WritePPMImage (HSV): Warning - floating point data being clipped and written to unsigned char\n";

  // Note that it's faster to convert in one go than to do it pixel by pixel
  VW::ImageHSV<unsigned char> tmp;
  VW::ImageCopy(image, tmp);
  VW::WritePPMImage(filepointer,tmp);
  return (true);
}

template <class valType>
inline bool WritePPMImage(FILE* filepointer,
                      const VW::ImageRGB<valType>& image)
{
  cerr << "WritePPMImage (RGB): Warning - floating point data being clipped and written to unsigned char\n";

  // Note that it's faster to convert in one go than to do it pixel by pixel
  VW::ImageRGB<unsigned char> tmp;
  VW::ImageCopy(image, tmp);
  VW::WritePPMImage(filepointer,tmp);
  return (true);
}










///////////////////////////////////////////////////////////
// Reading/Writing unsigned chars can be made efficient

template <> inline bool 
ReadPPMImage<unsigned char>(FILE* filepointer,
			    VW::ImageMono<unsigned char>& image)
{
  cerr << "VW::ReadPPMImage(mono)\n";
  typedef unsigned char Type;
  unsigned char pixbuf[4];
  unsigned int w, h, maxval, ptype;
  int val;
  unsigned int size;

  if(_PNMReadHeader(filepointer, ptype, w, h, maxval) == false)
    return false;	
	
  if (ptype != 2 && ptype != 5 && ptype != 3 && ptype != 6) {
    cerr << "VW::ReadPPMImage(): bad type (P" << ptype <<
      ") for input image (legal: P2,P5,P3,P6)\n";
    return false;
  }
	
  if (image.IsAlloced()) {
    if (w != image.GetWidth() || h != image.GetHeight()) {
      cerr << "VW::ReadPPMImage(): Image size mismatch\n";
      return false;
    }
  } else {
    image.AllocImageData(w, h);
  }
  size = image.GetWidth() * image.GetHeight();
	
  switch (ptype) {
  case 2: { // Mono ASCII
      VW::PixelMono<Type>* ptr = image.GetImageBuffer();
      for (unsigned int i = 0; i < size; i++,ptr++) {
        if (fscanf(filepointer, "%d", &val) != 1) {
          cerr << "VW::ReadPPMImage(): image data could not be read\n";
          return false;
        }
        ptr->y = val;
      }
      break;
    }
  case 5: { // Mono binary
      if (fread(image.GetRawBuffer(), 1, size, filepointer) != size) {
        cerr << "VW::ReadPPMImage(): image data could not be read\n";
        return false;
      }
      break;
    }
  case 3: { // RGB ASCII
      int r, g, b;
      cerr << "VW::ReadPPMImage(): Warning - converting RGB (ASCII) image data into ImageMono\n";
      VW::PixelMono<Type>* ptr = image.GetImageBuffer();
      for (unsigned int j = 0; j < size; j++,ptr++) {
        if (fscanf(filepointer, "%d%d%d", &r, &g, &b) != 3) {
          cerr << "VW::ReadPPMImage(): image data could not be read\n";
          return false;
        }
        VW::ImageConversions::RGB2Y(r,g,b,ptr->y);
      }
      break;
    }
  case 6: { // RGB binary
      cerr << "VW::ReadPPMImage(): Warning - converting RGB (binary) image data into ImageMono\n";
      VW::PixelMono<Type>* ptr = image.GetImageBuffer();
      for (unsigned int j = 0; j < size; j++,ptr++) {
        if (fread(pixbuf, 1, 3, filepointer) != 3) {
          cerr << "VW::ReadPPMImage(): image data could not be read\n";
          return false;
        }
        VW::ImageConversions::RGB2Y(pixbuf[0],pixbuf[1],pixbuf[2],ptr->y);
      }
      break;
    }
  default:
    // should never get here
    ;
  }
	
  return (true);

}



template <> inline bool 
ReadPPMImage<unsigned char>(FILE* filepointer,
				       VW::ImageYUV<unsigned char>& image)
{
  typedef unsigned char Type;
  char buf[256];
  unsigned int w, h, maxval, ptype;
  int          val;

  if(_PNMReadHeader(filepointer, ptype, w, h, maxval) == false)
    return false; 
  
   if (ptype!=2 && ptype!=5 && ptype!=3 && ptype!=6) {
     cerr << "VW::ReadPPMImage(): bad type (P" << ptype
          << ") for input image (legal: P2,P5,P3,P6)\n";
     return false;
   }

   if ( image.IsAlloced() ) {
     if ( w!=image.GetWidth() || h!=image.GetHeight() )  {
       cerr << "VW::ReadPPMImage(): Image size mismatch\n";
       return false;
     }
   } else {
     image.AllocImageData(w, h);
   }
   
   switch (ptype) { // Mono ASCII
   case 2: {
     cerr << "VW::ReadPPMImage(): Warning - reading monchrome data into ImageYUV\n";
     for (unsigned int i=0; i<h; i++) {
       VW::PixelYUV<Type>* ptr = image.GetRowStart(i);
       for (unsigned int j=0; j<w; j++) {
         if (fscanf(filepointer, "%d", &val) != 1) {
           cerr << "VW::ReadPPMImage(): image data could not be read\n";
           return false;
         }
         *ptr++ = VW::PixelYUV<Type>(val,127,127,0);
       }
     }
     break;
   }
   case 5: { // Mono binary
     cerr << "VW::ReadPPMImage(): Warning - reading monchrome data into ImageYUV\n";
     for (unsigned int i=0; i<h; i++) {
       VW::PixelYUV<Type>* ptr = image.GetRowStart(i);
       for (unsigned int j=0; j<w; j++) {
         if (fread(&val, 1, 1, filepointer) != 1) {
           cerr << "VW::ReadPPMImage(): image data could not be read\n";
           return false;
         }
         *ptr++ = VW::PixelYUV<Type>(val,127,127,0);
       }
     }
     break;
   }
   case 3: { // RGB ASCII
     int r, g, b;
     cerr << "VW::ReadPPMImage(): Warning - reading PPM data as ImageYUV\n";
     for (unsigned int i=0; i<h; i++) {
       VW::PixelYUV<Type>* ptr = image.GetRowStart(i);
       for (unsigned int j=0; j<w; j++) {
         if (fscanf(filepointer, "%d%d%d", &r, &g, &b) != 3) {
           cerr << "VW::ReadPPMImage(): image data could not be read\n";
           return false;
         }
         // convert to YUV
         VW::ImageConversions::RGB2YUV(r, g, b,
                                       ptr->y, ptr->u, ptr->v);
         ptr++->alpha = 0;
       }
     }
     break;
   }
   case 6: { // RGB binary
     cerr << "VW::ReadPPMImage(): Warning - reading PPM data as ImageYUV\n";
     for (unsigned int i=0; i<h; i++) {
       VW::PixelYUV<Type>* ptr = image.GetRowStart(i);
       for (unsigned int j=0; j<w; j++) {
         if (fread(buf, 1, 3, filepointer) != 3) {
           cerr << "VW::ReadPPMImage(): image data could not be read\n";
           return false;
         }
        // convert to YUV
        VW::ImageConversions::RGB2YUV((unsigned char)buf[0],
                                       (unsigned char)buf[1],
				                               (unsigned char)buf[2],
				                               ptr->y, ptr->u, ptr->v);
         ptr++->alpha = 0;
       }
     }
     break;
   }
   default:
     // should never get here
     ;
   }
     
   return(true);

}

template <> inline bool
ReadPPMImage<unsigned char>(FILE* filepointer,
				       VW::ImageHSV<unsigned char>& image)
{
  typedef unsigned char Type;
  char buf[256];
  unsigned int w, h, maxval, ptype;
  int          val;

  if(_PNMReadHeader(filepointer, ptype, w, h, maxval) == false)
    return false; 
  
   if (ptype!=2 && ptype!=5 && ptype!=3 && ptype!=6) {
     cerr << "VW::ReadPPMImage(): bad type (P" << ptype
          << ") for input image (legal: P2,P5,P3,P6)\n";
     return false;
   }

   if ( image.IsAlloced() ) {
     if ( w!=image.GetWidth() || h!=image.GetHeight() )  {
       cerr << "VW::ReadPPMImage(): Image size mismatch\n";
       return false;
     }
   } else {
     image.AllocImageData(w, h);
   }
   
   switch (ptype) { // Mono ASCII
   case 2: {
     cerr << "VW::ReadPPMImage(): Warning - reading monchrome data into ImageHSV\n";
     for (unsigned int i=0; i<h; i++) {
       VW::PixelHSV<Type>* ptr = image.GetRowStart(i);
       for (unsigned int j=0; j<w; j++) {
         if (fscanf(filepointer, "%d", &val) != 1) {
           cerr << "VW::ReadPPMImage(): image data could not be read\n";
           return false;
         }
         *ptr++ = VW::PixelHSV<Type>(val,127,127,0);
       }
     }
     break;
   }
   case 5: { // Mono binary
     cerr << "VW::ReadPPMImage(): Warning - reading monchrome data into ImageYUV\n";
     for (unsigned int i=0; i<h; i++) {
       VW::PixelHSV<Type>* ptr = image.GetRowStart(i);
       for (unsigned int j=0; j<w; j++) {
         if (fread(&val, 1, 1, filepointer) != 1) {
           cerr << "VW::ReadPPMImage(): image data could not be read\n";
           return false;
         }
         *ptr++ = VW::PixelHSV<Type>(val,127,127,0);
       }
     }
     break;
   }
   case 3: { // RGB ASCII
     int r, g, b;
     cerr << "VW::ReadPPMImage(): Warning - reading (ASCII) PPM data as ImageYUV\n";
     for (unsigned int i=0; i<h; i++) {
       VW::PixelHSV<Type>* ptr = image.GetRowStart(i);
       for (unsigned int j=0; j<w; j++) {
         if (fscanf(filepointer, "%d%d%d", &r, &g, &b) != 3) {
           cerr << "VW::ReadPPMImage(): image data could not be read\n";
           return false;
         }
         // convert to YUV
         VW::ImageConversions::RGB2HSV(r, g, b,
                                       ptr->h, ptr->s, ptr->v);
         ptr++->alpha = 0;
       }
     }
     break;
   }
   case 6: { // RGB binary
     cerr << "VW::ReadPPMImage(): Warning - reading (binary) PPM data as ImageHSV\n";
     for (unsigned int i=0; i<h; i++) {
       VW::PixelHSV<Type>* ptr = image.GetRowStart(i);
       for (unsigned int j=0; j<w; j++) {
         if (fread(buf, 1, 3, filepointer) != 3) {
           cerr << "VW::ReadPPMImage(): image data could not be read\n";
           return false;
         }
        // convert to YUV
        VW::ImageConversions::RGB2HSV((unsigned char)buf[0],
				      (unsigned char)buf[1],
				      (unsigned char)buf[2],
				      ptr->h, ptr->s, ptr->v);
         ptr++->alpha = 0;
       }
     }
     break;
   }
   default:
     // should never get here
     ;
   }
     
   return(true);

}

template <> inline bool 
ReadPPMImage<unsigned char>(FILE* filepointer,
				     VW::ImageRGB<unsigned char>& image)
{
   typedef unsigned char Type;
   unsigned int        w, h, maxval, ptype;
   char buf[256];
   int        val;
//  unsigned char temp;  // Added by Eric

  if(_PNMReadHeader(filepointer, ptype, w, h, maxval) == false)
    return false; 
  
   if (ptype!=2 && ptype!=5 && ptype!=3 && ptype!=6) {
     cerr << "VW::ReadPPMImage(): bad type (P" << ptype
          << ") for input image (legal: P2,P5,P3,P6)\n";
     return false;
   }

   if ( image.IsAlloced() ) {
     if ( w!=image.GetWidth() || h!=image.GetHeight() )  {
       cerr << "VW::ReadPPMImage(): Image size mismatch\n";
       return false;
     }
   } else {
     image.AllocImageData(w, h);
   }
   
   switch (ptype) {
   case 2: { // Mono ASCII
     cerr << "VW::ReadPPMImage(): Warning - reading monchrome data into ImageRGB\n";
     for (unsigned int i=0; i<h; i++) {
       VW::PixelRGB<Type>* ptr = image.GetRowStart(i);
       for (unsigned int j=0; j<w; j++) {
         if (fscanf(filepointer, "%d", &val) != 1) {
           cerr << "VW::ReadPPMImage(): image data could not be read\n";
           return false;
         }
         *ptr++ = VW::PixelRGB<Type>(val,val,val,0);
       }
     }
     break;
   }
   case 5: { // Mono binary
     cerr << "VW::ReadPPMImage(): Warning: reading monchrome data into ImageRGB\n";
     for (unsigned int i=0; i<h; i++) {
       VW::PixelRGB<Type>* ptr = image.GetRowStart(i);
       for (unsigned int j=0; j<w; j++) {
         if (fread(&val, 1, 1, filepointer) != 1) {
           cerr << "VW::ReadPPMImage(): image data could not be read\n";
           return false;
         }
         *ptr++ = VW::PixelRGB<Type>(val,val,val,0);
       }
     }
     break;
   }
   case 3: { // RGB ASCII
     for (unsigned int i=0; i<h; i++) {
       VW::PixelRGB<Type>* ptr = image.GetRowStart(i);
       for (unsigned int j=0; j<w; j++) {
         int r=0,g=0,b=0;
         if (fscanf(filepointer, "%d%d%d", &r,&g,&b) != 3) {
           cerr << "VW::ReadPPMImage(): image data could not be read\n";
           return false;
         }
         *ptr++ = VW::PixelRGB<Type>(r,g,b,0);
       }
     }
     break;
   }
   case 6: { // RGB binary
     for (unsigned int i=0; i<h; i++) {
       VW::PixelRGB<Type>* ptr = image.GetRowStart(i);
         for (unsigned int j=0; j<w; j++) {
           int nRead = fread(buf, 1, 3, filepointer);
           if (nRead != 3) {
             if(ferror(filepointer)) {
               cerr<< "Read error" ;
               break;
             } else if(feof(filepointer)) {
               cerr<< "eof" ;							   
             } else {
               cerr<<"read "<<nRead<<"!";
             }
             cerr<<endl;
             cerr << "VW::ReadPPMImage(): image data could not be read\n";
             return false;
           }
           *ptr++ = VW::PixelRGB<Type>(buf[0],buf[1],buf[2],0);
         }
       }
     break;
   }
   default:
     // should never get here
     ;
   }
     
   return(true);

}

template <> inline bool 
ReadPPMImage<unsigned char>(FILE* filepointer,
				VW::ImageRGBPlanes<unsigned char>& image)
{
  typedef unsigned char Type;
   char	        buf[256];
   unsigned int w, h, maxval, ptype;
   int          val;
  
  if(_PNMReadHeader(filepointer, ptype, w, h, maxval) == false)
    return false; 
  
   if (ptype!=2 && ptype!=5 && ptype!=3 && ptype!=6) {
     cerr << "ReadImage(): bad type (P" << ptype
          << ") for input image (legal: P2,P5,P3,P6)\n";
     return false;
   }

   if ( image.IsAlloced() ) {
     if ( w!=image.GetWidth() || h!=image.GetHeight() )  {
       cerr << "Image size mismatch\n";
       return false;
     }
   } else {
     image.AllocImageData(w, h);
   }
   
   switch (ptype) {
   case 2: // mono ascii
     cerr << "Warning: reading monchrome data into ImageRGBplanes\n";
	 for (unsigned int i=0; i<h; i++) {
       VW::PixelMono<Type>* r_ptr = image.r.GetRowStart(i);
       VW::PixelMono<Type>* g_ptr = image.g.GetRowStart(i);
       VW::PixelMono<Type>* b_ptr = image.b.GetRowStart(i);
       for (unsigned int j=0; j<w; j++) {
	 if (fscanf(filepointer, "%d", &val) != 1) {
	   cerr << "ReadImage(): image data could not be read\n";
	   return false;
	 }
	 r_ptr++->y = val;
	 g_ptr++->y = val;
	 b_ptr++->y = val;
       }
	 }
     break;
   case 5: // mono binary
     cerr << "Warning: reading monchrome data into ImageRGBplanes\n";
	 for (unsigned int i=0; i<h; i++) {
       VW::PixelMono<Type>* r_ptr = image.r.GetRowStart(i);
       VW::PixelMono<Type>* g_ptr = image.g.GetRowStart(i);
       VW::PixelMono<Type>* b_ptr = image.b.GetRowStart(i);
       for (unsigned int j=0; j<w; j++) {
	 if (fread(&val, 1, 1, filepointer) != 1) {
	   cerr << "ReadImage(): image data could not be read\n";
	   return false;
	 }
	 r_ptr++->y = val;
	 g_ptr++->y = val;
	 b_ptr++->y = val;
      }
	 }
     break;
   case 3: // rgb ascii
	   for (unsigned int i=0; i<h; i++) {
       VW::PixelMono<Type>* r_ptr = image.r.GetRowStart(i);
       VW::PixelMono<Type>* g_ptr = image.g.GetRowStart(i);
       VW::PixelMono<Type>* b_ptr = image.b.GetRowStart(i);
       for (unsigned int j=0; j<w; j++) {
         int r=0,g=0,b=0;
	 if (fscanf(filepointer, "%d%d%d", &r, &g, &b) != 3) {
	   cerr << "ReadImage(): image data could not be read\n";
	   return false;
	 }
	 r_ptr++->y = r;
	 g_ptr++->y = g;
	 b_ptr++->y = b;
       }
	   }
     break;
   case 6:
     for (unsigned int i=0; i<h; i++) {
       VW::PixelMono<Type>* r_ptr = image.r.GetRowStart(i);
       VW::PixelMono<Type>* g_ptr = image.g.GetRowStart(i);
       VW::PixelMono<Type>* b_ptr = image.b.GetRowStart(i);
       for (unsigned int j=0; j<w; j++) {
	 if (fread(buf, 1, 3, filepointer) != 3) {
	   cerr << "ReadImage(): image data could not be read\n";
	   return false;
	 }
	 r_ptr++->y = buf[0];
	 g_ptr++->y = buf[1];
	 b_ptr++->y = buf[2];
       }
     }
     break;
   default:
     // should never get here
     ;
   }
     
   return(true);


}












template <> inline bool 
WritePPMImage<unsigned char>(FILE* filepointer,
			     const VW::ImageMono<unsigned char>& image)
{
  const int width = image.GetWidth();
  const int height = image.GetHeight();
  const unsigned int size = width*height;
  
  if (filepointer == NULL) {
     cerr << "VW::WritePPMImage(): file stream NULL\n";
     return false;
   }

  fprintf(filepointer, "P5\n%d %d\n%d\n", width, height, 255);
  if (fwrite(image.GetRawBuffer(), 1, size, filepointer) != size) {
    cerr << "VW::WritePPMImage(Mono): image data could not be written\n";
    return (false);
  }
	
  return (true);
}


template <> inline bool 
WritePPMImage<unsigned char>(FILE* filepointer,
			     const VW::ImageYUV<unsigned char>& image)
{
  typedef unsigned char Type;
  unsigned char rgb[4];
  const unsigned int w = image.GetWidth();
  const unsigned int h = image.GetHeight();
  if (filepointer == NULL) {
     cerr << "VW::WritePPMImage(): file stream NULL\n";
     return false;
   }

   fprintf ( filepointer, "P6\n%d %d\n%d\n", w, h, 255 );
   for (unsigned int i=0; i<h; i++) {
     VW::PixelYUV<Type>* ptr = image.GetRowStart(i);     
     for (unsigned int j=0; j<w; j++,ptr++) {
       ImageConversions::YUV2RGB(ptr->y, ptr->u, ptr->v,
                                 rgb[0],rgb[1],rgb[2]);
       if (fwrite(rgb, 1, 3, filepointer) != 3) {
         cerr << "VW::WritePPMImage(): image data could not be written\n";
         return (false);
       }
     }
   }
   return(true);
}



template <> inline bool 
WritePPMImage<unsigned char>(FILE* filepointer,
				 const VW::ImageHSV<unsigned char>& image)
{
  typedef unsigned char Type;
  unsigned char rgb[4];
  const unsigned int w = image.GetWidth();
  const unsigned int h = image.GetHeight();
  if (filepointer == NULL) {
     cerr << "VW::WritePPMImage(): file stream NULL\n";
     return false;
   }

   fprintf ( filepointer, "P6\n%d %d\n%d\n", w, h, 255 );
   for (unsigned int i=0; i<h; i++) {
     VW::PixelHSV<Type>* ptr = image.GetRowStart(i);     
     for (unsigned int j=0; j<w; j++,ptr++) {
       ImageConversions::HSV2RGB(ptr->h, ptr->s, ptr->v,
                                 rgb[0],rgb[1],rgb[2]);
       if (fwrite(rgb, 1, 3, filepointer) != 3) {
         cerr << "VW::WritePPMImage(): image data could not be written\n";
         return (false);
       }
     }
   }
   return(true);
}



template <> inline bool 
WritePPMImage<unsigned char>(FILE* filepointer,
                      const VW::ImageRGB<unsigned char>& image)
{
  typedef unsigned char Type;
  const unsigned int width = image.GetWidth();
  const unsigned int height = image.GetHeight();
  char buf[4];

   if (filepointer== NULL) {
     cerr << "VW::WritePPMImage(): file stream NULL\n";
     return (false);
   }

   fprintf ( filepointer, "P6\n%d %d\n%d\n", width, height, 255 );
   for (unsigned int i=0; i<height; i++) {
    VW::PixelRGB<Type>* ptr = image.GetRowStart(i);
    for (unsigned int j=0; j<width; j++,ptr++) {
       buf[0] = ptr->r;
       buf[1] = ptr->g;
       buf[2] = ptr->b;
       if (fwrite(buf, 1, 3, filepointer) != 3) {
         cerr << "VW::WritePPMImage(): image data could not be written\n";
         return (false);
       }
     }
   }
   return(true);
}



template <> inline bool 
WritePPMImage<unsigned char>(FILE* filepointer,
                      const VW::ImageRGBPlanes<unsigned char>& image)
{
   char buf[16];

   if (filepointer== NULL) {
     cerr << "WriteImage(): file stream NULL\n";
     return (false);
   }
   const unsigned int h = image.GetHeight();
   const unsigned int w = image.GetWidth();
   fprintf ( filepointer, "P6\n%d %d\n%d\n", w, h, 255 );
   for (unsigned int i=0; i<h; i++) {
     VW::PixelMono<unsigned char>* r_ptr = image.r.GetRowStart(i);
     VW::PixelMono<unsigned char>* g_ptr = image.g.GetRowStart(i);
     VW::PixelMono<unsigned char>* b_ptr = image.b.GetRowStart(i);
     for (unsigned int j=0; j<w; j++) {
       buf[0] = r_ptr++->y;
       buf[1] = g_ptr++->y;
       buf[2] = b_ptr++->y;
       if (fwrite(buf, 1, 3, filepointer) != 3) {
	 cerr << "WriteImage(): image data could not be written\n";
	 return (false);
       }
     }
   }
 
   return(true);
}



}; // End namespace VW

#endif // _vw_imageio_ppm_tpp_
