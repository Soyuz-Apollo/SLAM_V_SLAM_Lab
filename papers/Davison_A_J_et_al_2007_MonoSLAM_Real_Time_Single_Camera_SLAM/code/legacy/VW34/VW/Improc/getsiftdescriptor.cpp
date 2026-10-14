#ifdef _WIN32
#  define _USE_MATH_DEFINES
#endif
#include <math.h> // For M_PI

#include <VW/Improc/pyramid.h>
#include <VW/Improc/siftfeature.h>
#include <VW/Utils/timer.h>

#include "getsiftdescriptor.h"

#define VERBOSE false
// Defines come from siftfeature.h
#define SAMPLE_ARRAY_SIZE SIFT_CELL_SIZE*SIFT_NUM_CELLS


#define TWOPI 6.2831853071795
#define PIBYTWO 1.5707963267948

using namespace std;

//////////// GaussianMask


// Simple classes to hold weighting arrays
template <unsigned int ARRAYSIZE>
class _SIFT_GaussianMask {
public:
  _SIFT_GaussianMask(double sigma=1.0);
  void Print() const;
  const double* operator[](int row) const;
public:
  double weight[ARRAYSIZE][ARRAYSIZE];

};


template <unsigned int S>  
inline _SIFT_GaussianMask<S>::_SIFT_GaussianMask(double sigma) 
{
  // Calculate the orientation calculation weighting mask
  const int half_array_size = S/2;
  
  const double inv_cov_scale = -0.5 / (sigma * sigma);
  for (int r=0; r<int(S); ++r) {
    const double y = double(r - half_array_size) + 0.5;
    for (int c=0; c<int(S); ++c) {
      const double x = double(c - half_array_size) + 0.5;
      const double e = (x*x + y*y) * inv_cov_scale;
      weight[r][c] = exp(e); 
    }
  }
};

template <unsigned int S> inline void 
_SIFT_GaussianMask<S>::Print() const
{
  for (unsigned int r=0;r<SAMPLE_ARRAY_SIZE;r++) {
    for (unsigned int c=0;c<SAMPLE_ARRAY_SIZE;c++) {
      cerr << weight[r][c] << " ";
    }
    cerr << endl;
  }
}

template <unsigned int S> inline const double* 
_SIFT_GaussianMask<S>::operator[](int row) const
{
  return weight[row];
}




//////////// DistanceMask


template <unsigned int ARRAYSIZE>
class _SIFT_DistanceMask {
public:
  _SIFT_DistanceMask();
  void Print() const;

public:
  double weight_1D[ARRAYSIZE];
  double weight_2D[ARRAYSIZE][ARRAYSIZE];
};



template <unsigned int S>  
inline _SIFT_DistanceMask<S>::_SIFT_DistanceMask() 
{
  // Calculate the orientation calculation weighting mask
  const int half_array_size = S/2;
  const double distance_threshold = double(S)/2.0;
  double tmp_1D[S], tmp;
  for (int i=0; i<int(S); ++i) {
    tmp = double(i - half_array_size) + 0.5;
    weight_1D[i] = tmp;
    tmp_1D[i] = tmp * tmp;
  }
  for (unsigned int r=0; r<int(S); ++r) {
	 for (unsigned int c=r; c<int(S); ++c) {
      weight_2D[r][c] = sqrt(tmp_1D[r] + tmp_1D[c]);
      if (weight_2D[r][c]>distance_threshold)  weight_2D[r][c] = -1.0;
    } // End for c
  } // End for r
  for (unsigned int r=0; r<int(S-1); ++r) {
    for (unsigned int c=r+1; c<int(S); ++c) {
      weight_2D[c][r] = weight_2D[r][c];
    } // End for c
  } // End for r
};

template <unsigned int S> inline void 
_SIFT_DistanceMask<S>::Print() const
{
  for (unsigned int r=0;r<SAMPLE_ARRAY_SIZE;r++) {
    for (unsigned int c=0;c<SAMPLE_ARRAY_SIZE;c++) {
      cerr << weight_2D[r][c] << " ";
    }
    cerr << endl;
  }
}



