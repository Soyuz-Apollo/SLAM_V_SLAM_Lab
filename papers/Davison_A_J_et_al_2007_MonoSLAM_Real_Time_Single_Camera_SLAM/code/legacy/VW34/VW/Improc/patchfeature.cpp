/** 
 * @file    patchfeature.cpp
 * @author  BJ Tordoff
 * @date    24/11/2004
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

#include "patchfeature.h"
#include <iostream>
#include <assert.h>

using namespace std;


// Simple classes to hold weighting arrays
class _pf_GaussianMask {
public:
  _pf_GaussianMask(unsigned int size,
	       float sigma=1.0);
  ~_pf_GaussianMask();
public:
  float* weight;

};


_pf_GaussianMask::_pf_GaussianMask(unsigned int size,
			   float sigma) 
{
  weight = new float[size*size];
  // Calculate the orientation calculation weighting mask
  const int half_array_size = size/2;
  
  float tot_weight = 0.0f;
  const float inv_cov_scale = -0.5f / (sigma * sigma);
  float* ptr = weight;
  for (int r=0; r<int(size); ++r) {
    const float y = float(r - half_array_size);
    for (int c=0; c<int(size); ++c, ++ptr) {
      const float x = float(c - half_array_size);
      const float e = (x*x + y*y) * inv_cov_scale;
      *ptr = exp(e); 
      tot_weight += *ptr;
    }
  }
  const float scale = 1.0f/tot_weight;

  for (unsigned int i=0; i<(size*size); i++) weight[i]*=scale;

};

_pf_GaussianMask::~_pf_GaussianMask()
{
  delete[] weight;
}

VW::ZNPatch::ZNPatch(unsigned int size)
  : _size(size), _data(0)
{
  _AllocData();
}

VW::ZNPatch::ZNPatch(const ZNPatch& p)
  : _size(p._size), _data(0)
{
  _AllocData();
  memcpy(_data, p._data, _size*_size*sizeof(_data[0]));
}

VW::ZNPatch& VW::ZNPatch::operator=(const ZNPatch& p)
{
  _size = p._size;
  _AllocData();
   memcpy(_data, p._data, _size*_size*sizeof(_data[0])); 
	return *this;
}

VW::ZNPatch::~ZNPatch()
{
  delete[] _data;
}

void VW::ZNPatch::_AllocData()
{
  if (_data != 0) delete[] _data;
   _data = new float[_size*_size];
}

float VW::ZNPatch::GetValue(unsigned int x, unsigned int y) const
{
  assert(x<_size);
  assert(y<_size);
  return _data[y*_size+x];
}

void VW::ZNPatch::SetValue(unsigned int x, unsigned int y, float val)
{
  assert(x<_size);
  assert(y<_size);
  _data[y*_size+x] = val;
}

void VW::ZNPatch::SetData(unsigned int size, float* new_values)
{
  _size = size;
  _AllocData();
  for (unsigned int i=0;i<(_size*_size);i++) _data[i] = new_values[i];
}

float VW::ZNPatch::GetMaxValue() const
{
  if (_size==0) return 0.0f;
  const unsigned int n = _size*_size;
  float* ptr = _data;
  float result = *ptr;
  for (unsigned int i=1;i<n;i++,ptr++) {
    if (*ptr > result) result = *ptr;
  }
  return result;
}

float VW::ZNPatch::GetMinValue() const
{
  if (_size==0) return 0.0f;
  const unsigned int n = _size*_size;
  float* ptr = _data;
  float result = *ptr;
  for (unsigned int i=1;i<n;i++,ptr++) {
    if (*ptr < result) result = *ptr;
  }
  return result;
}

float* VW::ZNPatch::operator[](int row) 
{
  return _data + row*_size;
}

const float* VW::ZNPatch::operator[](int row) const
{
  return _data + row*_size;
}

unsigned int VW::ZNPatch::GetSize() const
{
  return _size;
}

void VW::ZNPatch::Normalise(bool weighted)
{
  assert(_size>0);
  const float* data_end = _data+_size*_size;

  if (weighted) {
    // Obtain the weight mask
    _pf_GaussianMask g(_size,float(_size)/4.0f); // Make edge be 2*sigma

    float mean = 0.0f;
    { // Build weighted mean
      float* gptr = g.weight;
      float* ptr = _data;
      for ( ; ptr != data_end; ptr++, gptr++) mean += (*ptr)*(*gptr);
    }
    float var = 0.0f;
    { // Build weighted variance
      float* gptr = g.weight;
      float* ptr = _data;
      for ( ; ptr != data_end; ptr++, gptr++) {
	(*ptr) = (*gptr) * ((*ptr) - mean);
	var += (*ptr)*(*ptr);
      }
    }
    //    const float invstddev = float(_size)/sqrt(var);
    const float invstddev = 1.0f/sqrt(var);

    { // And normalise
      float* ptr = _data;
      float* gptr = g.weight;
     for ( ; ptr != data_end; ptr++, gptr++) {
       (*ptr) *= invstddev;
      }
    }

  } else {
    // Do unweighted norm
    
    // First calculate mean and variance
    float sum = 0.0f, sum2 = 0.0f;
    const float* data_end = _data+_size*_size;
    for (float* ptr = _data ; ptr != data_end; ptr++) {
      sum += (*ptr);
      sum2 += (*ptr)*(*ptr);
    }
    const float mean = sum/(_size*_size);
    const float var = sum2/(_size*_size) - (mean*mean);
    const float invstddev = 1.0f/(_size*sqrt(var));
    // Now correct values
    for (float* ptr = _data; ptr != data_end; ptr++) {
      *ptr = (*ptr - mean) * invstddev;
    }
  }

//   {    // Check
//     float sum=0.0f;
//     float sum2=0.0f;
//     const unsigned int N = (_size*_size);
//     for (unsigned int i=0;i<N;i++) {
//       sum += _data[i];
//       sum2 += _data[i]*_data[i];
//     }
//     cerr << "Mean=" << sum << endl;
//     cerr << "Var=" << sum2 << endl;
//   }


}

bool VW::ZNPatch::WriteASCII(std::ostream& s) const
{
  s << _size << "x" << _size << endl;
  const float* ptr = _data;
  for (unsigned int row=0;row<_size;row++) {
    for (unsigned int col=0;col<_size;col++) {
      s << *ptr++ << " ";
    }
    s << endl;
  }
  return s.good();
}

bool VW::ZNPatch::WriteBinary(std::ostream& s) const
{
  VW::WriteBinary(s,_size);
  VW::WriteBinary(s,_data,_size*_size);
  return s.good();
}

bool VW::ZNPatch::ReadBinary(std::istream& s)
{
  VW::ReadBinary(s,_size);
  _AllocData();
  VW::ReadBinary(s,_data,_size*_size);
  return s.good();
}

std::ostream& VW::operator<<(std::ostream& s, const VW::ZNPatch& p)
{
  p.WriteASCII(s);
  return(s);
}






VW::PatchFeature::PatchFeature(unsigned int patch_size)
  : _orientation(0.0),
    _patch(patch_size)
{
}


VW::PatchFeature::PatchFeature(const CornerFeature& base_feature,
			       unsigned int patch_size,
			       const double orientation)
  : CornerFeature(base_feature),
    _orientation(orientation),
    _patch(patch_size)
{
}

VW::PatchFeature::PatchFeature(const float x, const float y, 
			       const float strength, 
			       const float scale,
			       unsigned int patch_size,
			       const double orientation)
  : VW::CornerFeature(x,y,strength,scale),
    _orientation(orientation),
    _patch(patch_size)
{
}


bool VW::PatchFeature::WriteASCII(std::ostream& stream) const
{
  // Corner feature bits
  stream << _x << " " << _y << " " 
	 << _scale << " " << _strength << " ";
  // Patch bits
  stream << _orientation << std::endl;
  stream << _patch << std::endl;
  return stream.good();
}

bool VW::PatchFeature::WriteXML(std::ostream& stream,
			       const std::string& name) const
{
  std::cerr << "VW::PatchFeature::WriteXML: Not implemented yet." << std::endl;
  return(false);
}

bool VW::PatchFeature::WriteBinary(std::ostream& stream) const
{
  // Corner feature bits
  VW::WriteBinary(stream,_x);
  VW::WriteBinary(stream,_y);
  VW::WriteBinary(stream,_scale);
  VW::WriteBinary(stream,_strength);
  // Patch bits
  VW::WriteBinary(stream,_orientation);
  _patch.WriteBinary(stream);
  return stream.good();
}

bool VW::PatchFeature::ReadASCII(std::istream& stream)
{
  std::cerr << "VW::PatchFeature::ReadASCII: Not implemented yet." << std::endl;
  return(false);
}

bool VW::PatchFeature::ReadXML(std::istream& stream, 
			      const std::string& name)
{
  std::cerr << "VW::PatchFeature::ReadXML: Not implemented yet." << std::endl;
  return(false);
}

bool VW::PatchFeature::ReadBinary(std::istream& stream)
{
  // Corner feature bits
  VW::ReadBinary(stream,_x);
  VW::ReadBinary(stream,_y);
  VW::ReadBinary(stream,_scale);
  VW::ReadBinary(stream,_strength);
  // Patch bits
  VW::ReadBinary(stream,_orientation);
  _patch.ReadBinary(stream);
  return stream.good();
}
