/** 
 * @file    dogdetector.cpp
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
 */

// System includes

// VW includes
#include <VW/Improc/cornerfeature.h>
#include <VW/Improc/convolve.h>
#include <VW/Improc/pyramid.h>

// Local
#include "dogdetector.h"


#define VERBOSE false

using namespace VW;
using namespace std;

static inline void
MakeDiffPyramid(const VW::Pyramid<float>& image_pyramid,
	        VW::Pyramid<float>& diff_pyramid)
{
  const unsigned int num_levels = image_pyramid.NumLevels();
  if (diff_pyramid.NumLevels() != num_levels) {
    cerr << diff_pyramid.NumLevels() << "!=" << num_levels << endl;
  }
  assert(image_pyramid.NumLevels()==diff_pyramid.NumLevels());

  VW::ConvolutionMask1D<float> gaussian1(5), gaussian2(5);
  gaussian1.MakeGaussian(1.4142136); // sqrt(2)
  gaussian2.MakeGaussian(2); // sqrt(2)

  // For each level, do additional smoothing then take difference
  for (unsigned int i=0;i<image_pyramid.NumLevels();i++) {
    if (VERBOSE) cerr << "Do level " << i << endl;
    const VW::ImageMono<float>& im = image_pyramid[i];
    VW::ImageMono<float>& df1 = diff_pyramid[i];
    VW::ImageMono<float> df2(df1.GetWidth(), df1.GetHeight());
    Convolve<float>(im, gaussian1, gaussian1, df1);
    Convolve<float>(im, gaussian2, gaussian2, df2);


    const unsigned int col0 = df1.ROI().Left();
    const unsigned int col1 = df1.ROI().Right();
    const unsigned int row0 = df1.ROI().Top();
    const unsigned int row1 = df1.ROI().Bottom();

    for (unsigned int row=row0; row<row1; ++row) {
      VW::PixelMono<float>* df1_ptr = df1.GetRowStart(row) + col0;
      VW::PixelMono<float>* df2_ptr = df2.GetRowStart(row) + col0;
      for (unsigned int col=col0; col<col1; ++col, ++df1_ptr, ++df2_ptr) {
	//	dst_ptr->y = 0.5f*(src_ptr->y - dst_ptr->y) + 127.5f;
 	df1_ptr->y -= df2_ptr->y;
      }
    }

  }

}

static inline void
MakeDiffImage(const VW::ImageMono<unsigned char>& image,
	        VW::ImageMono<float>& diff_image)
{
  VW::ConvolutionMask1D<float> gaussian(5);
  gaussian.MakeGaussian(1.4142136); // sqrt(2)

  // do additional smoothing then take difference

  Convolve<float>(image, gaussian, gaussian, diff_image);
  const unsigned int col0 = diff_image.ROI().Left();
  const unsigned int col1 = diff_image.ROI().Right();
  const unsigned int row0 = diff_image.ROI().Top();
  const unsigned int row1 = diff_image.ROI().Bottom();

  for (unsigned int row=row0; row<row1; ++row) {
    VW::PixelMono<unsigned char>* src_ptr = image.GetRowStart(row) + col0;
    VW::PixelMono<float>* dst_ptr = diff_image.GetRowStart(row) + col0;
    for (unsigned int col=col0; col<col1; ++col,++src_ptr, ++dst_ptr) {
      dst_ptr->y = float(src_ptr->y) - dst_ptr->y;
    }
  }
}

