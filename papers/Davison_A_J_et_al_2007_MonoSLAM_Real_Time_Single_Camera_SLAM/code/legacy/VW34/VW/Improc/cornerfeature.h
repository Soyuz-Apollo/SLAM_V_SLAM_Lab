/** 
 * @file    cornerfeature.h
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
 * @class   VW::CornerFeature
 * @ingroup ImageProcessing
 * @author  ID Reid and BJ Tordoff
 * @date    02/08/2002
 * @brief  A simple image location with a detection strength and scale.
 *
 *
 * #include <VW/Improc/cornerfeature.h>
 *
 */




#ifndef _cornerfeature_h_
#define _cornerfeature_h_

// System includes
#include <iostream>
#include <vector>

// VW includes (these had better all be above this in the dependency tree)
#include <VW/GeomObjects/point2d.h>


namespace VW {

  class CornerFeature : public VW::Point2D {
    
  public:
    
    CornerFeature() {};
    CornerFeature(const Point2D& p);
    CornerFeature(const float x, const float y, 
		  const float strength=0.0f, 
		  const float scale=1.0f);
    virtual ~CornerFeature() {};
    
    
    void Set(const float x, const float y, 
	     const float strength=0.0f, 
	     const float scale=1.0f);

    int GetPixelRow() const;
    int GetPixelColumn() const;
    float GetCornerStrength() const;
    void SetCornerStrength(const float new_strength);
    float GetFeatureScale() const;
    void SetFeatureScale(const float new_scale);

  // To enable us to sort corner lists
    bool operator> (const CornerFeature& p) const;
    bool operator< (const CornerFeature& p) const;
    bool operator== (const CornerFeature& p) const;
    


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



  protected:
    int c, r;
    float _strength,_scale;
  };

  // Some utility funcs
  /** Sort into order of decreasing strength. */
  void SortStrongestFirst(std::vector<CornerFeature>& list);

  /** Sort into order of increasing strength. */
  void SortByStrength(std::vector<CornerFeature>& list);

  /** Sort into order of increasing X-value. */
  void SortByXValue(std::vector<CornerFeature>& list);

  /** Sort into order of increasing Y-value. */
  void SortByYValue(std::vector<CornerFeature>& list);


  
}; // end namespace VW
  
  



  
  
// Inlines follow

inline 
VW::CornerFeature::CornerFeature(const VW::Point2D& p) 
  : Point2D(p), _strength(0), _scale(1.0)
{
  c = int(_x + 0.5);
  r = int(_y + 0.5);
};

inline 
VW::CornerFeature::CornerFeature(const float x, const float y, 
				 const float strength, 
				 const float scale) 
  : Point2D(x, y), 
    _strength(strength), 
    _scale(scale)
{
  c = int(x + 0.5);
  r = int(y + 0.5);
}

inline 
void VW::CornerFeature::Set(const float x, const float y, 
			    const float strength, 
			    const float scale) 
{
  this->_x=x;
  this->_y=y;
  this->_strength = strength;
  this->_scale = scale;
  this->c = int(x + 0.5);
  this->r = int(y + 0.5);
}

inline 
int VW::CornerFeature::GetPixelRow() const 
{
  return(r);
}

inline 
int VW::CornerFeature::GetPixelColumn() const 
{
  return(c);
}

inline 
float VW::CornerFeature::GetCornerStrength() const 
{
  return(_strength);
}

inline 
void VW::CornerFeature::SetCornerStrength(const float new_strength) 
{
  _strength=new_strength;
}

inline 
float VW::CornerFeature::GetFeatureScale() const 
{
  return(_scale);
}

inline 
void VW::CornerFeature::SetFeatureScale(const float new_scale) 
{
  _scale=new_scale;
}


// To enable us to sort corner lists
inline 
bool VW::CornerFeature::operator> (const VW::CornerFeature& p) const 
{
  return this->_strength > p.GetCornerStrength();
}

inline 
bool VW::CornerFeature::operator< (const VW::CornerFeature& p) const 
{
  return this->_strength < p.GetCornerStrength();
}

inline 
bool VW::CornerFeature::operator== (const VW::CornerFeature& p) const 
{
  return this->_strength == p.GetCornerStrength();
}

inline 
bool VW::CornerFeature::WriteASCII(std::ostream& stream) const
{
  stream << _x << "\t" << _y << "\t" << _strength << "\t" << "\t" << _scale << "\n";
  return(true);
}

inline 
bool VW::CornerFeature::WriteXML(std::ostream& stream, 
				 const std::string& name) const
{
  
  // Start tag
  if (name.size()>0)
    stream << "<CornerFeature NAME=\"" << name << "\"> " << std::endl;
  else
    stream << "<CornerFeature> " << std::endl;
  
  // Contents
  stream << "  <X>" << _x << "</X> ";
  stream << "<Y>" << _y << "</Y> ";
  stream << "<Strength>" << _strength << "</Strength> " << std::endl;
  stream << "<Scale>" << _strength << "</Scale> " << std::endl;

    // End tag
  stream << "</CornerFeature>";
  
  return(true);
}

inline 
bool VW::CornerFeature::WriteBinary(std::ostream& stream) const
{
  VW::WriteBinary(stream,_x);
  VW::WriteBinary(stream,_y);
  VW::WriteBinary(stream,_strength);
  VW::WriteBinary(stream,_scale);
  return(true);
}

inline 
bool VW::CornerFeature::ReadASCII(std::istream& stream)
{
  stream >> _x >> _y >> _strength >> _scale;

  return(true);
}

inline 
bool VW::CornerFeature::ReadXML(std::istream& stream, 
				const std::string& name)
{
  stream >> _x >> _y >> _strength >> _scale;

  return(stream.good()); // true only if stream is still ok (not EOF etc)
}

inline 
bool VW::CornerFeature::ReadBinary(std::istream& stream)
{
  VW::ReadBinary(stream,_x);
  VW::ReadBinary(stream,_y);
  VW::ReadBinary(stream,_strength);
  VW::ReadBinary(stream,_scale);

  return(stream.good()); // true only if stream is still ok (not EOF etc)
}


#endif /* _cornerfeature_h_ */
