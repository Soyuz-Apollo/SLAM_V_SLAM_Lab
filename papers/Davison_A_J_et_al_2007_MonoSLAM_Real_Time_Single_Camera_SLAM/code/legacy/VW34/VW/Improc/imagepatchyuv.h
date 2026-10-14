/** 
 * @file   imagepatchyuv.h
 * @author BJ Tordoff
 * @date   28/03/2002
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
 * @class   VW::ImagePatchYUV
 * @author  BJ Tordoff
 * @date    28/03/2002
 * @ Brief  A storage class for colour (YUV) image patches.
 *
 * Functions are provided for creating patches
 * from mono images using a supplied image position.
 * If the position is supplied as floating point then
 * bilinear interpolation is used to get the values.
 * This doesn't cost much processing, and
 * Smith:Sinclair:Cipolla:Wood:BMVC1998 indicate that it has
 * some serious effect on the matching. However, I have some
 * reservations about the fairness of their comparisons, so
 * the choice is yours.
 * Note that getting and setting of data is
 * unchecked, so be careful of sizes.
 *
 */

#ifndef _imagepatchyuv_h
#define _imagepatchyuv_h

// System includes
#include <vector>

// VW includes
#include <VNL/matrixfixed.h>
#include <VW/Image/imageyuv.h>
#include <VW/iobasics.h>

namespace VW {
  


class ImagePatchYUV : public IOObject {
public: // functions
  /**
   * Data is created on construction, so height, width required.
   */
  inline ImagePatchYUV(const unsigned int size=5);

  /**
   * Data is created on construction, so height, width required.
   */
  inline ImagePatchYUV(const unsigned int width, 
		    const unsigned int height);
  inline ImagePatchYUV(const ImagePatchYUV& p);

  virtual ~ImagePatchYUV() {};

  inline void Resize(const unsigned int width, 
		     const unsigned int height);
    
  /** Return patch width */
  inline unsigned int GetWidth() const;

  /** Return patch height */
  inline unsigned int GetHeight() const;

  /** Set one element of data */
  inline void SetData(const unsigned int x,
		      const unsigned int y,
		      const VW::PixelYUV<float>& p);

  /** Get one element of data */
  inline VW::PixelYUV<float> GetData(const unsigned int x,
				      const unsigned int y) const;

  inline std::vector< VW::PixelYUV<float> >::const_iterator 
  GetRowStart(const unsigned int y) const;

  inline std::vector< VW::PixelYUV<float> >::iterator 
  GetRowStart(const unsigned int y);

  /** Return the mean of the data */
  inline VW::PixelYUV<float> GetMean() const;

  /** Return the variance of the data around the mean */
  inline VNL::MatrixFixed<3,3,float> GetCovariance() const;

  /** Get the ZNCC score with another patch. */
  float GetZNCC(const VW::ImagePatchYUV& p2) const;


  // IO functions

  // Stream output methods

  /** Stream as plain ASCII */
  virtual 
  bool WriteASCII(std::ostream& stream) const;
  /** Stream as XML style ASCII */
  virtual 
  bool WriteXML(std::ostream& stream, 
		const std::string& name="") const;
  /** Stream as binary */
  virtual 
  bool WriteBinary(std::ostream& stream) const;


  // Stream input methods

  /** Stream as plain ASCII */
  virtual 
  bool ReadASCII(std::istream& stream);
  /** Stream as XML style ASCII */
  virtual 
  bool ReadXML(std::istream& stream, 
			   const std::string& name="");
  /** Stream as binary */
  virtual 
  bool ReadBinary(std::istream& stream);


private:
  void CalcSums() const;
  std::vector<VW::PixelYUV<float> > data;
  unsigned int my_width;
  unsigned int my_height;
  mutable bool sums_up_to_date;
  mutable VW::PixelYUV<float> my_mean;
  mutable VNL::MatrixFixed<3,3,float> covar;
  mutable std::vector<VW::PixelYUV<float> > norm_data;
};

  
  /** 
   * Create a re-sampled image patch from the Y channel
   * of a YUV image using bilinear interpolation.
   */
  void GetInterpolatedImagePatch(const ImageYUV<unsigned char>& im,
				 const double x,
				 const double y,
				 VW::ImagePatchYUV& patch);
  /** 
   * Create an image patch from the Y channel
   * of a YUV image without interpolation.
   */
  void GetImagePatch(const ImageYUV<unsigned char>& im,
		     const double x,
		     const double y,
		     VW::ImagePatchYUV& patch);


}; // End of namespace VW


