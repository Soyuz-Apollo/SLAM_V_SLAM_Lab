/** 
 * @file    patchfeature.h
 * @author  BJ Tordoff
 * @date    24/11/2004
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
 *
 * 17/11/2004 BJT: Extracted as a separate class
 */
/**
 * @class   VW::PatchFeature
 * @ingroup ImageProcessing
 * @author  BJ Tordoff
 * @date    24/11/2004
 * @brief   A class to store a feature location along with the zero-normalised
 * patch of the surrounding image
 *
 *
 * #include <VW/Improc/patchfeature.h>
 */


#ifndef _patchfeature_h_
#define _patchfeature_h_

#include <ostream>
#include <VW/Improc/cornerfeature.h>


namespace VW {

  class ZNPatch {
  public:
    ZNPatch(unsigned int size);
    ZNPatch(const ZNPatch& p);
    ~ZNPatch();
    ZNPatch& operator=(const ZNPatch& p);
    unsigned int GetSize() const;

    float GetValue(unsigned int x, unsigned int y) const;
    void SetValue(unsigned int x, unsigned int y, float val);
    void SetData(unsigned int size, float* new_values);

    float* operator[](int row);
    const float* operator[](int row) const;

    float GetMaxValue() const;
    float GetMinValue() const;

    float GetMatchScore(const VW::ZNPatch& p2) const;

    /** Use the (raw) values stored in the patch and calculate the
     * zero-normalised values. 
     * Optionally the values can be weighted using a Gaussian to
     * emphasize the central region - this adds some computation time
     * but can (supposedly) improve recall rates.
     */
    void Normalise(bool weighted=false);


    bool WriteASCII(std::ostream& s) const;
    bool WriteBinary(std::ostream& s) const;

    bool ReadBinary(std::istream& s);

  private:
    void _AllocData();
    unsigned int _size;
    float* _data;
  };

  std::ostream& operator<<(std::ostream& s, const ZNPatch& p);




  class PatchFeature : public CornerFeature {
  public:
    PatchFeature(unsigned int patch_size = 0);
    PatchFeature(const float x, const float y, 
		 const float strength, 
		 const float scale,
		 const unsigned int patch_size,
		 const double orientation);
    PatchFeature(const CornerFeature& base_feature,
		 unsigned int patch_size,
		const double orientation);

    /** Orientation is anti-clockwise from right. */
    double GetOrientation() const;
    void SetOrientation(const double new_orientation);

    /** The match score is the zero-normalised cross correlation (ZNCC) */
    double GetMatchScore(const VW::PatchFeature& p2) const;

    unsigned int GetPatchSize() const;
    VW::ZNPatch GetPatch();
    VW::ZNPatch& Patch();
    const VW::ZNPatch& Patch() const;

    /** Try to avoid this one, as it copies the whole patch... */
    void SetPatch(const VW::ZNPatch& new_patch);

    // Streaming IO
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


  public: // Data
    double       _orientation;
    unsigned int _patchsize;
    ZNPatch      _patch;

  private:

};


  inline std::ostream& operator<<(std::ostream& s, const PatchFeature& f)
  {
    f.WriteASCII(s);
    return s;
  }

}; // end of VW namespace

inline float 
VW::ZNPatch::GetMatchScore(const VW::ZNPatch& p2) const
{
  const unsigned int num = _size*_size;
  const float* ptr1 = _data;
  const float* ptr2 = p2._data;
  float score = 0.0;
  for (unsigned int i=0;i<num;i++) {
    score += (*ptr1++) * (*ptr2++);
  }
  return score;
}



inline double 
VW::PatchFeature::GetMatchScore(const VW::PatchFeature& p2) const
{
  return _patch.GetMatchScore(p2.Patch());
}

inline double 
VW::PatchFeature::GetOrientation() const
{
  return _orientation;
}

inline void 
VW::PatchFeature::SetOrientation(double new_orientation)
{
  _orientation = new_orientation;
}

inline unsigned int 
VW::PatchFeature::GetPatchSize() const
{
  return _patch.GetSize();
}

inline VW::ZNPatch 
VW::PatchFeature::GetPatch()
{
  return _patch;
}

inline VW::ZNPatch& 
VW::PatchFeature::Patch()
{
  return _patch;
}

inline const VW::ZNPatch& 
VW::PatchFeature::Patch() const
{
  return _patch;
}

#endif // patchfeature_h_

