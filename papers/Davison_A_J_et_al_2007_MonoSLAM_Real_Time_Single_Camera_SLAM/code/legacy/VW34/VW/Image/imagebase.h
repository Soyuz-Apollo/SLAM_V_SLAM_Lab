/** @file imagebase.h
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
 *
 * @class   VW::ImageBase 
 * @ingroup Image
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/02
 * @brief   The base class from which all images are made.
 *
 * The base image class provides much of the functionality for the
 * specific image classes, but more importantly as an abstract class enforces
 * consistency between them. All images contain a RIO (region of interest)
 * which specifies which region of the image is valid (many convolutions
 * and similar leave a border of unprocessed pixels), and when designing
 * algorithms one should work only inside the specified ROI.
 *
 * Note that the '=' operator is deliberately disabled for all image classes
 * to prevent nasty accidents. If you really need to copy an image, use one
 * of the ImageCopy functions provided.
 *
 * IO can be performed on a range of standard image formats using the ReadImage()
 * and WriteImage() functions, or, in order to preserve all attributes with the 
 * Read/WriteBinary() functions. The latter pair use an internal format which
 * is similar to the PPM raw format, ie type and size are specified in plain text
 * and pixel data as binary. Note that when reading a raw VW image the format
 * (RGB, YUV etc), type (Char, UChar, Double etc) and size must match exactly - no
 * conversion will be performed.
 */


#ifndef _baseimage_h_
#define _baseimage_h_

// C-style includes
#include <assert.h>
#include <math.h> // for sqrt

// C++ includes
#include <iostream>
#include <string>

// Local includes
#include "roi.h"

namespace VW {


  template < class PixelType, class PixelPtr > 
  class ImageBase {


  public:
       
    ImageBase(const unsigned int w, const unsigned int h);
    ImageBase(PixelType* data, const unsigned int w, const unsigned int h);
    ImageBase();

    virtual ~ ImageBase();

    void AllocImageData(const unsigned int w, const unsigned int h);
    void SetImageData(PixelType* data, const unsigned int w, const unsigned int h);
    void FreeImageData();

    bool IsAlloced() const;

    void GetSize(unsigned int &w, unsigned int &h) const;
    unsigned int GetWidth(void) const;
    unsigned int GetHeight(void) const;

    void SetROI(const VW::ROI & new_roi);
//     VW::ROI GetROI() const;
    VW::ROI& ROI();
    const VW::ROI& ROI() const;

    /** Set the ROI to be the whole image. */
    void ResetROI();

    PixelType *GetImageBuffer(void) const;
    void *GetRawBuffer(void) const;
    PixelType GetPixel(const int c, const int r) const;
    void SetPixel(const int c, const int r, const PixelType & new_value);

    PixelPtr  GetRowStart(int r) const;
    PixelPtr* GetRowPointer(int r) const;
    PixelPtr  operator[] (int r) const; // same as GetRowStart()
    PixelPtr  GetROIRowStart(int r) const;

    /** Set the whole image to a particular value. */
    void Clear(const PixelType & new_value = PixelType());

    bool ReadImage(const std::string& filename);
    bool WriteImage(const std::string& filename) const;

    // Some basic transformations
    void FlipHorizontal();
    void FlipVertical();

    /** Swap this image's data for other image data - size, ROI etc are
     * also exchanged. This is handy to avoid an image copy when doing
     * things like:
     *
     * void MyFunc(ImageXXX& im)
     * {
     *    VW::ImageXXX result;
     *    DoSomeProcessing(im,result);
     *    im.ExchangeData(result);
     * }
     *
     * which would otherwise require a copy of the result data back to the 
     * original.
     */
    void ExchangeData(ImageBase<PixelType,PixelPtr>& other);


    // Streaming ops

    bool ReadBinary(std::istream& stream);
    bool WriteBinary(std::ostream& stream) const;


    PixelType operator() (int r) const;
    PixelType operator() (int r, int c);

    /** Pointers to the start of each row.
     * A pointer to the start of the row r is given by image.pixel[r], and a
     * pixel at row r, column c, by image.pixel[r][c]. This is functionally 
     * identical to saying image[r][c], but in some circumstances may be 
     * faster.
     */
    PixelPtr *pixel;

    /** For drawing, the image has a pen colour much like a display */
    void SetPenColour(const PixelType& colour);

    /** Return the current pen colour. */
    PixelType GetPenColour() const;