// Inlines are below


// {{{ Inlines for ImagePatchYUV

inline VW::ImagePatchYUV::ImagePatchYUV(const unsigned int size)
  : my_width(size), 
    my_height(size), 
    sums_up_to_date(false) 
{
  data.resize(my_width*my_height);
  for (unsigned int i=0;i<(my_width*my_height);i++) 
    data[i]=0;
}
inline VW::ImagePatchYUV::ImagePatchYUV(const unsigned int width, 
				  const unsigned int height)
  : my_width(width), 
    my_height(height), 
    sums_up_to_date(false) 
{
  data.resize(my_width*my_height);
  for (unsigned int i=0;i<(my_width*my_height);i++) 
    data[i]=0;
}


inline 
VW::ImagePatchYUV::ImagePatchYUV(const ImagePatchYUV& p)
  : my_width(p.GetWidth()), 
    my_height(p.GetHeight()), 
    sums_up_to_date(false) 
{
  data.resize(my_width*my_height);
  for (unsigned int j=0;j<my_height;j++) 
    for (unsigned int i=0;i<my_width;i++) 
      data[j*my_width+i]=p.GetData(i,j);
}


inline void 
VW::ImagePatchYUV::Resize(const unsigned int width, 
		       const unsigned int height) 
{
  my_width=width;
  my_height=height;
  data.resize(my_width*my_height);
  for (unsigned int i=0;i<(my_width*my_height);i++) 
    data[i]=0;
  sums_up_to_date=false;
}
   

inline unsigned int 
VW::ImagePatchYUV::GetWidth() const 
{
  return(my_width);
};


inline unsigned int 
VW::ImagePatchYUV::GetHeight() const 
{
  return(my_height);
};

inline void 
VW::ImagePatchYUV::SetData(const unsigned int x,
			   const unsigned int y,
			   const VW::PixelYUV<float>& value) 
{
  data[y*my_width+x]=value;
  sums_up_to_date=false;
};

inline VW::PixelYUV<float> 
VW::ImagePatchYUV::GetData(const unsigned int x,
			   const unsigned int y) const 
{
  return(data[y*my_width+x]);
};

inline std::vector<VW::PixelYUV<float> >::const_iterator 
VW::ImagePatchYUV::GetRowStart(const unsigned int y) const
{
  return data.begin() + y*my_width;
}

inline std::vector<VW::PixelYUV<float> >::iterator 
VW::ImagePatchYUV::GetRowStart(const unsigned int y)
{
  return data.begin() + y*my_width;
}


inline 
VW::PixelYUV<float> VW::ImagePatchYUV::GetMean() const 
{
  if (!sums_up_to_date) CalcSums();
  return(my_mean);
};

inline VNL::MatrixFixed<3,3,float>
VW::ImagePatchYUV::GetCovariance() const 
{
  if (!sums_up_to_date) CalcSums();
  return covar;
};


inline 
bool VW::ImagePatchYUV::WriteASCII(std::ostream& stream) const
{
  // Size
  stream << my_width << " x " << my_height << std::endl;
  // Number
  unsigned int i=0;
  for (unsigned int y=0;y<my_height;y++) {
   for (unsigned int x=0;x<my_width;x++,i++) {
      stream << data[i].y << "," << data[i].u << "," << data[i].v << " ";
    }
    stream << std::endl; 
  }

  return(true);
}

