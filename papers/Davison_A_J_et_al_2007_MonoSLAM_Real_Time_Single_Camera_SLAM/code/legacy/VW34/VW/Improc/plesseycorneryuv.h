/** 
 * @file    plesseycorneryuv.h
 * @author  BJ Tordoff
 * @date    03/08/2004
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
/**
 * @class   VW::PlesseycornerYUV
 * @ingroup ImageProcessing
 * @author  BJ Tordoff
 * @date    03/08/2004
 * @brief  A simple image feature suitable for representing Plessey corners.
 *
 */




#ifndef _plesseycorneryuv_h_
#define _plesseycorneryuv_h_

// System includes
#include <iostream>
#include <vector>

// VW includes (these had better all be above this in the dependency tree)

// Local includes
#include "cornerfeature.h"
#include "imagepatchyuv.h"


namespace VW {

  class PlesseyCornerYUV : public VW::CornerFeature {
    
  public:
    
    PlesseyCornerYUV() {};
    PlesseyCornerYUV(const Point2D& p);
    PlesseyCornerYUV(const CornerFeature& p);
    PlesseyCornerYUV(const float x, const float y, 
		  const float s=0, const unsigned int patch_size=5, 
		  const float scale=1.0);
    virtual ~PlesseyCornerYUV() {};
    
    
    void SetPatchSize(const unsigned int new_size);

   void Set(const float x, const float y, 
	     const float s=0, 
	     const unsigned int patch_size=5, 
	     const float scale=1.0);

    VW::ImagePatchYUV* GetImagePatch();
    /** Get a const pointer (useful for using const PlesseyCornerYUVs) */
    const VW::ImagePatchYUV* GetImagePatch() const;

    /** Try to avoid this one, as it copies the whole patch... */
    void SetImagePatch(const VW::ImagePatchYUV& new_patch);
    
  // To enable us to sort corner lists
    bool operator> (const PlesseyCornerYUV& p) const;
    bool operator< (const PlesseyCornerYUV& p) const;
    bool operator== (const PlesseyCornerYUV& p) const;
    


  // Streaming methods (see IOObject for docs)

  virtual 
  bool WriteASCII(std::ostream& stream) const;

  virtual 
  bool WriteXML(std::ostream& stream,
			 const std::string& name="") const;
  virtual 
  bool WriteBinary(std::ostream& stream) const;

  virtual 
  bool ReadASCII(std::istream& stream);

  virtual 
  bool ReadXML(std::istream& stream, 
			const std::string& name="");
  virtual 
  bool ReadBinary(std::istream& stream);



  private:
    VW::ImagePatchYUV _patch;
    
  };

  // Some utility funcs
  /** Sort into order of in/decreasing strength. */
  void SortByStrength(std::vector<PlesseyCornerYUV>& list, 
		      bool strongest_first=true);

  /** Sort into order of increasing X-value. */
  void SortByXValue(std::vector<PlesseyCornerYUV>& list);

  /** Sort into order of increasing Y-value. */
  void SortByYValue(std::vector<PlesseyCornerYUV>& list);


  
}; // end namespace VW
  
  



  
  
// Inlines follow

inline 
VW::PlesseyCornerYUV::PlesseyCornerYUV(const VW::Point2D& p) 
  : CornerFeature(p), _patch(0,0) 
{
};

inline 
VW::PlesseyCornerYUV::PlesseyCornerYUV(const VW::CornerFeature& c) 
  : CornerFeature(c), _patch(0,0) 
{
};

inline 
VW::PlesseyCornerYUV::PlesseyCornerYUV(const float x, const float y, 
				 const float s, 
				 const unsigned int patch_size, 
				 const float scale) 
  : CornerFeature(x, y, s, scale), _patch(patch_size,patch_size) 
{
}

inline 
void VW::PlesseyCornerYUV::Set(const float x, const float y, 
			    const float s, 
			    const unsigned int patch_size, 
			    const float scale) 
{
  this->_x=x;
  this->_y=y;
  this->_strength=s;
  this->_patch.Resize(patch_size,patch_size);
  this->_scale=scale;
  c = int(x + 0.5);
  r = int(y + 0.5);
}


inline void VW::PlesseyCornerYUV::SetPatchSize(const unsigned int new_size)
{
  this->_patch.Resize(new_size,new_size);
}


inline 
VW::ImagePatchYUV* VW::PlesseyCornerYUV::GetImagePatch() 
{
  return(&_patch);
}

inline 
const VW::ImagePatchYUV* VW::PlesseyCornerYUV::GetImagePatch() const 
{
  return(&_patch);
}

inline 
void VW::PlesseyCornerYUV::SetImagePatch(const VW::ImagePatchYUV& new_patch) 
{
  _patch=new_patch;
}

// To enable us to sort corner lists
inline 
bool VW::PlesseyCornerYUV::operator> (const VW::PlesseyCornerYUV& p) const 
{
  return this->_strength > p.GetCornerStrength();
}

inline 
bool VW::PlesseyCornerYUV::operator< (const VW::PlesseyCornerYUV& p) const 
{
  return this->_strength < p.GetCornerStrength();
}

inline 
bool VW::PlesseyCornerYUV::operator== (const VW::PlesseyCornerYUV& p) const 
{
  return this->_strength == p.GetCornerStrength();
}

inline 
bool VW::PlesseyCornerYUV::WriteASCII(std::ostream& stream) const
{
  stream << _x << "\t" << _y << "\t" << _strength << "\t" << _scale << "\n";
  _patch.WriteASCII(stream);
  return(true);
}

inline 
bool VW::PlesseyCornerYUV::WriteXML(std::ostream& stream, 
				 const std::string& name) const
{
  
  // Start tag
  if (name.size()>0)
    stream << "<PlesseyCornerYUV NAME=\"" << name << "\"> " << std::endl;
  else
    stream << "<PlesseyCornerYUV> " << std::endl;
  
  // Contents
  stream << "  <X>" << _x << "</X> ";
  stream << "<Y>" << _y << "</Y> ";
  stream << "<Strength>" << _strength << "</Strength> " << std::endl;
  stream << "<Scale>" << _strength << "</Scale> " << std::endl;
  _patch.WriteXML(stream);

    // End tag
  stream << "</PlesseyCornerYUV>";
  
  return(true);
}

inline 
bool VW::PlesseyCornerYUV::WriteBinary(std::ostream& stream) const
{
  VW::WriteBinary(stream,_x);
  VW::WriteBinary(stream,_y);
  VW::WriteBinary(stream,_strength);
  VW::WriteBinary(stream,_scale);
  _patch.WriteBinary(stream);
  return(true);
}

inline 
bool VW::PlesseyCornerYUV::ReadASCII(std::istream& stream)
{
  stream >> _x >> _y >> _strength >> _scale;

  _patch.ReadASCII(stream);

  return(true);
}

inline 
bool VW::PlesseyCornerYUV::ReadXML(std::istream& stream, 
				const std::string& name)
{
  stream >> _x >> _y >> _strength >> _scale;

  _patch.ReadXML(stream);

  return(stream.good()); // true only if stream is still ok (not EOF etc)
}

inline 
bool VW::PlesseyCornerYUV::ReadBinary(std::istream& stream)
{
  VW::ReadBinary(stream,_x);
  VW::ReadBinary(stream,_y);
  VW::ReadBinary(stream,_strength);
  VW::ReadBinary(stream,_scale);

  _patch.ReadBinary(stream);

  return(stream.good()); // true only if stream is still ok (not EOF etc)
}


#endif /* _plesseycorner_h_ */
