/** 
 * @file    sift.cpp
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


#include "sift.h"

#include <vector>
#include <fstream>
#include <math.h> // for pow etc

#include <VW/Image/imagecopy.h>
#include <VW/Improc/convolve.h>

using namespace VW;
using namespace std;

#define WRITE_G_IMAGES 0 // gaussian images
#define WRITE_E_IMAGES 0 // extrema images
#define VERBOSE        0 // be verbose


#define DESCRIPTOR_CAPTURE_SIZE       4 // nxn pixels used per accumulator
#define DESCRIPTOR_POSITIONAL_SIZE    4 // nxn positional accumulators
#define DESCRIPTOR_ORIENTATION_SIZE   8 // accumulators in n orientations
#define SAMPLE_ARRAY_SIZE            16 // Must be DESCRIPTOR_CAPTURE_SIZE * DESCRIPTOR_POSITIONAL_SIZE

#define SIMPLE_FEATURE_VECTOR 0
#define ARRAY_FEATURE         1
#define FAST_CONTRAST_TEST // threshold the closest pixel maxima rather than the sub-pixel one

#define PI  3.141592654
#define LN2 0.693147181 // ln(2)

template <class IN_TYPE, class OUT_TYPE, class TEMP_TYPE>
bool GenerallySmooth(const ImageMono<IN_TYPE> & in, ImageMono<OUT_TYPE> & out, ImageMono<TEMP_TYPE> & tmp, double sigma,
                     int first_u, int first_v, int last_u, int last_v);

bool
VW::SaveSiftFeature(const SiftFeature & feature, const char * filename)
{
    std::ofstream s(filename);
    if (!s.is_open()) return false;
    s << feature.x << " " << feature.y << " " << feature.scale << " " << feature.orientation << endl;
    for (unsigned int i=0; i<128; ++i) {
        s << feature.descriptor[i] << endl;
    }
    s.close();
    return true;
}


bool
VW::LoadSiftFeature(SiftFeature & feature, const char * filename)
{
    std::ifstream s(filename);
    if (!s.is_open()) return false;
    s >> feature.x >>  feature.y >> feature.scale >> feature.orientation;
    for (unsigned int i=0; i<128; ++i) {
        s >> feature.descriptor[i];
    }
    s.close();
    return true;
}

ostream &
VW::operator << (ostream& os, const SiftParameters & p)
{
    os << " Number of octaves:                  " << p.n_octaves << endl;
    os << " Gaussian levels per octave:         " << p.n_levels_per_octave << endl;
    os << " Maxumum curvature ratio:            " << p.max_principal_curvature_ratio << endl;
    os << " Minimum contrast:                   " << p.extrema_peak_threshold << endl;
    os << " Estimated smoothing in input image: " << p.smoothing_in_input_image << endl;

    if (p.quick) os << " Quick pyramid method" << endl;
    else         os << " Full Lowe pyramid method" << endl;

    if (!p.limited_orientation_range) os << " Using full orientation range" << endl;
    else {
        os << " Using orientation range " << (180.0/PI) * p.orientation_range_start;
        os << " to " <<                     (180.0/PI) * p.orientation_range_end << endl;
    }

    if (!p.limited_scale_range) os << " Using full scale range" << endl;
    else {
        os << " Using scale range " << p.scale_range_start;
        os << " to " <<               p.scale_range_end << endl;
    }

    return os;
}

static void TestGradientAndHessian(double peak_threshold_double, int peak_threshold_int);


SiftFeature::SiftFeature()
#if ARRAY_FEATURE
#else
: descriptor(DESCRIPTOR_POSITIONAL_SIZE * DESCRIPTOR_POSITIONAL_SIZE * DESCRIPTOR_ORIENTATION_SIZE, 0.0) 
#endif
{
#if ARRAY_FEATURE
    for (int i=0; i<128; ++i) descriptor[i] = 0.0;
#endif
}

Sift::Sift(const VW::ImageMono<unsigned char> & image,
                             unsigned int roi_r0, unsigned int roi_c0, 
                             unsigned int roi_r1, unsigned int roi_c1,
                             SiftParameters parameters)
: image_(image)
, roi_sr(roi_r0)
, roi_sc(roi_c0)
, roi_fr(roi_r1+1)
, roi_fc(roi_c1+1) 
, roi_width (roi_c1-roi_c0+1)
, roi_height(roi_r1-roi_r0+1)
, G(0)
, Gs(0)
, DOG(0)
, DOGs(0)
, tmp((roi_c1-roi_c0+1)*2, (roi_r1-roi_r0+1)*2)
, features_(0)
, feature_level_ids_(0)
//, resized_feature_size(false)
, DOG_extrema_x(0)
, DOG_extrema_y(0)
, DOG_extrema_s(0)
, DOG_extrema_si(0)
, DOG_extrema_or(0)
, DOG_extrema_sc(0)
, DOG_extrema_n(0)
, max_extrema(image.GetWidth() * image.GetHeight() / 2)
, p(parameters)
{   
     max_n_octaves = p.n_octaves;
    if (p.quick) ++max_n_octaves;

	// Resize arrays and force pointers to zero
    G.resize(max_n_octaves);
    Gs.resize(max_n_octaves);
    DOG .resize(max_n_octaves);
    DOGs.resize(max_n_octaves);

    for (int o=0; o<max_n_octaves; ++o) {
	G   [o].resize(p.n_levels_per_octave+1);
	Gs  [o].resize(p.n_levels_per_octave+1);
	DOG [o].resize(p.n_levels_per_octave);
	DOGs[o].resize(p.n_levels_per_octave);
    }

    delta_sigma = pow(2.0, (1.0/p.n_levels_per_octave));

    max_trace_sq_det_ratio = (p.max_principal_curvature_ratio+1)*(p.max_principal_curvature_ratio+1)/p.max_principal_curvature_ratio;
    //max_trace_sq_det_ratio = p.max_principal_curvature_ratio;

    peak_threshold_double = 255.0 * p.extrema_peak_threshold / (double)p.n_levels_per_octave;
    peak_threshold_int = (int)(peak_threshold_double + 0.5);

    #if VERBOSE
     cout << "SIFT Detector  Window(" << roi_sc << ", " << roi_sr << ")-(" << roi_fc-1 << ", " << roi_fr-1 << ")";
     cout << " [" << roi_width << ", " << roi_height << "]" << endl;
    #endif

    Compute();
}

Sift::~Sift()
{
    for (int oct=0; oct<max_n_octaves; ++oct) {
	for (int lvl=0; lvl<(p.n_levels_per_octave+1); ++lvl) {
	    if (G[oct][lvl] != 0) delete G[oct][lvl];
	}
	for (int lvl=0; lvl<p.n_levels_per_octave; ++lvl) {
	    if (DOG[oct][lvl] != 0) delete DOG[oct][lvl];
	}
    }

    if (DOG_extrema_x)  { delete [] DOG_extrema_x;  DOG_extrema_x  = 0; } 
    if (DOG_extrema_y)  { delete [] DOG_extrema_y;  DOG_extrema_y  = 0; } 
    if (DOG_extrema_s)  { delete [] DOG_extrema_s;  DOG_extrema_s  = 0; } 
    if (DOG_extrema_si) { delete [] DOG_extrema_si; DOG_extrema_si = 0; }
    if (DOG_extrema_or) { delete [] DOG_extrema_or; DOG_extrema_or = 0; } 
    if (DOG_extrema_sc) { delete [] DOG_extrema_sc; DOG_extrema_sc = 0; }

    //unsigned int endsz = features_.size();
    //cout << "Feature vector size increased from " << start_feature_size << " to " << endsz;
    //if (resized_feature_size) cout << " increased" << endl;
    //else                      cout << " not increased" << endl;

}

void
Sift::AllocateMemory()
{
    int oct_width  = roi_width*4;
    int oct_height = roi_height*4;
    for (int o=0; o<max_n_octaves; ++o) {
        oct_width  = (oct_width +1)/2;
        oct_height = (oct_height+1)/2;
        for (int l=0; l<(p.n_levels_per_octave+1); ++l) {

            int lvl_id = (p.n_levels_per_octave+1) * o  + l;
            if (lvl_id < G_start_lvl_id) continue;
            if (lvl_id > G_end_lvl_id  ) continue;

            if (G[o][l]) delete G[o][l];
            G[o][l] = new VW::ImageMono<G_TYPE>(oct_width, oct_height);
	}
    }

    for (int o=0; o<max_n_octaves; ++o) {
        for (int l=0; l<p.n_levels_per_octave; ++l) {

            int lvl_id = p.n_levels_per_octave * o  + l;
            if (lvl_id < DOG_start_lvl_id) continue;
            if (lvl_id > DOG_end_lvl_id  ) continue;

            oct_width  = G[o][l]->GetWidth();
            oct_height = G[o][l]->GetHeight();

            if (DOG[o][l]) delete DOG[o][l];
            DOG[o][l] = new VW::ImageMono<DOG_TYPE>(oct_width, oct_height);
	}
    }

    if (DOG_extrema_x ) { delete [] DOG_extrema_x;  DOG_extrema_x  = 0; } 
    if (DOG_extrema_y ) { delete [] DOG_extrema_y;  DOG_extrema_y  = 0; } 
    if (DOG_extrema_s ) { delete [] DOG_extrema_s;  DOG_extrema_s  = 0; } 
    if (DOG_extrema_si) { delete [] DOG_extrema_si; DOG_extrema_si = 0; }
    if (DOG_extrema_or) { delete [] DOG_extrema_or; DOG_extrema_or = 0; } 
    if (DOG_extrema_sc) { delete [] DOG_extrema_sc; DOG_extrema_sc = 0; }
    DOG_extrema_x  = new double[max_extrema];
    DOG_extrema_y  = new double[max_extrema];
    DOG_extrema_s  = new double[max_extrema];
    DOG_extrema_si = new unsigned int[max_extrema];
    DOG_extrema_or = new double[max_extrema];
    DOG_extrema_sc = new double[max_extrema];

}

void
Sift::Compute()
{
    bool show_progress = VERBOSE;

    if (!CheckInputData()) return;

    CalculateProcessingLevels();
    if (show_progress) cout << "==> Calculated Processing Levels" << endl;

    AllocateMemory();
    if (show_progress) cout << "==> Allocated Memory" << endl;

    MakeGaussianPyramid();
    if (show_progress) cout << "==> Made Gaussian Pyramid" << endl;

    //for (int o=0; o<max_n_octaves; ++o) {
    //    for (int l=0; l<p.n_levels_per_octave; ++l) {
    //        cout << "Octave " << o << " Level " << l << " Sigma " << Gs[o][l] << endl;
    //    }
    //}

    //DisplayGAverageBrightness();

    MakeDOGPyramid();
    if (show_progress) cout << "==> Made Difference of Gaussian Pyramid" << endl;

    //DisplayDOGAverageBrightness();

    FindLocalExtrema();
    if (show_progress) cout << "==> Found Local Extrema" << endl;

    //TestGradientAndHessian(peak_threshold_double, peak_threshold_int);
    //ShowExtrema();

    CalculateFeatures();
    if (show_progress) cout << "==> Calculated Features" << endl;

    AddWindowOffset();
    if (show_progress) cout << "==> Done" << endl;
}


bool
Sift::CheckInputData()
{
    // Descriptor is build from a patch of a certain size but we initially upsample by 2
    unsigned int min_image_size = DESCRIPTOR_CAPTURE_SIZE*DESCRIPTOR_POSITIONAL_SIZE / 2;

    if ( (roi_fr - roi_sr) < min_image_size ) return false;
    if ( (roi_fc - roi_sc) < min_image_size ) return false;

    return true;
}


void
Sift::CalculateProcessingLevels()
{
    double oct_0_scale_range_start = p.scale_range_start * 2.0;
    double oct_0_scale_range_end   = p.scale_range_end   * 2.0;

    top_sigma = 2.0 * p.smoothing_in_input_image;
    if (p.quick) top_sigma = p.smoothing_in_input_image;

    int min_octave = 0;
    int min_extrema_lvl_id = 1;
    int max_extrema_lvl_id = p.n_levels_per_octave * max_n_octaves - 2;
    if (p.quick) {
        min_octave = 1;
        min_extrema_lvl_id = p.n_levels_per_octave + 1;
    }

    if (p.limited_scale_range) {
        int start_o, start_lvl, finish_o, finish_lvl;
 
        double log_del = log(delta_sigma);
        double start_val = log(oct_0_scale_range_start) / log_del;
        start_o    = (int)(start_val / (double)p.n_levels_per_octave);
        start_lvl  = (int)(start_val - (double)(start_o * p.n_levels_per_octave) );
        double end_val = 1.0 + log(oct_0_scale_range_end) / log_del;
        finish_o = (int)(end_val / (double)p.n_levels_per_octave);
        finish_lvl  = (int)(end_val - (double)(finish_o * p.n_levels_per_octave) );

        if (p.quick) {
            ++start_o;
            ++finish_o;
	}

        if (start_o<min_octave) {
            start_o    = min_octave;
            start_lvl  = 0;
        }
        if (finish_o>max_n_octaves-1) {
            finish_o   = max_n_octaves-1;
            finish_lvl = p.n_levels_per_octave-1;
        }
        Extrema_start_lvl_id = p.n_levels_per_octave * start_o  + start_lvl;
        Extrema_end_lvl_id   = p.n_levels_per_octave * finish_o + finish_lvl;

    }
    else {
        Extrema_start_lvl_id = min_extrema_lvl_id;
        Extrema_end_lvl_id   = max_extrema_lvl_id;
    }

    // Set to range imposed by DOG pyramid
    if (Extrema_start_lvl_id < min_extrema_lvl_id) Extrema_start_lvl_id = min_extrema_lvl_id;
    if (Extrema_end_lvl_id   > max_extrema_lvl_id) Extrema_end_lvl_id   = max_extrema_lvl_id;

    DOG_start_lvl_id = Extrema_start_lvl_id - 1;
    DOG_end_lvl_id   = Extrema_end_lvl_id   + 1;

    int G_start_o = DOG_start_lvl_id / p.n_levels_per_octave;
    int G_start_l = DOG_start_lvl_id % p.n_levels_per_octave;
    G_start_lvl_id = (p.n_levels_per_octave+1) * G_start_o  + G_start_l;

    int G_end_o = DOG_end_lvl_id / p.n_levels_per_octave;
    int G_end_l = DOG_end_lvl_id % p.n_levels_per_octave + 1;
    G_end_lvl_id = (p.n_levels_per_octave+1) * G_end_o  + G_end_l;

//    cout << endl;
//    cout << "Extrema Start " << 0.5*oct_0_scale_range_start << " Octave " << Extrema_start_lvl_id / p.n_levels_per_octave;
//     cout << " Level " << Extrema_start_lvl_id % p.n_levels_per_octave << " Sig " << 0.5*top_sigma*pow(delta_sigma, Extrema_start_lvl_id) << endl;
//    cout << "Extrema End   " << 0.5*oct_0_scale_range_end << " Octave " << Extrema_end_lvl_id / p.n_levels_per_octave;
//     cout << " Level " << Extrema_end_lvl_id % p.n_levels_per_octave << " Sig " << 0.5*top_sigma*pow(delta_sigma, Extrema_end_lvl_id) << endl;

//    cout << "G start octave " << G_start_o << " lvl " << G_start_l << " end octave " << G_end_o << " lvl " << G_end_l << endl;

}


// temp float image to hold 0.0625*image & 0.1875*image ??
// pointers might be faster
void
Sift::MakeUpsampledImage()
{
    VW::ImageMono<G_TYPE> & up_image = *G[0][0];

    double p_cen, p_c0m, p_m0, p_c0p, p_p0;
    int rd, cd;

    // Centre Pixels
    for (unsigned int r=roi_sr+1; r<(roi_fr-1); ++r) {
        rd = r - roi_sr;
        for (unsigned int c=roi_sc+1; c<(roi_fc-1); ++c) {
            cd = c - roi_sc;
            p_cen = 0.5625*image_.pixel[r  ][c  ].y;
            p_m0  = 0.1875*image_.pixel[r-1][c  ].y;
            p_p0  = 0.1875*image_.pixel[r+1][c  ].y;
            p_c0p = 0.1875*image_.pixel[r  ][c+1].y + p_cen;
            p_c0m = 0.1875*image_.pixel[r  ][c-1].y + p_cen;
            up_image.pixel[2*rd  ][2*cd  ].y = (G_TYPE)(0.0625*image_.pixel[r-1][c-1].y + p_c0m + p_m0);
            up_image.pixel[2*rd  ][2*cd+1].y = (G_TYPE)(0.0625*image_.pixel[r-1][c+1].y + p_c0p + p_m0);
            up_image.pixel[2*rd+1][2*cd  ].y = (G_TYPE)(0.0625*image_.pixel[r+1][c-1].y + p_c0m + p_p0);
            up_image.pixel[2*rd+1][2*cd+1].y = (G_TYPE)(0.0625*image_.pixel[r+1][c+1].y + p_c0p + p_p0);
	}
    }

    // Edges
    unsigned int rm, cm;
    unsigned int rp, cp;
    unsigned int roi_frb = roi_fr-1;
    unsigned int roi_fcb = roi_fc-1;
    for (unsigned int r=roi_sr; r<roi_fr; r+=(roi_fr-roi_sr-1)) {
        rd = r - roi_sr;
        for (unsigned int c=roi_sc; c<roi_fc; ++c) {
            cd = c - roi_sc;
            rm = (r>roi_sr ) ? r-1 : r;
            rp = (r<roi_frb) ? r+1 : r;
            cm = (c>roi_sc ) ? c-1 : c;
            cp = (c<roi_fcb) ? c+1 : c;
            p_cen = 0.5625*image_.pixel[r ][c ].y;
            p_m0  = 0.1875*image_.pixel[rm][c ].y;
            p_p0  = 0.1875*image_.pixel[rp][c ].y;
            p_c0p = 0.1875*image_.pixel[r ][cp].y + p_cen;
            p_c0m = 0.1875*image_.pixel[r ][cm].y + p_cen;
            up_image.pixel[2*rd  ][2*cd  ].y = (G_TYPE)(0.0625*image_.pixel[rm][cm].y + p_c0m + p_m0);
            up_image.pixel[2*rd  ][2*cd+1].y = (G_TYPE)(0.0625*image_.pixel[rm][cp].y + p_c0p + p_m0);
            up_image.pixel[2*rd+1][2*cd  ].y = (G_TYPE)(0.0625*image_.pixel[rp][cm].y + p_c0m + p_p0);
            up_image.pixel[2*rd+1][2*cd+1].y = (G_TYPE)(0.0625*image_.pixel[rp][cp].y + p_c0p + p_p0);
	}
    }

    for (unsigned int c=roi_sc; c<roi_fc; c+=(roi_fc-roi_sc-1)) {
        cd = c - roi_sc;
        for (unsigned int r=roi_sr; r<roi_fr; ++r) {
            rd = r - roi_sr;
            rm = (r>roi_sr ) ? r-1 : r;
            rp = (r<roi_frb) ? r+1 : r;
            cm = (c>roi_sc ) ? c-1 : c;
            cp = (c<roi_fcb) ? c+1 : c;
            p_cen = 0.5625*image_.pixel[r ][c ].y;
            p_m0  = 0.1875*image_.pixel[rm][c ].y;
            p_p0  = 0.1875*image_.pixel[rp][c ].y;
            p_c0p = 0.1875*image_.pixel[r ][cp].y + p_cen;
            p_c0m = 0.1875*image_.pixel[r ][cm].y + p_cen;
            up_image.pixel[2*rd  ][2*cd  ].y = (G_TYPE)(0.0625*image_.pixel[rm][cm].y + p_c0m + p_m0);
            up_image.pixel[2*rd  ][2*cd+1].y = (G_TYPE)(0.0625*image_.pixel[rm][cp].y + p_c0p + p_m0);
            up_image.pixel[2*rd+1][2*cd  ].y = (G_TYPE)(0.0625*image_.pixel[rp][cm].y + p_c0m + p_p0);
            up_image.pixel[2*rd+1][2*cd+1].y = (G_TYPE)(0.0625*image_.pixel[rp][cp].y + p_c0p + p_p0);
	}
    }

}


void
Sift::Smooth(const ImageMono<G_TYPE> & in, ImageMono<G_TYPE> & out, ImageMono<TMP_TYPE> & tmp, double sigma)
{
    ConvolutionMask1D<float> gauss_mask(5);
    gauss_mask.MakeGaussian(sigma);

    const double mask0 = gauss_mask.mask[0];
    const double mask1 = gauss_mask.mask[1];
    const double mask2 = gauss_mask.mask[2];

    //cout << "Smoothing mask " << mask0 << " " << mask1 << " " << mask2 << endl;

    int n = 2;
    if (in.GetWidth() <(unsigned int)(2*n)) return;
    if (in.GetHeight()<(unsigned int)(2*n)) return;

    vector<double> mask_vals(2*n+1);
    for (int i=0; i<n; ++i) {
        mask_vals[i]     = gauss_mask.mask[n-i];
        mask_vals[2*n-i] = gauss_mask.mask[n-i];
    }
    mask_vals[n] = gauss_mask.mask[0];

    vector<vector<double> > masks(n, vector<double>(2*n+1,0.0) );
    for (int m=0; m<n; ++m) {
	double sum = 0.0;
        for (int i=0; i<(2*n-m); ++i) {
            masks[m][i] = mask_vals[i];
            sum += masks[m][i];
	}
        for (int i=0; i<(2*n-m); ++i) {
	    masks[m][i] /= sum;
	}
    }

    //    cout << "Full Mask ";
    //    for (int i=0; i<(2*n+1); ++i) cout << mask_vals[i] << " ";
    //    cout << endl;
    //    for (int m=0; m<n; ++m) {
    //        cout << "Mask " << m << " ";
    //        for (int i=0; i<(2*n+1); ++i) cout << masks[m][i] << " ";
    //        cout << endl;
    //    }


    int r, c;

    int c_end = in.GetWidth();
    int r_end = in.GetHeight();

    int cc_start = n;
    int rr_start = n;
    int cc_end = c_end - n;
    int rr_end = r_end - n;

    PixelMono<G_TYPE>   **pp_in;
    PixelMono<TMP_TYPE>  *p_tmp;
    PixelMono<G_TYPE>    *p_out; 
  
    for (r=rr_start; r<rr_end; ++r) {
        pp_in = in.pixel + r;    // address of a pointer to row r
        p_tmp = tmp.pixel[r]-1;  // ptr to tmp pixel row r, column -1
        for (c=0; c<c_end; ++c) {
            (++p_tmp)->y = (TMP_TYPE)(mask0 * pp_in[0][c].y + mask1 * (pp_in[1][c].y + pp_in[-1][c].y) + mask2 * (pp_in[2][c].y + pp_in[-2][c].y));
        }
    }

    float pixval;

    // Top border
    for (r=0; r<rr_start; ++r) {
        pp_in = in.pixel + r;    // address of a pointer to row r
        p_tmp = tmp.pixel[r]-1;  // ptr to tmp pixel row r, column -1
        for (c=0; c<c_end; ++c) {
            pixval = 0.0f;
            for (int i=0; i<(n+r+1); ++i) {
                pixval += float(masks[n-r-1][n+r-i] * pp_in[i-r][c].y);
	    }
            (++p_tmp)->y = (TMP_TYPE)pixval;
        }
    }

    // Bottom border
    int rd, cd;
    for (r=rr_end; r<r_end; ++r) {
	rd = r_end-r-1;
        pp_in = in.pixel + r;    // address of a pointer to row r
        p_tmp = tmp.pixel[r]-1;  // ptr to tmp pixel row r, column -1
        for (c=0; c<c_end; ++c) {
            pixval = 0.0;
            for (int i=0; i<(n+rd+1); ++i) {
                pixval += float(masks[n-rd-1][n+rd-i] * pp_in[rd-i][c].y);
	    }
            (++p_tmp)->y = (TMP_TYPE)pixval;
        }
    }

    //tmp.WriteImage("tmpimage.pgm");

    for (r=0; r<r_end; ++r) {
        p_out = out.pixel[r]+cc_start-1;
        p_tmp = tmp.pixel[r]+cc_start;
        for (c=cc_start; c<cc_end; ++c, ++p_tmp) {
            (++p_out)->y = (G_TYPE)(mask0 * p_tmp->y + mask1 * (p_tmp[1].y + p_tmp[-1].y) + mask2 * (p_tmp[2].y + p_tmp[-2].y));
        }
    }

    // Left border
    for (r=0; r<r_end; ++r) {
        p_out = out.pixel[r]-1;
        p_tmp = tmp.pixel[r];
        for (c=0; c<cc_start; ++c, ++p_tmp) {
            pixval = 0.0;
            for (int i=0; i<(n+c+1); ++i) {
                pixval += float(masks[n-c-1][n+c-i] * p_tmp[i-c].y);
	    }
            (++p_out)->y = (G_TYPE)pixval;
        }
    }

    // Right border
    for (r=0; r<r_end; ++r) {
        p_out = out.pixel[r]+cc_end-1;
        p_tmp = tmp.pixel[r]+cc_end;
        for (c=cc_end; c<c_end; ++c, ++p_tmp) {
	    cd = c_end-c-1;
            pixval = 0.0;
            for (int i=0; i<(n+cd+1); ++i) {
                pixval += float(masks[n-cd-1][n+cd-i] * p_tmp[cd-i].y);
	    }
            (++p_out)->y = (G_TYPE)pixval;
        }
    }

}



void
Sift::GeneralSmooth(const ImageMono<G_TYPE> & in, ImageMono<G_TYPE> & out, ImageMono<TMP_TYPE> & tmp, double sigma)
{
    double standard_deviations_covered_by_mask = 3.0;
    
    int n  = (int)(standard_deviations_covered_by_mask*sigma+0.5); // half mask size
    int ms = 2*n+1;

    ConvolutionMask1D<float> gauss_mask(ms);
    gauss_mask.MakeGaussian(sigma);

    double * mask = new double[n+1];
    for (int i=0; i<n+1; ++i) mask[i] = gauss_mask.mask[i];

    if (in.GetWidth() <(unsigned int)(2*n)) return;
    if (in.GetHeight()<(unsigned int)(2*n)) return;

    vector<double> mask_vals(2*n+1);
    for (int i=0; i<n; ++i) {
        mask_vals[i]     = gauss_mask.mask[n-i];
        mask_vals[2*n-i] = gauss_mask.mask[n-i];
    }
    mask_vals[n] = gauss_mask.mask[0];

    vector<vector<double> > masks(n, vector<double>(2*n+1,0.0) );
    for (int m=0; m<n; ++m) {
	double sum = 0.0;
        for (int i=0; i<(2*n-m); ++i) {
            masks[m][i] = mask_vals[i];
            sum += masks[m][i];
	}
        for (int i=0; i<(2*n-m); ++i) {
	    masks[m][i] /= sum;
	}
    }

    int r, c;

    int c_end = in.GetWidth();
    int r_end = in.GetHeight();

    int cc_start = n;
    int rr_start = n;
    int cc_end = c_end - n;
    int rr_end = r_end - n;

    PixelMono<G_TYPE>   **pp_in;
    PixelMono<TMP_TYPE>  *p_tmp;
    PixelMono<G_TYPE>    *p_out; 
  
    double pixel_accumulator;

    for (r=rr_start; r<rr_end; ++r) {
        pp_in = in.pixel + r;    // address of a pointer to row r
        p_tmp = tmp.pixel[r]-1;  // ptr to tmp pixel row r, column -1
        for (c=0; c<c_end; ++c) {
            pixel_accumulator = mask[0] * pp_in[0][c].y;
            for (int i=1; i<n+1; ++i) {
                pixel_accumulator += mask[i] * (pp_in[i][c].y + pp_in[-i][c].y);
	    }
            (++p_tmp)->y = (TMP_TYPE)pixel_accumulator;
            //(mask0 * pp_in[0][c].y + mask1 * (pp_in[1][c].y + pp_in[-1][c].y) + mask2 * (pp_in[2][c].y + pp_in[-2][c].y));
        }
    }

    float pixval;

    // Top border
    for (r=0; r<rr_start; ++r) {
        pp_in = in.pixel + r;    // address of a pointer to row r
        p_tmp = tmp.pixel[r]-1;  // ptr to tmp pixel row r, column -1
        for (c=0; c<c_end; ++c) {
            pixval = 0.0;
            for (int i=0; i<(n+r+1); ++i) {
                pixval += float(masks[n-r-1][n+r-i] * pp_in[i-r][c].y);
	    }
            (++p_tmp)->y = (TMP_TYPE)pixval;
        }
    }

    // Bottom border
    int rd, cd;
    for (r=rr_end; r<r_end; ++r) {
	rd = r_end-r-1;
        pp_in = in.pixel + r;    // address of a pointer to row r
        p_tmp = tmp.pixel[r]-1;  // ptr to tmp pixel row r, column -1
        for (c=0; c<c_end; ++c) {
            pixval = 0.0;
            for (int i=0; i<(n+rd+1); ++i) {
                pixval += float(masks[n-rd-1][n+rd-i] * pp_in[rd-i][c].y);
	    }
            (++p_tmp)->y = (TMP_TYPE)pixval;
        }
    }

    //tmp.WriteImage("tmpimage.pgm");

    for (r=0; r<r_end; ++r) {
        p_out = out.pixel[r]+cc_start-1;
        p_tmp = tmp.pixel[r]+cc_start;
        for (c=cc_start; c<cc_end; ++c, ++p_tmp) {

            pixel_accumulator = mask[0] * p_tmp->y;
            for (int i=1; i<n+1; ++i) {
                pixel_accumulator += mask[i] * (p_tmp[i].y + p_tmp[-i].y);
	    }
            (++p_out)->y = (G_TYPE)pixel_accumulator;

            //(++p_out)->y = (G_TYPE)(mask0 * p_tmp->y + mask1 * (p_tmp[1].y + p_tmp[-1].y) + mask2 * (p_tmp[2].y + p_tmp[-2].y));
        }
    }

    // Left border
    for (r=0; r<r_end; ++r) {
        p_out = out.pixel[r]-1;
        p_tmp = tmp.pixel[r];
        for (c=0; c<cc_start; ++c, ++p_tmp) {
            pixval = 0.0;
            for (int i=0; i<(n+c+1); ++i) {
                pixval += float(masks[n-c-1][n+c-i] * p_tmp[i-c].y);
	    }
            (++p_out)->y = (G_TYPE)pixval;
        }
    }

    // Right border
    for (r=0; r<r_end; ++r) {
        p_out = out.pixel[r]+cc_end-1;
        p_tmp = tmp.pixel[r]+cc_end;
        for (c=cc_end; c<c_end; ++c, ++p_tmp) {
	    cd = c_end-c-1;
            pixval = 0.0;
            for (int i=0; i<(n+cd+1); ++i) {
                pixval += float(masks[n-cd-1][n+cd-i] * p_tmp[cd-i].y);
	    }
            (++p_out)->y = (G_TYPE)pixval;
        }
    }

}



void
Sift::DownSample(const ImageMono<G_TYPE> & in, ImageMono<G_TYPE> & out)
{
    PixelMono<G_TYPE> *p_in;
    PixelMono<G_TYPE> *p_out; 
    unsigned int in_width  = in.GetWidth();
    unsigned int in_height = in.GetHeight();
    unsigned int r, c;
    for (r=0; r<in_height; r+=2) {
        p_in  =  in.pixel[r  ];
        p_out = out.pixel[r/2] - 1;
        for (c=0; c<in_width; c+=2) {
            (++p_out)->y = p_in->y;
            p_in += 2;
        }
    }
}

void
Sift::GeneralDownSample(const ImageMono<G_TYPE> & in, ImageMono<G_TYPE> & out, int n_shrinks)
{
    PixelMono<G_TYPE> *p_in;
    PixelMono<G_TYPE> *p_out; 
    unsigned int in_width  = in.GetWidth();
    unsigned int in_height = in.GetHeight();
    unsigned int r, c;
    int s = 1;
    for (int i=0; i<n_shrinks; ++i) s *= 2;
//    cout << "N Shrinks " << n_shrinks << " Multiplyer " << s << endl;
//pow(2, n_shrinks);
    for (r=0; r<in_height; r+=s) {
        p_in  =  in.pixel[r  ];
        p_out = out.pixel[r/s] - 1;
        for (c=0; c<in_width; c+=s) {
            (++p_out)->y = p_in->y;
            p_in += s;
        }
    }
}

// out[r][c] = 128 + in1[r][c] - in2[r][c] for each image pixel
void
Sift::DifferenceofImages(const ImageMono<G_TYPE> & in1, const ImageMono<G_TYPE> & in2, ImageMono<DOG_TYPE> & out)
{
    PixelMono<G_TYPE> *p_in1;
    PixelMono<G_TYPE> *p_in2;
    PixelMono<DOG_TYPE> *p_out; 
    unsigned int in_width  = in1.GetWidth();
    unsigned int in_height = in1.GetHeight();
    unsigned int r, c;
    for (r=0; r<in_height; ++r) {
        p_in1 = in1.pixel[r]-1;
        p_in2 = in2.pixel[r]-1;
        p_out = out.pixel[r]-1;
        for (c=0; c<in_width; ++c) {
            (++p_out)->y = (DOG_TYPE)128 + (++p_in1)->y - (++p_in2)->y;
        }
    }
}

// Maybe Smooth
void
Sift::MakeInitialGaussianImage(int o, int l, double effective_sigma)
{
    bool verbose = VERBOSE;
    if (verbose) {
        cout << "Initial Image taken from octave " << o << " level " << l << " sigma " << effective_sigma << endl;
    }

    if (o==0) {
        MakeUpsampledImage();
        if (l>0) {
            //double extra_sigma = 2.0 * p.smoothing_in_input_image * sqrt(effective_sigma*effective_sigma - 1.0);
	  cerr << "Smoothing=" << p.smoothing_in_input_image << endl;
            double extra_sigma = sqrt(effective_sigma*effective_sigma - 4.0*p.smoothing_in_input_image*p.smoothing_in_input_image);
            VW::ImageMono<G_TYPE> & im1 = *(G[o][0]);
            VW::ImageMono<G_TYPE> & im2 = *(G[o][l]);
            GeneralSmooth(im1, im2, tmp, extra_sigma);
	}
	return;
    }
    //double extra_sigma = p.smoothing_in_input_image * sqrt(effective_sigma*effective_sigma - 1.0);
    double extra_sigma = 0.5 * sqrt(fabs(effective_sigma*effective_sigma - 4.0*p.smoothing_in_input_image*p.smoothing_in_input_image));
    const VW::ImageMono<unsigned char> & im1 = image_;

    if (verbose) cout << "Extra sigma " << extra_sigma << endl;
    if (extra_sigma<1e-6) {
        if (verbose) cout << "Generating initial image by copying" << endl;
        VW::ImageMono<G_TYPE> & im2 = *(G[o][l]);

        int rd, cd;
        for (unsigned int r=roi_sr; r<roi_fr; ++r) {
            rd = r - roi_sr;
            for (unsigned int c=roi_sc; c<roi_fc; ++c) {
                cd = c - roi_sc;
                im2.pixel[rd][cd].y = (G_TYPE)im1.pixel[r][c].y;
	    }
	}

//        for (unsigned int r=0; r<im1.GetHeight(); ++r) {
//            for (unsigned int c=0; c<im1.GetWidth(); ++c) {
//		cout << r << " " << c << endl;
//                im2.pixel[r][c].y = (G_TYPE)im1.pixel[r][c].y;
//	    }
//	}

        return;
    }

    bool ok;
    if (o==1) {
        if (verbose) cout << "Generating initial image by smoothing" << endl;
        VW::ImageMono<G_TYPE> & im2 = *(G[o][l]);
        ok = GenerallySmooth(im1, im2, tmp, extra_sigma, roi_sc, roi_sr, roi_fc-1, roi_fr-1);
//	if (ok) cout << "done" << endl;
//        else    cout << "badly done" << endl;
    }
    else {
        if (verbose) cout << "Generating initial image by smoothing and downsampling" << endl;
        VW::ImageMono<G_TYPE> im2(roi_width, roi_height); // im1.GetWidth(), im1.GetHeight());
//        VW::ImageMono<G_TYPE> & im3 = *(G[o][l]);
        ok = GenerallySmooth(im1, im2, tmp, extra_sigma, roi_sc, roi_sr, roi_fc-1, roi_fr-1);
//	if (ok) cout << "done" << endl;
//        else    cout << "badly done" << endl;
        VW::ImageMono<G_TYPE> & im3 = *(G[o][l]);
        GeneralDownSample(im2, im3, o-1);
    }

    if (!ok) cout << "Failed to create first image of pyramid" << endl;

//    cout << "done more" << endl;
}

void
Sift::MakeGaussianPyramid()
{
    bool verbose = VERBOSE;

    double effective_sigma=0.0; // Current sigma value in octave 0 space
    double working_sigma=0.0;   // Current sigma in current octave space
    double extra_sigma=0.0;     // Extra sigma smoothing required to create the next level

    //int lvl_id;
    int first_octave = G_start_lvl_id/(p.n_levels_per_octave+1);
    int post_octave  = G_end_lvl_id  /(p.n_levels_per_octave+1)+1;
    int final_level  = G_end_lvl_id % (p.n_levels_per_octave+1);

    if (verbose) cout << "First Octave " << first_octave << " Post Octave " << post_octave << endl;

    for (int o=first_octave; o<post_octave; ++o) {

        int l; // Next level to make in this octave
        if (o==first_octave) {

            // Make the first Gaussian image
            l = G_start_lvl_id % (p.n_levels_per_octave+1);
            effective_sigma = top_sigma * pow(delta_sigma, o*p.n_levels_per_octave+l);
            working_sigma   = top_sigma * pow(delta_sigma, l);

            if (verbose) cout << "Make initial gaussian image at " << o << " " << l << " with sigma " << effective_sigma << endl;
            MakeInitialGaussianImage(o, l, effective_sigma);
            if (verbose) cout << "Made initial gaussian image at " << o << " " << l << endl;
	}
	else {
           
            // Downsample to make the first image of this octave
            VW::ImageMono<G_TYPE> & im1 = *(G[o-1][p.n_levels_per_octave]);
	    VW::ImageMono<G_TYPE> & im2 = *(G[o  ][0]);

            if (0) { // Option to smooth the first image of each new octave
		VW::ImageMono<G_TYPE> tmp_a(G[o-1][0]->GetWidth(), G[o-1][0]->GetHeight());
		VW::ImageMono<TMP_TYPE>      tmp_b(G[o-1][0]->GetWidth(), G[o-1][0]->GetHeight());
                Smooth(im1, tmp_a, tmp_b, 1.6);
                DownSample(tmp_a, im2);
	    }
	    else {
                DownSample(im1, im2);
	    }

            l = 0;
	    working_sigma /= 2.0;
        }

        Gs  [o][l] = effective_sigma;
        DOGs[o][l] = effective_sigma;

        if (verbose) {
	 cout << "Sigma[" << o << "][" << l << "] " << Gs[o][l] << "  " << "Working Sigma[" << o << "][" << l << "] " << working_sigma << endl;
        }

        #if WRITE_G_IMAGES
         char filename[10];
         sprintf(filename,"G%i%i.pgm", o, l);
         G[o][l]->WriteImage(filename);
        #endif

	for (++l; l<(p.n_levels_per_octave+1); ++l) {

            if ( (o==(post_octave-1)) && (l>final_level) ) break;

            extra_sigma = working_sigma * sqrt(delta_sigma*delta_sigma - 1.0);

	    //cout << "Making octave " << o << " level " << l << " extra " << extra_sigma << endl;

            VW::ImageMono<G_TYPE> & im1 = *(G[o][l-1]);
            VW::ImageMono<G_TYPE> & im2 = *(G[o][l]);
            Smooth(im1, im2, tmp, extra_sigma);

            #if WRITE_G_IMAGES
             sprintf(filename,"G%i%i.pgm", o, l);
             G[o][l]->WriteImage(filename);
            #endif

	    working_sigma   = sqrt ( extra_sigma*extra_sigma + working_sigma*working_sigma );
	    effective_sigma = pow(2.0, o) * working_sigma;

	    Gs[o][l] = effective_sigma;
            if (l<p.n_levels_per_octave) DOGs[o][l] = effective_sigma;
            if (verbose) {
             cout << "Sigma[" << o << "][" << l << "] " << Gs[o][l] << " Working Sigma[" << o << "][" << l << "] " << working_sigma <<endl;
	    }

	}
    }

    bool show_sigma_table = false;
    if (show_sigma_table) {
        cout << "Sigma Table" << endl;
        for (unsigned int o=0; o<DOGs.size(); ++o) {
            cout << o << " > ";
            for (unsigned int l=0; l<DOGs[o].size(); ++l) {
                cout << DOGs[o][l] << " ";
	    }
            cout << endl;
	}
    }




}


void
Sift::MakeDOGPyramid()
{
    int lvl_id;
    for (int o=0; o<max_n_octaves; ++o) {
        for (int l=0; l<p.n_levels_per_octave; ++l) {

            lvl_id = p.n_levels_per_octave * o  + l;
            if (lvl_id < DOG_start_lvl_id) continue;
            if (lvl_id > DOG_end_lvl_id  ) continue;

            VW::ImageMono<G_TYPE> & im1 = *(G[o][l+1]);
            VW::ImageMono<G_TYPE> & im2 = *(G[o][l]);
            VW::ImageMono<DOG_TYPE> & im3 = *(DOG[o][l]);

            DifferenceofImages(im1, im2, im3);

            #if WRITE_G_IMAGES
             char filename[10];
             sprintf(filename,"DOG%i%i.pgm", o, l);
             DOG[o][l]->WriteImage(filename);
            #endif

	}
    }

}


// s=sigma e=delta_sigma 
// sigf1 = e/(s(e-1))
// sigf2 = 1/(s(e-1))
// sigf3 = (e-1)/(e*e-1)
// sigf4 = 2e/(s(e*e-1))
//
inline void
GradientAndHessian2(                              const DOG_TYPE & x1y0s0,
                   const DOG_TYPE & x0y1s0, const DOG_TYPE & x1y1s0, const DOG_TYPE & x2y1s0, 
                                                 const DOG_TYPE & x1y2s0,

                   const DOG_TYPE & x0y0s1, const DOG_TYPE & x1y0s1, const DOG_TYPE & x2y0s1, 
                   const DOG_TYPE & x0y1s1, const DOG_TYPE & x1y1s1, const DOG_TYPE & x2y1s1, 
                   const DOG_TYPE & x0y2s1, const DOG_TYPE & x1y2s1, const DOG_TYPE & x2y2s1, 

                                                 const DOG_TYPE & x1y0s2,
                   const DOG_TYPE & x0y1s2, const DOG_TYPE & x1y1s2, const DOG_TYPE & x2y1s2, 
                                                 const DOG_TYPE & x1y2s2,
						  const double & sigf1,
						  const double & sigf2,
						  const double & sigf3,
						  const double & sigf4,
						  const double & sigf5a,
						  const double & sigf5b,
						  const double & sigf5c,
                   double Grad[3], double Hess[3][3], bool s0_half_pixel_separation, bool s2_double_pixel_separation)
{
    Grad[0] = (double)((int)x2y1s1 - x0y1s1)/2.0; // /p
    Grad[1] = (double)((int)x1y2s1 - x1y0s1)/2.0; // /p
    double Gs1 = (x1y1s1 - x1y1s0)*sigf1;
    double Gs2 = (x1y1s2 - x1y1s1)*sigf2;
    Grad[2] = Gs1 + (Gs2-Gs1)*sigf3;

    Hess[0][0] = (double)((int)x0y1s1 + x2y1s1 - x1y1s1 - x1y1s1);                  // /p*p
    Hess[0][1] = (double)((int)x0y0s1 + x2y2s1 - x0y2s1 - x2y0s1)/4.0;              // /p*p
    Hess[1][0] = Hess[0][1];
    Hess[1][1] = (double)((int)x1y0s1 + x1y2s1 - x1y1s1 - x1y1s1);                  // /p*p

    double Gxs0 = (double)((int)x2y1s0 - x0y1s0)/2.0; // /p
    double Gxs2 = (double)((int)x2y1s2 - x0y1s2)/2.0; // /p
    double Gys0 = (double)((int)x1y2s0 - x1y0s0)/2.0; // /p
    double Gys2 = (double)((int)x1y2s2 - x1y0s2)/2.0; // /p
    if (s0_half_pixel_separation) {
        Gxs0 *= 2.0;
        Gys0 *= 2.0;
    }
    if (s2_double_pixel_separation) {
        Gxs2 *= 0.5;
        Gys2 *= 0.5;
    }
    Hess[0][2] = Gxs0*sigf5a + Grad[0]*sigf5b + Gxs2*sigf5c;
    Hess[1][2] = Gys0*sigf5a + Grad[1]*sigf5b + Gys2*sigf5c;
    Hess[2][0] = Hess[0][2];
    Hess[2][1] = Hess[1][2];
    Hess[2][2] = (Gs2-Gs1)*sigf4;
}


static void
print_double_matrix(double matrix[3][3])
{
    cout << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << endl;
    cout << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << endl;
    cout << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << endl;
}


static void
print_double_vector(double vector[3])
{
    cout << vector[0] << " " << vector[1] << " " << vector[2] << endl;
}


static inline bool
LocaliseAndReject(                          const DOG_TYPE & x1y0s0,
                   const DOG_TYPE & x0y1s0, const DOG_TYPE & x1y1s0, const DOG_TYPE & x2y1s0, 
                                            const DOG_TYPE & x1y2s0,

                   const DOG_TYPE & x0y0s1, const DOG_TYPE & x1y0s1, const DOG_TYPE & x2y0s1, 
                   const DOG_TYPE & x0y1s1, const DOG_TYPE & x1y1s1, const DOG_TYPE & x2y1s1, 
                   const DOG_TYPE & x0y2s1, const DOG_TYPE & x1y2s1, const DOG_TYPE & x2y2s1, 

                                            const DOG_TYPE & x1y0s2,
                   const DOG_TYPE & x0y1s2, const DOG_TYPE & x1y1s2, const DOG_TYPE & x2y1s2, 
                                            const DOG_TYPE & x1y2s2,
						  const double & sigf1,
						  const double & sigf2,
						  const double & sigf3,
						  const double & sigf4,
						  const double & sigf5a,
						  const double & sigf5b,
						  const double & sigf5c,
         const double x, const double y, const double & sigma,
         double & res_x, double & res_y, double & res_s, double max_trace_sq_det_ratio,
         bool s0_half_pixel_separation, bool s2_double_pixel_separation,
         double peak_threshold_double, int peak_threshold_int, double & peak_value)
{ 
    #ifdef FAST_CONTRAST_TEST
     // Contrast Test
     int int_peak = abs((int)(x1y1s1)-128);
     if (int_peak < peak_threshold_int) {
        return false; // 0.03*255.0 = 7.65
     }
    #endif

    double Gr[3];    // Gradient
    double Hs[3][3]; // Hessian

    Hs[0][0] = (double)((int)x0y1s1 + x2y1s1 - x1y1s1 - x1y1s1);                  // /p*p
    Hs[0][1] = (double)((int)x0y0s1 + x2y2s1 - x0y2s1 - x2y0s1)/4.0;              // /p*p
    Hs[1][0] = Hs[0][1];
    Hs[1][1] = (double)((int)x1y0s1 + x1y2s1 - x1y1s1 - x1y1s1);                  // /p*p

    if (Hs[0][0]<0.0) Hs[0][0] = -Hs[0][0];
    //if (Hs[0][1]<0.0) Hs[0][1] = -Hs[0][1];
    //if (Hs[1][0]<0.0) Hs[1][0] = -Hs[1][0];
    if (Hs[1][1]<0.0) Hs[1][1] = -Hs[1][1];

    // Local Curvature Test
    double trace = Hs[0][0] + Hs[1][1];
    double det   = Hs[0][0]*Hs[1][1] - Hs[0][1]*Hs[1][0];

//  double BB = sqrt( (Hs[0][0] + Hs[1][1]) * (Hs[0][0] + Hs[1][1]) - 4 * (Hs[0][0] * Hs[1][1] - Hs[0][1] * Hs[0][1]) );
//  double e1 = (Hs[0][0] + Hs[1][1] + BB) / 2.0;
//  double e2 = (Hs[0][0] + Hs[1][1] - BB) / 2.0;
//  double r = e1/e2;
//  if (r<0.0) r = -r;
//  if (r<1.0) r = 1.0/r;
//  double func = (r+1)*(r+1)/r;
//  cout << "e1 " << e1 << " e2 " << e2 << " r " << r << " func " << func << " (trace*trace / det) ";
//  cout << trace*trace / det) << " trace " << trace << " det " << det << endl;

    if ( (trace*trace / det) > max_trace_sq_det_ratio) {
         return false;
    }

    Gr[0] = (double)((int)x2y1s1 - x0y1s1)/2.0; // /p
    Gr[1] = (double)((int)x1y2s1 - x1y0s1)/2.0; // /p
    double Gs1 = (x1y1s1 - x1y1s0)*sigf1;
    double Gs2 = (x1y1s2 - x1y1s1)*sigf2;
    Gr[2] = Gs1 + (Gs2-Gs1)*sigf3;

    double Gxs0 = (double)((int)x2y1s0 - x0y1s0)/2.0; // /p
    double Gxs2 = (double)((int)x2y1s2 - x0y1s2)/2.0; // /p
    double Gys0 = (double)((int)x1y2s0 - x1y0s0)/2.0; // /p
    double Gys2 = (double)((int)x1y2s2 - x1y0s2)/2.0; // /p
    if (s0_half_pixel_separation) {
        Gxs0 *= 2.0;
        Gys0 *= 2.0;
    }
    if (s2_double_pixel_separation) {
        Gxs2 *= 0.5;
        Gys2 *= 0.5;
    }
    Hs[0][2] = Gxs0*sigf5a + Gr[0]*sigf5b + Gxs2*sigf5c;
    Hs[1][2] = Gys0*sigf5a + Gr[1]*sigf5b + Gys2*sigf5c;
    Hs[2][0] = Hs[0][2];
    Hs[2][1] = Hs[1][2];
    Hs[2][2] = (Gs2-Gs1)*sigf4;

    double Hinv[3][3];
    Hinv[0][0] = Hs[2][2]*Hs[1][1]-Hs[2][1]*Hs[1][2];   Hinv[0][1] = Hs[2][1]*Hs[0][2]-Hs[2][2]*Hs[0][1];   Hinv[0][2] = Hs[1][2]*Hs[0][1]-Hs[1][1]*Hs[0][2];
    Hinv[1][0] = Hs[2][0]*Hs[1][2]-Hs[2][2]*Hs[1][0];   Hinv[1][1] = Hs[2][2]*Hs[0][0]-Hs[2][0]*Hs[0][2];   Hinv[1][2] = Hs[1][0]*Hs[0][2]-Hs[1][2]*Hs[0][0];
    Hinv[2][0] = Hs[2][1]*Hs[1][0]-Hs[2][0]*Hs[1][1];   Hinv[2][1] = Hs[2][0]*Hs[0][1]-Hs[2][1]*Hs[0][0];   Hinv[2][2] = Hs[1][1]*Hs[0][0]-Hs[1][0]*Hs[0][1];
    double det_Hess  =   Hs[0][0]*(Hs[2][2]*Hs[1][1]-Hs[2][1]*Hs[1][2])
                        -Hs[1][0]*(Hs[2][2]*Hs[0][1]-Hs[2][1]*Hs[0][2])
                        +Hs[2][0]*(Hs[1][2]*Hs[0][1]-Hs[1][1]*Hs[0][2]);
    double det_Hess_inv = 1.0 / det_Hess;

    double del_x = -det_Hess_inv * (Hinv[0][0]*Gr[0] + Hinv[0][1]*Gr[1] + Hinv[0][2]*Gr[2]);
    double del_y = -det_Hess_inv * (Hinv[1][0]*Gr[0] + Hinv[1][1]*Gr[1] + Hinv[1][2]*Gr[2]);
    double del_s = -det_Hess_inv * (Hinv[2][0]*Gr[0] + Hinv[2][1]*Gr[1] + Hinv[2][2]*Gr[2]);

    res_x = x + del_x;
    res_y = y + del_y;
    res_s = sigma + del_s;

//s0_half_pixel_separation
    if (fabs(del_x) > 1.0) return false;
    if (fabs(del_y) > 1.0) return false;
    if (del_s>(1.0/sigf2)) return false; // >(sigma*delta_sigma - sigma)
    double del_sig = 1.0 + 1.0/(sigma*sigf2);
    if (del_s<(-1.0/del_sig*sigf2)) return false; //  <(sigma/delta_sigma - sigma)

    if (res_s<0.0) {
	cout << "Calculated negative sigma " << res_s << " " << sigma << " " << del_s << endl;

        cout << "        " << x1y0s0 << endl;
        cout << x0y1s0 << " " << x1y1s0 << " " << x2y1s0 << endl;
        cout << "        " << x1y2s0 << endl;

        cout << x0y0s1 << " " << x1y0s1 << " " << x2y0s1 << endl; 
        cout << x0y1s1 << " " << x1y1s1 << " " << x2y1s1 << endl;
        cout << x0y2s1 << " " << x1y2s1 << " " << x2y2s1 << endl;

        cout << "        " << x1y0s2 << endl;
        cout << x0y1s2 << " " << x1y1s2 << " " << x2y1s2 << endl;
        cout << "        " << x1y2s2 << endl;

        cout << "delta sigma " << 1.0 + (1.0/(sigma*sigf2)) << endl;

        cout << "sig " << sigf1  << endl;
        cout << sigf2  << endl;
        cout << sigf3  << endl;
        cout << sigf4  << endl;
        cout << sigf5a << endl;
        cout << sigf5b << endl;
        cout << sigf5c << endl;

        cout << "x " << x << " " << y << " " << sigma << " " << res_x << " " << res_y << " " << res_s << " " << max_trace_sq_det_ratio << endl;
        cout << s0_half_pixel_separation << " " << s2_double_pixel_separation << " ";
        cout << peak_threshold_double << " " << peak_threshold_int << " " << peak_value << endl;

	cout << "Hs"   << endl; print_double_matrix(Hs);
	cout << "Hinv" << endl; print_double_matrix(Hinv);
	cout << "Gr"   << endl; print_double_vector(Gr);











	exit(0);
    }

    #ifndef FAST_CONTRAST_TEST 
     // Contrast Test
     peak_value = (double)x1y1s1 + 0.5*( Gr[0]*del_x + Gr[1]*del_y + Gr[2]*del_s ) - 128.0;
     if (fabs(peak_value) < peak_threshold_double) return false;
    #else
     peak_value = (double)int_peak;
    #endif

    return true;
}



void
TestGradientAndHessian(double peak_threshold_double, int peak_threshold_int)
{
    DOG_TYPE I[3][3][3];

    double C[3][3];
    double deviation = 1.0;
    C[0][0] = deviation;  C[0][1] = 0.0;        C[0][2] = 0.0;  
    C[1][0] = 0.0;        C[1][1] = deviation;  C[1][2] = 0.0;  
    C[2][0] = 0.0;        C[2][1] = 0.0;        C[2][2] = deviation;  

    double scale = 1.0 / sqrt( (2.0*PI) * (2.0*PI) * (2.0*PI) );

    double extrema[3];
    extrema[0] = 99.5;
    extrema[1] = 199.8;
    extrema[2] = 1.5;

    bool s0_half_pixel_separation   = false;
    bool s2_double_pixel_separation = true;

    double pixel_spacing[3];
    pixel_spacing[0] = 1.0;
    pixel_spacing[1] = 1.0;
    pixel_spacing[2] = 1.0;

    if (s0_half_pixel_separation  ) pixel_spacing[0] *= 0.5;
    if (s2_double_pixel_separation) pixel_spacing[2] *= 2.0;

    int pixel_centre[2];
    pixel_centre[0] = 100;
    pixel_centre[1] = 200;

    double sigma       = 1.0; // Sigma for centre image
    double delta_sigma = 2.0; // Sigma multiplier between levels

    double sig_vals[3];
    sig_vals[0] = sigma/delta_sigma;
    sig_vals[1] = sigma;
    sig_vals[2] = sigma*delta_sigma;

    for (int s=0; s<3; ++s) {
	for (int r=0; r<3; ++r) {
	    for (int c=0; c<3; ++c) {
		double x = pixel_centre[0] + (c-1)*pixel_spacing[s];
		double y = pixel_centre[1] + (r-1)*pixel_spacing[s];
		double dm[3];
                dm[0] = x - extrema[0];
                dm[1] = y - extrema[1];
                dm[2] = sig_vals[s] - extrema[2];
		double power = -0.5 * (1.0/deviation) * (dm[0]*dm[0] + dm[1]*dm[1] + dm[2]*dm[2]);
                double val = scale*exp(power); // Gaussian distribution value
		double val2 = val * 4000.0;
                //cout << " I " << val2 << endl;
                I[s][r][c] = (DOG_TYPE)( (int)val2);
	    }
	}
    }

    double sigf1 = delta_sigma/(sigma*(delta_sigma-1));
    double sigf2 =         1.0/(sigma*(delta_sigma-1));
    double sigf3 = (delta_sigma-1)/(delta_sigma*delta_sigma-1);
    double sigf4 = (2.0*delta_sigma)/(sigma*(delta_sigma*delta_sigma-1));
    double sigf5m = 1.0/(sigma*(delta_sigma*delta_sigma-1.0));
    double sigf5a = sigf5m*(-delta_sigma*delta_sigma);
    double sigf5b = sigf5m*(delta_sigma*delta_sigma-1);
    double sigf5c = sigf5m;

    double Grad[3];
    double Hess[3][3];

    GradientAndHessian2(            I[0][0][1],  
                       I[0][1][0], I[0][1][1],  I[0][1][2],
                                   I[0][2][1],  
                       I[1][0][0], I[1][0][1],  I[1][0][2],
                       I[1][1][0], I[1][1][1],  I[1][1][2],
                       I[1][2][0], I[1][2][1],  I[1][2][2],
                                   I[2][0][1],
                       I[2][1][0], I[2][1][1],  I[2][1][2],
                                   I[2][2][1],
			  sigf1, sigf2, sigf3, sigf4, sigf5a, sigf5b, sigf5c,
                          Grad, Hess, s0_half_pixel_separation, s2_double_pixel_separation);


    cout << "Gradient and Hessian Test" << endl;
    cout << "-------------------------" << endl;
    cout << "Image 0 " << (int)I[0][0][0] << " " << (int)I[0][0][1] << " " <<  (int)I[0][0][2] << endl;
    cout << "        " << (int)I[0][1][0] << " " << (int)I[0][1][1] << " " <<  (int)I[0][1][2] << endl;
    cout << "        " << (int)I[0][2][0] << " " << (int)I[0][2][1] << " " <<  (int)I[0][2][2] << endl;
    cout << "Image 1 " << (int)I[1][0][0] << " " << (int)I[1][0][1] << " " <<  (int)I[1][0][2] << endl;
    cout << "        " << (int)I[1][1][0] << " " << (int)I[1][1][1] << " " <<  (int)I[1][1][2] << endl;
    cout << "        " << (int)I[1][2][0] << " " << (int)I[1][2][1] << " " <<  (int)I[1][2][2] << endl;
    cout << "Image 2 " << (int)I[2][0][0] << " " << (int)I[2][0][1] << " " <<  (int)I[2][0][2] << endl;
    cout << "        " << (int)I[2][1][0] << " " << (int)I[2][1][1] << " " <<  (int)I[2][1][2] << endl;
    cout << "        " << (int)I[2][2][0] << " " << (int)I[2][2][1] << " " <<  (int)I[2][2][2] << endl;
    cout << "Sigma 0 " << sigma/delta_sigma << endl;
    cout << "Sigma 1 " << sigma             << endl;
    cout << "Sigma 2 " << sigma*delta_sigma << endl;

    cout << "Gradient " << Grad[0] << " " << Grad[1] << " " <<  Grad[2] << endl;

    cout << "Hessian " << Hess[0][0] << " " << Hess[0][1] << " " <<  Hess[0][2] << endl;
    cout << "        " << Hess[1][0] << " " << Hess[1][1] << " " <<  Hess[1][2] << endl;
    cout << "        " << Hess[2][0] << " " << Hess[2][1] << " " <<  Hess[2][2] << endl;


    unsigned int xxc = pixel_centre[0];
    unsigned int yyc = pixel_centre[1];

    double xfin, yfin, sfin;

    double max_trace_sq_det_ratio = 10.0;

    double score;
    bool ok = LocaliseAndReject(   I[0][0][1],  
                       I[0][1][0], I[0][1][1],  I[0][1][2],
                                   I[0][2][1],  
                       I[1][0][0], I[1][0][1],  I[1][0][2],
                       I[1][1][0], I[1][1][1],  I[1][1][2],
                       I[1][2][0], I[1][2][1],  I[1][2][2],
                                   I[2][0][1],
                       I[2][1][0], I[2][1][1],  I[2][1][2],
                                   I[2][2][1],
                       sigf1, sigf2, sigf3, sigf4, sigf5a, sigf5b, sigf5c, 
                       xxc, yyc, sigma, 
                       xfin, yfin, sfin, max_trace_sq_det_ratio, s0_half_pixel_separation, s2_double_pixel_separation, 
                       peak_threshold_double, peak_threshold_int, score);

    const char * success = ok ? "ok " : "bad ";
    cout << "Final Result "  << success << xfin << " " << yfin << " " << sfin << endl;

    cout << "\n" << endl;
}






void
Sift::FindLocalExtrema()
{
    bool rising; // lower down
    PixelMono<DOG_TYPE> * cptr;
    PixelMono<DOG_TYPE> * cptr_last;
    PixelMono<DOG_TYPE> * cptr_max;

    bool first_level;
    bool last_level;

    DOG_extrema_n = 0;

    double sigf3 = (delta_sigma-1)/(delta_sigma*delta_sigma-1);

    int lvl_id;

    for (int o=0; o<max_n_octaves; ++o) {
        for (int l=0; l<p.n_levels_per_octave; ++l) {

//            cout << "Start" << endl;

            lvl_id = p.n_levels_per_octave * o  + l;

            if (lvl_id < Extrema_start_lvl_id) continue;
            if (lvl_id > Extrema_end_lvl_id  ) continue;
            
//            cout << "Processing Octave " << o << " Level " << l << endl;

//	    if ( (o==0) && (l==0) ) continue;                                       // Don't process first level
//	    if ( (o==(max_n_octaves-1)) && (l==(p.n_levels_per_octave-1)) ) continue; // Don't process last level

            if (l==0) first_level = true;
	    else      first_level = false;
            if (l==(p.n_levels_per_octave-1)) last_level = true;
	    else                              last_level = false;

            bool s0_half_pixel_separation   = first_level;
            bool s2_double_pixel_separation = last_level;

            const VW::ImageMono<DOG_TYPE> & im = *(DOG[o][l]);    // Current DOG Image

            const VW::ImageMono<DOG_TYPE> * p_im_d;
            if (first_level) p_im_d = DOG[o-1][p.n_levels_per_octave-1];
	    else             p_im_d = DOG[o][l-1];
            const VW::ImageMono<DOG_TYPE> & im_d = *p_im_d;       // Next Lowest DOG Image

            const VW::ImageMono<DOG_TYPE> * p_im_u;
            if (last_level) p_im_u = DOG[o+1][0];
	    else            p_im_u = DOG[o][l+1];
            const VW::ImageMono<DOG_TYPE> & im_u = *p_im_u;       // Next Highest DOG Image

//            if (1) {
//                cout << "Finding o " << o << " l " << l << " Im- " << im_d.GetWidth() << " Im " << im.GetWidth() << " Im+ " << im_u.GetWidth() << endl;
//                cout << "Finding o " << o << " l " << l << " Im- " << im_d.GetHeight() << " Im " << im.GetHeight() << " Im+ " << im_u.GetHeight() << endl;
//	    }


            double sigma  = DOGs[o][l];

            double sigf1 = delta_sigma/(sigma*(delta_sigma-1));
            double sigf2 =         1.0/(sigma*(delta_sigma-1));
            double sigf4 = (2.0*delta_sigma)/(sigma*(delta_sigma*delta_sigma-1));
            double sigf5m = 1.0/(sigma*(delta_sigma*delta_sigma-1.0));
            double sigf5a = sigf5m*(-delta_sigma*delta_sigma);
            double sigf5b = sigf5m*(delta_sigma*delta_sigma-1);
            double sigf5c = sigf5m;

            #if WRITE_E_IMAGES
             char filename[20];
             sprintf(filename,"TheImage%i%i.pgm", o, l);
             im.WriteImage(filename);

	     VW::ImageMono<unsigned char> min_im(im.GetWidth(), im.GetHeight());
	     VW::ImageMono<unsigned char> max_im(im.GetWidth(), im.GetHeight());
	     VW::ImageMono<unsigned char> mmin_im(im.GetWidth(), im.GetHeight());
	     VW::ImageMono<unsigned char> mmax_im(im.GetWidth(), im.GetHeight());
	     VW::ImageMono<unsigned char> mmmin_im(im.GetWidth(), im.GetHeight());
	     VW::ImageMono<unsigned char> mmmax_im(im.GetWidth(), im.GetHeight());
	     VW::ImageMono<unsigned char> fmmmin_im(im.GetWidth(), im.GetHeight());
	     VW::ImageMono<unsigned char> fmmmax_im(im.GetWidth(), im.GetHeight());

	     ImageCopy(im, min_im);
	     ImageCopy(im, max_im);
	     ImageCopy(im, mmin_im);
	     ImageCopy(im, mmax_im);
	     ImageCopy(im, mmmin_im);
	     ImageCopy(im, mmmax_im);
	     ImageCopy(im, fmmmin_im);
	     ImageCopy(im, fmmmax_im);
            #endif


            unsigned int cm = im.GetWidth();
            unsigned int rm = im.GetHeight();

            unsigned int cm_u = (cm+1)/2;
	    unsigned int rm_u = (rm+1)/2;

            if (cm<3) continue;
            if (rm<3) continue;

            if (cm_u<3 && last_level) continue;
            if (rm_u<3 && last_level) continue;

//	    cout << "Before Loop" << endl;

            unsigned int c, cc, rr;
            for (unsigned int r=1; r<(rm-1); ++r) {

                cptr = im.pixel[r]+1;
                cptr_last = cptr-1;
		cptr_max = cptr + cm - 1;
                rising = cptr->y > cptr_last->y;
                ++cptr_last;
                ++cptr;

                while (cptr<cptr_max) {
		    if ( rising != (cptr->y > cptr_last->y) ) { // was rising/falling, now doing the opposite
                        

                        #if WRITE_E_IMAGES
			 if (rising) max_im.pixel[r][cptr_last-im.pixel[r]].y = (unsigned char)255;
                         else        min_im.pixel[r][cptr_last-im.pixel[r]].y = (unsigned char)255;
                        #endif

			if (rising) { // potential maxima
                            if (cptr->y!=cptr_last->y) {
                            // store *cptr_last at some point ?

			    if ( (cptr_last->y>(cptr_last-cm)->y) && (cptr_last->y>(cptr_last+cm)->y) ) {
                                #if WRITE_E_IMAGES
                                 mmax_im.pixel[r][cptr_last-im.pixel[r]].y = (unsigned char)255;
                                #endif

			        if ( (cptr_last->y>(cptr_last-cm-1)->y) && (cptr_last->y>(cptr_last+cm-1)->y) && (cptr_last->y>(cptr_last-cm+1)->y) && (cptr_last->y>(cptr_last+cm+1)->y) ) {
                                    #if WRITE_E_IMAGES
                                     mmmax_im.pixel[r][cptr_last-im.pixel[r]].y = (unsigned char)255;
                                    #endif

				    if (first_level) {

                                        c = cptr_last-im.pixel[r];
					cc = 2*c;
                                        rr = 2*r;
                                        
                                        if((cptr_last->y>im_d[rr-1][cc-1].y) && // Next lowest DOG image is twice as big as this one
                                           (cptr_last->y>im_d[rr-1][cc  ].y) &&
                                           (cptr_last->y>im_d[rr-1][cc+1].y) &&
                                           (cptr_last->y>im_d[rr  ][cc-1].y) &&
                                           (cptr_last->y>im_d[rr  ][cc  ].y) &&
                                           (cptr_last->y>im_d[rr  ][cc+1].y) &&
                                           (cptr_last->y>im_d[rr+1][cc-1].y) &&
                                           (cptr_last->y>im_d[rr+1][cc  ].y) &&
                                           (cptr_last->y>im_d[rr+1][cc+1].y) &&

                                           (cptr_last->y>im_u[r-1][c-1].y) &&
                                           (cptr_last->y>im_u[r-1][c  ].y) &&
                                           (cptr_last->y>im_u[r-1][c+1].y) &&
                                           (cptr_last->y>im_u[r  ][c-1].y) &&
                                           (cptr_last->y>im_u[r  ][c  ].y) &&
                                           (cptr_last->y>im_u[r  ][c+1].y) &&
                                           (cptr_last->y>im_u[r+1][c-1].y) &&
                                           (cptr_last->y>im_u[r+1][c  ].y) &&
                                           (cptr_last->y>im_u[r+1][c+1].y) ) {
					    #if WRITE_E_IMAGES
                                             fmmmax_im.pixel[r][c].y = (unsigned char)255;
                                            #endif
                                            if (LocaliseAndReject(                     im_d[rr-1][cc  ].y,
                                                                   im_d[rr  ][cc-1].y, im_d[rr  ][cc  ].y, im_d[rr  ][cc+1].y, 
                                                                                       im_d[rr+1][cc  ].y,
                                                                     im[r-1][c-1].y,   im[r-1][c  ].y, im[r-1][c+1].y,
                                                                     im[r  ][c-1].y,   im[r  ][c  ].y, im[r  ][c+1].y, 
                                                                     im[r+1][c-1].y,   im[r+1][c  ].y, im[r+1][c+1].y,
                                                                                     im_u[r-1][c  ].y,
                                                                   im_u[r  ][c-1].y, im_u[r  ][c  ].y, im_u[r  ][c+1].y, 
                                                                                     im_u[r+1][c  ].y,
	                                                           sigf1, sigf2, sigf3, sigf4, sigf5a, sigf5b, sigf5c, 
										     c, r, sigma,
                                                                   DOG_extrema_x[DOG_extrema_n], DOG_extrema_y[DOG_extrema_n], 
                                                                   DOG_extrema_s[DOG_extrema_n], max_trace_sq_det_ratio,
                                                                   s0_half_pixel_separation, s2_double_pixel_separation, 
                                                                   peak_threshold_double, peak_threshold_int, DOG_extrema_sc[DOG_extrema_n])) {
                                                DOG_extrema_si[DOG_extrema_n] = o*p.n_levels_per_octave + l;
                                                ++DOG_extrema_n;
					    }

					}

				    }

				    if (last_level) {

					c = cptr_last-im.pixel[r];

                                        cc = c/2;
                                        rr = r/2;
                                        if (cc<1) cc=1;
                                        if (rr<1) rr=1;
                                        if (cc>(cm_u-2)) cc = cm_u-2;
                                        if (rr>(rm_u-2)) rr = rm_u-2;

                                        if((cptr_last->y>im_d[r-1][c-1].y) &&
                                           (cptr_last->y>im_d[r-1][c  ].y) &&
                                           (cptr_last->y>im_d[r-1][c+1].y) &&
                                           (cptr_last->y>im_d[r  ][c-1].y) &&
                                           (cptr_last->y>im_d[r  ][c  ].y) &&
                                           (cptr_last->y>im_d[r  ][c+1].y) &&
                                           (cptr_last->y>im_d[r+1][c-1].y) &&
                                           (cptr_last->y>im_d[r+1][c  ].y) &&
                                           (cptr_last->y>im_d[r+1][c+1].y) &&

                                           (cptr_last->y>im_u[rr-1][cc-1].y) && // Next highest DOG image is half as big as this one
                                           (cptr_last->y>im_u[rr-1][cc  ].y) &&
                                           (cptr_last->y>im_u[rr-1][cc+1].y) &&
                                           (cptr_last->y>im_u[rr  ][cc-1].y) &&
                                           (cptr_last->y>im_u[rr  ][cc  ].y) &&
                                           (cptr_last->y>im_u[rr  ][cc+1].y) &&
                                           (cptr_last->y>im_u[rr+1][cc-1].y) &&
                                           (cptr_last->y>im_u[rr+1][cc  ].y) &&
                                           (cptr_last->y>im_u[rr+1][cc+1].y) ) {
					    #if WRITE_E_IMAGES
                                             fmmmax_im.pixel[r][c].y = (unsigned char)255;
                                            #endif
                                            if (LocaliseAndReject(                 im_d[r-1][c  ].y,
                                                                   im_d[r  ][c-1].y, im_d[r  ][c  ].y, im_d[r  ][c+1].y, 
                                                                                   im_d[r+1][c  ].y,
                                                                     im[r-1][c-1].y,   im[r-1][c  ].y, im[r-1][c+1].y,
                                                                     im[r  ][c-1].y,   im[r  ][c  ].y, im[r  ][c+1].y, 
                                                                     im[r+1][c-1].y,   im[r+1][c  ].y, im[r+1][c+1].y,
                                                                                     im_u[rr-1][cc  ].y,
                                                                   im_u[rr  ][cc-1].y, im_u[rr  ][cc  ].y, im_u[rr  ][cc+1].y, 
                                                                                     im_u[rr+1][cc  ].y,
	                                                           sigf1, sigf2, sigf3, sigf4, sigf5a, sigf5b, sigf5c, 
										   c, r, sigma,
                                                                   DOG_extrema_x[DOG_extrema_n], DOG_extrema_y[DOG_extrema_n], 
                                                                   DOG_extrema_s[DOG_extrema_n], max_trace_sq_det_ratio,
                                                                   s0_half_pixel_separation, s2_double_pixel_separation,
                                                                   peak_threshold_double, peak_threshold_int, DOG_extrema_sc[DOG_extrema_n])) {
                                                DOG_extrema_si[DOG_extrema_n] = o*p.n_levels_per_octave + l;
                                                ++DOG_extrema_n;
					    }
					}




				    }

				    if ( !first_level && !last_level ) {
                                         
					c = cptr_last-im.pixel[r];
                                        
                                        if((cptr_last->y>im_d[r-1][c-1].y) &&
                                           (cptr_last->y>im_d[r-1][c  ].y) &&
                                           (cptr_last->y>im_d[r-1][c+1].y) &&
                                           (cptr_last->y>im_d[r  ][c-1].y) &&
                                           (cptr_last->y>im_d[r  ][c  ].y) &&
                                           (cptr_last->y>im_d[r  ][c+1].y) &&
                                           (cptr_last->y>im_d[r+1][c-1].y) &&
                                           (cptr_last->y>im_d[r+1][c  ].y) &&
                                           (cptr_last->y>im_d[r+1][c+1].y) &&

                                           (cptr_last->y>im_u[r-1][c-1].y) &&
                                           (cptr_last->y>im_u[r-1][c  ].y) &&
                                           (cptr_last->y>im_u[r-1][c+1].y) &&
                                           (cptr_last->y>im_u[r  ][c-1].y) &&
                                           (cptr_last->y>im_u[r  ][c  ].y) &&
                                           (cptr_last->y>im_u[r  ][c+1].y) &&
                                           (cptr_last->y>im_u[r+1][c-1].y) &&
                                           (cptr_last->y>im_u[r+1][c  ].y) &&
                                           (cptr_last->y>im_u[r+1][c+1].y) ) {
					    #if WRITE_E_IMAGES
                                             fmmmax_im.pixel[r][c].y = (unsigned char)255;
                                            #endif
                                            if (LocaliseAndReject(                 im_d[r-1][c  ].y,
                                                                   im_d[r  ][c-1].y, im_d[r  ][c  ].y, im_d[r  ][c+1].y, 
                                                                                   im_d[r+1][c  ].y,
                                                                     im[r-1][c-1].y,   im[r-1][c  ].y, im[r-1][c+1].y,
                                                                     im[r  ][c-1].y,   im[r  ][c  ].y, im[r  ][c+1].y, 
                                                                     im[r+1][c-1].y,   im[r+1][c  ].y, im[r+1][c+1].y,
                                                                                   im_u[r-1][c  ].y,
                                                                   im_u[r  ][c-1].y, im_u[r  ][c  ].y, im_u[r  ][c+1].y, 
                                                                                   im_u[r+1][c  ].y,
	                                                           sigf1, sigf2, sigf3, sigf4, sigf5a, sigf5b, sigf5c, 
										   c, r, sigma,
                                                                   DOG_extrema_x[DOG_extrema_n], DOG_extrema_y[DOG_extrema_n], 
                                                                   DOG_extrema_s[DOG_extrema_n], max_trace_sq_det_ratio,
                                                                   s0_half_pixel_separation, s2_double_pixel_separation,
                                                                   peak_threshold_double, peak_threshold_int, DOG_extrema_sc[DOG_extrema_n])) {
                                                DOG_extrema_si[DOG_extrema_n] = o*p.n_levels_per_octave + l;
                                                ++DOG_extrema_n;
					    }

					}

				    }

				}
			    }
                            }
			}
                        else {        // potential minima
                            if ((cptr_last-1)->y!=cptr_last->y) {

			    if ( (cptr_last->y<(cptr_last-cm)->y) && (cptr_last->y<(cptr_last+cm)->y) ) {
				#if WRITE_E_IMAGES
                                 mmin_im.pixel[r][cptr_last-im.pixel[r]].y = (unsigned char)255;
                                #endif

			        if ( (cptr_last->y<(cptr_last-cm-1)->y) && (cptr_last->y<(cptr_last+cm-1)->y) && (cptr_last->y<(cptr_last-cm+1)->y) && (cptr_last->y<(cptr_last+cm+1)->y) ) {
				    #if WRITE_E_IMAGES
                                     mmmin_im.pixel[r][cptr_last-im.pixel[r]].y = (unsigned char)255;
                                    #endif

				    if (first_level) {

                                        c = cptr_last-im.pixel[r];
					cc = 2*c;
                                        rr = 2*r;
                                        
                                        if((cptr_last->y<im_d[rr-1][cc-1].y) && // Next lowest DOG image is twice as big as this one
                                           (cptr_last->y<im_d[rr-1][cc  ].y) &&
                                           (cptr_last->y<im_d[rr-1][cc+1].y) &&
                                           (cptr_last->y<im_d[rr  ][cc-1].y) &&
                                           (cptr_last->y<im_d[rr  ][cc  ].y) &&
                                           (cptr_last->y<im_d[rr  ][cc+1].y) &&
                                           (cptr_last->y<im_d[rr+1][cc-1].y) &&
                                           (cptr_last->y<im_d[rr+1][cc  ].y) &&
                                           (cptr_last->y<im_d[rr+1][cc+1].y) &&

                                           (cptr_last->y<im_u[r-1][c-1].y) &&
                                           (cptr_last->y<im_u[r-1][c  ].y) &&
                                           (cptr_last->y<im_u[r-1][c+1].y) &&
                                           (cptr_last->y<im_u[r  ][c-1].y) &&
                                           (cptr_last->y<im_u[r  ][c  ].y) &&
                                           (cptr_last->y<im_u[r  ][c+1].y) &&
                                           (cptr_last->y<im_u[r+1][c-1].y) &&
                                           (cptr_last->y<im_u[r+1][c  ].y) &&
                                           (cptr_last->y<im_u[r+1][c+1].y) ) {
					    #if WRITE_E_IMAGES
                                             fmmmin_im.pixel[r][c].y = (unsigned char)255;
                                            #endif
                                            if (LocaliseAndReject(                   im_d[rr-1][cc  ].y,
                                                                   im_d[rr  ][cc-1].y, im_d[rr  ][cc  ].y, im_d[rr  ][cc+1].y, 
                                                                                     im_d[rr+1][cc  ].y,
                                                                     im[r-1][c-1].y,   im[r-1][c  ].y, im[r-1][c+1].y,
                                                                     im[r  ][c-1].y,   im[r  ][c  ].y, im[r  ][c+1].y, 
                                                                     im[r+1][c-1].y,   im[r+1][c  ].y, im[r+1][c+1].y,
                                                                                   im_u[r-1][c  ].y,
                                                                   im_u[r  ][c-1].y, im_u[r  ][c  ].y, im_u[r  ][c+1].y, 
                                                                                   im_u[r+1][c  ].y,
	                                                           sigf1, sigf2, sigf3, sigf4, sigf5a, sigf5b, sigf5c, 
										     c, r, sigma,
                                                                   DOG_extrema_x[DOG_extrema_n], DOG_extrema_y[DOG_extrema_n], 
                                                                   DOG_extrema_s[DOG_extrema_n], max_trace_sq_det_ratio,
                                                                   s0_half_pixel_separation, s2_double_pixel_separation,
                                                                   peak_threshold_double, peak_threshold_int, DOG_extrema_sc[DOG_extrema_n])) {
                                                DOG_extrema_si[DOG_extrema_n] = o*p.n_levels_per_octave + l;
                                                ++DOG_extrema_n;
					    }
					}
				    }

				    if (last_level) {

					c = cptr_last-im.pixel[r];

                                        cc = c/2;
                                        rr = r/2;
                                        if (cc<1) cc=1;
                                        if (rr<1) rr=1;
                                        if (cc>(cm_u-2)) cc = cm_u-2;
                                        if (rr>(rm_u-2)) rr = rm_u-2;

                                        if((cptr_last->y<im_d[r-1][c-1].y) &&
                                           (cptr_last->y<im_d[r-1][c  ].y) &&
                                           (cptr_last->y<im_d[r-1][c+1].y) &&
                                           (cptr_last->y<im_d[r  ][c-1].y) &&
                                           (cptr_last->y<im_d[r  ][c  ].y) &&
                                           (cptr_last->y<im_d[r  ][c+1].y) &&
                                           (cptr_last->y<im_d[r+1][c-1].y) &&
                                           (cptr_last->y<im_d[r+1][c  ].y) &&
                                           (cptr_last->y<im_d[r+1][c+1].y) &&

                                           (cptr_last->y<im_u[rr-1][cc-1].y) && // Next highest DOG image is half as big as this one
                                           (cptr_last->y<im_u[rr-1][cc  ].y) &&
                                           (cptr_last->y<im_u[rr-1][cc+1].y) &&
                                           (cptr_last->y<im_u[rr  ][cc-1].y) &&
                                           (cptr_last->y<im_u[rr  ][cc  ].y) &&
                                           (cptr_last->y<im_u[rr  ][cc+1].y) &&
                                           (cptr_last->y<im_u[rr+1][cc-1].y) &&
                                           (cptr_last->y<im_u[rr+1][cc  ].y) &&
                                           (cptr_last->y<im_u[rr+1][cc+1].y) ) {
					    #if WRITE_E_IMAGES
                                             fmmmin_im.pixel[r][c].y = (unsigned char)255;
                                            #endif

                                            if (LocaliseAndReject(                 im_d[r-1][c  ].y,
                                                                   im_d[r  ][c-1].y, im_d[r  ][c  ].y, im_d[r  ][c+1].y, 
                                                                                   im_d[r+1][c  ].y,
                                                                     im[r-1][c-1].y,   im[r-1][c  ].y, im[r-1][c+1].y,
                                                                     im[r  ][c-1].y,   im[r  ][c  ].y, im[r  ][c+1].y, 
                                                                     im[r+1][c-1].y,   im[r+1][c  ].y, im[r+1][c+1].y,
                                                                                     im_u[rr-1][cc  ].y,
                                                                   im_u[rr  ][cc-1].y, im_u[rr  ][cc  ].y, im_u[rr  ][cc+1].y, 
                                                                                     im_u[rr+1][cc  ].y,
	                                                           sigf1, sigf2, sigf3, sigf4, sigf5a, sigf5b, sigf5c, 
										   c, r, sigma,
                                                                   DOG_extrema_x[DOG_extrema_n], DOG_extrema_y[DOG_extrema_n], 
                                                                   DOG_extrema_s[DOG_extrema_n], max_trace_sq_det_ratio,
                                                                   s0_half_pixel_separation, s2_double_pixel_separation,
                                                                   peak_threshold_double, peak_threshold_int, DOG_extrema_sc[DOG_extrema_n])) {
                                                DOG_extrema_si[DOG_extrema_n] = o*p.n_levels_per_octave + l;
                                                ++DOG_extrema_n;
					    }
					}
				    }

				    if ( !first_level && !last_level ) {
                                         
					c = cptr_last-im.pixel[r];
                                        if((cptr_last->y<im_d[r-1][c-1].y) &&
                                           (cptr_last->y<im_d[r-1][c  ].y) &&
                                           (cptr_last->y<im_d[r-1][c+1].y) &&
                                           (cptr_last->y<im_d[r  ][c-1].y) &&
                                           (cptr_last->y<im_d[r  ][c  ].y) &&
                                           (cptr_last->y<im_d[r  ][c+1].y) &&
                                           (cptr_last->y<im_d[r+1][c-1].y) &&
                                           (cptr_last->y<im_d[r+1][c  ].y) &&
                                           (cptr_last->y<im_d[r+1][c+1].y) &&

                                           (cptr_last->y<im_u[r-1][c-1].y) &&
                                           (cptr_last->y<im_u[r-1][c  ].y) &&
                                           (cptr_last->y<im_u[r-1][c+1].y) &&
                                           (cptr_last->y<im_u[r  ][c-1].y) &&
                                           (cptr_last->y<im_u[r  ][c  ].y) &&
                                           (cptr_last->y<im_u[r  ][c+1].y) &&
                                           (cptr_last->y<im_u[r+1][c-1].y) &&
                                           (cptr_last->y<im_u[r+1][c  ].y) &&
                                           (cptr_last->y<im_u[r+1][c+1].y) ) {
					    #if WRITE_E_IMAGES
                                             fmmmin_im.pixel[r][c].y = (unsigned char)255;
                                            #endif
                                            if (LocaliseAndReject(                 im_d[r-1][c  ].y,
                                                                   im_d[r  ][c-1].y, im_d[r  ][c  ].y, im_d[r  ][c+1].y, 
                                                                                   im_d[r+1][c  ].y,
                                                                     im[r-1][c-1].y,   im[r-1][c  ].y, im[r-1][c+1].y,
                                                                     im[r  ][c-1].y,   im[r  ][c  ].y, im[r  ][c+1].y, 
                                                                     im[r+1][c-1].y,   im[r+1][c  ].y, im[r+1][c+1].y,
                                                                                   im_u[r-1][c  ].y,
                                                                   im_u[r  ][c-1].y, im_u[r  ][c  ].y, im_u[r  ][c+1].y, 
                                                                                   im_u[r+1][c  ].y,
	                                                           sigf1, sigf2, sigf3, sigf4, sigf5a, sigf5b, sigf5c, 
										   c, r, sigma,
                                                                   DOG_extrema_x[DOG_extrema_n], DOG_extrema_y[DOG_extrema_n], 
                                                                   DOG_extrema_s[DOG_extrema_n], max_trace_sq_det_ratio,
                                                                   s0_half_pixel_separation, s2_double_pixel_separation,
                                                                   peak_threshold_double, peak_threshold_int, DOG_extrema_sc[DOG_extrema_n])) {
                                                DOG_extrema_si[DOG_extrema_n] = o*p.n_levels_per_octave + l;
                                                ++DOG_extrema_n;
					    }
					}


				    }



				}

                            }
			    }

                        }

			rising = !rising;
		    }
		    ++cptr;
		    ++cptr_last;
		}

		
	    }

            #if WRITE_E_IMAGES
             int minc = 0, maxc = 0, mminc = 0, mmaxc = 0, mmminc = 0, mmmaxc = 0;
             int fmmminc = 0, fmmmaxc = 0;
             for (unsigned int r=0; r<im.GetHeight(); ++r) {
	         for (unsigned int c=0; c<im.GetWidth(); ++c) {
		     if (min_im[r][c].y    == (unsigned char)255) ++minc;
		     if (max_im[r][c].y    == (unsigned char)255) ++maxc;
		     if (mmin_im[r][c].y   == (unsigned char)255) ++mminc;
		     if (mmax_im[r][c].y   == (unsigned char)255) ++mmaxc;
		     if (mmmin_im[r][c].y  == (unsigned char)255) ++mmminc;
		     if (mmmax_im[r][c].y  == (unsigned char)255) ++mmmaxc;
		     if (fmmmin_im[r][c].y == (unsigned char)255) ++fmmminc;
		     if (fmmmax_im[r][c].y == (unsigned char)255) ++fmmmaxc;
 		 }
	     }
             cout << "\nOctave " << o << " Level " << l << endl;
             cout << "Minimums " << minc << " / " << im.GetHeight()*im.GetWidth() << " ("; 
              cout << 100.0*(double)minc/(double)(im.GetHeight()*im.GetWidth()) << "\%)" << endl;
             cout << "Maximums " << maxc << " / " << im.GetHeight()*im.GetWidth() << " ("; 
              cout << 100.0*(double)maxc/(double)(im.GetHeight()*im.GetWidth()) << "\%)" << endl;
             cout << "MMinimums " << mminc << " / " << im.GetHeight()*im.GetWidth() << " ("; 
              cout << 100.0*(double)mminc/(double)(im.GetHeight()*im.GetWidth()) << "\%)" << endl;
             cout << "MMaximums " << mmaxc << " / " << im.GetHeight()*im.GetWidth() << " ("; 
              cout << 100.0*(double)mmaxc/(double)(im.GetHeight()*im.GetWidth()) << "\%)" << endl;
             cout << "MMMinimums " << mmminc << " / " << im.GetHeight()*im.GetWidth() << " ("; 
              cout << 100.0*(double)mmminc/(double)(im.GetHeight()*im.GetWidth()) << "\%)" << endl;
             cout << "MMMaximums " << mmmaxc << " / " << im.GetHeight()*im.GetWidth() << " ("; 
              cout << 100.0*(double)mmmaxc/(double)(im.GetHeight()*im.GetWidth()) << "\%)" << endl;
             cout << "FMMMinimums " << fmmminc <<" / "<< im.GetHeight()*im.GetWidth() <<" ("; 
              cout << 100.0*(double)fmmminc/(double)(im.GetHeight()*im.GetWidth()) << "\%)" << endl;
             cout << "FMMMaximums " << fmmmaxc <<" / "<< im.GetHeight()*im.GetWidth() <<" ("; 
              cout << 100.0*(double)fmmmaxc/(double)(im.GetHeight()*im.GetWidth()) << "\%)" << endl;

             cout << "Total number of extrema is " << DOG_extrema_n << " / " << max_extrema << endl;

             sprintf(filename,"MinImage%i%i.pgm", o, l);
             min_im.WriteImage(filename);
             sprintf(filename,"MaxImage%i%i.pgm", o, l);
             max_im.WriteImage(filename);
             sprintf(filename,"MMinImage%i%i.pgm", o, l);
             mmin_im.WriteImage(filename);
             sprintf(filename,"MMaxImage%i%i.pgm", o, l);
             mmax_im.WriteImage(filename);
             sprintf(filename,"MMMinImage%i%i.pgm", o, l);
             mmmin_im.WriteImage(filename);
             sprintf(filename,"MMMaxImage%i%i.pgm", o, l);
             mmmax_im.WriteImage(filename);
             sprintf(filename,"FMMMinImage%i%i.pgm", o, l);
             fmmmin_im.WriteImage(filename);
             sprintf(filename,"FMMMaxImage%i%i.pgm", o, l);
             fmmmax_im.WriteImage(filename);
            #endif

//	     cout << "Finished" << endl;

	}
    }

    #if 0
     cout << "\nThere were " << DOG_extrema_n << " extrema" << endl;
     for (unsigned int i=0; i<DOG_extrema_n; ++i) {
         cout << " " << DOG_extrema_x[i] << " " << DOG_extrema_y[i] << " " << DOG_extrema_s[i] << " " << DOG_extrema_si[i] << endl;
     }
     cout << endl;
    #endif




}

static void DrawPixel(VW::ImageMono<unsigned char> & image, int r, int c, unsigned char value)
{
    if (r<0)                  return;
    if ((unsigned int)r>=image.GetHeight()) return;
    if (c<0)                  return;
    if ((unsigned int)c>=image.GetWidth())  return;
    if (1) {
        if (image[r][c].y<(unsigned char)128) image[r][c].y = 255;
        else                                image[r][c].y = 0;
    }
    else {
        image[r][c].y = value;
    }
}



static void DrawLine(VW::ImageMono<unsigned char> & image, double x1, double x2, double y1, double y2, unsigned char value)
{

    double dx = x2 - x1;
    double dy = y2 - y1;
 
    bool vertical = fabs(dy/dx) > 1.0;
    if (vertical) {
        double xs = x1, xe = x2, ys = y1, ye = y2;
        if (dy<0) {
            xs = x2; xe = x1; ys = y2; ye = y1;
	}
        double delx = (xe-xs)/(ye-ys);
        while(ys<ye) {
            DrawPixel(image, (int)(ys+0.5), (int)(xs+0.5), value);
            xs += delx;
            ys += 1.0;
	}
    }
    else {
        double xs = x1, xe = x2, ys = y1, ye = y2;
        if (dx<0) {
            xs = x2; xe = x1; ys = y2; ye = y1;
	}
        double dely = (ye-ys)/(xe-xs);
        while(xs<xe) {
            DrawPixel(image, (int)(ys+0.5), (int)(xs+0.5), value);
            xs += 1.0;
            ys += dely;
	}
    }

}



void
Sift::ShowExtrema()
{
    VW::ImageMono<unsigned char> display_image(G[0][0]->GetWidth(), G[0][0]->GetHeight());
    VW::ImageCopy(*G[0][0], display_image);

    double magnification = 6.0;

    for (unsigned int i=0; i<DOG_extrema_n; ++i) {

	int o = DOG_extrema_si[i] / p.n_levels_per_octave;
       
        if (o<2) continue;

        double pixel_scale = pow(2.0,o);
	double x = roi_sc + DOG_extrema_x[i] * pixel_scale;
	double y = roi_sr + DOG_extrema_y[i] * pixel_scale;

        double len = magnification * DOG_extrema_s[i];
        DrawLine(display_image, x, x - len*cos(DOG_extrema_or[i]), y, y - len*sin(DOG_extrema_or[i]),(unsigned char)255);

        int r = (int)(y+0.5);
        int c = (int)(x+0.5); 

        DrawPixel(display_image, r, c,(unsigned char)255);
        DrawPixel(display_image, r-1, c  ,(unsigned char)255);
        DrawPixel(display_image, r+1, c  ,(unsigned char)255);
        DrawPixel(display_image, r  , c-1,(unsigned char)255);
        DrawPixel(display_image, r  , c+1,(unsigned char)255);
    }

    display_image.WriteImage("final_image.pgm");
}

void
Sift::MakeResultImage(VW::ImageMono<unsigned char> & result) const
{
    VW::ImageCopy(image_, result);
    double magnification = 6.0;
    for (unsigned int i=0; i<features_.size(); ++i) {
        double len = magnification * features_[i].scale;
        DrawLine(result, features_[i].x, features_[i].x - len*cos(features_[i].orientation), 
                         features_[i].y, features_[i].y - len*sin(features_[i].orientation),(unsigned char)255);
        int r = (int)(features_[i].y+0.5);
        int c = (int)(features_[i].x+0.5); 
        DrawPixel(result, r, c,(unsigned char)255);
        DrawPixel(result, r-1, c  ,(unsigned char)255);
        DrawPixel(result, r+1, c  ,(unsigned char)255);
        DrawPixel(result, r  , c-1,(unsigned char)255);
        DrawPixel(result, r  , c+1,(unsigned char)255);
    }

}


void
Sift::DisplayGAverageBrightness()
{
    vector<vector<double> > average(max_n_octaves, vector<double>(p.n_levels_per_octave+1, 0.0) );
    for (int o=0; o<max_n_octaves; ++o) {
	for (int l=0; l<(p.n_levels_per_octave+1); ++l) {
            const VW::ImageMono<G_TYPE> & image = *G[o][l];
	    for (unsigned int r=0; r<image.GetHeight(); ++r) {
		for (unsigned int c=0; c<image.GetWidth(); ++c) {
                    double val = (double)image.pixel[r][c].y;
                    average[o][l] += val;
		}
	    }
            cout << "Octave " << o << " Level " << l << " G Average "
                 << average[o][l] / (double)(image.GetHeight()*image.GetWidth()) << endl;
	}
    }
}

void
Sift::DisplayDOGAverageBrightness()
{
    vector<vector<double> > average(max_n_octaves, vector<double>(p.n_levels_per_octave, 0.0) );
    for (int o=0; o<max_n_octaves; ++o) {
	for (int l=0; l<p.n_levels_per_octave; ++l) {
            const VW::ImageMono<DOG_TYPE> & image = *DOG[o][l];
	    for (unsigned int r=0; r<image.GetHeight(); ++r) {
		for (unsigned int c=0; c<image.GetWidth(); ++c) {
                    double val = (double)image.pixel[r][c].y;
                    average[o][l] += val;
		}
	    }
            cout << "Octave " << o << " Level " << l << " DOG Average "
                 << -128.0 + average[o][l] / (double)(image.GetHeight()*image.GetWidth()) << endl;
	}
    }
}


struct weight_mask {
    double mask[SAMPLE_ARRAY_SIZE][SAMPLE_ARRAY_SIZE];
};


// Do it properly:
//  Subpixel location
//  Take into account feature scale
//
void
Sift::CalculateFeatures()
{
    // Calculate the orientation calculation weighting mask for each level (same at each octave)
    int half_array_size = SAMPLE_ARRAY_SIZE/2;
    double d_half_array_size = (double)half_array_size;

    vector<weight_mask> orientation_weights(p.n_levels_per_octave);
    for (int i=0; i<p.n_levels_per_octave; ++i) {
        double sigma = top_sigma * pow(delta_sigma, i); //Gs[0][i];
        double inv_cov_scale = 1.0 / (9.0 * sigma * sigma);
	for (unsigned int r=0; r<SAMPLE_ARRAY_SIZE; ++r) {
            double y = (double)((int)r-(int)half_array_size) + 0.5;
	    for (unsigned int c=0; c<SAMPLE_ARRAY_SIZE; ++c) {
                double x = (double)((int)c-(int)half_array_size) + 0.5;
                double e = -0.5*(x*x + y*y) * inv_cov_scale;
                orientation_weights[i].mask[r][c] = exp(e); 
	    }
	}
    }

    #if 0 // Show the orientation calculation weighting mask
     for (int i=0; i<p.n_levels_per_octave; ++i) {
	 cout << "Mask for sigma of " << top_sigma * pow(delta_sigma, i) << endl;
	 for (unsigned int r=0; r<SAMPLE_ARRAY_SIZE; ++r) {
	     for (unsigned int c=0; c<SAMPLE_ARRAY_SIZE; ++c) {
                 cout << orientation_weights[i].mask[r][c] << " ";
	     }
	     cout << endl;
	 }
	 cout << endl;
     }
    #endif

    // Create a lookup of the distance of each pixel from the descriptor centre
    double distance_threshold = SAMPLE_ARRAY_SIZE / 2;
    double sample_distance[SAMPLE_ARRAY_SIZE][SAMPLE_ARRAY_SIZE];
    double tmp_1D[SAMPLE_ARRAY_SIZE], tmp;
    for (int i=0; i<SAMPLE_ARRAY_SIZE; ++i) {
        tmp = (double)((int)i-half_array_size) + 0.5;
        tmp_1D[i] = tmp * tmp;
    }
    for (unsigned int r=0; r<SAMPLE_ARRAY_SIZE; ++r) {
        for (unsigned int c=0; c<SAMPLE_ARRAY_SIZE; ++c) {
            sample_distance[r][c] = sqrt(tmp_1D[r] + tmp_1D[c]);
            if (sample_distance[r][c]>distance_threshold) sample_distance[r][c] = -1.0;
	}
    }
    for (int i=0; i<SAMPLE_ARRAY_SIZE; ++i) {
        tmp_1D[i] = (double)((int)i-half_array_size) + 0.5;
    }

    // Create the centre weighting mask for the descriptor calculation
    double descriptor_weights[SAMPLE_ARRAY_SIZE][SAMPLE_ARRAY_SIZE];
    double sigma = (double)half_array_size;
    double inv_cov_scale = 1.0 / (sigma * sigma);
    for (unsigned int r=0; r<SAMPLE_ARRAY_SIZE; ++r) {
        double y = (double)((int)r-(int)half_array_size) + 0.5;
        for (unsigned int c=0; c<SAMPLE_ARRAY_SIZE; ++c) {
            double x = (double)((int)c-(int)half_array_size) + 0.5;
            double e = -0.5*(x*x + y*y) * inv_cov_scale;
            descriptor_weights[r][c] = exp(e); 
	}
    }

    // Store the local gradient magnitude and orientation at each pixel
    double ar, ac;
    double twopi = 2.0*PI;
    double loc_mag[SAMPLE_ARRAY_SIZE][SAMPLE_ARRAY_SIZE];
    double loc_ori[SAMPLE_ARRAY_SIZE][SAMPLE_ARRAY_SIZE];

    // Pre-allocate storage space for the features

#if SIMPLE_FEATURE_VECTOR
#else
    // Pre-allocate storage space for the features
    //start_feature_size = (unsigned int) ((double)DOG_extrema_n*1.5);
    //features_ = std::vector<SiftFeature>((unsigned int)((double)DOG_extrema_n*1.5));
    //feature_level_ids_ = vector<int>(features_.size());
    unsigned int new_feature_size = (unsigned int) ((double)DOG_extrema_n*1.5);
    features_.resize(new_feature_size);
    feature_level_ids_.resize(new_feature_size);
    int max_features = features_.size();
    int current_feature = 0;
#endif
    
    //cout << "Potential Feature Positions " << DOG_extrema_n << " Allocated Features " << features_.size() << endl;

    double vec_len_sq;
    double vec_len_inv = 0.0;
    double orientation;

    double dor;
    int    ior;
    int hist_o_low;
    int hist_o_high;
    double o_low_dist;
    double o_low_weight;
    double o_high_weight;

    double bin_width_inv  = (double)DESCRIPTOR_ORIENTATION_SIZE / twopi;

    unsigned int h_position;
    double       h_weight;

    int hist_r_low;
    int hist_r_high;
    double r_low_dist;
    double r_low_weight;
    double r_high_weight;

    int hist_c_low;
    int hist_c_high;
    double c_low_dist;
    double c_low_weight;
    double c_high_weight;
/*
    for (unsigned int i=0; i<DOG_extrema_n; ++i) {
 	int o = DOG_extrema_si[i] / p.n_levels_per_octave;
	int l = DOG_extrema_si[i] % p.n_levels_per_octave;
//	cout << "Full Feature " << i << "/" << DOG_extrema_n << " Octave " << o << " Level " << l << endl;
    }
*/
    for (unsigned int i=0; i<DOG_extrema_n; ++i) {
 
	int o = DOG_extrema_si[i] / p.n_levels_per_octave;
	int l = DOG_extrema_si[i] % p.n_levels_per_octave;

//        cout << "Feature " << i << "/" << DOG_extrema_n << " Octave " << o << " Level " << l << endl;

        const VW::ImageMono<DOG_TYPE> & im = *G[o][l];
	int w = im.GetWidth();
	int h = im.GetHeight();

	//im.WriteImage("myim.pgm");
	//exit(0);

//	cout << "Width " << w << " Height " << h << endl;

	double x = DOG_extrema_x[i];
	double y = DOG_extrema_y[i];

        int rs = (int)y + 1 - (int)half_array_size;
        int cs = (int)x + 1 - (int)half_array_size;
        int re = rs + SAMPLE_ARRAY_SIZE;
        int ce = cs + SAMPLE_ARRAY_SIZE;

	int mrs = 0;
	int mcs = 0;
	int mre = SAMPLE_ARRAY_SIZE;
	int mce = SAMPLE_ARRAY_SIZE;

        if (rs<1)          { mrs += 1-rs;   rs += 1-rs;   }
        if (re>(int)(h-1)) { mre += h-re-1; re += h-re-1; }
        if (cs<1)          { mcs += 1-cs;   cs += 1-cs;   }
        if (ce>(int)(w-1)) { mce += w-ce-1; ce += w-ce-1; }

	//if ( (mrs<mre) && (mcs<mce) ) { // accept partial mask results
        if ( ((mre-mrs)>=SAMPLE_ARRAY_SIZE) && ((mce-mcs)>=SAMPLE_ARRAY_SIZE) ) { // accept only full mask results

            vector<double> bins(36, 0.0);
            vector<int>    binsc(36, 0);

            //cout << "Feature " << i << " Centre r " << (rs+half_array_size)*pixel_scale << " c " << (cs+half_array_size)*pixel_scale << endl;
            //cout << "X " << DOG_extrema_x[i]*pixel_scale << " Y " << DOG_extrema_y[i]*pixel_scale << " Sigma " << DOG_extrema_s[i];
            //cout << " Sigma Level " << DOG_extrema_si[i] << endl;

            // Create the orientation histogram
            int ir = rs;
            for (int mr = mrs; mr<mre; ++mr, ++ir) {
                int ic = cs;
                for (int mc = mcs; mc<mce; ++mc, ++ic) {
                    ar = (double)im[ir+1][ic].y - (double)im[ir-1][ic].y;
                    ac = (double)im[ir][ic+1].y - (double)im[ir][ic-1].y;
                    loc_ori[mr][mc] = PI + atan2(ar,ac);
                    loc_mag[mr][mc] = sqrt(ar*ar + ac*ac);
                    bins[(int)(1e-8+36.0*loc_ori[mr][mc]/twopi)%36] += orientation_weights[l].mask[mr][mc] * loc_mag[mr][mc];
                    ++binsc[(int)(1e-8+36.0*loc_ori[mr][mc]/twopi)%36];
		    //if ( (i==1) || (i==2) ) { cout << "ar " << ar << " ac " << ac << " ang " << PI + atan2(ar,ac) << " dbin ";
                    // cout << (36.0*loc_ori[mr][mc]/twopi) << " abin " << (int)(1e-8+36.0*loc_ori[mr][mc]/twopi)%36 << endl;; }
		}
	    }

            /*
	    cout << "Bins " << i << endl;
	    for (int ji=0; ji<36; ++ji) {
		cout << bins[ji] << " ";
		if (ji%9==8) cout << endl;
	    }
	    cout << endl;

	    int bcb = 0;
	    cout << "Bin count " << i << endl;
	    for (int ji=0; ji<36; ++ji) {
                bcb += binsc[ji];
		cout << binsc[ji] << " ";
		if (ji%9==8) cout << endl;
	    }
	    cout << "Tot " << bcb << endl;
	    cout << endl;
            */

            // Find Maxima in the orientation histogram
	    const int max_multiples = 6;
	    int nmax = 0;
            vector<double> cbins = bins;
            vector<double> max_v(max_multiples, 0.0);
            vector<int>    max_b(max_multiples, 0);
	    for (int m=0; m<max_multiples; ++m) {
		for (int mm=0; mm<m; ++mm) { // Remove previous maximums and their immediate neighbours
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


            // Calculate the feature vector
            for (int m=0; m<nmax; ++m) {
                // Quick quadratic fit
                max_v[m] = ((double)(2*max_b[m]+1)*PI/36.0) - ( (PI/36.0)*(bins[(max_b[m]+1)%36]-bins[(max_b[m]+35)%36])
                            /( bins[(max_b[m]+1)%36] - 2.0*bins[max_b[m]] + bins[(max_b[m]+35)%36] ) );

#if SIMPLE_FEATURE_VECTOR
#else
		if ((current_feature+1)>max_features) {
                    features_.resize(max_features+10);
                    feature_level_ids_.resize(max_features+10);
 		    max_features = features_.size();
                    //resized_feature_size = true;
		}
#endif

                if ( p.limited_orientation_range && ( max_v[m] < p.orientation_range_start ) ) continue;   
                if ( p.limited_orientation_range && ( max_v[m] > p.orientation_range_end   ) ) continue;   


#if SIMPLE_FEATURE_VECTOR
                SiftFeature feature;
#else
                SiftFeature & feature = features_[current_feature];
                feature_level_ids_[current_feature] = DOG_extrema_si[i];
#endif
                double pixel_scale  = pow(2.0,o-1);
                feature.x           = DOG_extrema_x[i]*pixel_scale;
                feature.y           = DOG_extrema_y[i]*pixel_scale;
                feature.scale       = DOG_extrema_s[i]/2.0; // Switch from scale wrt octave 0 to scale wrt the input image (octave 1)
//                feature.scale       = sqrt(delta_sigma/top_sigma)*DOG_extrema_s[i]/2.0;
                feature.orientation = max_v[m];
                feature.strength    = DOG_extrema_sc[i];
//                feature_level_ids_[current_feature] = DOG_extrema_si[i];
            
//		cout << "Feature at " << feature.x << "," << feature.y << " scale " << feature.scale << " orietation " << feature.orientation;
//		cout << " Octave " << o << " Level " << l << endl;

                // the angle which must be added to orientation to make it point upwards
                double global_rot_correction = PI/2.0 - feature.orientation; 

		double dmr, dmc;

                double bin_correction = global_rot_correction + PI/(double)DESCRIPTOR_ORIENTATION_SIZE - PI/2.0;

                // This effectively places the feature at the nearest intersection of 4 pixels, rather than being sub-pixel
                // Otherwise the number of pixels being binned would be varianble - so keep simple for now
                if ( ((mre-mrs)>=DESCRIPTOR_CAPTURE_SIZE*DESCRIPTOR_POSITIONAL_SIZE) &&  // Mask fits fully onto image
                     ((mce-mcs)>=DESCRIPTOR_CAPTURE_SIZE*DESCRIPTOR_POSITIONAL_SIZE) ) { 

                    for (int mr = mrs; mr<mre; ++mr) {
                        for (int mc = mcs; mc<mce; ++mc) {

                            if (sample_distance[mr][mc]<0.0) continue;

                            // Calculate the local intensity orientation
                            orientation = loc_ori[mr][mc] + bin_correction;
                            if (orientation<0.0) orientation += twopi;
                            dor = orientation * bin_width_inv; // Orientation in orientation histogram space [0-DESCRIPTOR_ORIENTATION_SIZE]
                            ior = (int)(dor+1e-8);
                            hist_o_low    = ior%DESCRIPTOR_ORIENTATION_SIZE;
                            hist_o_high   = (1 + hist_o_low)%DESCRIPTOR_ORIENTATION_SIZE;
                            o_low_dist    = dor - (double)ior;
                            o_low_weight  = 1.0 - o_low_dist;
                            o_high_weight = o_low_dist;

                            // Angle of sample position clockwise from a vector pointing right with orientation correction ('global_rot_correction')
                            double sample_angle = global_rot_correction + atan2(tmp_1D[mr], tmp_1D[mc]);

                            // Position of the point in sample histogram space [0-DESCRIPTOR_POSITIONAL_SIZE]
                            dmr = (d_half_array_size + sample_distance[mr][mc] * sin(sample_angle)) / (double)DESCRIPTOR_CAPTURE_SIZE + 0.5;
                            dmc = (d_half_array_size + sample_distance[mr][mc] * cos(sample_angle)) / (double)DESCRIPTOR_CAPTURE_SIZE + 0.5;

                            // Work out which histograms to add to and the weight for each one - max of four (per orientation value)
                            hist_r_high   = (int)dmr;       // Upper histogram row index
                            hist_r_low    = hist_r_high - 1; // Lower histogram row index
                            r_low_dist    = dmr - (int)dmr;
                            r_low_weight  = 1.0 - r_low_dist;
                            r_high_weight = r_low_dist;

                            hist_c_high   = (int)dmc;       // Upper histogram column index
                            hist_c_low    = hist_c_high - 1; // Lower histogram column index
                            c_low_dist    = dmc - (int)dmc;
                            c_low_weight  = 1.0 - c_low_dist;
                            c_high_weight = c_low_dist;

			    //cout << "r " << mr << " c " << mc << " vr " << tmp_1D[mr] << " vc " << tmp_1D[mc] << " dist " << sample_distance[mr][mc];
                            //cout << " new angle " << sample_angle << " dmr " << dmr << " dmc " << dmc;
			    //cout << " r low hist " << hist_r_low << " r high hist " << hist_r_high;
                            //cout << " c low hist " << hist_c_low << " c_high hist " << hist_c_high;
			    //cout << " r low w " << r_low_weight << " r high w " << r_high_weight;
                            //cout << " c low w " << c_low_weight << " c_high w " << c_high_weight << endl;

                            if ( (hist_r_low>-1) && (hist_c_low>-1) ) {
                                h_position = DESCRIPTOR_ORIENTATION_SIZE * (DESCRIPTOR_POSITIONAL_SIZE*hist_r_low + hist_c_low);
                                h_weight   = loc_mag[mr][mc] * descriptor_weights[mr][mc]*r_low_weight*c_low_weight;
                                feature.descriptor[hist_o_low  + h_position] += o_low_weight  * h_weight;
                                feature.descriptor[hist_o_high + h_position] += o_high_weight * h_weight;
                	    }
                            if ( (hist_r_high<DESCRIPTOR_POSITIONAL_SIZE) && (hist_c_low>-1) ) {
                                h_position = DESCRIPTOR_ORIENTATION_SIZE * (DESCRIPTOR_POSITIONAL_SIZE*hist_r_high + hist_c_low);
                                h_weight   = loc_mag[mr][mc] * descriptor_weights[mr][mc]*r_high_weight*c_low_weight;
                                feature.descriptor[hist_o_low  + h_position] += o_low_weight  * h_weight;
                                feature.descriptor[hist_o_high + h_position] += o_high_weight * h_weight;
                	    }
                            if ( (hist_r_low>-1) && (hist_c_high<DESCRIPTOR_POSITIONAL_SIZE) ) {
                                h_position = DESCRIPTOR_ORIENTATION_SIZE * (DESCRIPTOR_POSITIONAL_SIZE*hist_r_low + hist_c_high);
                                h_weight   = loc_mag[mr][mc] * descriptor_weights[mr][mc]*r_low_weight*c_high_weight;
                                feature.descriptor[hist_o_low  + h_position] += o_low_weight  * h_weight;
                                feature.descriptor[hist_o_high + h_position] += o_high_weight * h_weight;
                	    }
                            if ( (hist_r_high<DESCRIPTOR_POSITIONAL_SIZE) && (hist_c_high<DESCRIPTOR_POSITIONAL_SIZE) ) {
                                h_position = DESCRIPTOR_ORIENTATION_SIZE * (DESCRIPTOR_POSITIONAL_SIZE*hist_r_high + hist_c_high);
                                h_weight   = loc_mag[mr][mc] * descriptor_weights[mr][mc]*r_high_weight*c_high_weight;
                                feature.descriptor[hist_o_low  + h_position] += o_low_weight  * h_weight;
                                feature.descriptor[hist_o_high + h_position] += o_high_weight * h_weight;
                	    }

			}
                    }


		}

                // Normalise feature vector to unit length
                vec_len_sq = 0.0;
                for (int d=0; d<(DESCRIPTOR_POSITIONAL_SIZE * DESCRIPTOR_POSITIONAL_SIZE * DESCRIPTOR_ORIENTATION_SIZE); ++d) {
                    vec_len_sq += feature.descriptor[d] * feature.descriptor[d];
		}
                if (vec_len_sq>1e-8) vec_len_inv = 1.0 / sqrt(vec_len_sq);
                else                 vec_len_inv = 1.0;
                for (int d=0; d<(DESCRIPTOR_POSITIONAL_SIZE * DESCRIPTOR_POSITIONAL_SIZE * DESCRIPTOR_ORIENTATION_SIZE); ++d) {
                    feature.descriptor[d] *= vec_len_inv;
		}

		// Threshold each element at 0.2
                for (int d=0; d<(DESCRIPTOR_POSITIONAL_SIZE * DESCRIPTOR_POSITIONAL_SIZE * DESCRIPTOR_ORIENTATION_SIZE); ++d) {
                    if (feature.descriptor[d] > 0.2) feature.descriptor[d] = 0.2;
		}
                // Renormalise feature vector to unit length
                vec_len_sq = 0.0;
                for (int d=0; d<(DESCRIPTOR_POSITIONAL_SIZE * DESCRIPTOR_POSITIONAL_SIZE * DESCRIPTOR_ORIENTATION_SIZE); ++d) {
                    vec_len_sq += feature.descriptor[d] * feature.descriptor[d];
		}
                if (vec_len_sq>1e-8) vec_len_inv = 1.0 / sqrt(vec_len_sq);
                else                 vec_len_inv = 1.0;

                for (int d=0; d<(DESCRIPTOR_POSITIONAL_SIZE * DESCRIPTOR_POSITIONAL_SIZE * DESCRIPTOR_ORIENTATION_SIZE); ++d) {
                    feature.descriptor[d] *= vec_len_inv;
		}

#if SIMPLE_FEATURE_VECTOR
                features_.push_back(feature);
                feature_level_ids_.push_back(DOG_extrema_si[i]);
#else
                ++current_feature;
#endif

	    }

	}
        
    }

#if SIMPLE_FEATURE_VECTOR
#else
    // Remove unused features
    features_.resize(current_feature);
#endif

    #if VERBOSE
     cout << "Found " << features_.size() << " features" << endl;
    #endif
}


void
Sift::WriteDescriptorImage(int index, const char * filename) const
{
    int o = feature_level_ids_[index] / p.n_levels_per_octave;
    int l = feature_level_ids_[index] % p.n_levels_per_octave;
    WriteDescriptorImage(features_[index], o, l, filename);
}

void
Sift::AddWindowOffset()
{
    for (unsigned int i=0; i<features_.size(); ++i) {
        features_[i].x += roi_sc;
        features_[i].y += roi_sr;
    }
}


void
Sift::WriteDescriptorImage(const SiftFeature & feature, int o, int l, const char * filename) const
{
    const int    image_scale    = 40;
    double magnification  = 10.0;

    double pixel_scale = pow(2.0,o-1);
    const VW::ImageMono<G_TYPE> & im = *G[o][l];

    double x = (feature.x - roi_sc) / pixel_scale;
    double y = (feature.y - roi_sr) / pixel_scale;
    int half_array_size = SAMPLE_ARRAY_SIZE/2;

    int rs = (int)y + 1 - (int)half_array_size;
    int cs = (int)x + 1 - (int)half_array_size;
    int re = rs + SAMPLE_ARRAY_SIZE;
    int ce = cs + SAMPLE_ARRAY_SIZE;

    int border = 0;
    int strtr = (rs-border);
    int strtc = (cs-border);
    int endpr = (re+border);
    int endpc = (ce+border);

    VW::ImageMono<unsigned char> conv_im( image_scale*(endpc-strtc),  image_scale*(endpr-strtr));
    int cw = conv_im.GetWidth();
    int ch = conv_im.GetHeight();

    double global_rot_correction = PI/2.0 - feature.orientation;
    int      half_image_size = SAMPLE_ARRAY_SIZE*image_scale/2;
    double d_half_image_size = (double)half_image_size;

    double tmp_1D[SAMPLE_ARRAY_SIZE*image_scale];
    for (int i=0; i<SAMPLE_ARRAY_SIZE*image_scale; ++i) {
        tmp_1D[i] = (double)((int)i-half_image_size) + 0.5;
    }

    // Draw the Gaussian image closest to the feature's scale, rotated to have an upwards pointing principal orientation
    for (int r = strtr; r<endpr; ++r) {
        for (int c = strtc; c<endpc; ++c) {

            for (int rr=0; rr<image_scale; ++rr) {
                for (int cc=0; cc<image_scale; ++cc) {
                    int ir = rr+image_scale*(r-strtr);
		    int ic = cc+image_scale*(c-strtc);
                    double distance = sqrt(tmp_1D[ir]*tmp_1D[ir] + tmp_1D[ic]*tmp_1D[ic]);
                    double angle    = global_rot_correction + atan2(tmp_1D[ir], tmp_1D[ic]);
                    int dmr = (int)(d_half_image_size + distance * sin(angle));
                    int dmc = (int)(d_half_image_size + distance * cos(angle));
                    if ( (dmr>-1) && (dmr<ch) && (dmc>-1) && (dmc<cw) ) {
                        conv_im.pixel[dmr][dmc].y = (unsigned char)im.pixel[r][c].y;
                        if (distance > d_half_image_size) {
                            conv_im.pixel[dmr][dmc].y = (unsigned char)0;
			}
		    }
		}
            }
        }
    }
    for (unsigned int r=1; r<(conv_im.GetHeight()-1); ++r) {
        for (unsigned int c=1; c<(conv_im.GetWidth()-1); ++c) {
            if (conv_im.pixel[r][c].y == (unsigned char)0) {
                if ( (conv_im.pixel[r][c-1].y != (unsigned char)0) && (conv_im.pixel[r][c+1].y != (unsigned char)0) ) {
                    conv_im.pixel[r][c].y = (conv_im.pixel[r][c-1].y + conv_im.pixel[r][c+1].y)/2;
		}
	    }
	}
    }

    double twopi = 2.0*PI;
    for (unsigned int pr=0; pr<DESCRIPTOR_POSITIONAL_SIZE; ++pr) {
        for (unsigned int pc=0; pc<DESCRIPTOR_POSITIONAL_SIZE; ++pc) {
            for (unsigned int ort=0; ort<DESCRIPTOR_ORIENTATION_SIZE; ++ort) {
                double angle = PI/2.0 + ort*twopi/(double)DESCRIPTOR_ORIENTATION_SIZE;
                double value = magnification * (double)image_scale * 
                               feature.descriptor[pr*DESCRIPTOR_POSITIONAL_SIZE*DESCRIPTOR_ORIENTATION_SIZE + pc*DESCRIPTOR_ORIENTATION_SIZE + ort];
                double cx = pc*DESCRIPTOR_POSITIONAL_SIZE + (DESCRIPTOR_POSITIONAL_SIZE/2) - 0.5 + cs - strtc;
                double cy = pr*DESCRIPTOR_POSITIONAL_SIZE + (DESCRIPTOR_POSITIONAL_SIZE/2) - 0.5 + rs - strtr;
                cx = (double)image_scale*(cx+0.5)-0.5;
                cy = (double)image_scale*(cy+0.5)-0.5;
                DrawLine(conv_im, cx, cx - value*cos(angle), cy, cy - value*sin(angle),(unsigned char)128);
                if (0) {
                     int xx = (int)(0.5 + cx - value*cos(angle));
                     int yy = (int)(0.5 + cy - value*sin(angle));
                     DrawPixel(conv_im, xx, yy,(unsigned char)128);
                     DrawPixel(conv_im, xx-1, yy-1,(unsigned char)128);
                     DrawPixel(conv_im, xx-1, yy+1,(unsigned char)128);
                     DrawPixel(conv_im, xx+1, yy-1,(unsigned char)128);
                     DrawPixel(conv_im, xx+1, yy+1,(unsigned char)128);
                }
            }
        }
    }
    /* orientation
    double value = magnification * (double)image_scale * 0.2;
    double ccx = (double)(image_scale*(endpc-strtc)/2)  - 0.5;
    double ccy = (double)(image_scale*(endpr-strtr)/2)  - 0.5;
    DrawLine(conv_im, ccx, ccx - value*cos(feature.orientation), ccy, ccy - value*sin(feature.orientation),(unsigned char)255);
    int xx = (int)(0.5 + ccx - value*cos(feature.orientation));
    int yy = (int)(0.5 + ccy - value*sin(feature.orientation));
    DrawPixel(conv_im, yy, xx,(unsigned char)255);
    DrawPixel(conv_im, yy-1, xx-1,(unsigned char)255);
    DrawPixel(conv_im, yy-1, xx+1,(unsigned char)255);
    DrawPixel(conv_im, yy+1, xx-1,(unsigned char)255);
    DrawPixel(conv_im, yy+1, xx+1,(unsigned char)255);
    */

    int ccc = image_scale*(endpc-strtc)/2;
    DrawPixel(conv_im, ccc-1, ccc-1,(unsigned char)255);
    DrawPixel(conv_im, ccc-1, ccc  ,(unsigned char)255);
    DrawPixel(conv_im, ccc  , ccc-1,(unsigned char)255);
    DrawPixel(conv_im, ccc  , ccc  ,(unsigned char)255);

    conv_im.WriteImage(filename);

}





int
VW::SiftMatchNN(const SiftFeature & key, const std::vector<SiftFeature> & database, 
                double distance_ratio_max, double min_strength)
{
    int    best_index = -1;
    double best_value = 1000000.0;
    int    second_index = -1;
    double second_value = 1000000.0;

    double current_value;
    double tmp;
#if ARRAY_FEATURE
    unsigned int descriptor_size = 128;
#else
    unsigned int descriptor_size = key.descriptor.size();
#endif

    for (unsigned int i=0; i<database.size(); ++i) {
        current_value = 0.0;
        for (unsigned int v=0; v<descriptor_size; ++v) {
            tmp = database[i].descriptor[v] - key.descriptor[v];
            current_value += tmp * tmp;
            if (current_value > second_value) break;
	}
        if (current_value > second_value) continue;
        if (current_value > best_value) {
	    second_index = i;
	    second_value = current_value;
	}
	else {
	    second_index = best_index;
	    second_value = best_value;
	    best_index   = i;
	    best_value   = current_value;
	}
    }

    double final_distance = sqrt(best_value);

    //cout << "Final distance is " << final_distance << " Second is " << sqrt(second_value) << " Threshold is " << min_strength << endl;

    if (final_distance / sqrt(second_value) > distance_ratio_max) return -1;
    if (final_distance * min_strength       > 1.0)                return -1;

    return best_index;
}











template <class IN_TYPE, class OUT_TYPE, class TEMP_TYPE>
bool
GenerallySmooth(const ImageMono<IN_TYPE> & in, ImageMono<OUT_TYPE> & out, ImageMono<TEMP_TYPE> & tmp, double sigma,
                int first_u, int first_v, int last_u, int last_v)

//void
//GeneralSmooth(const ImageMono<G_TYPE> & in, ImageMono<G_TYPE> & out, ImageMono<TMP_TYPE> & tmp, double sigma)
{
    double standard_deviations_covered_by_mask = 3.0;
    
    int n  = (int)(standard_deviations_covered_by_mask*sigma+0.5); // half mask size
    int ms = 2*n+1;

    ConvolutionMask1D<float> gauss_mask(ms);
    gauss_mask.MakeGaussian(sigma);

    double * mask = new double[n+1];
    for (int i=0; i<n+1; ++i) mask[i] = gauss_mask.mask[i];

    unsigned int area_w = last_u - first_u + 1;
    unsigned int area_h = last_v - first_v + 1;

    if (area_w<(unsigned int)(2*n)) return false; // Check that the region of interest is big enough for processing
    if (area_h<(unsigned int)(2*n)) return false;
    if (area_w!=out.GetWidth())     return false; // Check that the output image is the same size as the region of interest
    if (area_h!=out.GetHeight())    return false;
    if (area_w>tmp.GetWidth())      return false; // Check that the temporary image is big enough
    if (area_h>tmp.GetHeight())     return false;
    if (first_u<0)                  return false; // Check that region of interest lies within the input image
    if (first_v<0)                  return false;
    if ((unsigned int)last_u>=in.GetWidth())      return false;
    if ((unsigned int)last_v>=in.GetHeight())     return false;

    vector<double> mask_vals(2*n+1);
    for (int i=0; i<n; ++i) {
        mask_vals[i]     = gauss_mask.mask[n-i];
        mask_vals[2*n-i] = gauss_mask.mask[n-i];
    }
    mask_vals[n] = gauss_mask.mask[0];

    vector<vector<double> > masks(n, vector<double>(2*n+1,0.0) );
    for (int m=0; m<n; ++m) {
	double sum = 0.0;
        for (int i=0; i<(2*n-m); ++i) {
            masks[m][i] = mask_vals[i];
            sum += masks[m][i];
	}
        for (int i=0; i<(2*n-m); ++i) {
	    masks[m][i] /= sum;
	}
    }

    int r, c;

//    int c_end = in.GetWidth();
//    int r_end = in.GetHeight();

    int cc_start = n;
    int rr_start = n;
    int cc_end = area_w - n;
    int rr_end = area_h - n;

    PixelMono<IN_TYPE>   **pp_in;
    PixelMono<TEMP_TYPE>  *p_tmp;
    PixelMono<OUT_TYPE>   *p_out; 
  
    long double pixel_accumulator; // Changed by BJT - this should be at least as big as the pixel type 

    for (r=rr_start; r<rr_end; ++r) {
        pp_in = in.pixel + r + first_v; // address of a pointer to row r + first_v
        p_tmp = tmp.pixel[r]-1;         // ptr to tmp pixel row r, column -1
        for (c=first_u; c<first_u+(int)area_w; ++c) {
            pixel_accumulator = mask[0] * double(pp_in[0][c].y);
            for (int i=1; i<n+1; ++i) {
                pixel_accumulator += mask[i] * (double(pp_in[i][c].y) + double(pp_in[-i][c].y));
	    }
            (++p_tmp)->y = (TEMP_TYPE)pixel_accumulator;
        }
    }

    // Top border
    for (r=0; r<rr_start; ++r) {
        pp_in = in.pixel + r + first_v; // address of a pointer to row r + first_v
        p_tmp = tmp.pixel[r]-1;         // ptr to tmp pixel row r, column -1
        for (c=first_u; c<first_u+(int)area_w; ++c) {
            pixel_accumulator = 0.0;
            for (int i=0; i<(n+r+1); ++i) {
                pixel_accumulator += masks[n-r-1][n+r-i] * (double)pp_in[i-r][c].y;
	    }
            (++p_tmp)->y = (TEMP_TYPE)pixel_accumulator;
        }
    }

    // Bottom border
    int rd, cd;
    for (r=rr_end; r<(int)area_h; ++r) {
	rd = area_h-r-1;
        pp_in = in.pixel + r + first_v; // address of a pointer to row r + first_v
        p_tmp = tmp.pixel[r]-1;         // ptr to tmp pixel row r, column -1
        for (c=first_u; c<first_u+(int)area_w; ++c) {
            pixel_accumulator = 0.0;
            for (int i=0; i<(n+rd+1); ++i) {
                pixel_accumulator += masks[n-rd-1][n+rd-i] * (double)pp_in[rd-i][c].y;
	    }
            (++p_tmp)->y = (TEMP_TYPE)pixel_accumulator;
        }
    }

    //tmp.WriteImage("tmpimage.pgm");

    for (r=0; r<(int)area_h; ++r) {
        p_out = out.pixel[r]+cc_start-1;
        p_tmp = tmp.pixel[r]+cc_start;
        for (c=cc_start; c<cc_end; ++c, ++p_tmp) {

            pixel_accumulator = mask[0] * p_tmp->y;
            for (int i=1; i<n+1; ++i) {
                pixel_accumulator += mask[i] * (p_tmp[i].y + p_tmp[-i].y);
	    }
            (++p_out)->y = (OUT_TYPE)pixel_accumulator;
        }
    }

    // Left border
    for (r=0; r<(int)area_h; ++r) {
        p_out = out.pixel[r]-1;
        p_tmp = tmp.pixel[r];
        for (c=0; c<cc_start; ++c, ++p_tmp) {
            pixel_accumulator = 0.0;
            for (int i=0; i<(n+c+1); ++i) {
                pixel_accumulator += masks[n-c-1][n+c-i] * p_tmp[i-c].y;
	    }
            (++p_out)->y = (OUT_TYPE)pixel_accumulator;
        }
    }

    // Right border
    for (r=0; r<(int)area_h; ++r) {
        p_out = out.pixel[r]+cc_end-1;
        p_tmp = tmp.pixel[r]+cc_end;
        for (c=cc_end; c<(int)area_w; ++c, ++p_tmp) {
	    cd = area_w-c-1;
            pixel_accumulator = 0.0;
            for (int i=0; i<(n+cd+1); ++i) {
                pixel_accumulator += masks[n-cd-1][n+cd-i] * p_tmp[cd-i].y;
	    }
            (++p_out)->y = (OUT_TYPE)pixel_accumulator;
        }
    }

    return true;
}








#undef SAMPLE_ARRAY_SIZE
#undef PI
#undef DESCRIPTOR_CAPTURE_SIZE
#undef DESCRIPTOR_POSITIONAL_SIZE
#undef DESCRIPTOR_ORIENTATION_SIZE