//////////// GetSIFTDescriptor


/** fill the gradient arrays but don't bother with orientations */
static inline int 
SIFT_GetGradientsOnly(int col0, int row0,
		      int mcol0,int mrow0,int mcol1,int mrow1,
		      const VW::ImageMono<float>& im, 
		      const _SIFT_GaussianMask<SAMPLE_ARRAY_SIZE>& weights,
		      double gradient_orientations[SAMPLE_ARRAY_SIZE][SAMPLE_ARRAY_SIZE], 
		      double gradient_magnitudes[SAMPLE_ARRAY_SIZE][SAMPLE_ARRAY_SIZE])
{
  // Calculate gradient magnitudes and orientations at each pixel,
  int irow = row0;
  for (int mrow = mrow0; mrow<mrow1; ++mrow, ++irow) {
    int icol = col0;
    for (int mcol = mcol0; mcol<mcol1; ++mcol, ++icol) {
      const double grad_y = double(im[irow+1][icol].y) 
	- double(im[irow-1][icol].y);
      const double grad_x = double(im[irow][icol+1].y) 
	- double(im[irow][icol-1].y);
      gradient_orientations[mrow][mcol] = M_PI + atan2(grad_y,grad_x);
      gradient_magnitudes[mrow][mcol] = sqrt(grad_y*grad_y + grad_x*grad_x);
    }
  }
//   cerr << "Orientation array:" << endl;
//   for (int mrow = mrow0; mrow<mrow1; ++mrow) {
//     for (int mcol = mcol0; mcol<mcol1; ++mcol) {
//       cerr << gradient_orientations[mrow][mcol] << " ";
//     }
//     cerr << endl;
//   }
  return 0;
}



