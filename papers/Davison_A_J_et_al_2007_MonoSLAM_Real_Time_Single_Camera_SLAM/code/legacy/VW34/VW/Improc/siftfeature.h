/** 
 * @file    siftfeature.h
 * @author  ND Molton
 * @date    15/10/2003
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
 *
 * 17/11/2004 BJT: Extracted as a separate class
 */
/**
 * @class   VW::SIFTFeature
 * @ingroup ImageProcessing
 * @author  ND Molton
 * @date    15/10/2003
 * @brief   A class to store a feature location along with the descriptor
 * as proposed by David Lowe.
 *
 *
 *
 * The implementation is based on the
 *  following papers:
 *   
 * @inproceedings{ lowe99object,
 *    author = "David G. Lowe",
 *    title = "Object Recognition from Local Scale-Invariant Features",
 *    booktitle = "Proc. of the International Conference on Computer Vision {ICCV}, Corfu",
 *    pages = "1150-1157",
 *    year = "1999",
 *    url = "citeseer.nj.nec.com/lowe99object.html" }
 *
 * @misc{ keypoints-distinctive,
 *  author = "David G. Lowe",
 *  title = "Distinctive image features from scale-invariant keypoints",
 *  note = "submitted to International Journal of Computer Vision, June 2003",
 *  url = "citeseer.nj.nec.com/582422.html" }
 */


// Notes:
// 
//  Not sure if images should be smoothed as part of the subsampling process
//  Scales tend to vary in a quantised, rather than continuous way
//  
//

#ifndef _siftfeature_h_
#define _siftfeature_h_

#include <ostream>
#include "cornerfeature.h"


// These will eventually be params
#define SIFT_CELL_SIZE 4
#define SIFT_NUM_CELLS 4
#define SIFT_NUM_ORIENTATIONS 8

// Each cell contributes "NUM_ORIENTATIONS" elements to descriptor
#define SIFT_DESCRIPTOR_SIZE SIFT_NUM_CELLS*SIFT_NUM_CELLS*SIFT_NUM_ORIENTATIONS


namespace VW {

  class SIFTFeature : public CornerFeature {
  public:
    SIFTFeature();
    SIFTFeature(const float x, const float y, 
		const float strength, 
		const float scale,
		const double orientation,
		const double descriptor[128]);
    SIFTFeature(const CornerFeature& base_feature,
		const double orientation);
    SIFTFeature(const SIFTFeature& feature);

    SIFTFeature& operator=(const SIFTFeature& feature);

    /** Orientation is anti-clockwise from right. */
    double GetOrientation() const;
    void SetOrientation(const double new_orientation);

    /** The size of the image patch used to build the descriptor */
    unsigned int GetPatchSize() const;

    /** The match score is proportional to the cosine of the angle between 
     * feature vectors but expanded to the range -1<s<1, since the
     * descriptor vector elements are all positive and the max angle is 90. 
     */
    double GetMatchScore(const VW::SIFTFeature& f) const;


    double GetDescriptorCosAngle(const VW::SIFTFeature& f) const;

    /** Ensure that the descriptor is a unit vector. */
    void NormaliseDescriptor();


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
    double              _orientation;  // Orientation (radians), measured clockwise from an axis pointing left [0..2pi]
    double              _descriptor[SIFT_DESCRIPTOR_SIZE];

  // Streaming methods (see IOObject for docs)

  private:

};


  inline std::ostream& operator<<(std::ostream& s, const SIFTFeature& f)
  {
    f.WriteASCII(s);
    return s;
  }

}; // end of VW namespace

inline double 
VW::SIFTFeature::GetOrientation() const
{
  return _orientation;
}

inline void 
VW::SIFTFeature::SetOrientation(double new_orientation)
{
  _orientation = new_orientation;
}

inline double VW::SIFTFeature::GetMatchScore(const VW::SIFTFeature& f) const
{
  return 2.0*GetDescriptorCosAngle(f)-1.0;
}

inline double VW::SIFTFeature::
GetDescriptorCosAngle(const VW::SIFTFeature& f) const
{
  double cos_ang = 0.0;
  const double* d1_ptr = _descriptor;
  const double* d2_ptr = f._descriptor;
  for (unsigned int i=0;i<SIFT_DESCRIPTOR_SIZE;i++)
    cos_ang += (*d1_ptr++) * (*d2_ptr++);

  return cos_ang;
}

inline unsigned int VW::SIFTFeature::
GetPatchSize() const
{
  return SIFT_CELL_SIZE * SIFT_NUM_CELLS;
}

#endif // sift_h_

