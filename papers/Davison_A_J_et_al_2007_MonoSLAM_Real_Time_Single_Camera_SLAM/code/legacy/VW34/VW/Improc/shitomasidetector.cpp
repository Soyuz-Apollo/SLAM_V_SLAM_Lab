/** 
 * @file    shitomasidetector.cpp
 * @author  BJ Tordoff & AJ Davison
 * @date    29/11/2004
 *
 * Modified by BJT to class form
 *
 *             (c) Copyright University of Cambridge
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

#include <iostream>
#include <VW/Improc/cornerfeature.h>
#include <VW/Improc/pyramid.h>
#include <VW/Image/imagemono.h>
#include "shitomasidetector.h"

#define VERBOSE false

using namespace std;

/** Simple function to find the smallest eigenvalue of the 2*2 symmetric 
 * matrix   [A B]
 *          [B C] 
 */
static inline double Find2xSmallestEigenvalue(double A, double B, double C)
{
  const double BB = sqrt( (A + C) * (A + C) - 4 * (A * C - B * B) );

  //  const double big_eigenval = 0.5*(A + C + BB);
  //  const double small_eigenval = 0.5*(A + C - BB);
  return (A + C - BB);
}

static inline float InsertFeature(std::vector<VW::CornerFeature>& features,
											 const VW::CornerFeature& feature,
											 unsigned int limit)
{
  const unsigned int num = features.size();
  const float this_score = feature.GetCornerStrength();
  if (num>0) {
	 // Find insertion point
	 float upper_score = features[0].GetCornerStrength();
	 float lower_score = features[num-1].GetCornerStrength();

	 if (this_score > upper_score) {
		// This is the new best, so add to front
		features.insert(features.begin(),feature);
	 } else if (this_score < lower_score) {
		// This is the new worst, so add to back
		if (num<limit) features.push_back(feature);
	 } else {
		// Somewhere in between, so find it
		int upper_index = 0;
		int lower_index = num-1;
		int region_size = lower_index - upper_index;
		// upper_index always points to an entry >this_score
		// lower_index always points to an entry <=this_score
		while (region_size>1) {
		  int mid_index = int(0.5*(upper_index + lower_index));
		  if (features[mid_index].GetCornerStrength() <= this_score) {
			 // We want top half
			 lower_index = mid_index;
		  } else {
			 // We want bottom half
			 upper_index = mid_index;
		  }
		  region_size = lower_index - upper_index;
		}
		features.insert(features.begin()+lower_index,feature);
	 }
  } else {
	 // Empty list, so just add
	 features.push_back(feature);
  }

  if (num>=limit) {
	 // list is overfull, so clip and return end value
	 features.resize(limit);
	 return features[num].GetCornerStrength();
  } else if(num==(limit-1)) {
	 // List is exactly filled, so return end value
	 return features[num].GetCornerStrength();
  }
  // Haven't reached limit yet, so any score is OK
  return 0.0f;
}

// Return whether modulus of elem1 is less than modulus of elem2
static inline bool IsStronger(const VW::CornerFeature& f1, 
										const VW::CornerFeature& f2)
{
  return f1.GetCornerStrength() > f2.GetCornerStrength();
}


void VW::ShiTomasiDetector::Compute(const VW::Pyramid<float>& pyramid,
												std::vector<CornerFeature>& features)
{
  features.clear();
  features.reserve(_max_features+1);
  for (unsigned int level=0;level<pyramid.NumLevels();level++) {
    const double this_scale = 1.0/pyramid.GetScale(level);
    _DoCompute(pyramid[level],features,this_scale);
  }
}


