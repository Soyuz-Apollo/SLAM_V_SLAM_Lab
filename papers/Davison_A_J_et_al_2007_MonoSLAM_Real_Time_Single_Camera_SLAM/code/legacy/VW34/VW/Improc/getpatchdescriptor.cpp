/** 
 * @file    getpatchdescriptor.cpp
 * @author  BJ Tordoff
 * @date    25/11/2004
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

#include <VW/Improc/pyramid.h>
#include <VW/Utils/timer.h>

#include "getpatchdescriptor.h"
#include "patchfeature.h"

#define VERBOSE false
#define PI 3.14159265358979323846
#define TWOPI 6.2831853071795
#define PIBYTWO 1.57079632679489661923

using namespace std;

//////////// GaussianMask


// Simple classes to hold weighting arrays
class _Patch_GaussianMask {
public:
  _Patch_GaussianMask(unsigned int size, float sigma=1.0);
  ~_Patch_GaussianMask();
  unsigned int GetSize() const;
  void Print() const;
  const float* operator[](int row) const;
private:
  unsigned int _size;
  float* _weight;

};


inline _Patch_GaussianMask::_Patch_GaussianMask(unsigned int size, float sigma) 
  : _size(size), _weight(0)
{
  // Calculate the orientation calculation weighting mask
  const int half_array_size = _size/2;
  _weight = new float[_size*_size];
  
  const float inv_cov_scale = -0.5f / (sigma * sigma);
  float* ptr = _weight;
  for (int r=0; r<int(_size); ++r) {
    const float y = float(r - half_array_size);
    for (int c=0; c<int(_size); ++c, ++ptr) {
      const float x = float(c - half_array_size);
      const float e = (x*x + y*y) * inv_cov_scale;
      *ptr = exp(e); 
    }
  }
};

inline _Patch_GaussianMask::~_Patch_GaussianMask()
{
  delete[] _weight;
}

inline unsigned int 
_Patch_GaussianMask::GetSize() const
{
  return _size;
}

inline void 
_Patch_GaussianMask::Print() const
{
  const float* ptr = _weight;
  for (unsigned int r=0;r<_size;r++) {
    for (unsigned int c=0;c<_size;c++,ptr++) {
      cerr << (*ptr) << " ";
    }
    cerr << endl;
  }
}

inline const float* 
_Patch_GaussianMask::operator[](int row) const
{
  return _weight + row*_size;
}


//////////// GetSIFTDescriptor



static inline int Patch_GetOrientation(const int x, const int y,
				 const _Patch_GaussianMask& weights,
				 const VW::ImageMono<float>& im,
				 std::vector<double>& orientations)
{
  if (VERBOSE) cerr << "GetOrientation" << endl;
  vector<double> bin_score(36, 0.0);
  
  const int halfsize = weights.GetSize()/2;
  
  // Assume points are in image (shouldn't get this far otherwise)

  for (int iy = -halfsize; iy<=halfsize; ++iy) {
    VW::PixelMono<float>* imptr_m = im[y+iy-1] + x - halfsize;
    VW::PixelMono<float>* imptr_0 = im[y+iy]   + x - halfsize;
    VW::PixelMono<float>* imptr_p = im[y+iy+1] + x - halfsize;
    const float* w_ptr = weights[iy+halfsize];
    for (int ix = -halfsize; ix<=halfsize; ++ix,++imptr_m,++imptr_0,++imptr_p, ++w_ptr) {
      const float grad_y = imptr_p->y - imptr_m->y ;
      const float grad_x = imptr_0[1].y - imptr_0[-1].y;
      const double this_ori = PI + atan2(grad_y,grad_x);
      const double this_grad = sqrt(grad_y*grad_y + grad_x*grad_x);
      const double contrib = *w_ptr * this_grad;
      const int bindex = int(1e-8+36.0*this_ori/TWOPI)%36;
      const int bindex_dn = (bindex+35)%36;
      const int bindex_up = (bindex+1)%36;
      bin_score[bindex_dn]    += 0.2*contrib;
      bin_score[bindex]       += 0.6*contrib;
      bin_score[bindex_up]    += 0.2*contrib;
    }
  }
  
    if (VERBOSE) cerr << "   Find maxima" << endl;
    double max_score=0.0;
    vector<double> max_v;max_v.reserve(18);
    vector<int>    max_b;max_b.reserve(18);
    // first and last element (only one of them can be a maxima!)
    if ((bin_score[0]>bin_score[35]) && (bin_score[0]>bin_score[1])) {
      max_score = bin_score[0];
      max_v.push_back(bin_score[0]);
      max_b.push_back(0);
    } else if ((bin_score[35]>bin_score[34]) 
	       && (bin_score[35]>bin_score[0])) {
      max_score = bin_score[35];
      max_v.push_back(bin_score[35]);
      max_b.push_back(35);
    }
    // middle elements
    for (int bin_num=1;bin_num<34;bin_num++) {
      const double this_score = bin_score[bin_num];
      if ((this_score>bin_score[bin_num-1]) 
	  && (this_score>bin_score[bin_num+1])) {
	max_v.push_back(this_score);
	max_b.push_back(bin_num);
	if (max_score<this_score) max_score = this_score;
      }
    }
    const double acceptance_threshold = max_score*0.8;
    if (VERBOSE) cerr << "   Got " << max_v.size() << " maxima (acceptance threshold="<<acceptance_threshold<<"):" << endl;
    orientations.clear();
    for (unsigned int m=0;m<max_v.size();m++) {
      const double val_0 = max_v[m];
      if (val_0>acceptance_threshold) {
	//	cerr << "   angle=" << max_b[m]*10 << " value=" <<val_0<<endl;
	const double val_p = bin_score[(max_b[m]+1)%36];
	const double val_m = bin_score[(max_b[m]+35)%36];
	const double grad_x = (val_p-val_m); // Actually half this, but it cancels!
	const double grad2_x = (val_p-2.0*val_0+val_m); // Actually half this
	const double delta_x =  - 0.5*grad_x/grad2_x;
	const double bin_to_radians = TWOPI/36.0;
	orientations.push_back(bin_to_radians*(max_b[m]+0.5+delta_x)) ;
      }
    }     
    if (VERBOSE) 
      cerr << "   Left with " << orientations.size() << " maxima\n";

  return orientations.size();
}








static inline void GenerateDescriptor(const float x, const float y,
				      VW::PatchFeature& feature,
				      const VW::ImageMono<float>& im)
{
  if (VERBOSE) cerr << "GenerateDescriptor at (" <<x<<","<<y<<")"<< endl;
  // Simply dump pixel values (interpolated, of course) into the patch
  const unsigned int size = feature.Patch().GetSize();
  const unsigned int halfsize = size/2;
  const int ix = int(x);
  const int iy = int(y);
  const float dx = x - float(ix);
  const float dy = y - float(iy);

  // coeffs for bi-linear interp
  const float a00 = (1.0f - dy)*(1.0f - dx);
  const float a01 = (1.0f - dy)*dx;
  const float a10 = dy*(1.0f - dx);
  const float a11 = dy*dx;
  
  for (unsigned int row=0;row<size;row++) {
    const VW::PixelMono<float>* ptr_0 = im[row+iy-halfsize] + ix - halfsize;
    const VW::PixelMono<float>* ptr_1 = im[row+iy-halfsize+1] + ix - halfsize;
    float* p_ptr = feature.Patch()[row];
    for (unsigned int col=0;col<size;col++, ptr_0++, ptr_1++) {
      *p_ptr++ = a00*ptr_0[0].y + a01*ptr_0[1].y 
	       + a10*ptr_1[0].y + a11*ptr_1[1].y;
    }
  }
  if (VERBOSE) cerr << "   done generating descriptor" << endl;
}

static inline void GenerateOrientedDescriptor(const float x, const float y,
					      VW::PatchFeature& feature,
					      const VW::ImageMono<float>& im)
{
  if (VERBOSE) cerr << "GenerateDescriptor at (" <<x<<","<<y<<") orientation "<< feature.GetOrientation() << endl;
  // Simply dump pixel values (interpolated, of course) into the patch
  const unsigned int size = feature.Patch().GetSize();
  const int halfsize = size/2;
  const float c = cos(float(feature.GetOrientation()));
  const float s = sin(float(feature.GetOrientation()));
  //  cerr << "Centre point (" << x << "," << y << ")"<<endl;
  //  cerr << "Orientation "<< feature.GetOrientation() << endl;
      
  for (int row=-halfsize;row<=halfsize;row++) {
    // coord starts at [cos -sin] [ -halfsize ]
    //                 [sin  cos] [    row    ]
    float tx = x - halfsize*c - row*s;
    float ty = y - halfsize*s + row*c;

    // steps are  [cos -sin] [ 1 ] = [ cos ]
    //            [sin  cos] [ 0 ]   [ sin ]
    float* p_ptr = feature.Patch()[row+halfsize];
    for (unsigned int col=0;col<size;col++, tx+=c, ty+=s) {
      //          cerr << "Sample point (" << tx<<","<<ty<<")"<<endl;
      *p_ptr++ = im.GetInterpolatedPixel(tx,ty).y;
    }
    //   cerr << "Done "<< row << endl;
  }
}

bool VW::GetPatchDescriptor::
Compute(const std::vector<VW::CornerFeature>& locations,
	const VW::Pyramid<float>& pyramid,
	std::vector<VW::PatchFeature>& resulting_features) const
{
  if (VERBOSE) cerr << "VW::GetPatchDescriptor" << endl;
  const unsigned int num_features = locations.size();
  if (VERBOSE) cerr << " going to process " << num_features << " locations." << endl;

  // Pre-allocate storage space for the features (plus a bit more in case
  // of ambiguous ones)
  resulting_features.clear();
  resulting_features.reserve(num_features*3/2);
  
  // Check patch size is an odd number (otherwise can't be symmetrical)
  if (((_patchsize+1)/2)==(_patchsize/2)) {
    cerr << "Patch size must be an odd number (you requested " << _patchsize << ")" << endl;
    return false;
  }

  // Calculate the orientation weighting mask (so that border = 3 sigma)
  const int half_array_size = _patchsize/2;
  const _Patch_GaussianMask orientation_weight(_patchsize,float(_patchsize)/6.0f);
  // Safety margin is one bigger than patch for gradient operator
  int safety_margin = half_array_size+1;
  // If orientations can vary, need to be even further from image edge
  if (_oriented) {
    safety_margin = int(2.0+double(half_array_size)*sqrt(2.0));
    //    cerr << "new safety margin of " << safety_margin << " (half size is " << half_array_size << ")" << endl;
  }
 
  for (unsigned int i=0; i<num_features; ++i) {
 
    if (VERBOSE) cerr << "Process feature " << i << endl;
    const VW::CornerFeature& location = locations[i];
 
    // Find the closest entry in the pyramid
    const int level = pyramid.ClosestLevel(1.0f/location.GetFeatureScale());
    const float scale = pyramid.GetScale(level);

    const VW::ImageMono<float>& im = pyramid[level];

    // Check this feature isn't too close to the edge (half_size+1)
    const float fx = scale*float(location.GetX());
    const float fy = scale*float(location.GetY());
    //cerr << "   scale=" << scale << " location=("<<fx<<","<<fy<<")"<<endl;
    const int x = int(fx);
    const int y = int(fy);
    //    cerr << "Point limits ("<<x-safety_margin<<","<<y-safety_margin<<") to (" << x+safety_margin << ","<<y+safety_margin<<")"<<endl;
    if (((x-safety_margin)>=im.ROI().Left())
	&& ((x+safety_margin)<im.ROI().Right())
	&& ((y-safety_margin)>=im.ROI().Top())
	&& ((y+safety_margin)<im.ROI().Bottom())) {
      if (_oriented) {
	// Fits in image OK, so get the orientations
	std::vector<double> orientations;
	const int unsigned num_oris = Patch_GetOrientation(x, y,
						       orientation_weight,
						       im,
						       orientations);
	for (unsigned int o=0;o<num_oris;o++) {
	  if (VERBOSE) cerr << "   generate feature " << i << " orientation " << o+1 << "/" << num_oris << endl;
	  PatchFeature feature(location,_patchsize,orientations[o]);
	  if (VERBOSE) cerr << "   get descriptor" << endl;
	  ::GenerateOrientedDescriptor(fx,fy,feature,im);
	  feature.Patch().Normalise(_weighted);
	  resulting_features.push_back(feature);
	} // End for each orientation
      } else {
	// Not oriented
	PatchFeature feature(location,_patchsize,0);
	::GenerateDescriptor(fx,fy,feature,im);
	feature.Patch().Normalise(_weighted);
	resulting_features.push_back(feature);
      } // End if (_oriented)
    } else {// End if (fits in image)
      if (VERBOSE) cerr << "Too close to image edge ("<<x<<","<<y<<")"<<endl;
    }
  } // End for (input_features)
   return true;
}