    void DrawLine(const double x0, const double y0,
		  const double x1, const double y1);

  protected:
    // Funcs

    /** Return strings describing the format of image data.
     * This must be overloaded on a per-type basis.
     */
   virtual void GetFormatStrings(std::string& format, 
				 std::string& type) const = 0;

    /** Operator= is dangerous and has been disabled. 
     * (See the ImageConversions functions if you really need to copy an image)
     */
    ImageBase & operator=(const ImageBase & im)
;
    /** The copy constructor is dangerous and has been disabled. 
     * (See the ImageConversions functions if you really need to copy an image)
     */
    ImageBase(const ImageBase &);


    void _CreateRowPointers();

    // Data
    unsigned int _width;	// width of image
    unsigned int _height;	// height of image
    
    VW::ROI _roi;		// Region of interest
    
    
    PixelType *_data;
    bool _dataalloced;	// has memory been allocated
    bool _rowsalloced;	// has memory been allocated

    // For drawing on images like displays
    PixelType _pen_colour;

  };



};				// end namespace VW













// Inlines


/** Construct using pre-specified data storage (eg for wrapping
 * another image class's data). Note that the format of the data
 * must match the target pixel format but no checks are made.
 * @param addr the start of the memory array containing the image data
 * @param w width (in pixels) of the data array
 * @param h height (in pixels) of the data array 
 */
template < class PixelType, class PixelPtr > inline 
VW::ImageBase < PixelType, PixelPtr >::ImageBase(PixelType* data, 
						 const unsigned int w,
						 const unsigned int h)
  : _data(data), _dataalloced(false), _rowsalloced(false)
{
  assert(data != 0 && w != 0 && h != 0);
  _width = w;
  _height = h;
  _roi.Set(0, 0, _width, _height);

  _CreateRowPointers();

  _dataalloced = false;
  _rowsalloced = true;
}


/** Construct and allocate data storage.
 * @param w desired width (in pixels) of the image
 * @param h desired height (in pixels) of the image 
 */
template < class PixelType, class PixelPtr > inline 
VW::ImageBase < PixelType,PixelPtr >::ImageBase(const unsigned int w, 
						const unsigned int h)
  : _dataalloced(false), _rowsalloced(false)
{
  assert(w != 0 && h != 0);
  _width = w;
  _height = h;
  _roi.Set(0, 0, _width, _height);
  AllocImageData(_width, _height);
}


/** Construct without allocating storage.
 * Storage space will be allocated automatically when an image is read from a file
 * or stream, or must otherwise by created using the AllocImageData() function.
 */
template < class PixelType, class PixelPtr > inline 
  VW::ImageBase < PixelType, PixelPtr >::ImageBase()
    :  pixel(0),
     _width(0),
     _height(0),
     _roi(0, 0, _width, _height),
     _data(0),
     _dataalloced(false),
     _rowsalloced(false)
{
}

/** Allocate the image data array. 
 * @param w the width of array (number of columns) to allocate
 * @param h the height of array (number of rows) to allocate
 */
template < class PixelType, class PixelPtr > inline void 
VW::ImageBase < PixelType,PixelPtr >::AllocImageData(const unsigned int w, 
						     const unsigned int h)
{
  _width = w;
  _height = h;
  _data = new PixelType[_width * _height];
  _CreateRowPointers();
  assert(_data != NULL && pixel != NULL);

  _roi.Set(0, 0, _width, _height);

  _dataalloced = true;
  _rowsalloced = true;
}

/** Allocate the image data array using some existing data. 
 * @param data the memory location of the existing data array
 * @param w the width of array (number of columns) to allocate
 * @param h the height of array (number of rows) to allocate
 */
template < class PixelType, class PixelPtr > inline void 
VW::ImageBase < PixelType,PixelPtr >::SetImageData(PixelType* data, const unsigned int w, const unsigned int h)
{
	assert(data != 0 && w != 0 && h != 0);
  _width = w;
  _height = h;
  _data = data;
  _roi.Set(0, 0, _width, _height);

  _CreateRowPointers();

  _dataalloced = false;
  _rowsalloced = true;
}


template < class PixelType, class PixelPtr >
inline VW::ImageBase < PixelType, PixelPtr >::~ImageBase()
{
  if (_dataalloced) {
    FreeImageData();
  } else if (_rowsalloced) {
    delete[] pixel;
    _rowsalloced = false;
  }
}