template <class T>
void VW::ShiTomasiDetector::_DoCompute(const VW::ImageMono<T>& image,
													std::vector<CornerFeature>& features,
													float image_scale)
{
  if (VERBOSE) cerr << "VW::ShiTomasiDetector::Compute(image,feature)" << endl;
  const unsigned int imwidth = image.GetWidth();

  // work out the threshold in intensity levels
  const double max_val = 255*255*(4*(_patchsize-1) - 2);
  const double unnorm_threshold = _threshold*max_val;

  /* All pixels must lie inside ROI, so  */
  const unsigned int halfsize = _patchsize/2;
  const int grad_first_row = image.ROI().Top() + 1;
  const int grad_last_row = image.ROI().Bottom() - 1;
  const int grad_first_col = image.ROI().Left() + 1;
  const int grad_last_col = image.ROI().Right() - 1;
  const int rslt_first_row = grad_first_row + halfsize;
  const int rslt_last_row = grad_last_row - halfsize;
  const int rslt_first_col = grad_first_col + halfsize;
  const int rslt_last_col = grad_last_col - halfsize;
  if ((rslt_first_row>=rslt_last_row) || (rslt_first_col>=rslt_last_col)) {
    cerr << "VW::ShiTomasiDetector::Compute: Aaargh, ROI is too small! (ROI is " << image.ROI().Width() << "x" << image.ROI().Height() << ", convolution patch is " << _patchsize << "x" << _patchsize << ")" << endl;
    exit(0);
  }
  if (VERBOSE) {
    cerr << "Result range: ("<<rslt_first_col<<","<<rslt_first_row
	 <<") - ("<<rslt_last_col<<","<<rslt_last_row<<")" << endl;
    cerr << "Gradient range: ("<<grad_first_col<<","<<grad_first_row
	 <<") - ("<<grad_last_col<<","<<grad_last_row<<")" << endl;
  }

  /* Calculate the width we need to find gradients in (one pixel away from
   * each edge). 
   */
  const unsigned int grad_width = image.ROI().Width() - 2;
  const unsigned int result_width = rslt_last_col - rslt_first_col;
  const unsigned int result_height = rslt_last_row - rslt_first_row;

  /* Arrays which store column sums of height BOXSIZE */
  vector<double> CSgxsq(grad_width), 
                 CSgysq(grad_width), 
                 CSgxgy(grad_width);

  double* score_array = new double[result_height*result_width];
  double** score = new double*[result_height];
  for (unsigned int s=0;s<result_height;s++) 
    score[s] = score_array + s*result_width;

  /* Initial stage: fill these sums for the first horizontal position */
  unsigned int i = 0;
  for (int col = grad_first_col; col < grad_last_col; col++, i++) {
    CSgxsq[i] = 0; CSgysq[i] = 0; CSgxgy[i] = 0;
    // Pointers to current row, row above and below
    VW::PixelMono<T>* ptr_m = image[grad_first_row-1] + col;
    VW::PixelMono<T>* ptr_0 = image[grad_first_row]   + col;
    VW::PixelMono<T>* ptr_p = image[grad_first_row+1] + col;
    for (unsigned int row = grad_first_row; row < grad_first_row+_patchsize; 
	 row++, ptr_m+=imwidth, ptr_0+=imwidth, ptr_p+=imwidth) {
      const double gx = ptr_0[1].y - ptr_0[-1].y;
      const double gy = ptr_p->y - ptr_m->y;

      CSgxsq[i] += gx * gx;
      CSgysq[i] += gy * gy;
      CSgxgy[i] += gx * gy;      
    }
  }

  /* Now loop through u and v to form final sums at each position (u, v) */
  double TSgxsq = 0.0, TSgysq = 0.0, TSgxgy = 0.0;
  double* score_ptr = score_array;
  for (int row = rslt_first_row; row < rslt_last_row; row++) {
    int col = rslt_first_col;

    /* Form first sums for first column of result */
    TSgxsq = 0.0, TSgysq = 0.0, TSgxgy = 0.0;
    for (unsigned int i = 0; i < _patchsize; i++) {
      TSgxsq += CSgxsq[i];
      TSgysq += CSgysq[i];
      TSgxgy += CSgxgy[i];
    }
    *score_ptr++ = Find2xSmallestEigenvalue(TSgxsq, TSgxgy, TSgysq);
    // Done first element, now do rest


    unsigned int old_index=0, new_index=_patchsize;
    for (col = rslt_first_col+1; col < rslt_last_col; 
	 col++, old_index++, new_index++) {
      /* Subtract old column, add new one */
      TSgxsq += CSgxsq[new_index] - CSgxsq[old_index]; 
      TSgysq += CSgysq[new_index] - CSgysq[old_index];
      TSgxgy += CSgxgy[new_index] - CSgxgy[old_index];
      
      *score_ptr++ = Find2xSmallestEigenvalue(TSgxsq, TSgxgy, TSgysq);
    }

    if (row < (rslt_last_row - 1)) {
      /* Update the column sums for the next row */
      const unsigned int oldrow = row-halfsize;
      const unsigned int newrow = row+halfsize+1;
      { // First subtract the old one
		  VW::PixelMono<T>* ptr_m = image[oldrow-1]+grad_first_col;
		  VW::PixelMono<T>* ptr_0 = image[oldrow]  +grad_first_col;
		  VW::PixelMono<T>* ptr_p = image[oldrow+1]+grad_first_col;
		  for (unsigned int index=0; index<grad_width; 
				 index++, ptr_m++, ptr_0++, ptr_p++){
			 // Gradients at old pixel
			 const double gx = ptr_0[1].y - ptr_0[-1].y;
			 const double gy = ptr_p->y - ptr_m->y;
			 CSgxsq[index] -= gx * gx;
			 CSgysq[index] -= gy * gy;
			 CSgxgy[index] -= gx * gy;      
		  }    
      } 
      { // Now add the new one
		  VW::PixelMono<T>* ptr_m = image[newrow-1]+grad_first_col;
		  VW::PixelMono<T>* ptr_0 = image[newrow]  +grad_first_col;
		  VW::PixelMono<T>* ptr_p = image[newrow+1]+grad_first_col;
		  for (unsigned int index=0; index<grad_width; 
				 index++, ptr_m++, ptr_0++, ptr_p++){
			 // Gradients at old pixel
			 const double gx = ptr_0[1].y - ptr_0[-1].y;
			 const double gy = ptr_p->y - ptr_m->y;
			 
			 CSgxsq[index] += gx * gx;
			 CSgysq[index] += gy * gy;
			 CSgxgy[index] += gx * gy;      
		  }    
      } 
    }
  }
 
  // Now scan the score array for maxima
  const double max_val_scaling = 1.0/(255*255*(4*(_patchsize-1) - 2));
  const unsigned int row_max = result_height-1;
  const unsigned int col_max = result_width-1;

  // If a limit on number has been set, respect it
  if (_max_features > 0) {
	 // If adding to an existing list, check th current min score
	 float min_score=0.0f;
	 if (features.size() >= _max_features) {
		min_score = features[_max_features-1].GetCornerStrength();
	 }
	 for (unsigned int row=1;row<row_max;row++) {
		double* sptr_m = score[row-1]+1;
		double* sptr_0 = score[row  ]+1;
		double* sptr_p = score[row+1]+1;
		for (unsigned int col=1;col<col_max;col++,sptr_m++, sptr_0++, sptr_p++) {
		  const double s = sptr_0[0];
		  if ((s>unnorm_threshold) && (s>min_score)
				&& (s>sptr_m[-1]) && (s>sptr_m[0]) && (s>sptr_m[1])
				&& (s>sptr_0[-1])                  && (s>sptr_0[1])
				&& (s>sptr_p[-1]) && (s>sptr_p[0]) && (s>sptr_p[1])) {

			 const CornerFeature f(float(col+rslt_first_col)*image_scale,
										  float(row+rslt_first_row)*image_scale,
										  float(max_val_scaling*s),
										  image_scale);
			 min_score = InsertFeature(features,f,_max_features);
		  }
		}
	 }  
  } else {
	 for (unsigned int row=1;row<row_max;row++) {
		double* sptr_m = score[row-1]+1;
		double* sptr_0 = score[row  ]+1;
		double* sptr_p = score[row+1]+1;
		for (unsigned int col=1;col<col_max;col++,sptr_m++, sptr_0++, sptr_p++) {
		  const double s = sptr_0[0];
		  if ((s>unnorm_threshold)
				&& (s>sptr_m[-1]) && (s>sptr_m[0]) && (s>sptr_m[1])
				&& (s>sptr_0[-1])                  && (s>sptr_0[1])
				&& (s>sptr_p[-1]) && (s>sptr_p[0]) && (s>sptr_p[1])) {
			 const CornerFeature f(float(col+rslt_first_col)*image_scale,
										  float(row+rslt_first_row)*image_scale,
										  float(max_val_scaling*s),
										  image_scale);
			 features.push_back(f);
		  }
		}
	 }
  }
  delete[] score;
  delete[] score_array;
}