static inline bool
LocaliseAndTestCurvature(float& x,float& y,float& v,  
			 float df_mm, float df_m0, float df_mp, 
			 float df_0m, float df_00, float df_0p, 
			 float df_pm, float df_p0, float df_pp,
			 const float max_curvature_ratio)
{
  // Fit a 3x3 bi-quadratic patch

  // Gradients are calculated using the 1st and second order Sobel filters
  float A,B,C,D,E;

  A =    (     df_mm  - 2*df_m0  +   df_mp
	   + 2*df_0m  - 4*df_00  + 2*df_0p
	   +   df_0m  - 2*df_p0  +   df_pp  ) / 8.0f; // d^2I / dx^2

  B =    (  ( df_mm  + 2*df_m0  + df_mp  )
	- 2*( df_0m  + 2*df_00  + df_0p  )
	+   ( df_0m  + 2*df_p0  + df_pp  )) / 8.0f; // d^2I / dy^2

  C = (  + df_mm  - df_mp 
	 - df_0m  + df_pp  ) / 4.0f;                // d^2I / dxdy

  // Calculate the trace and determinant of the hessian to get curvature.
  // Nature of stationary point comes from eigenvalues:
  //   - both zero = plane
  //   - one zero = edge
  //   - both positive = minima
  //   - both negative = maxima
  //   - one positive one negative = saddle
  // so can test trace^2/det (approx ratio of eigenvalues) to make sure
  // point is a good one
  const float trace = A + B;
  const float det   = A*B - 0.25f*C*C;

  // Min curvature should be at least 1
    if ( det < 0.0f ) {
     return false;
   }
  if ( (trace*trace / det - 2.0f) > max_curvature_ratio) {
    return false;
  }
 

  D = ( - df_mm  - 2*df_0m  - df_0m
	+ df_mp  + 2*df_0p  + df_pp  ) / 8.0f;      // dI / dx

  E = ( - df_mm  - 2*df_m0  - df_mp 
	+ df_0m  + 2*df_p0  + df_pp  ) / 8.0f;      // dI / dy



  // Calculate peak position and value
  const float detinv = 1.0f/det;
  float dx = -1* (B*D - 0.5f*C*E) * detinv; // X-Offset of quadratic peak
  float dy = -1* (A*E - 0.5f*C*D) * detinv; // Y-Offset of quadratic peak
  float dv = 0.5f*(A*dx*dx + B*dy*dy) + C*dx*dy // Quadratic part
                           + D*dx     + E*dy;   // Linear part
 
  //  cerr << "dx,dy,dv=" << dx << "," << dy << "," << dv << endl;

  // Check if it should be maxima or minima
  if (df_00>df_mm) {
    // Maxima
    if (dv<0) {
      cerr << "Maxima is below central value!" << endl;
      cerr << "   " << df_mm << " " << df_m0 << " " << df_mp << endl;
      cerr << "   " << df_0m << " " << df_00 << " " << df_0p << endl;
      cerr << "   " << df_pm << " " << df_p0 << " " << df_pp << endl;
      cerr << "   Quadfit: (A,B,C,D,E,F)=("
	   <<A<<","<<B<<","
	   <<C<<","<<D<<","
	   <<E<<","<<df_00<<")"<<endl;
      cerr << "   det=" << det << endl;
      cerr << "   dv=" << dv << endl;

    }
  } else {
    // Minima
    if (dv>0) {
       cerr << "Minima is above central value!" << endl;
      cerr << "   " << df_mm << " " << df_m0 << " " << df_mp << endl;
      cerr << "   " << df_0m << " " << df_00 << " " << df_0p << endl;
      cerr << "   " << df_pm << " " << df_p0 << " " << df_pp << endl;
     cerr << "   Quadfit: (A,B,C,D,E,F)=("
	   <<A<<","<<B<<","
	   <<C<<","<<D<<","
	   <<E<<","<<df_00<<")"<<endl;
       cerr << "   det=" << det << endl;
     cerr << "   dv=" << dv << endl;

   }
  }


  // When the feature has weak gradient in one direction, bad shit
  // happens, but the most this fit should move the peak is half
  // way to a neighbouring pixel (otherwise it would be higher).
  if (dx>=0.5) dx=0.49999f;
  if (dx<-0.5) dx=-0.5f;
  if (dy>=0.5) dy=0.49999f;
  if (dy<-0.5) dy=-0.5f;
  x += dx;
  y += dy;
  v += dv;

  return true;
}

