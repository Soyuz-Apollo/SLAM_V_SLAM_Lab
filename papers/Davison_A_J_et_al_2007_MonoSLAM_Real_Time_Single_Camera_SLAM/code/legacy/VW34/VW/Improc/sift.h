/** 
 * @file    sift.h
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
/**
 * @class   VW::Sift
 * @ingroup ImageProcessing
 * @author  ND Molton
 * @date    15/10/2003
 * @brief   Interest point detection using David Lowe's SIFT (Scale invariant feature transform) descriptor
 *
 * 
 * This is an implementation of David Lowe's SIFT (Scale invariant feature transform)
 *  feature detector and a simple feature matcher. The implementation is based on the
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
 *
 * Classes/Structures/Functions are: 
 *
 *  SiftFeature    - The feature vector
 *  SiftParameters - Parameters of the SIFT Detector
 *  Sift           - The detector
 *  SiftMatchNN    - A basic nearest neighbour feature matching function
 * 
 */


// Notes:
// 
//  Not sure if images should be smoothed as part of the subsampling process
//  Scales tend to vary in a quantised, rather than continuous way
//  
//

#ifndef sift_h_
#define sift_h_

#include <vector>
#include <ostream>

#include <VW/Image/imagemono.h>

namespace VW {

// The descriptor is formed from a 4 x 4 x 8 grid (row, column, orientation)
// Elements in the feature vector are arranged as Pr0_Pc0_Or0, .. Pr0_Pc0_Or7, Pr0_Pc1_Or0, ... etc

struct SiftFeature {

    double              x;            // x image position (centre of top left pixel is 0.0, 0.0)
    double              y;            // y image position
    double              scale;        // Scale (pixels)
    double              orientation;  // Orientation (radians), measured clockwise from an axis pointing left [0..2pi]
    double              strength;     // The DOG value at the local maxima in DOG space
    //std::vector<double> descriptor;   // Descriptor vector
    double              descriptor[128];
    
    SiftFeature();
        
};

bool SaveSiftFeature(const SiftFeature & feature, const char * filename);
bool LoadSiftFeature(SiftFeature & feature, const char * filename);

#define TMP_TYPE float
#define G_TYPE   float
#define DOG_TYPE float


struct SiftParameters 
{
    int             n_octaves;
    int             n_levels_per_octave;
    double          max_principal_curvature_ratio; 
    double          extrema_peak_threshold;        // Peak threshold on DOG, as a fraction of image intensity range, taken per octave
    
    bool            quick;                         // False will give more small scale features
    double          smoothing_in_input_image;      // Estimated smoothing level in the input image
    
    bool            limited_orientation_range; // Limit features to those within a certain orientation range
    double          orientation_range_start;   // (radians, measured clockwise from an axis pointing left [0..2pi])
    double          orientation_range_end;     

    bool            limited_scale_range; // Limit features to those within a certain scale range
    double          scale_range_start;   
    double          scale_range_end;     

    SiftParameters() { SetDefault(); }

    void SetDefault() {
        n_octaves                     = 8;
        n_levels_per_octave           = 2;
        max_principal_curvature_ratio = 9.0;
        extrema_peak_threshold        = 0.09;
 
        quick                         = false;
	smoothing_in_input_image      = 0.5;
	
        limited_orientation_range     = false;
        orientation_range_start       = 0;
	orientation_range_end         = 0;
	
        limited_scale_range           = false;
        scale_range_start             = 0;
	scale_range_end               = 0;
    }
	
};

std::ostream & operator << (std::ostream& os, const SiftParameters & p);

class Sift 
{
    
 public:

    // Pass an image and the position (inclusive) of a rectangular window of interest
    Sift(const VW::ImageMono<unsigned char> & image, 
                  unsigned int roi_r0, unsigned int roi_c0, 
                  unsigned int roi_r1, unsigned int roi_c1,
                  SiftParameters parameters = 	SiftParameters());

    ~Sift();
    
    // The resulting features
    const std::vector<SiftFeature> & Features() const { return features_; }

    // Write a descriptor image for feature i of the vector accessed through the Features() function
    void WriteDescriptorImage(int index, const char * filename) const;

