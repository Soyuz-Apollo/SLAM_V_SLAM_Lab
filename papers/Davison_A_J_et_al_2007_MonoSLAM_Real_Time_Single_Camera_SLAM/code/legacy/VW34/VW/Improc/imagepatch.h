/** 
 * @file   imagepatch.h
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
 * @class   VW::ImagePatch 
 * @author  BJ Tordoff
 * @date    28/03/2002
 * @ Brief  A storage class for simple (mono) image patches.
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

#ifndef _imagepatch_h
#define _imagepatch_h

// System includes
#include <vector>
#include <math.h> // for sqrt

// VW includes
#include <VW/Image/imagemono.h>
#include <VW/Image/imageyuv.h>
#include <VW/iobasics.h>

namespace VW {
  


class ImagePatch : public IOObject {
public: // functions
  /**
   * Data is created on construction, so height, width required.
   */
  inline ImagePatch(const unsigned int size=5);

  /**
   * Data is created on construction, so height, width required.
   */
  inline ImagePatch(const unsigned int width, 
		    const unsigned int height);
  inline ImagePatch(const ImagePatch& p);

  virtual ~ImagePatch() {};

  inline void Resize(const unsigned int width, 
		     const unsigned int height);
    
  /** Return patch width */
  inline unsigned int GetWidth() const;

  /** Return patch height */
  inline unsigned int GetHeight() const;

  /** Set one element of data */
  inline void SetData(const unsigned int x,
		      const unsigned int y,
		      const float value);

  /** Get one element of data */
  inline float GetData(const unsigned int x,
		       const unsigned int y) const;

  inline std::vector<float>::const_iterator 
  GetRowStart(const unsigned int y) const;

  inline std::vector<float>::iterator 
  GetRowStart(const unsigned int y);

  /** Return the mean of the data */
  inline float GetMean() const;

  /** Return the variance of the data around the mean */
  inline float GetVariance() const;

  /** Return the variance of the data around the mean */
  inline float GetZNCC(const VW::ImagePatch& p2) const;

  /** Return the normalised sum-squared.
   * This is \f$\sum_i(x_i - \bar{x}_i)^2\f$.
   */
  inline float GetNormalisedSumSquared() const ;

  /** Get the element-by-element product of this patch with another. */
  inline float GetElementProduct(const VW::ImagePatch& p2) const;


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
  inline void CalcSums() const;
  std::vector<float> data;
  unsigned int my_width;
  unsigned int my_height;
  mutable bool sums_up_to_date;
  mutable float my_mean;
  mutable float my_normalised_sum_squared;
};

  
// {{{ Some useful functions



  /** 
   * Create a re-sampled image patch using bilinear interpolation.
   */
  void GetInterpolatedImagePatch(const ImageMono<unsigned char>& im,
				 const double x,
				 const double y,
				 VW::ImagePatch& patch);
  /** 
   * Create a re-sampled image patch using bilinear interpolation.
   */
  void GetInterpolatedImagePatch(const ImageMono<float>& im,
				 const double x,
				 const double y,
				 VW::ImagePatch& patch);
  /** 
   * Create a re-sampled image patch from the Y channel
   * of a YUV image using bilinear interpolation.
   */
  void GetInterpolatedImagePatch(const ImageYUV<unsigned char>& im,
				 const double x,
				 const double y,
				 VW::ImagePatch& patch);
  /** 
   * Create an image patch without interpolation.
   */
  void GetImagePatch(const ImageMono<unsigned char>& im,
		     const double x,
		     const double y,
		     VW::ImagePatch& patch);
  /** 
   * Create an image patch without interpolation.
   */
  void GetImagePatch(const ImageMono<float>& im,
		     const double x,
		     const double y,
		     VW::ImagePatch& patch);
  /** 
   * Create an image patch from the Y channel
   * of a YUV image without interpolation.
   */
  void GetImagePatch(const ImageYUV<unsigned char>& im,
		     const double x,
		     const double y,
		     VW::ImagePatch& patch);

// }}}


}; // End of namespace VW


// Inlines are below