static inline bool
CheckExtremal(bool is_minima, int row, int col, float val, 
	     const VW::ImageMono<float>& diff_image)
{
  if (is_minima) {
  VW::PixelMono<float>* rowm = diff_image.GetRowStart(row-1)+ col - 1;
  if (rowm++->y<val) return false;
  if (rowm++->y<val) return false;
  if (rowm->y<val) return false;
  
  VW::PixelMono<float>* row0 = diff_image.GetRowStart(row)  + col - 1;
  if (row0++->y<val) return false;
  if (row0++->y<val) return false;
  if (row0->y<val) return false;
  
  VW::PixelMono<float>* rowp = diff_image.GetRowStart(row+1) + col - 1;
  if (rowp++->y<val) return false;
  if (rowp++->y<val) return false;
  if (rowp->y<val) return false;
  } else {
  VW::PixelMono<float>* rowm = diff_image.GetRowStart(row-1)+ col - 1;
  if (rowm++->y>val) return false;
  if (rowm++->y>val) return false;
  if (rowm->y>val) return false;
  
  VW::PixelMono<float>* row0 = diff_image.GetRowStart(row)  + col - 1;
  if (row0++->y>val) return false;
  if (row0++->y>val) return false;
  if (row0->y>val) return false;
  
  VW::PixelMono<float>* rowp = diff_image.GetRowStart(row+1) + col - 1;
  if (rowp++->y>val) return false;
  if (rowp++->y>val) return false;
  if (rowp->y>val) return false;
  }

  return true;
}

static inline void
Print3x3(int row, int col, 
	 const VW::ImageMono<float>& diff_image)
{
  VW::PixelMono<float>* rowm = diff_image.GetRowStart(row-1)+ col - 1;
  cerr << int(rowm++->y) << " ";
  cerr << int(rowm++->y) << " ";
  cerr << int(rowm->y) << "\n";
  
  VW::PixelMono<float>* row0 = diff_image.GetRowStart(row)  + col - 1;
  cerr << int(row0++->y) << " ";
  cerr << int(row0++->y) << " ";
  cerr << int(row0->y) << "\n";
  
  VW::PixelMono<float>* rowp = diff_image.GetRowStart(row+1) + col - 1;
  cerr << int(rowp++->y) << " ";
  cerr << int(rowp++->y) << " ";
  cerr << int(rowp->y) << "\n";

}


