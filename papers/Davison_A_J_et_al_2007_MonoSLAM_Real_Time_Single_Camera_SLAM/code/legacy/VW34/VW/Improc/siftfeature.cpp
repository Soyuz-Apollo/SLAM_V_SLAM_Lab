/** 
 * @file    siftfeature.cpp
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

#include "siftfeature.h"
#include <iostream>


VW::SIFTFeature::SIFTFeature()
  : _orientation(0.0)
{
    for (int i=0; i<SIFT_DESCRIPTOR_SIZE; ++i) _descriptor[i] = 0.0;
}

VW::SIFTFeature::SIFTFeature(const CornerFeature& base_feature,
			     const double orientation)
  : CornerFeature(base_feature),
    _orientation(orientation)
{
  for (int i=0; i<SIFT_DESCRIPTOR_SIZE; ++i) _descriptor[i] = 0.0;
}

VW::SIFTFeature::SIFTFeature(const float x, const float y, 
			     const float strength, 
			     const float scale,
			     const double orientation,
			     const double descriptor[SIFT_DESCRIPTOR_SIZE])
  : VW::CornerFeature(x,y,strength,scale),
    _orientation(orientation)
{
  for (int i=0; i<SIFT_DESCRIPTOR_SIZE; ++i) _descriptor[i] = descriptor[i];
}

VW::SIFTFeature::SIFTFeature(const VW::SIFTFeature& feature)
  : CornerFeature(feature),
    _orientation(feature._orientation)
{
  //std::cerr << "SIFTFeature: Copy constructed" << std::endl;
  const int num_bytes = sizeof(double)*SIFT_DESCRIPTOR_SIZE;
  memcpy(&_descriptor, &feature._descriptor, num_bytes);
}

VW::SIFTFeature& VW::SIFTFeature::operator=(const VW::SIFTFeature& feature)
{
  //std::cerr << "SIFTFeature: operator= !" << std::endl;
  // First use the baseclass
  this->_x = feature._x;
  this->_y = feature._y;
  this->c = feature.c;
  this->r = feature.r;
  this->_strength = feature._strength;
  this->_scale = feature._scale;

  // Now the extras
  _orientation = feature._orientation;
  const int num_bytes = sizeof(double)*SIFT_DESCRIPTOR_SIZE;
  memcpy(&_descriptor, &feature._descriptor, num_bytes);
  return *this;
}

bool VW::SIFTFeature::WriteASCII(std::ostream& stream) const
{
  // Corner feature bits
  stream << _x << " " << _y << " " 
	 << _scale << " " << _strength << " ";
  // SIFT bits
  stream << _orientation << std::endl;
  const double* ptr = _descriptor;
  for (unsigned int i=0; i<SIFT_NUM_CELLS*SIFT_NUM_CELLS; i++) {
    for (unsigned int j=0;j<SIFT_NUM_ORIENTATIONS;j++,ptr++) {
      stream << *ptr << " ";
    }
    stream << std::endl;
  }
  return stream.good();
}

bool VW::SIFTFeature::WriteXML(std::ostream& stream,
			       const std::string& name) const
{
  std::cerr << "VW::SIFTFeature::WriteXML: Not implemented yet." << std::endl;
  return(false);
}

bool VW::SIFTFeature::WriteBinary(std::ostream& stream) const
{
  // Corner feature bits
  VW::WriteBinary(stream,_x);
  VW::WriteBinary(stream,_y);
  VW::WriteBinary(stream,_scale);
  VW::WriteBinary(stream,_strength);
  // SIFT bits
  VW::WriteBinary(stream,_orientation);
  VW::WriteBinary(stream,_descriptor,SIFT_DESCRIPTOR_SIZE);
  return stream.good();
}

bool VW::SIFTFeature::ReadASCII(std::istream& stream)
{
  std::cerr << "VW::SIFTFeature::ReadASCII: Not implemented yet." << std::endl;
  return(false);
}

bool VW::SIFTFeature::ReadXML(std::istream& stream, 
			      const std::string& name)
{
  std::cerr << "VW::SIFTFeature::ReadXML: Not implemented yet." << std::endl;
  return(false);
}

bool VW::SIFTFeature::ReadBinary(std::istream& stream)
{
  // Corner feature bits
  VW::ReadBinary(stream,_x);
  VW::ReadBinary(stream,_y);
  VW::ReadBinary(stream,_scale);
  VW::ReadBinary(stream,_strength);
  // SIFT bits
  VW::ReadBinary(stream,_orientation);
  VW::ReadBinary(stream,_descriptor,SIFT_DESCRIPTOR_SIZE);
  return stream.good();
}

void VW::SIFTFeature::NormaliseDescriptor()
{
  double total=0.0;
  const double* d1_ptr = _descriptor;
  for (unsigned int i=0;i<SIFT_DESCRIPTOR_SIZE;i++,d1_ptr++)
    total += (*d1_ptr)*(*d1_ptr);

  if (total>1e-8) {
    total = 1.0/sqrt(total);
    double* d2_ptr = _descriptor;
    double newtotal = 1.0;
    for (unsigned int i=0;i<SIFT_DESCRIPTOR_SIZE;i++,d2_ptr++) {
      *d2_ptr *= total;
      // At this point the original code also truncates any large values to 
      // 0.2, although no reason is given.
      if (*d2_ptr > 0.2) {
	newtotal += 0.04 - (*d2_ptr)*(*d2_ptr);
	*d2_ptr = 0.2;
      }
    }
    if (newtotal < 1.0) {
      newtotal = 1.0/sqrt(newtotal);
      // Truncating means we have to renormalise
      d2_ptr = _descriptor;
      for (unsigned int i=0;i<SIFT_DESCRIPTOR_SIZE;i++,d2_ptr++)
	*d2_ptr *= newtotal;
    }
  }
  // Do a quick check
//   d1_ptr = _descriptor;
//   double t=0;
//   std::cerr << "Descriptor: " << std::endl;
//   for (unsigned int i=0;i<SIFT_NUM_CELLS*SIFT_NUM_CELLS;i++) {
//     for (unsigned int j=0;j<SIFT_NUM_ORIENTATIONS;j++,d1_ptr++) {
//       std::cerr << *d1_ptr << " ";
//       t+=(*d1_ptr)*(*d1_ptr);
//     }
//     std::cerr << std::endl;
//   }
//   std::cerr << "   Magnitude=" << sqrt(t) << std::endl << std::endl;
 

}
