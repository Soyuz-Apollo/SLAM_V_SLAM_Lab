/** @file imageio_gdi.tpp
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
#ifndef _vw_imageio_gdi_tpp_
#define _vw_imageio_gdi_tpp_


#include "pixelconversions.h"
#include "imagemono.h"
#include "imagergb.h"
#include "imageyuv.h"
#include "imagehsv.h"
#include "imagergbplanes.h"

#include "imagecopy.tpp"

#include "imageio_gdi.h"


// GDI+ stuff
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")


using namespace std;
using namespace Gdiplus;

namespace VW {


// Case insensitive comparison of strings, from Stroustrup p591
static int _CaseInsensitiveCompare(const string& s1, const string& s2)
{
  string::const_iterator p1 = s1.begin();
  string::const_iterator p2 = s2.begin();
  const string::const_iterator pe1 = s1.end();
  const string::const_iterator pe2 = s2.end();
  while (p1!=pe1 && p2!=pe2) {
    const char c1 = toupper(*p1);
    const char c2 = toupper(*p2);
    if (c1 != c2) return (c1<c2) ? -1 : 1;
    ++p1;
    ++p2;
  }
  const unsigned int sz1 = s1.size();
  const unsigned int sz2 = s2.size();
  return (sz2==sz1) ? 0 : (sz1<sz2) ? -1 : 1;
}


/** Find the file extension (suffix) of a file, if one exists. */
static string _FindSuffix(const string& filename)
{
 unsigned int len = filename.size();
  // scan backwards for the '.'
  bool found=false;
  unsigned int pos=0;
  for (pos=len-1;pos>0;pos--) {
    if (filename[pos]=='.') {
      found = true;
      break;
    }
  }

  // now check for problems
  if (!found) {
    return(""); // Dot did not exist
  }
  // Was found, but is it empty?
  const int num_chars = len-pos-1;
  if (num_chars <= 0) {
      return(""); // dot was last char, so return an empty string
  }

  // It was OK, so return the remainder
  return(filename.substr(pos+1,num_chars));

}



static bool _gdi_init = false;

static ULONG_PTR m_GDIPToken = 0;

static inline void _InitGDI()
{
// for use with GDI+
  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  Gdiplus::GdiplusStartup(&m_GDIPToken, &gdiplusStartupInput, NULL);
	_gdi_init = true;
}


static inline WCHAR* _GetAsWChar(const std::string& s)
{
	const unsigned int n = s.size();
	WCHAR* wchars = new WCHAR[n+1];
	for (unsigned int i=0;i<n;i++) {
		wchars[i] = s[i];
	}
	wchars[n] = 0;
	return wchars;
}