static inline float InsertFeature(std::vector<VW::CornerFeature>& features,
											 const VW::CornerFeature& feature,
											 unsigned int limit)
{
  if (limit==0) {
	 features.push_back(feature);
	 return 0.0f;
  }

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

// Find all extremal points - that is points which are maximum both in their
// own level and when compared to the same locations in the adjacent levels
static inline void
FindExtrema(const VW::Pyramid<float>& diff_pyramid,
				std::vector<CornerFeature>& features,
				const float max_curvature_ratio,
				const unsigned int max_number)
{
  // First find extrema in each level
  const unsigned int num_levels = diff_pyramid.GetNumLevels();
  std::vector<std::vector<CornerFeature> > level_features;
  std::vector<std::vector<bool> > is_minima;
  level_features.resize(num_levels);
  is_minima.resize(num_levels);

  unsigned int total_num_features=0;
  for (unsigned int level=0; level<num_levels; level++) {
    const float this_scale = 1.0f/diff_pyramid.GetScale(level);
    const VW::ImageMono<float>& dog = diff_pyramid[level];
    const unsigned int col0 = dog.ROI().Left()+1;
    const unsigned int col1 = dog.ROI().Right()-1;
    const unsigned int row0 = dog.ROI().Top()+1;
    const unsigned int row1 = dog.ROI().Bottom()-1;
    for (unsigned int row = row0; row<row1; row++) {
      // Pointers to this row, one above and one below
      VW::PixelMono<float>* ptrm = dog.GetRowStart(row-1)+ col0;
      VW::PixelMono<float>* ptr0 = dog.GetRowStart(row)  + col0;
      VW::PixelMono<float>* ptrp = dog.GetRowStart(row+1)+ col0;
      for (unsigned int col=col0; col<col1; col++, ptrm++, ptr0++, ptrp++) {
	float v = ptr0[0].y;
	// Check is maxima or minima compared to 8 surrounding vals
	if (   (v>ptrm[-1].y) && (v>ptrm[0].y) && (v>ptrm[1].y)
	    && (v>ptr0[-1].y)                  && (v>ptr0[1].y)
	    && (v>ptrp[-1].y) && (v>ptrp[0].y) && (v>ptrp[1].y)) {
	  float x=col, y=row;
	  if (LocaliseAndTestCurvature(x,y,v,  
				       ptrm[-1].y, ptrm[0].y, ptrm[1].y,
				       ptr0[-1].y, ptr0[0].y, ptr0[1].y,
				       ptrp[-1].y, ptrp[0].y, ptrp[1].y,
				       max_curvature_ratio)) {
	    VW::CornerFeature new_corner(x*this_scale, y*this_scale, 
					 v, this_scale);
	    level_features[level].push_back(new_corner);
	    is_minima[level].push_back(false);
	    total_num_features++;
	  }
	} else if (   (v<ptrm[-1].y) && (v<ptrm[0].y) && (v<ptrm[1].y)
		   && (v<ptr0[-1].y)                  && (v<ptr0[1].y)
		   && (v<ptrp[-1].y) && (v<ptrp[0].y) && (v<ptrp[1].y)) {
	  float x=col, y=row;
	  if (LocaliseAndTestCurvature(x,y,v,  
				       ptrm[-1].y, ptrm[0].y, ptrm[1].y,
				       ptr0[-1].y, ptr0[0].y, ptr0[1].y,
				       ptrp[-1].y, ptrp[0].y, ptrp[1].y,
				       max_curvature_ratio)) {
	    VW::CornerFeature new_corner(x*this_scale, y*this_scale, 
					 v, this_scale);
	    level_features[level].push_back(new_corner);
	    is_minima[level].push_back(true);
	    total_num_features++;
	  }
	}
      }
    }
    if (VERBOSE) 
cerr << "Level " << level << " (scale " << this_scale << ") has " << level_features[level].size() << " features before culling." << endl;
  }


  // min_score stores the worst feature score
  // that is on the list (0 for no limit on list size)
  float min_score =0.0f;
  // Now scan each level removing non-maximal features
  // first level 0 (only compare to level above)
  {
    const unsigned int level = 0;
   const float scale_up = diff_pyramid.GetScale(level+1);
   vector<CornerFeature>::const_iterator i = level_features[level].begin();
    vector<CornerFeature>::const_iterator ie = level_features[level].end();
   vector<bool>::const_iterator m = is_minima[level].begin();
   unsigned int num_added = 0;
    for ( ; i!=ie ; i++, m++ ) {
		// Only bother checking if it's good enough anyway
		if (i->GetCornerStrength() > min_score) {
		  const int row = int(i->GetY()*scale_up+0.5f);
		  const int col = int(i->GetX()*scale_up+0.5f);
		  if (CheckExtremal(*m, row,col, 
								  i->GetCornerStrength(), 
								  diff_pyramid[level+1])) {
			 min_score = InsertFeature(features,*i,max_number);
			 num_added++;
		  }
      } // End if (>min_score)
    }
    if (VERBOSE) 
      cerr << "Level " << level << " has " << num_added << " features after culling." << endl;
  }
  // now middle levels  
  for (unsigned level=1; level<(num_levels-1); level++) {
   const float scale_up = diff_pyramid.GetScale(level+1);
   const float scale_dn = diff_pyramid.GetScale(level-1);
    vector<CornerFeature>::const_iterator i = level_features[level].begin();
    vector<CornerFeature>::const_iterator ie = level_features[level].end();
   vector<bool>::const_iterator m = is_minima[level].begin();
   unsigned int num_added = 0;
    for ( ; i!=ie ; i++, m++ ) {
		// Only bother checking if it's good enough anyway
		if (i->GetCornerStrength() > min_score) {
		  const int row_up = int(i->GetY()*scale_up+0.5f);
		  const int col_up = int(i->GetX()*scale_up+0.5f);
		  const int row_dn = int(i->GetY()*scale_dn+0.5f);
		  const int col_dn = int(i->GetX()*scale_dn+0.5f);
		  if (CheckExtremal(*m, row_dn,col_dn, 
								  i->GetCornerStrength(), 
								  diff_pyramid[level-1])) {
			 if (CheckExtremal(*m, row_up,col_up, 
									 i->GetCornerStrength(), 
									 diff_pyramid[level+1])) {
				min_score = InsertFeature(features,*i,max_number);
				num_added++;
			 }
		  }
		} // End if (>min_score)
    }
    if (VERBOSE) 
      cerr << "Level " << level << " has " << num_added << " features after culling." << endl;
  }
  // now last level (only compare to level below)
  {
    const unsigned int level = diff_pyramid.GetNumLevels()-1;
    const float scale_dn = diff_pyramid.GetScale(level-1);
    //    const float scale = diff_pyramid.GetScale(level);
    vector<CornerFeature>::const_iterator i = level_features[level].begin();
    vector<CornerFeature>::const_iterator ie = level_features[level].end();
    vector<bool>::const_iterator m = is_minima[level].begin();
    unsigned int num_added = 0;
    for ( ; i!=ie ; i++, m++ ) {
 		// Only bother checking if it's good enough anyway
		if (i->GetCornerStrength() > min_score) {
		  const int row = int(i->GetY()*scale_dn+0.5f);
		  const int col = int(i->GetX()*scale_dn+0.5f);
		  if (CheckExtremal(*m, row,col, 
								  i->GetCornerStrength(), 
								  diff_pyramid[level-1])) {
			 min_score = InsertFeature(features,*i,max_number);
			 num_added++;
		  }
		} // End if (>min_score)
    }
    if (VERBOSE) 
      cerr << "Level " << level << " has " << num_added << " features after culling." << endl;
  }
}

// Find the single strongest extremal point - this is much easier than finding
// the maximal ones, since the best is automatically maximal!
static inline bool
FindExtrema(const VW::Pyramid<float>& diff_pyramid,
	    CornerFeature& best_feature)
{


  // First find extrema in each level
  const unsigned int num_levels = diff_pyramid.GetNumLevels();
  bool got_one = false;

  for (unsigned int level=0; level<num_levels; level++) {
    const float this_scale = 1.0f/diff_pyramid.GetScale(level);
    const VW::ImageMono<float>& dog = diff_pyramid[level];
    const unsigned int col0 = dog.ROI().Left()+1;
    const unsigned int col1 = dog.ROI().Right()-1;
    const unsigned int row0 = dog.ROI().Top()+1;
    const unsigned int row1 = dog.ROI().Bottom()-1;
    for (unsigned int row = row0; row<row1; row++) {
      // Pointers to this row, one above and one below
      VW::PixelMono<float>* ptrm = dog.GetRowStart(row-1)+ col0;
      VW::PixelMono<float>* ptr0 = dog.GetRowStart(row)  + col0;
      VW::PixelMono<float>* ptrp = dog.GetRowStart(row+1)+ col0;
      for (unsigned int col=col0; col<col1; col++, ptrm++, ptr0++, ptrp++) {
	float v = ptr0[0].y;
	// Check is best (can be +ve (maxima) or -ve (minima))
	if (v>best_feature.GetCornerStrength()) {
	  best_feature.Set(col*this_scale, row*this_scale, 
			   v, this_scale);
	  got_one = true;
	} else if ((-v)>best_feature.GetCornerStrength()) {
	  best_feature.Set(col*this_scale, row*this_scale, 
			   -v, this_scale);
	  got_one = true;
	}
      }
    }
  }
  return got_one;
}


// Find the single strongest extremal point - this is much easier than finding
// the maximal ones, since the best is automatically maximal!
static inline bool
FindExtrema(const VW::ImageMono<float>& diff_image,
	    CornerFeature& best_feature)
{
  // First find extrema in each level
  bool got_one = false;

  const unsigned int col0 = diff_image.ROI().Left()+1;
  const unsigned int col1 = diff_image.ROI().Right()-1;
  const unsigned int row0 = diff_image.ROI().Top()+1;
  const unsigned int row1 = diff_image.ROI().Bottom()-1;
  for (unsigned int row = row0; row<row1; row++) {
    // Pointers to this row, one above and one below
   VW::PixelMono<float>* ptr = diff_image.GetRowStart(row)  + col0;
    for (unsigned int col=col0; col<col1; col++, ptr++) {
      float v = ptr->y;
      // Check is best (can be +ve (maxima) or -ve (minima))
      if (v>best_feature.GetCornerStrength()) {
	best_feature.Set(col, row, v, 1.0);
	got_one = true;
      } else if ((-v)>best_feature.GetCornerStrength()) {
	best_feature.Set(col, row, -v, 1.0);
	got_one = true;
      }
    }
  }
  return got_one;
}



void VW::DOGDetector::Compute(const VW::Pyramid<float>& pyramid,
			      std::vector<CornerFeature>& features)
{
  features.clear();

  if (VERBOSE) cerr << "Create diff pyramid" << endl;
  VW::Pyramid<float> diff_pyramid(pyramid.GetWidth(),pyramid.GetHeight(),
				  pyramid.GetMinSize(),
				  pyramid.GetNumLevelsPerOctave());

  if (VERBOSE) cerr << "   pyramid/diff minsize=" << pyramid.GetMinSize() << "/" << diff_pyramid.GetMinSize() << endl;

  if (VERBOSE) cerr << "Fill diff pyramid" << endl;
  MakeDiffPyramid(pyramid,diff_pyramid);

//   cerr << "Save pyramid" << endl;
//   for (unsigned int i=0;i<pyramid.NumLevels();i++) {
//     ostringstream filename;
//     filename << "pyr_" << i << ".ppm";
//     pyramid[i].WriteImage(filename.str());
//   }

//   cerr << "Save diff pyramid" << endl;
//   for (unsigned int i=0;i<diff_pyramid.NumLevels();i++) {
//     ostringstream filename;
//     filename << "diff_" << i << ".ppm";
//     diff_pyramid[i].WriteImage(filename.str());
//   }

  if (VERBOSE) cerr << "Hunt for features" << endl;
  FindExtrema(diff_pyramid, 
				  features, 
				  _params.max_curvature_ratio,
				  _params.max_features);


}
void VW::DOGDetector::Compute(const VW::Pyramid<float>& pyramid,
			      CornerFeature& best_feature)
{
  if (VERBOSE) cerr << "Create diff pyramid" << endl;
  VW::Pyramid<float> diff_pyramid(pyramid.GetWidth(),pyramid.GetHeight(),
				  pyramid.GetMinSize(),
				  pyramid.GetNumLevelsPerOctave());

  if (VERBOSE) cerr << "   pyramid/diff minsize=" << pyramid.GetMinSize() << "/" << diff_pyramid.GetMinSize() << endl;

  if (VERBOSE) cerr << "Fill diff pyramid" << endl;
  MakeDiffPyramid(pyramid,diff_pyramid);

  if (VERBOSE) cerr << "Hunt for features" << endl;
  FindExtrema(diff_pyramid, best_feature);


}
void VW::DOGDetector::Compute(const VW::ImageMono<unsigned char>& image,
			      CornerFeature& best_feature)
{
  if (VERBOSE) cerr << "Create diff image" << endl;
  VW::ImageMono<float> diff_image(image.GetWidth(),image.GetHeight());

  if (VERBOSE) cerr << "Fill diff image" << endl;
  MakeDiffImage(image,diff_image);

  if (VERBOSE) cerr << "Hunt for features" << endl;
  FindExtrema(diff_image, best_feature);


}