/** De-allocate the image data array.
 * Once freed, storage space will be reallocated automatically when an image is
 * read from a file or stream, or must otherwise by created using the 
 * AllocImageData() function.
 */
template < class PixelType, class PixelPtr >
inline void VW::ImageBase < PixelType, PixelPtr >::FreeImageData()
{
  assert(_dataalloced && _rowsalloced && _data != NULL && pixel != NULL);
  delete[] _data;
  delete[] pixel;
  _dataalloced = false;
  _rowsalloced = false;
  _width = 0;
  _height = 0;
  _roi.Set(0,0,0,0);
}


/** Set the region of interest (ROI) of the image.
 * @param new_roi the new region of interest to use.
 */
template < class PixelType, class PixelPtr >
inline void VW::ImageBase < PixelType,
			    PixelPtr >::SetROI(const VW::ROI & new_roi)
{
  _roi.Set(new_roi);
}

/** Copy of the region of interest (ROI) of the image.
 * @returns the current region of interest in the image.
 */
// template < class PixelType, class PixelPtr >
// inline VW::ROI VW::ImageBase < PixelType, PixelPtr >::GetROI() const
// {
//   return _roi;
// }

/** Reference to the region of interest (ROI) of the image.
 * @returns the current region of interest in the image.
 */
template < class PixelType, class PixelPtr >
inline VW::ROI & VW::ImageBase < PixelType, PixelPtr >::ROI()
{
  return _roi;
}

/** Constant reference to the region of interest (ROI) of the image.
 * @returns the current region of interest in the image.
 */
template < class PixelType, class PixelPtr >
inline const VW::ROI & VW::ImageBase < PixelType,
				       PixelPtr >::ROI() const
{
  return _roi;
}

/** Get a pointer to the first pixel in the data array. The array is stored
 * in row-major form, and can be traversed using pointer arithmatic, eg:
 * const unsigned int num_pix = im.GetWidth() * im.GetHeight();
 * PixelType* p = im.GetImageBuffer();
 * for (unsigned int i=0; i<num_pix; i++,p++) {
 *    p->ScaleIntensity(0.5);
 * }
 */
template < class PixelType, class PixelPtr >
inline PixelType * VW::ImageBase < PixelType,
				   PixelPtr >::GetImageBuffer(void) const
{
  return _data;
};

/** Get a pointer to the first memory location used in the data array.
 * As this returns a void*, you can cast it to any type you like, with
 * the usual pitfalls that this entails if the pointer type doesn't match
 * the type of the data stored.
 */
template < class PixelType, class PixelPtr >
inline void *VW::ImageBase < PixelType,
			     PixelPtr >::GetRawBuffer(void) const
{
  return static_cast < void *>(_data);
}

/** Recover (copy) the pixel at column c, row r. If VW is compiled in 
 * debug mode, this function is range-checked.
 * @param c the column index (0<c<width)
 * @param r the row index (0<r<height)
 * @returns a copy of the pixel (so changes to this pixel will not affect the
 * underlying image)
 */
template < class PixelType, class PixelPtr >
inline PixelType VW::ImageBase < PixelType,
				 PixelPtr >::GetPixel(const int c, const int r) const
{
#ifdef _VWDEBUG
  using namespace std;
  if ((c<0) || (c>=_width)) {
    cerr << "VW::ImageBase::GetPixel: Column "
	      << c << "is out of bounds." << endl;
    cerr << "                         (valid range is 0<c<"
	      << _width << ")" <<endl;
    exit(0);
  }
  if ((r<0) || (r>=_height)) {
    cerr << "VW::ImageBase::GetPixel: Row "
	      << r << "is out of bounds." << endl;
    cerr << "                         (valid range is 0<r<"
	      << _height << ")" <<endl;
    exit(0);
  }
#endif
  return pixel[r][c];
};

/** Set the pixel at column c, row r. If VW is compiled in debug mode, this 
 * function is range-checked.
 * @param c the column index (0<c<width)
 * @param r the row index (0<r<height)
 * @param new_value the new data for this pixel
 */