// {{{ Inlines for ImagePatch
inline VW::ImagePatch::ImagePatch(const unsigned int size)
  : my_width(size), 
    my_height(size), 
    sums_up_to_date(false) 
{
  data.resize(my_width*my_height);
  for (unsigned int i=0;i<(my_width*my_height);i++) 
    data[i]=0;
}
inline VW::ImagePatch::ImagePatch(const unsigned int width, 
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
VW::ImagePatch::ImagePatch(const ImagePatch& p)
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
VW::ImagePatch::Resize(const unsigned int width, 
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
VW::ImagePatch::GetWidth() const 
{
  return(my_width);
};


inline unsigned int 
VW::ImagePatch::GetHeight() const 
{
  return(my_height);
};

inline void 
VW::ImagePatch::SetData(const unsigned int x,
			const unsigned int y,
			const float value) 
{
  data[y*my_width+x]=value;
  sums_up_to_date=false;
};

inline float 
VW::ImagePatch::GetData(const unsigned int x,
			const unsigned int y) const 
{
  return(data[y*my_width+x]);
};

inline std::vector<float>::const_iterator 
VW::ImagePatch::GetRowStart(const unsigned int y) const
{
  return data.begin() + y*my_width;
}

inline std::vector<float>::iterator 
VW::ImagePatch::GetRowStart(const unsigned int y)
{
  return data.begin() + y*my_width;
}

inline void 
VW::ImagePatch::CalcSums() const 
{
  float sum=0.0f, sum2=0.0f;
  for (unsigned int i=0;i<(my_width*my_height);i++) {
    sum  += data[i];
    sum2 += data[i]*data[i];
  }

  my_mean = sum / float(data.size());
  my_normalised_sum_squared = sum2 - sum*my_mean;

  sums_up_to_date=true;
}

inline 
float VW::ImagePatch::GetMean() const 
{
  if (!sums_up_to_date) CalcSums();
  return(my_mean);
};

inline 
float VW::ImagePatch::GetVariance() const 
{
  if (!sums_up_to_date) CalcSums();
  return(my_normalised_sum_squared/float(data.size()));
};

inline float 
VW::ImagePatch::GetZNCC(const VW::ImagePatch& p2) const
{
  using namespace std;
  const float N = float(data.size());
  const float cross=GetElementProduct(p2);
  const float mean1=GetMean();
  const float mean2=p2.GetMean();
  const float ssq1=GetNormalisedSumSquared();
  const float ssq2=p2.GetNormalisedSumSquared();
  const float denom2=ssq1*ssq2;

  // If less than this, sqrt will be zero
  if (denom2<0.0001f) return 0.0f;

  return (cross - N*mean1*mean2) / sqrt(denom2);
}

inline 
float VW::ImagePatch::GetNormalisedSumSquared() const 
{
  if (!sums_up_to_date) CalcSums();
  return(my_normalised_sum_squared);
};

inline float VW::ImagePatch::GetElementProduct(const VW::ImagePatch& p2) const
{
  assert(this->GetWidth() == p2.GetWidth());
  assert(this->GetHeight() == p2.GetHeight());

  float result=0.0f;
  std::vector<float>::const_iterator i1 = data.begin(), i2 = p2.data.begin();
  const unsigned int n=data.size();
  for (unsigned int i=0;i<n;i++)
    result += (*i1++) * (*i2++);

  return result;
};

inline 
bool VW::ImagePatch::WriteASCII(std::ostream& stream) const
{
  // Size
  stream << my_width << " x " << my_height << std::endl;
  // Number
  unsigned int i=0;
  for (unsigned int y=0;y<my_height;y++) {
   for (unsigned int x=0;x<my_width;x++,i++) {
      stream << data[i] << " ";
    }
    stream << std::endl; 
  }

  return(true);
}

inline 
bool VW::ImagePatch::WriteXML(std::ostream& stream, 
			      const std::string& name) const
{
  
  // Start tag
  if (name.size()>0)
    stream << "<VW::ImagePatch NAME=\"" << name << "\"> " << std::endl;
  else
    stream << "<VW::ImagePatch> " << std::endl;
  
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
    stream << std::endl << "    "; 
   for (unsigned int x=0;x<my_width;x++,i++) {
      stream << data[i] << " ";
    }
  }
  stream << std::endl << "  </Data>" << std::endl;
  
    // End tag
  stream << "</VW::ImagePatch>" << std::endl;
  
  return(true);
}

inline 
bool VW::ImagePatch::WriteBinary(std::ostream& stream) const
{
  VW::WriteBinary(stream,my_width);
  VW::WriteBinary(stream,my_height);
  for (unsigned int i=0;i<(my_width*my_height);i++)
    VW::WriteBinary(stream,data[i]);
   
  return(true);
}

inline 
bool VW::ImagePatch::ReadASCII(std::istream& stream)
{
  sums_up_to_date = false;
  stream >> my_width >> my_height;

  this->Resize(my_width,my_height);

  for (unsigned int i=0;i<(my_width*my_height);i++)
    stream >> data[i];

  return(true);
}

inline 
bool VW::ImagePatch::ReadXML(std::istream& stream, 
				const std::string& name)
{
  sums_up_to_date = false;

  std::string tagname;
  std::istringstream contents(" ");
  if (!VW::FindXMLBlock("VW::ImagePatch",stream,tagname)) {
    std::cerr << "VW::ImagePatch::ReadXML - Could not find opening tag.\n";
    return(false);
  } else {
    if (!VW::GetXMLBlockAsNumber("Width",stream,my_width,tagname)) {
      std::cerr << "VW::ImagePatch::ReadXML - error reading width.\n";
      return(false);
    }
    if (!VW::GetXMLBlockAsNumber("Height",stream,my_width,tagname)) {
      std::cerr << "VW::ImagePatch::ReadXML - error reading height.\n";
      return(false);
    }
    this->Resize(my_width,my_height);
    std::istringstream datastream;
    if (!VW::GetXMLBlock("Data",stream,datastream,tagname)) {
      std::cerr << "VW::ImagePatch::ReadXML - error reading data.\n";
      return(false);
    }
    for (unsigned int i=0;i<(my_width*my_height);i++)
      datastream >> data[i];
    

  }
  return(true);
}

inline 
bool VW::ImagePatch::ReadBinary(std::istream& stream)
{
  sums_up_to_date = false;

  VW::ReadBinary(stream,my_width);
  VW::ReadBinary(stream,my_height);

  this->Resize(my_width,my_height);

  for (unsigned int i=0;i<(my_width*my_height);i++)
    VW::ReadBinary(stream,data[i]);


  return(true);
}

// }}}

#endif