/** fill the gradient arrays and also the dominant orientations */
static inline int 
SIFT_GetGradientsAndOrientation(int col0, int row0,
				int mcol0,int mrow0,int mcol1,int mrow1,
				const VW::ImageMono<float>& im, 
		    const _SIFT_GaussianMask<SAMPLE_ARRAY_SIZE>& weights,
		    std::vector<double>& orientations,
		    double gradient_orientations[SAMPLE_ARRAY_SIZE][SAMPLE_ARRAY_SIZE], 
		    double gradient_magnitudes[SAMPLE_ARRAY_SIZE][SAMPLE_ARRAY_SIZE])
{
  vector<double> bin_score(36, 0.0);
  //  vector<int> num_in_bin(36, 0);
  
  // Calculate gradient magnitudes and orientations at each pixel,
  // and create the (smoothed) orientation histogram
  int irow = row0;
  for (int mrow = mrow0; mrow<mrow1; ++mrow, ++irow) {
    int icol = col0;
    for (int mcol = mcol0; mcol<mcol1; ++mcol, ++icol) {
      const double grad_y = double(im[irow+1][icol].y) 
	- double(im[irow-1][icol].y);
      const double grad_x = double(im[irow][icol+1].y) 
	- double(im[irow][icol-1].y);
      gradient_orientations[mrow][mcol] = M_PI + atan2(grad_y,grad_x);
      gradient_magnitudes[mrow][mcol] = sqrt(grad_y*grad_y + grad_x*grad_x);
      const double contrib = weights[mrow][mcol] * gradient_magnitudes[mrow][mcol];
      const int bindex = int(1e-8+36.0*gradient_orientations[mrow][mcol]/TWOPI)%36;
      const int bindex_dn = (bindex+35)%36;
      const int bindex_up = (bindex+1)%36;
      bin_score[bindex_dn]    += 0.2*contrib;
      bin_score[bindex]       += 0.6*contrib;
      bin_score[bindex_up]    += 0.2*contrib;
      //      num_in_bin[bindex]      ++;
    }
  }
  
  
//   cout << "Bins " << endl;
//   for (int ji=0; ji<36; ++ji) {
//     cout << bin_score[ji] << " ";
//     if (ji%9==8) cout << endl;
//   }
//   cout << endl;
  
//   double bcb = 0;
//   cout << "Bin count " << endl;
//   for (int ji=0; ji<36; ++ji) {
//     bcb += num_in_bin[ji];
//     cout << num_in_bin[ji] << " ";
//     if (ji%9==8) cout << endl;
//   }
//   cout << "Tot " << bcb << endl;
//   cout << endl;

  if (0) { // NDM method
    // Find Maxima in the orientation histogram
    if (VERBOSE) cerr << "Find maxima (NDM)" << endl;
    const int max_multiples = 6;
    int nmax = 0;
    vector<double> cbins = bin_score;
    vector<double> max_v(max_multiples, 0.0);
    vector<int>    max_b(max_multiples, 0);
    for (int m=0; m<max_multiples; ++m) {
      for (int mm=0; mm<m; ++mm) { 
	// Remove previous maximums and their immediate neighbours
	cbins[(max_b[mm]+35)%36] = 0.0;
	cbins[ max_b[mm]       ] = 0.0;
	cbins[(max_b[mm]+1)%36 ] = 0.0;
      }
      for (int b=0; b<36; ++b) {
	if (cbins[b]>max_v[m]) {
	  max_v[m] = cbins[b];
	  max_b[m] = b;
	}
      }
      if ( (max_v[m]/max_v[0]) < 0.8) break;  // Constant here
      ++nmax;
    }
    //cerr << "Got " << nmax << " maxima"<< endl;
      
    // Quick quadratic fit
    orientations.clear();
    for (int m=0;m<nmax;m++) {
      //cerr << "   angle=" << max_b[m]*10 << " value=" <<max_v[m]<<endl;
      const double val_p = bin_score[(max_b[m]+1)%36];
      const double val_0 = bin_score[max_b[m]];
      const double val_m = bin_score[(max_b[m]+35)%36];
      const double grad_x = (val_p-val_m); // Actually half this, but it cancels!
      const double grad2_x = (val_p-2.0*val_0+val_m); // Actually half this
      const double delta_x =  - 0.5*grad_x/grad2_x;
      const double bin_to_radians = TWOPI/36.0;
      orientations.push_back(bin_to_radians*(max_b[m]+0.5+delta_x)) ;
    }
      
  } else {
    // BJT method (approx 2x faster)
    if (VERBOSE) cerr << "Find maxima (BJT)" << endl;
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
    //    cerr << "Got " << max_v.size() << " maxima (acceptance threshold="<<acceptance_threshold<<"):" << endl;
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
  } // End BJT maxima finder

//   cerr << "Orientation array:" << endl;
//   for (int mrow = mrow0; mrow<mrow1; ++mrow) {
//     for (int mcol = mcol0; mcol<mcol1; ++mcol) {
//       cerr << gradient_orientations[mrow][mcol] << " ";
//     }
//     cerr << endl;
//   }
//   cerr << "Which produces " << orientations.size() << " maxima." << endl;


  return orientations.size();
}