template < class PixelType, class PixelPtr >
inline void VW::ImageBase < PixelType,
			    PixelPtr >::SetPixel(const int c, const int r,
						 const PixelType & new_value)
{
#ifdef _VWDEGBUG
  using namespace std;
  if ((c<0) || (c>=_width)) {
    cerr << "VW::ImageBase::SetPixel: Column "
	      << c << "is out of bounds." << endl;
    cerr << "                         (valid range is 0<c<"
	      << _width << ")" <<endl;
    exit(0);
  }
  if ((r<0) || (r>=_height)) {
    cerr << "VW::ImageBase::SetPixel: Row "
	      << r << "is out of bounds." << endl;
    cerr << "                         (valid range is 0<r<"
	      << _height << ")" <<endl;
    exit(0);
  }
#endif
  pixel[r][c] = new_value;
};

template < class PixelType, class PixelPtr >
inline PixelPtr VW::ImageBase < PixelType,
				PixelPtr >::GetRowStart(int r) const
{
  return pixel[r];
}

template < class PixelType, class PixelPtr >
inline PixelPtr* VW::ImageBase < PixelType,
				PixelPtr >::GetRowPointer(int r) const
{
  return pixel + r;
}

template < class PixelType, class PixelPtr >
inline PixelPtr VW::ImageBase < PixelType,
				PixelPtr >::operator[](int r)
  const {
  return pixel[r];
};
template < class PixelType, class PixelPtr >
inline PixelPtr VW::ImageBase < PixelType,
				PixelPtr >::GetROIRowStart(int r) const
{
  return GetRowStart(r) + _roi.Left();
};

/** Read a standard image format from disk. 
 * The type of file is determined by the name extension (how very Windows...)
 * @param filename the name of the image file to load (can be relative or absolute)
 * @returns true if the image was loaded successfully
 */
template < class PixelType, class PixelPtr >
inline bool VW::ImageBase < PixelType,
			    PixelPtr >::ReadImage(const std::string& filename)
{
  std::cerr << "VW::ImageBase::ReadImage: Base class ReadImage called in error!" << std::endl;
  return false;
};

/** Write a standard image format to disk. 
 * The type of file is determined by the name extension (how very Windows...)
 * @param filename the name of the image file to save (can be relative or absolute)
 * @returns true if the image was saved successfully
 */
template < class PixelType, class PixelPtr >
inline bool VW::ImageBase < PixelType,
			    PixelPtr >::WriteImage(const std::string& filename) const
{
  std::cerr << "VW::ImageBase::WriteImage: Base class WriteImage called in error!" << std::endl;
  return false;
};


/** Get the pixel at the start of row r. This function is not range checked.
 * @param r the row number (y coord) of the pixel
 * @returns the pixel value at the start of this row
 */
template < class PixelType, class PixelPtr >
inline PixelType VW::ImageBase < PixelType,
				 PixelPtr >::operator()(int r)
  const {
  return pixel[r];
};


/** Recover a pixel (note y,x NOT x,y). This function is not range-checked.
 * @param r the row number (y coord) of the pixel
 * @param c the column number (x coord) of the pixel
 * @returns the pixel value at this position
 */
template < class PixelType, class PixelPtr >
inline PixelType VW::ImageBase < PixelType,
				 PixelPtr >::operator()(int r, int c)
{
  return pixel[r][c];
};


/** Is the image data array allocated? 
 * @returns true if data array exists
 */
template < class PixelType, class PixelPtr >
inline bool VW::ImageBase < PixelType, PixelPtr >::IsAlloced() const
{
  return _rowsalloced;
};				// has memory been allocated

/** Get width and height in a single command. 
 * @param w will be set to the width (number of columns)
 * @param h will be set to the height (number of rows)
 */
template < class PixelType, class PixelPtr >
inline void VW::ImageBase < PixelType,
			    PixelPtr >::GetSize(unsigned int &w, unsigned int &h) const
{
  w = _width;
  h = _height;
};

/** Determine the width of the image in pixels. 
 * @returns the width (number of columns) of the image
*/
template < class PixelType, class PixelPtr >
inline unsigned int VW::ImageBase < PixelType,
				    PixelPtr >::GetWidth(void) const
{
  return _width;
};

/** Determine the height of the image in pixels.
 * @returns the height (number of rows) of the image
 */
template < class PixelType, class PixelPtr >
inline unsigned int VW::ImageBase < PixelType,
				    PixelPtr >::GetHeight(void) const
{
  return _height;
};



template < class PixelType, class PixelPtr > 
inline VW::ImageBase < PixelType, PixelPtr >&
VW::ImageBase < PixelType, PixelPtr >::operator=(const VW::ImageBase< PixelType, PixelPtr >& im) {
  std::cerr << "NEVER call operator= on an image!\n" 
	    << " (use VW::ImageCopy() instead)\n";
  std::exit(1);
  return *this;
};

