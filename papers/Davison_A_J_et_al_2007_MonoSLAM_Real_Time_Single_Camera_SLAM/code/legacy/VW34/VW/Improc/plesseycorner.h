/** 
 * @file    plesseycorner.h
 * @author  ID Reid and BJ Tordoff
 * @date    02/08/2002
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
 * @class   VW::Plesseycorner
 * @ingroup ImageProcessing
 * @author  ID Reid and BJ Tordoff
 * @date    02/08/2002
 * @brief  A simple image feature which describes a location in terms of
 * a normalised patch of the surrounding image.
 *
 */




#ifndef _plesseycorner_h_
#define _plesseycorner_h_

// System includes
#include <iostream>
#include <vector>

// Local includes
#include "cornerfeature.h"
#include "imagepatch.h"


namespace VW {

  class PlesseyCorner : public VW::CornerFeature {
    
  public:
    
    PlesseyCorner() {};
    PlesseyCorner(const Point2D& p);
    PlesseyCorner(const CornerFeature& c);
    PlesseyCorner(const float x, const float y, 
		  const float s=0, const unsigned int patch_size=5, 
		  const float scale=1.0);
    virtual ~PlesseyCorner() {};
    
    
    void Set(const float x, const float y, 
	     const float s=0, 
	     const unsigned int patch_size=5, 
	     const float scale=1.0);

    void SetPatchSize(const unsigned int new_size);

    VW::ImagePatch* GetImagePatch();
    /** Get a const pointer (useful for using const PlesseyCorners) */
    const VW::ImagePatch* GetImagePatch() const;

    /** Try to avoid this one, as it copies the whole patch... */
    void SetImagePatch(const VW::ImagePatch& new_patch);
    
  // To enable us to sort corner lists
    bool operator> (const PlesseyCorner& p) const;
    bool operator< (const PlesseyCorner& p) const;
    bool operator== (const PlesseyCorner& p) const;
    


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
    VW::ImagePatch _patch;
    
  };

  // Some utility funcs
  /** Sort into order of increasing strength. */
  void SortByStrength(std::vector<PlesseyCorner>& list);

  /** Sort into order of increasing X-value. */
  void SortByXValue(std::vector<PlesseyCorner>& list);

  /** Sort into order of increasing Y-value. */
  void SortByYValue(std::vector<PlesseyCorner>& list);


  
}; // end namespace VW
  
  



  
  
// Inlines follow

inline 
VW::PlesseyCorner::PlesseyCorner(const VW::Point2D& p) 
  : CornerFeature(p), _patch(0,0) 
{
};

inline 
VW::PlesseyCorner::PlesseyCorner(const VW::CornerFeature& c) 
  : CornerFeature(c), _patch(0,0) 
{
};

inline 
VW::PlesseyCorner::PlesseyCorner(const float x, const float y, 
				 const float s, 
				 const unsigned int patch_size, 
				 const float scale) 
  : CornerFeature(x, y, s, scale), _patch(patch_size,patch_size) 
{
}

inline 
void VW::PlesseyCorner::Set(const float x, const float y, 
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

inline void VW::PlesseyCorner::SetPatchSize(const unsigned int new_size)
{
  this->_patch.Resize(new_size,new_size);
}


inline 
VW::ImagePatch* VW::PlesseyCorner::GetImagePatch() 
{
  return(&_patch);
}

inline 
const VW::ImagePatch* VW::PlesseyCorner::GetImagePatch() const 
{
  return(&_patch);
}

inline 
void VW::PlesseyCorner::SetImagePatch(const VW::ImagePatch& new_patch) 
{
  _patch=new_patch;
}

inline 
bool VW::PlesseyCorner::WriteASCII(std::ostream& stream) const
{
  stream << _x << "\t" << _y << "\t" << _strength << "\t" << _scale << "\n";
  _patch.WriteASCII(stream);
  return(true);
}

inline 
bool VW::PlesseyCorner::WriteXML(std::ostream& stream, 
				 const std::string& name) const
{
  
  // Start tag
  if (name.size()>0)
    stream << "<PlesseyCorner NAME=\"" << name << "\"> " << std::endl;
  else
    stream << "<PlesseyCorner> " << std::endl;
  
  // Contents
  stream << "  <X>" << _x << "</X> ";
  stream << "<Y>" << _y << "</Y> ";
  stream << "<Strength>" << _strength << "</Strength> " << std::endl;
  stream << "<Scale>" << _strength << "</Scale> " << std::endl;
  _patch.WriteXML(stream);

    // End tag
  stream << "</PlesseyCorner>";
  
  return(true);
}

inline 
bool VW::PlesseyCorner::WriteBinary(std::ostream& stream) const
{
  VW::WriteBinary(stream,_x);
  VW::WriteBinary(stream,_y);
  VW::WriteBinary(stream,_strength);
  VW::WriteBinary(stream,_scale);
  _patch.WriteBinary(stream);
  return(true);
}

inline 
bool VW::PlesseyCorner::ReadASCII(std::istream& stream)
{
  stream >> _x >> _y >> _strength >> _scale;

  _patch.ReadASCII(stream);

  return(true);
}

inline 
bool VW::PlesseyCorner::ReadXML(std::istream& stream, 
				const std::string& name)
{
  stream >> _x >> _y >> _strength >> _scale;

  _patch.ReadXML(stream);

  return(stream.good()); // true only if stream is still ok (not EOF etc)
}

inline 
bool VW::PlesseyCorner::ReadBinary(std::istream& stream)
{
  VW::ReadBinary(stream,_x);
  VW::ReadBinary(stream,_y);
  VW::ReadBinary(stream,_strength);
  VW::ReadBinary(stream,_scale);

  _patch.ReadBinary(stream);

  return(stream.good()); // true only if stream is still ok (not EOF etc)
}


#endif /* _plesseycorner_h_ */