static inline void 
GenerateDescriptor(VW::SIFTFeature& feature,
		   int mcol0,int mrow0,int mcol1,int mrow1,
		   const double gradient_orientations[SAMPLE_ARRAY_SIZE][SAMPLE_ARRAY_SIZE], 
		   const double gradient_magnitudes[SAMPLE_ARRAY_SIZE][SAMPLE_ARRAY_SIZE],
		   const _SIFT_DistanceMask<SAMPLE_ARRAY_SIZE>& dist_weight,
		   const _SIFT_GaussianMask<SAMPLE_ARRAY_SIZE>& desc_weight)
{
  // the angle which must be added to orientation to make it point upwards
  const double global_rot_correction = PIBYTWO - feature.GetOrientation(); 
  
  const double bin_width_inv  = double(SIFT_NUM_ORIENTATIONS) / TWOPI;
  const double d_half_array_size = double(SAMPLE_ARRAY_SIZE)/2.0;

  double bin_correction = global_rot_correction + M_PI/double(SIFT_NUM_ORIENTATIONS) - PIBYTWO;
       
  // Show the distance and orientation arrays
//   cerr << "Distance weight:" << endl;
//   for (unsigned int i=0;i<16;i++) {
//     for (unsigned int j=0;j<16;j++) {
//       cerr << dist_weight.weight_2D[i][j] << " ";
//     }
//     cerr << endl;
//   }
//   cerr << "Orientations:" << endl;
//   for (unsigned int i=0;i<16;i++) {
//     for (unsigned int j=0;j<16;j++) {
//       cerr << gradient_orientations[i][j] << " ";
//     }
//     cerr << endl;
//   }


  // This effectively places the feature at the nearest intersection 
  // of 4 pixels, rather than being sub-pixel
  // Otherwise the number of pixels being binned would be variable - so 
  // keep simple for now
  const int mwidth = mcol1-mcol0;
  const int mheight = mrow1-mrow0;
  if ( (mheight>=SIFT_CELL_SIZE*SIFT_NUM_CELLS) &&  
       (mwidth>=SIFT_CELL_SIZE*SIFT_NUM_CELLS) ) { 
    // Mask fits fully onto image
    for (int mrow = mrow0; mrow<mrow1; ++mrow) {
      for (int mcol = mcol0; mcol<mcol1; ++mcol) {
// 	cerr << "Pos=["<<mcol<<","<<mrow<<"]"<<endl;
	// Check distance is in range
	if (dist_weight.weight_2D[mrow][mcol]>0.0) {

	  // Calculate the local intensity orientation
	  double orientation = gradient_orientations[mrow][mcol] + bin_correction;
	  if (orientation<0.0) orientation += TWOPI;
	  const double dor = orientation * bin_width_inv; // Orientation in orientation histogram space [0-SIFT_NUM_ORIENTATIONS]
	  const int ior = int(dor+1e-8);
	  const int hist_o_low    = ior%SIFT_NUM_ORIENTATIONS;
	  const int hist_o_high   = (1 + hist_o_low)%SIFT_NUM_ORIENTATIONS;
	  const double o_low_dist    = dor - double(ior);
	  const double o_low_weight  = 1.0 - o_low_dist;
	  const double o_high_weight = o_low_dist;
// 	  cerr << "params:" << endl;
// 	  cerr << "   " << orientation << endl;
// 	  cerr << "   " << dor << ",";
// 	  cerr << "   " << ior << endl;
// 	  cerr << "   " << hist_o_low << ",";
// 	  cerr << "   " << hist_o_high << endl;
// 	  cerr << "   " << o_low_dist << "," ;
// 	  cerr << "   " << o_low_weight << ",";
// 	  cerr << "   " << o_high_weight << endl;

	  // Angle of sample position clockwise from a vector pointing right with orientation correction ('global_rot_correction')
	  const double sample_angle = global_rot_correction + atan2(dist_weight.weight_1D[mrow], dist_weight.weight_1D[mcol]);
	       
	  // Position of the point in sample histogram space [0-DESCRIPTOR_POSITIONAL_SIZE]
	  const double dmr = (d_half_array_size + dist_weight.weight_2D[mrow][mcol] * sin(sample_angle)) / double(SIFT_CELL_SIZE) + 0.5;
	  const double dmc = (d_half_array_size + dist_weight.weight_2D[mrow][mcol] * cos(sample_angle)) / double(SIFT_CELL_SIZE) + 0.5;
	       
// 	  cerr << "   " << sample_angle << ",";
// 	  cerr << "   " << dmr << ",";
// 	  cerr << "   " << dmc << ",";

	  // Work out which histograms to add to and the weight for each one - max of four (per orientation value)
	  const int hist_r_high   = int(dmr);       // Upper histogram row index
	  const int hist_r_low    = hist_r_high - 1; // Lower histogram row index
	  const double r_low_dist    = dmr - int(dmr);
	  const double r_low_weight  = 1.0 - r_low_dist;
	  const double r_high_weight = r_low_dist;
	       
// 	  cerr << "   " << hist_r_high << ",";
// 	  cerr << "   " << hist_r_low << ",";
// 	  cerr << "   " << r_low_dist << ",";
// 	  cerr << "   " << r_low_weight << ",";
// 	  cerr << "   " << r_high_weight << endl;

	  const int hist_c_high   = int(dmc);       // Upper histogram column index
	  const int hist_c_low    = hist_c_high - 1; // Lower histogram column index
	  const double c_low_dist    = dmc - (int)dmc;
	  const double c_low_weight  = 1.0 - c_low_dist;
	  const double c_high_weight = c_low_dist;
	       
// 	  cerr << "   " << hist_c_high << ",";
// 	  cerr << "   " << hist_c_low << ",";
// 	  cerr << "   " << c_low_dist << ",";
// 	  cerr << "   " << c_low_weight << ",";
// 	  cerr << "   " << c_high_weight << endl;

	  const double this_mag = gradient_magnitudes[mrow][mcol];
	  const double this_weight = this_mag*desc_weight[mrow][mcol];

// 	  cerr << "   " << this_mag << ",";
// 	  cerr << "   " << this_weight << endl;

	  if ( (hist_r_low>-1) && (hist_c_low>-1) ) {
	    const int h_position = SIFT_NUM_ORIENTATIONS * (SIFT_NUM_CELLS*hist_r_low + hist_c_low);
	    const double h_weight   = this_weight*r_low_weight*c_low_weight;
	    feature._descriptor[hist_o_low  + h_position] += o_low_weight  * h_weight;
	    feature._descriptor[hist_o_high + h_position] += o_high_weight * h_weight;
	  }
	  if ( (hist_c_low>-1) 
	       && (hist_r_high<SIFT_NUM_CELLS) ) {
	    const int h_position = SIFT_NUM_ORIENTATIONS * (SIFT_NUM_CELLS*hist_r_high + hist_c_low);
	    const double h_weight   = this_weight*r_high_weight*c_low_weight;
	    feature._descriptor[hist_o_low  + h_position] += o_low_weight  * h_weight;
	    feature._descriptor[hist_o_high + h_position] += o_high_weight * h_weight;
	  }
	  if ( (hist_r_low>-1) 
	       && (hist_c_high<SIFT_NUM_CELLS) ) {
	    const int h_position = SIFT_NUM_ORIENTATIONS * (SIFT_NUM_CELLS*hist_r_low + hist_c_high);
	    const double h_weight   = this_weight*r_low_weight*c_high_weight;
	    feature._descriptor[hist_o_low  + h_position] += o_low_weight  * h_weight;
	    feature._descriptor[hist_o_high + h_position] += o_high_weight * h_weight;
	  }
	  if ( (hist_r_high<SIFT_NUM_CELLS) && (hist_c_high<SIFT_NUM_CELLS) ) {
	    const int h_position = SIFT_NUM_ORIENTATIONS * (SIFT_NUM_CELLS*hist_r_high + hist_c_high);
	    const double h_weight   = this_weight*r_high_weight*c_high_weight;
	    feature._descriptor[hist_o_low  + h_position] += o_low_weight  * h_weight;
	    feature._descriptor[hist_o_high + h_position] += o_high_weight * h_weight;
	  }   

	} // End if distance in range
      } // End for (col)
    } // End for (row)
  } // End if fits in image

  // Normalise feature vector to unit length
  feature.NormaliseDescriptor();
  if (VERBOSE) {
    cerr << "Generated descriptor: " << endl;
    double* d1_ptr = feature._descriptor;
    double t=0;
    std::cerr << "Descriptor: " << std::endl;
    for (unsigned int i=0;i<SIFT_NUM_CELLS*SIFT_NUM_CELLS;i++) {
      for (unsigned int j=0;j<SIFT_NUM_ORIENTATIONS;j++,d1_ptr++) {
	std::cerr << *d1_ptr << " ";
	t+=(*d1_ptr)*(*d1_ptr);
      }
      std::cerr << std::endl;
    }
    std::cerr << "   Magnitude=" << sqrt(t) << std::endl << std::endl;
  }

}