template < class PixelType, class PixelPtr >
inline VW::ImageBase < PixelType,
		       PixelPtr >::ImageBase(const VW::ImageBase< PixelType,
		       PixelPtr >&) {
  std::cerr << "NEVER call the copy constructor for an image!\n "
	    << "(use VW::ImageCopy() instead)\n";
  std::exit(1);
}

/** Fill the entire image with a pixel value. */
template < class PixelType, class PixelPtr >
inline void 
VW::ImageBase < PixelType,PixelPtr >::Clear(const PixelType & new_value)
{
  const unsigned int numpix = _width * _height;
  PixelPtr ptr = _data;
  --ptr;
  for (unsigned int i=0; i < numpix; ++i)
    *(++ptr) = new_value;
}


/** Write the image to a stream in the internal VW format.
 * This format is strongly related to the raw PPM format, but
 * keeps the byte ordering and alpha values of the image class
 * used.  The image format and data type are written in plain text
 * before the data, stored as binary.
 */
template < class PixelType, class PixelPtr >
inline bool 
VW::ImageBase < PixelType,PixelPtr >::WriteBinary(std::ostream& stream) const
{
  // First stick on a comment
  stream << "# This is a VW raw image" << std::endl;

  // then a format string (Type dependent)
  std::string format,type;
  GetFormatStrings(format,type);
  stream << format << std::endl 
	 << type << std::endl;

  // then the size
  stream << _width << " " << _height << std::endl;

  // Now the data
  std::cerr << "VW::ImageBase::WriteBinary: Going to write ("<< _width 
	    << "x" <<  _height << "x" << sizeof(PixelType) 
	    << ") bytes." << std::endl;
  char* ptr = static_cast<char*>(GetRawBuffer());
  unsigned int numbytes = _width*_height*sizeof(PixelType);

  stream.write(ptr,numbytes);

  return(true);
}

/** Read the image from a stream in the internal VW format.
 * This format is strongly related to the raw PPM format, but
 * keeps the byte ordering and alpha values of the image class
 * used. The image format and data type are written in plain text
 * before the data, stored as binary.
 * @param stream the input stream into which the image data will be written
 */
template < class PixelType, class PixelPtr >
inline bool 
VW::ImageBase < PixelType,PixelPtr >::ReadBinary(std::istream& stream)
{
  std::cerr << "VW::ImageBase::ReadBinary:" << std::endl;

  // First skip the comment line
  char line[100];
  stream.getline(line,100);

  // Try to read the image format
  std::string myformat,mytype;
  std::string fileformat,filetype;
  GetFormatStrings(myformat,mytype);

  stream >> fileformat;
  if (fileformat != myformat) {
    std::cerr << "VW::ImageBase::ReadBinary: Incorrect image format - \"" << fileformat 
	      << "\" should be \"" << myformat << "\"" << std::endl;
    exit(0);
  }
  std::cerr << "   Got format, look for type..." << std::endl;

  stream >> filetype;
  if (filetype != mytype) {
    std::cerr << "VW::ImageBase::ReadBinary: Incorrect image type - \"" << filetype 
	      << "\" should be \"" << mytype << "\"" << std::endl;
    exit(0);
  }
  std::cerr << "   Got type, look for size..." << std::endl;
  unsigned int w,h;
  stream >> w >> h;

  if (IsAlloced()) {
  if ((w != GetWidth()) || (h != GetHeight())) {
    std::cerr << "VW::ImageBase::ReadBinary: Size mismatch - image is ("
	      << GetWidth() << "x" <<  GetHeight() << ") but file is (" 
	      << w << "x" << h << ")" << std::endl;
    exit(0);    
  }
  } else {
    AllocImageData(w, h);
  }
  std::cerr << "   Reading data of size ("<< w << "x" << h << "x"
	    << sizeof(PixelType) <<")..." << std::endl;
  

  // Skip one char
   char c = stream.get();

  // Now the data
   char* ptr = static_cast<char*>(GetRawBuffer());
   unsigned int numbytes = GetWidth()*GetHeight()*sizeof(PixelType);

   stream.read(ptr,numbytes);
   return(true);
}

/** Reset the region of interest to be the whole image. */
template <class PixelType, class PixelPtr>
inline void 
VW::ImageBase<PixelType,PixelPtr>::ResetROI()
{
  _roi.Set(0,0,_width,_height);
}