static inline int _GetEncoderID(const std::string& filename, CLSID* pClsid)
{
	// First extract the extension from the filename
	std::string suffix = _FindSuffix(filename);
	std::string format;
	if (_CaseInsensitiveCompare(suffix, "png")==0) {
		format = "image/png";
	} else if (_CaseInsensitiveCompare(suffix, "jpg")==0) {
		format = "image/jpeg";
	} else if (_CaseInsensitiveCompare(suffix, "jpeg")==0) {
		format = "image/jpeg";
	} else if (_CaseInsensitiveCompare(suffix, "gif")==0) {
		format = "image/gif";
	} else if (_CaseInsensitiveCompare(suffix, "ppm")==0) {
		format = "image/ppm";
	} else if (_CaseInsensitiveCompare(suffix, "pgm")==0) {
		format = "image/pgm";
	} else if (_CaseInsensitiveCompare(suffix, "bmp")==0) {
		format = "image/bmp";
	} else {
		format = "";
		cerr << "Unrecognised image format \"" << suffix << "\"" << endl;
		return -1; // Failure
	}

   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   const WCHAR* wformat = _GetAsWChar(format);
   for(UINT j = 0; j < num; ++j)
   {
	  const WCHAR* this_format = pImageCodecInfo[j].MimeType;
      if( wcscmp(this_format, wformat) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}









//////////////////////////////////////////////////////////
// Default versions

template <class valType>
inline bool ReadGDIImage(const std::string& filename,
			 VW::ImageMono<valType>& image)
{
  // Just read as uchar then do VW::ImageCopy!
  VW::ImageMono<unsigned char> tmp_img;
  if (ReadGDIImage(filename, tmp_img)) {
    VW::ImageCopy(tmp_img,image);
    return true;
  }
  return false;
}

template <class valType>
inline bool ReadGDIImage(const std::string& filename,
                      VW::ImageRGB<valType>& image)
{
  // Just read as uchar then do VW::ImageCopy!
  VW::ImageRGB<unsigned char> tmp_img;
  if (ReadGDIImage(filename, tmp_img)) {
    VW::ImageCopy(tmp_img,image);
    return true;
  }
  return false;
}

template <class valType>
inline bool ReadGDIImage(const std::string& filename,
                      VW::ImageYUV<valType>& image)
{
  // Just read as uchar then do VW::ImageCopy!
  VW::ImageYUV<unsigned char> tmp_img;
  if (ReadGDIImage(filename, tmp_img)) {
    VW::ImageCopy(tmp_img,image);
    return true;
  }
  return false;
}

template <class valType>
inline bool ReadGDIImage(const std::string& filename,
                      VW::ImageHSV<valType>& image)
{
  // Just read as uchar then do VW::ImageCopy!
  VW::ImageHSV<unsigned char> tmp_img;
  if (ReadGDIImage(filename, tmp_img)) {
    VW::ImageCopy(tmp_img,image);
    return true;
  }
  return false;
}


template <class valType>
inline bool WriteGDIImage(const std::string& filename,
			  const VW::ImageMono<valType>& image)
{
  cerr << "WriteGDIImage (mono): Warning - floating point data being clipped and written to unsigned char\n";

  // Note that it's faster to convert in one go than to do it pixel by pixel
  VW::ImageMono<unsigned char> tmp;
  VW::ImageCopy(image, tmp);
  VW::WriteGDIImage(filename,tmp);
  return (true);
}

template <class valType>
inline bool WriteGDIImage(const std::string& filename,
                      const VW::ImageYUV<valType>& image)
{
  cerr << "WriteGDIImage (YUV): Warning - floating point data being clipped and written to unsigned char\n";

  // Note that it's faster to convert in one go than to do it pixel by pixel
  VW::ImageYUV<unsigned char> tmp;
  VW::ImageCopy(image, tmp);
  VW::WriteGDIImage(filename,tmp);
  return (true);
}

template <class valType>
inline bool WriteGDIImage(const std::string& filename,
                      const VW::ImageHSV<valType>& image)
{
  cerr << "WriteGDIImage (HSV): Warning - floating point data being clipped and written to unsigned char\n";

  // Note that it's faster to convert in one go than to do it pixel by pixel
  VW::ImageHSV<unsigned char> tmp;
  VW::ImageCopy(image, tmp);
  VW::WriteGDIImage(filename,tmp);
  return (true);
}

template <class valType>
inline bool WriteGDIImage(const std::string& filename,
                      const VW::ImageRGB<valType>& image)
{
  cerr << "WriteGDIImage (RGB): Warning - floating point data being clipped and written to unsigned char\n";

  // Note that it's faster to convert in one go than to do it pixel by pixel
  VW::ImageRGB<unsigned char> tmp;
  VW::ImageCopy(image, tmp);
  VW::WriteGDIImage(filename,tmp);
  return (true);
}










///////////////////////////////////////////////////////////
// Reading/Writing unsigned chars can be made efficient

template <> inline bool 
ReadGDIImage<unsigned char>(const std::string& filename,
				       VW::ImageMono<unsigned char>& image)
{
	cerr << "VW::ReadGDIImage(Mono): Not supported yet" << endl;
	return false;
}



template <> inline bool 
ReadGDIImage<unsigned char>(const std::string& filename,
				       VW::ImageYUV<unsigned char>& image)
{
	if (!_gdi_init) _InitGDI();

	const WCHAR* wname = _GetAsWChar(filename);
	Bitmap bmp( wname );
	if( bmp.GetFlags() == ImageFlagsNone ) {
		return false;
	}
	// Bitmap loaded OK, so copy into VW image
	const unsigned int nrows = bmp.GetHeight();
	const unsigned int ncols = bmp.GetWidth();
	image.AllocImageData(ncols, nrows);
	Rect rect(0,0,ncols,nrows);
	BitmapData data;
	bmp.LockBits(&rect,ImageLockModeRead,PixelFormat32bppARGB,&data);
	const unsigned int numpix = ncols * nrows;
	VW::PixelYUV<unsigned char>* dst_ptr = image[0];
	unsigned char* src_ptr = static_cast<unsigned char*>(data.Scan0);
	for (unsigned int i=0;i<numpix;i++, src_ptr+=4, dst_ptr++) {
		VW::ImageConversions::RGB2YUV(src_ptr[2], src_ptr[1], src_ptr[0],
                                 dst_ptr->y,dst_ptr->u,dst_ptr->v);
        dst_ptr->alpha = src_ptr[3];
	}
	bmp.UnlockBits(&data);
	return true;
}

template <> inline bool
ReadGDIImage<unsigned char>(const std::string& filename,
				       VW::ImageHSV<unsigned char>& image)
{
	cerr << "VW::ReadGDIImage(HSV): Not supported yet" << endl;
	return false;
}

template <> inline bool 
ReadGDIImage<unsigned char>(const std::string& filename,
				     VW::ImageRGB<unsigned char>& image)
{
	if (!_gdi_init) _InitGDI();

	const WCHAR* wname = _GetAsWChar(filename);
	Bitmap bmp( wname );
	if( bmp.GetFlags() == ImageFlagsNone ) {
		return false;
	}
	// Bitmap loaded OK, so copy into VW image
	const unsigned int nrows = bmp.GetHeight();
	const unsigned int ncols = bmp.GetWidth();
	image.AllocImageData(ncols, nrows);
	Rect rect(0,0,ncols,nrows);
	BitmapData data;
	bmp.LockBits(&rect,ImageLockModeRead,PixelFormat32bppARGB,&data);
	memcpy(image.GetRawBuffer(), data.Scan0, nrows*ncols*4);
	bmp.UnlockBits(&data);
	return true;
}

template <> inline bool 
ReadGDIImage<unsigned char>(const std::string& filename,
				VW::ImageRGBPlanes<unsigned char>& image)
{
	cerr << "VW::ReadGDIImage(RGBPlanes): Not supported yet" << endl;
	return false;
}












template <> inline bool 
WriteGDIImage<unsigned char>(const std::string& filename,
				 const VW::ImageMono<unsigned char>& image)
{
	cerr << "VW::WriteGDIImage(Mono): Not supported yet" << endl;
	return false;
}


template <> inline bool 
WriteGDIImage<unsigned char>(const std::string& filename,
				 const VW::ImageYUV<unsigned char>& image)
{
	if (!_gdi_init) _InitGDI();

	const unsigned int nrows = image.GetHeight();
	const unsigned int ncols = image.GetWidth();
	Bitmap bmp(ncols, nrows, PixelFormat32bppARGB);
	Rect rect(0,0,ncols,nrows);
	BitmapData data;
	const Status s = bmp.LockBits(&rect,ImageLockModeWrite,PixelFormat32bppARGB,&data);

	if (s != Ok) {
		cerr << "VW::WriteGDIImage: Could not lock image region." << endl;
		return false;
	}

	const unsigned int numpix = ncols * nrows;
	const VW::PixelYUV<unsigned char>* src_ptr = image[0];
	unsigned char* dst_ptr = static_cast<unsigned char*>(data.Scan0);
	for (unsigned int i=0;i<numpix;i++, dst_ptr+=4, src_ptr++) {
		VW::ImageConversions::YUV2RGB(src_ptr->y,src_ptr->u,src_ptr->v,
									dst_ptr[2], dst_ptr[1], dst_ptr[0]);
        dst_ptr[3] = src_ptr->alpha;
	}


	bmp.UnlockBits(&data);

	CLSID encoder_id;
	if (_GetEncoderID(filename, &encoder_id) < 0) {
		// Negative value indicates failure
		cerr << "VW::WriteGDIImage: Format \"" << _FindSuffix(filename) << "\" is not supported by the OS." << endl;
		return false;
	}
	const WCHAR* wname = _GetAsWChar(filename);
	bmp.Save(wname, &encoder_id, NULL);;
	return true;
}



template <> inline bool 
WriteGDIImage<unsigned char>(const std::string& filename,
				 const VW::ImageHSV<unsigned char>& image)
{
	cerr << "VW::WriteGDIImage(HSV): Not supported yet" << endl;
	return false;
}



template <> inline bool 
WriteGDIImage<unsigned char>(const std::string& filename,
                      const VW::ImageRGB<unsigned char>& image)
{
 	if (!_gdi_init) _InitGDI();

	const unsigned int nrows = image.GetHeight();
	const unsigned int ncols = image.GetWidth();
	Bitmap bmp(ncols, nrows, PixelFormat32bppARGB);
	Rect rect(0,0,ncols,nrows);
	BitmapData data;
	bmp.LockBits(&rect,ImageLockModeWrite,PixelFormat32bppARGB,&data);
	memcpy(data.Scan0, image.GetRawBuffer(), nrows*ncols*4);
	bmp.UnlockBits(&data);

	CLSID encoder_id;
	_GetEncoderID(filename, &encoder_id);
	const WCHAR* wname = _GetAsWChar(filename);
	bmp.Save(wname, &encoder_id, NULL);;
	return true;
}



template <> inline bool 
WriteGDIImage<unsigned char>(const std::string& filename,
                      const VW::ImageRGBPlanes<unsigned char>& image)
{
	cerr << "VW::WriteGDIImage(RGBPlanes): Not supported yet" << endl;
	return false;
}



}; // End namespace VW

#endif // _vw_imageio_gdi_tpp_