void VW::ShiTomasiDetector::Compute(const VW::Pyramid<float>& pyramid,
												CornerFeature& best_feature)
{
  best_feature.Set(0.0,0.0,
		   0.0,0.0);
  for (unsigned int level=0;level<pyramid.NumLevels();level++) {
    const double this_scale = 1.0/pyramid.GetScale(level);
    CornerFeature this_feature;
    Compute(pyramid[level],this_feature);
    if (this_feature.GetCornerStrength() > best_feature.GetCornerStrength()) {
      best_feature.Set(float(this_feature.GetX()*this_scale),
		       float(this_feature.GetY()*this_scale),
		       this_feature.GetCornerStrength(),
		       float(this_scale));
    }
  }
}

template <class T>
void VW::ShiTomasiDetector::Compute(const VW::ImageMono<T>& image,
												CornerFeature& best_feature)

{
  if (VERBOSE) cerr << "VW::ShiTomasiDetector::Compute(image,feature)" << endl;
  const unsigned int imwidth = image.GetWidth();

  const double max_val = 255*255*(4*(_patchsize-1) - 2);
  const double unnorm_threshold = _threshold*max_val;

  /* All pixels must lie inside ROI, so  */
  const unsigned int halfsize = _patchsize/2;
  const int grad_first_row = image.ROI().Top() + 1;
  const int grad_last_row = image.ROI().Bottom() - 1;
  const int grad_first_col = image.ROI().Left() + 1;
  const int grad_last_col = image.ROI().Right() - 1;
  const int rslt_first_row = grad_first_row + halfsize;
  const int rslt_last_row = grad_last_row - halfsize;
  const int rslt_first_col = grad_first_col + halfsize;
  const int rslt_last_col = grad_last_col - halfsize;
  if ((rslt_first_row>=rslt_last_row) || (rslt_first_col>=rslt_last_col)) {
    cerr << "VW::ShiTomasiDetector::Compute: Aaargh, ROI is too small!" << endl;
    exit(0);
  }
  if (VERBOSE) {
    cerr << "Result range: ("<<rslt_first_col<<","<<rslt_first_row
	 <<") - ("<<rslt_last_col<<","<<rslt_last_row<<")" << endl;
    cerr << "Gradient range: ("<<grad_first_col<<","<<grad_first_row
	 <<") - ("<<grad_last_col<<","<<grad_last_row<<")" << endl;
  }

  /* Calculate the width we need to find gradients in (one pixel away from
   * each edge). 
   */
  const unsigned int grad_width = image.ROI().Width() - 2;
  //  const unsigned int result_width = rslt_last_col - rslt_first_col;

  /* Arrays which store column sums of height BOXSIZE */
  vector<double> CSgxsq(grad_width), 
                 CSgysq(grad_width), 
                 CSgxgy(grad_width);

  /* Initial stage: fill these sums for the first horizontal position */
  unsigned int i = 0;
  for (int col = grad_first_col; col < grad_last_col; col++, i++) {
    CSgxsq[i] = 0; CSgysq[i] = 0; CSgxgy[i] = 0;
    // Pointers to current row, row above and below
    VW::PixelMono<T>* ptr_m = image[grad_first_row-1] + col;
    VW::PixelMono<T>* ptr_0 = image[grad_first_row]   + col;
    VW::PixelMono<T>* ptr_p = image[grad_first_row+1] + col;
    for (unsigned int row = grad_first_row; row < grad_first_row+_patchsize; 
	 row++, ptr_m+=imwidth, ptr_0+=imwidth, ptr_p+=imwidth) {
      const double gx = ptr_0[1].y - ptr_0[-1].y;
      const double gy = ptr_p->y - ptr_m->y;

      CSgxsq[i] += gx * gx;
      CSgysq[i] += gy * gy;
      CSgxgy[i] += gx * gy;      
    }
  }

  /* Now loop through u and v to form final sums at each position (u, v) */
  double TSgxsq = 0.0, TSgysq = 0.0, TSgxgy = 0.0;
  double best_score=0.0, best_x=0.0, best_y=0.0;
  for (int row = rslt_first_row; row < rslt_last_row; row++) {
    int col = rslt_first_col;

    /* Form first sums for first column of result */
    TSgxsq = 0.0, TSgysq = 0.0, TSgxgy = 0.0;
    for (unsigned int i = 0; i < _patchsize; i++) {
      TSgxsq += CSgxsq[i];
      TSgysq += CSgysq[i];
      TSgxgy += CSgxgy[i];
    }
    const double sev = Find2xSmallestEigenvalue(TSgxsq, TSgxgy, TSgysq);
    if ((sev>unnorm_threshold) && (sev > best_score)) {
      if (VERBOSE) cerr << "   New best feature (" << col << "," << row << ") with score " << sev << endl;
      best_x = col;
      best_y = row;
      best_score = sev;
    }
    // Done first element, now do rest


    unsigned int old_index=0, new_index=_patchsize;
    for (col = rslt_first_col+1; col < rslt_last_col; 
	 col++, old_index++, new_index++) {
      /* Subtract old column, add new one */
      TSgxsq += CSgxsq[new_index] - CSgxsq[old_index]; 
      TSgysq += CSgysq[new_index] - CSgysq[old_index];
      TSgxgy += CSgxgy[new_index] - CSgxgy[old_index];
      
      const double sev = Find2xSmallestEigenvalue(TSgxsq, TSgxgy, TSgysq);

      /* eval2 will be the smaller eigenvalue. Compare it with the one
	 we've already got */
      if ((sev>unnorm_threshold) && (sev > best_score)) {
	if (VERBOSE) cerr << "   New best feature (" << col << "," << row << ") with score " << sev << endl;
	best_x = col;
	best_y = row;
	best_score = sev;
      }
    }

    if (row < (rslt_last_row - 1)) {
      /* Update the column sums for the next row */
      const unsigned int oldrow = row-halfsize;
      const unsigned int newrow = row+halfsize+1;
      { // First subtract the old one
	VW::PixelMono<T>* ptr_m = image[oldrow-1]+grad_first_col;
	VW::PixelMono<T>* ptr_0 = image[oldrow]  +grad_first_col;
	VW::PixelMono<T>* ptr_p = image[oldrow+1]+grad_first_col;
	for (unsigned int index=0; index<grad_width; 
	     index++, ptr_m++, ptr_0++, ptr_p++){
	  // Gradients at old pixel
	  const double gx = ptr_0[1].y - ptr_0[-1].y;
	  const double gy = ptr_p->y - ptr_m->y;
	  CSgxsq[index] -= gx * gx;
	  CSgysq[index] -= gy * gy;
	  CSgxgy[index] -= gx * gy;      
	}    
      } 
      { // Now add the new one
	VW::PixelMono<T>* ptr_m = image[newrow-1]+grad_first_col;
	VW::PixelMono<T>* ptr_0 = image[newrow]  +grad_first_col;
	VW::PixelMono<T>* ptr_p = image[newrow+1]+grad_first_col;
	for (unsigned int index=0; index<grad_width; 
	     index++, ptr_m++, ptr_0++, ptr_p++){
	  // Gradients at old pixel
	  const double gx = ptr_0[1].y - ptr_0[-1].y;
	  const double gy = ptr_p->y - ptr_m->y;
  
	  CSgxsq[index] += gx * gx;
	  CSgysq[index] += gy * gy;
	  CSgxgy[index] += gx * gy;      
	}    
      } 
    }
  }
  // Normalise the best score before returning:
  best_feature.Set(float(best_x), float(best_y), float(best_score/max_val));
}

namespace VW {

// Instantiate some versions
#ifdef INSTANTIATE
#  undef INSTANTIATE
#endif
# define INSTANTIATE(T) \
  template void ShiTomasiDetector::Compute<T >(const VW::ImageMono<T>& image, \
					       CornerFeature& best_feature); \
  template void ShiTomasiDetector::Compute<T >(const VW::ImageMono<T>& image, \
					       std::vector<CornerFeature>& features)

INSTANTIATE(unsigned char); 
INSTANTIATE(float); 

#undef INSTANTIATE


}; //end namespace VW