/** Flip the image horizontally (size preserved) */
template <class PixelType, class PixelPtr>
inline void 
VW::ImageBase<PixelType,PixelPtr>::FlipHorizontal()
{
  if (IsAlloced()) {

    // Reserve some space
    PixelType *new_data = new PixelType[_width * _height];
    
    // Do the copy
    for (unsigned int row=0; row<_height; row++) {
      PixelType *source_ptr = &(_data[row*_width]);
      PixelType *dest_ptr = &(new_data[(row+1)*_width - 1]);
      for (unsigned int col=0; col<_width; col++, source_ptr++, dest_ptr--)
	*dest_ptr = *source_ptr;
    }
    
    // Reset the data pointers
    delete[] _data;
    _data = new_data;
    pixel[0] = _data;
    for (unsigned int i = 1; i < _height; i++) {
      pixel[i] = pixel[i - 1] + _width;
    }
  }
}


/** Flip the image vertically (size preserved) */
template <class PixelType, class PixelPtr>
inline void 
VW::ImageBase<PixelType,PixelPtr>::FlipVertical()
{
  if (IsAlloced()) {

  // We shift whole rows at a time to take advantage of CPU block copies
  const unsigned int num_bytes = _width * sizeof(PixelType);
  
  // Reserve some space
  PixelType *new_data = new PixelType[_width * _height];
  
  // Do the copy
  PixelType *source_ptr = _data;
  PixelType *dest_ptr = &(new_data[_width*(_height-1)]); // bottom row
  for (unsigned int row=0; row<_height; row++) {
    memcpy(dest_ptr, source_ptr, num_bytes);
    source_ptr += _width;
    dest_ptr -= _width;
  }
  
  // Reset the data pointers
  delete[] _data;
  _data = new_data;
  pixel[0] = _data;
  for (unsigned int i = 1; i < _height; i++) {
    pixel[i] = pixel[i - 1] + _width;
  }
  }
}

template <class PixelType, class PixelPtr>
inline void VW::ImageBase<PixelType,PixelPtr>::ExchangeData(ImageBase<PixelType,PixelPtr>& other)
{
  // First swap the sizes
  { 
    unsigned int t=0;
    t=this->_width;  this->_width  = other._width;  other._width = t;
    t=this->_height; this->_height = other._height; other._height = t;
  }
  
  // and then the ROI
  {
    VW::ROI t = this->_roi; this->_roi = other._roi; other._roi = t;
  }
   
   // And the flags
  {
    bool t = this->_dataalloced; 
    this->_dataalloced = other._dataalloced; 
    other._dataalloced = t;
  }

  // now the data pointers
  {
    PixelType* t = this->_data; this->_data = other._data; other._data = t;
    this->_CreateRowPointers();
    other._CreateRowPointers();
  }
}

template <class PixelType, class PixelPtr> inline void 
VW::ImageBase<PixelType,PixelPtr>::_CreateRowPointers()
  {
    if (_rowsalloced) {
      delete[] pixel;
    }
    if (_height>0) {
      pixel = new PixelPtr[_height];
      pixel[0] = _data;
      for (unsigned int i = 1; i < _height; i++) {
	pixel[i] = pixel[i - 1] + _width;
      }
      _rowsalloced = true;
    } else {
      // No data
      _rowsalloced = false;
    }
  }

template <class PixelType, class PixelPtr> inline void 
VW::ImageBase<PixelType,PixelPtr>::SetPenColour(const PixelType& colour)
{
  _pen_colour = colour;
}

template <class PixelType, class PixelPtr> inline PixelType 
VW::ImageBase<PixelType,PixelPtr>::GetPenColour() const
{
  return _pen_colour;
}

template <class PixelType, class PixelPtr> inline void 
VW::ImageBase<PixelType,PixelPtr>::DrawLine(const double x0, const double y0,
					    const double x1, const double y1)
{
  using namespace std; // sqrt is sometimes in (GCC), sometimes not (VC++)
  const double length = sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
  const double dx = (x1-x0)/length;
  const double dy = (y1-y0)/length;
  double x = x0, y = y0;
  for (double a=0.0;a<=length;a++) {
    pixel[int(y+0.5)][int(x+0.5)] = _pen_colour;
    x += dx; y += dy;
  }
}

#endif				/* _imagebase_h_ */