bool VW::GetSIFTDescriptor::
Compute(const std::vector<VW::CornerFeature>& locations,
		  const VW::Pyramid<float>& pyramid,
		  std::vector<VW::SIFTFeature>& resulting_features)
{
  if (VERBOSE) cerr << "VW::GetSIFTDescriptor" << endl;
  const unsigned int num_features = locations.size();
  if (VERBOSE) cerr << " going to process " << num_features << " locations." << endl;

  // Pre-allocate storage space for the features
  resulting_features.clear();
  resulting_features.reserve(num_features*3/2);
  
  if (VERBOSE) cerr << "Build data structures" << endl;
  _SIFT_DistanceMask<SAMPLE_ARRAY_SIZE> distance_weight;
  _SIFT_GaussianMask<SAMPLE_ARRAY_SIZE> orientation_weight(3*2.0);
  _SIFT_GaussianMask<SAMPLE_ARRAY_SIZE> descriptor_weight(double(SAMPLE_ARRAY_SIZE)/2.0);


  // Calculate the orientation and distance weighting masks 
  // (same at each level)
  const int half_array_size = SAMPLE_ARRAY_SIZE/2;
  
  // Store the local gradient magnitude and orientation at each pixel
  double loc_mag[SAMPLE_ARRAY_SIZE][SAMPLE_ARRAY_SIZE];
  double loc_ori[SAMPLE_ARRAY_SIZE][SAMPLE_ARRAY_SIZE];
  
  for (unsigned int i=0; i<num_features; ++i) {
 
    if (VERBOSE) cerr << "Process feature " << i << endl;
    const VW::CornerFeature& location = locations[i];
 
    // Find the closest entry in the pyramid
    const int level = pyramid.ClosestLevel(1.0f/location.GetFeatureScale());
    const float scale = pyramid.GetScale(level);

    const VW::ImageMono<float>& im = pyramid[level];
    const int w = im.GetWidth();
    const int h = im.GetHeight();

   // Get location in coords of this level
    const double x = location.GetX()*scale;
    const double y = location.GetY()*scale;

    // Mask limits
    int row0 = int(y+0.5) - half_array_size;
    int col0 = int(x+0.5) - half_array_size;
    int row1 = row0 + SAMPLE_ARRAY_SIZE;
    int col1 = col0 + SAMPLE_ARRAY_SIZE;

    int mrow0 = 0;
    int mcol0 = 0;
    int mrow1 = SAMPLE_ARRAY_SIZE;
    int mcol1 = SAMPLE_ARRAY_SIZE;

    if (row0<1)        { mrow0 += 1-row0;   row0 += 1-row0;   }
    if (row1>int(h-1)) { mrow1 += h-row1-1; row1 += h-row1-1; }
    if (col0<1)        { mcol0 += 1-col0;   col0 += 1-col0;   }
    if (col1>int(w-1)) { mcol1 += w-col1-1; col1 += w-col1-1; }

    const int mwidth = mcol1-mcol0;
    const int mheight = mrow1-mrow0;

    if ( (mheight>=SAMPLE_ARRAY_SIZE) && (mwidth>=SAMPLE_ARRAY_SIZE) ) {
      // accept only full mask results
      if (_oriented) {
	std::vector<double> orientations;
	const int num_orientations = SIFT_GetGradientsAndOrientation(col0,row0,
								    mcol0,mrow0,mcol1,mrow1,
								    im, 
								    orientation_weight,
								    orientations,
								    loc_ori, loc_mag);
	
	
	
	
	// Calculate the feature vector
	for (int m=0; m<num_orientations; ++m) {

	  SIFTFeature feature(location,orientations[m]);
       
	  ::GenerateDescriptor(feature,
			       mcol0,mrow0,mcol1,mrow1,
			       loc_ori, loc_mag,
			       distance_weight,
			       descriptor_weight);
	  
	  resulting_features.push_back(feature);
	} // End for orientations
      } else {
	// Not oriented
	SIFT_GetGradientsOnly(col0,row0,
			     mcol0,mrow0,mcol1,mrow1,
			     im, 
			     orientation_weight,
			     loc_ori, loc_mag);
	SIFTFeature feature(location,0.0);
	::GenerateDescriptor(feature,
			     mcol0,mrow0,mcol1,mrow1,
			     loc_ori, loc_mag,
			     distance_weight,
			     descriptor_weight);
	  resulting_features.push_back(feature);
      }
    } // End if (fits_in_image)
  } // End for (input_features)

  return true;
}