inline 
bool VW::ImagePatchYUV::WriteXML(std::ostream& stream, 
			      const std::string& name) const
{
  
  // Start tag
  if (name.size()>0)
    stream << "<VW::ImagePatchYUV NAME=\"" << name << "\"> " << std::endl;
  else
    stream << "<VW::ImagePatchYUV> " << std::endl;
  
  // Contents
  // Size
  stream << "  <Width> " << my_width 
	 << " </Width>";
  stream << "  <Height> " << my_height 
	 << " </Height>" << std::endl;
  // Number
  stream << "  <Data> ";
  unsigned int i=0;
  for (unsigned int y=0;y<my_height;y++) {
   for (unsigned int x=0;x<my_width;x++,i++) {
    stream << std::endl << "    "; 
    stream << "<VW::PixelYUV><Y>"
	   <<data[i].y<<"</Y><U>"
	   <<data[i].u<<"</U><V>"
	   <<data[i].v<<"</V></VW::PixelYUV>";
    }
  }
  stream << std::endl << "  </Data>" << std::endl;
  
    // End tag
  stream << "</VW::ImagePatchYUV>" << std::endl;
  
  return(true);
}

inline 
bool VW::ImagePatchYUV::WriteBinary(std::ostream& stream) const
{
  VW::WriteBinary(stream,my_width);
  VW::WriteBinary(stream,my_height);
  for (unsigned int i=0;i<(my_width*my_height);i++) {
    VW::WriteBinary(stream,data[i].y);
    VW::WriteBinary(stream,data[i].u);
    VW::WriteBinary(stream,data[i].v);
  }
   
  return(true);
}

inline 
bool VW::ImagePatchYUV::ReadASCII(std::istream& stream)
{
  sums_up_to_date = false;
  stream >> my_width >> my_height;

  this->Resize(my_width,my_height);

  for (unsigned int i=0;i<(my_width*my_height);i++) {
    stream >> data[i].y;
    stream >> data[i].u;
    stream >> data[i].v;
  }

  return(true);
}

inline 
bool VW::ImagePatchYUV::ReadXML(std::istream& stream, 
				const std::string& name)
{
  sums_up_to_date = false;

  std::string tagname;
  std::istringstream contents(" ");
  if (!VW::FindXMLBlock("VW::ImagePatchYUV",stream,tagname)) {
    std::cerr << "VW::ImagePatchYUV::ReadXML - Could not find opening tag.\n";
    return(false);
  } else {
    if (!VW::GetXMLBlockAsNumber("Width",stream,my_width,tagname)) {
      std::cerr << "VW::ImagePatchYUV::ReadXML - error reading width.\n";
      return(false);
    }
    if (!VW::GetXMLBlockAsNumber("Height",stream,my_width,tagname)) {
      std::cerr << "VW::ImagePatchYUV::ReadXML - error reading height.\n";
      return(false);
    }
    this->Resize(my_width,my_height);
    std::istringstream datastream,pixelstream;
    if (!VW::GetXMLBlock("Data",stream,datastream,tagname)) {
      std::cerr << "VW::ImagePatchYUV::ReadXML - error reading data.\n";
      return(false);
    }
    for (unsigned int i=0;i<(my_width*my_height);i++) {
      bool s = true;
      s=s&& VW::GetXMLBlock("VW::PixelYUV",datastream,pixelstream,tagname);
      s=s&& VW::GetXMLBlockAsNumber("Y",pixelstream,data[i].y,tagname);
      s=s&& VW::GetXMLBlockAsNumber("U",pixelstream,data[i].u,tagname);
      s=s&& VW::GetXMLBlockAsNumber("V",pixelstream,data[i].v,tagname);
      if (!s) {
	std::cerr << "VW::ImagePatchYUV::ReadXML - error reading pixel " << i <<".\n";
	return(false);
      }
    }
    

  }
  return(true);
}

inline 
bool VW::ImagePatchYUV::ReadBinary(std::istream& stream)
{
  sums_up_to_date = false;

  VW::ReadBinary(stream,my_width);
  VW::ReadBinary(stream,my_height);

  this->Resize(my_width,my_height);

  for (unsigned int i=0;i<(my_width*my_height);i++) {
    VW::ReadBinary(stream,data[i].y);
    VW::ReadBinary(stream,data[i].u);
    VW::ReadBinary(stream,data[i].v);
  }


  return(true);
}

// }}}

#endif