    // Make a copy of the input image with the detected features overlayed
    void MakeResultImage(VW::ImageMono<unsigned char> & result) const;
    
 private:

    bool CheckInputData();
    void AllocateMemory();
    void CalculateProcessingLevels();
    void Compute();
    void MakeInitialGaussianImage(int o, int l, double effective_sigma);
    void MakeUpsampledImage();
    void DownSample(const VW::ImageMono<G_TYPE> & in, VW::ImageMono<G_TYPE> & out);
    void GeneralDownSample(const ImageMono<G_TYPE> & in, ImageMono<G_TYPE> & out, int n_shrinks);
    
    void Smooth(const VW::ImageMono<G_TYPE> & in, 
                      VW::ImageMono<G_TYPE> & out,
                      VW::ImageMono<TMP_TYPE> & tmp, double sigma);
    
    void GeneralSmooth(const ImageMono<G_TYPE> & in, ImageMono<G_TYPE> & out,
                       ImageMono<TMP_TYPE> & tmp, double sigma);
    
    void DifferenceofImages(const VW::ImageMono<G_TYPE> & in1,
                            const VW::ImageMono<G_TYPE> & in2,
	            	          VW::ImageMono<DOG_TYPE> & out);
    
    void MakeGaussianPyramid();
    void MakeDOGPyramid();
    void FindLocalExtrema();
    void LocaliseExtrema();
    void ShowExtrema();
    void CalculateFeatures();
    void DisplayGAverageBrightness();
    void DisplayDOGAverageBrightness();
    void WriteDescriptorImage(const SiftFeature & feature, int o, int l, const char * filename) const;
    void AddWindowOffset();
    
 private:

    const VW::ImageMono<unsigned char> & image_;

    unsigned int      roi_sr; // top left pixel of region of interest [][]
    unsigned int      roi_sc;
    unsigned int      roi_fr; // (1,1) + bottom right pixel of region of interest [][]
    unsigned int      roi_fc; 

    unsigned int      roi_width;
    unsigned int      roi_height;

    std::vector<std::vector<VW::ImageMono<G_TYPE>*> > G;  // Gaussian Smoothed Images
    std::vector<std::vector<double                > > Gs; // Corresponding sigma values

    std::vector<std::vector<VW::ImageMono<DOG_TYPE>*> > DOG;  // Difference of Gaussian Images
    std::vector<std::vector<double                  > > DOGs; // Corresponding sigma values

    VW::ImageMono<TMP_TYPE> tmp;

    std::vector<SiftFeature>  features_;
    std::vector<int>          feature_level_ids_;
 
    //unsigned int start_feature_size;
    //bool         resized_feature_size;
    
    
    
    double         * DOG_extrema_x; 
    double         * DOG_extrema_y;
    double         * DOG_extrema_s; 
    unsigned int   * DOG_extrema_si;
    double         * DOG_extrema_or; // orientation in radians, clockwise from an axis pointing left
    double         * DOG_extrema_sc;

    unsigned int     DOG_extrema_n;
    unsigned int     max_extrema;
    
    SiftParameters   p;

    int              G_start_lvl_id;      // Levels in the pyramid to be processed
    int              G_end_lvl_id;        // lvl id is (octave * lvls per octave) + level in the octave
    int              DOG_start_lvl_id;
    int              DOG_end_lvl_id;
    int              Extrema_start_lvl_id;
    int              Extrema_end_lvl_id;

    double           delta_sigma;
    double           max_trace_sq_det_ratio;
    double           peak_threshold_double;
    int              peak_threshold_int;
    
    double           top_sigma; // Sigma for the first gaussian image (Gs[0][0]), whether or not it is calculated
    int              max_n_octaves;
    
};


// Simple nearest neighbour search, rather than the best-bin-first method suggested by Beis and Lowe
// Returns -1 if no distinctive match is found
// Match strength is defined as the inverse of the length of (feature_vector_1 - feature_vector_2)
int SiftMatchNN(const SiftFeature & key, const std::vector<SiftFeature> & database, 
                double distance_ratio_max = 0.8, double min_strength = 1.0);

}; // end of VW namespace

#endif // sift_h_

