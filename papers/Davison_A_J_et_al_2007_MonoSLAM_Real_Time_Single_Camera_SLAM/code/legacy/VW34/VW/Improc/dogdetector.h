/** 
 * @file    dogdetector.h
 * @author  ND Molton
 * @date    15/10/2003
 *
 * Modified by BJT to seperate detection from description
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
 * @class   VW::DogDetector
 * @ingroup ImageProcessing
 * @author  ND Molton
 * @date    15/10/2003
 * @brief   Interest point detection using David Lowe's extrema of 
 * Difference-of-Gaussian (DOG) operator.
 *
 * #include <VW/Improc/dogdetector.h>
 * 
 * This is half of an implementation of David Lowe's SIFT (Scale invariant feature transform)
 *  feature detector/descriptor (the other half is GetSIFTDescriptor). The implementation is based on the
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
 */


// Notes:
// 
//  NDM: Not sure if images should be smoothed as part of the subsampling 
// process. Scales tend to vary in a quantised, rather than continuous way
//  
//

#ifndef _dog_detector_h_
#define _dog_detector_h_

#include <vector>
#include <ostream>

// #include <VW/Improc/pyramid.h>
// #include <VW/Improc/cornerfeature.h>

namespace VW {

  template <class T> class Pyramid;
  class CornerFeature;


struct DOGDetectorParams 
{
  /** Maximum ratio between maximum and minimum curvatures. */
  float max_curvature_ratio;
  unsigned int max_features;
  
  DOGDetectorParams() { SetDefault(); }
  
  void SetDefault() {
    max_curvature_ratio = 9.0;
    max_features = 0;
  }
	
};


class DOGDetector 
{
    
 public:

  /** Construct and set parameters only */
  DOGDetector(DOGDetectorParams parameters = DOGDetectorParams());

  /** Construct and detect corners */
  DOGDetector(const VW::Pyramid<float>& pyramid,
				  std::vector<CornerFeature>& features,
				  DOGDetectorParams parameters = DOGDetectorParams());

  /** Construct and detect the single strongest feature */
  DOGDetector(const VW::Pyramid<float>& pyramid,
				  CornerFeature& best_feature,
				  DOGDetectorParams parameters = DOGDetectorParams());

  /** Construct and detect the single strongest feature */
  DOGDetector(const VW::ImageMono<unsigned char>& image,
				  CornerFeature& best_feature,
				  DOGDetectorParams parameters = DOGDetectorParams());

  ~DOGDetector();

  /** Compute features from a pyramid using pre-set params */
  void Compute(const VW::Pyramid<float>& pyramid,
					std::vector<CornerFeature>& features);

  /** Compute single best feature from a pyramid using pre-set params */
  void Compute(const VW::Pyramid<float>& pyramid,
					CornerFeature& best_feature);

  /** Compute single best feature from a single image using pre-set params */
  void Compute(const VW::ImageMono<unsigned char>& image,
					CornerFeature& best_feature);

  void SetParams(const DOGDetectorParams& new_params);
  /** Set the maximum number of features (note that a high curvature 
   * threshold may cause many fewer than this to be detected).
   */
  void SetMaxNumFeatures(unsigned int new_maximum);
  /** Curvature threshold controls the maximum ratio between the maximum and
   * minimum curvatures for a location (a corner has high curvature 
   * in all directions).
   */
  void SetCurvatureThreshold(float max_curvature_ratio);


 private:

  DOGDetectorParams _params;

};


std::ostream & operator << (std::ostream& os, const DOGDetectorParams & p);

}; // end of VW namespace





// INLINES

inline VW::DOGDetector::DOGDetector(VW::DOGDetectorParams parameters)
    : _params(parameters)
{
}

// Construct and detect corners
inline VW::DOGDetector::DOGDetector(const VW::Pyramid<float>& pyramid,
				    std::vector<CornerFeature>& features,
				    VW::DOGDetectorParams parameters)
  : _params(parameters)
{
  Compute(pyramid,features);
}

inline VW::DOGDetector::DOGDetector(const VW::Pyramid<float>& pyramid,
				    CornerFeature& best_feature,
				    DOGDetectorParams parameters)
  : _params(parameters)
{
  Compute(pyramid,best_feature);
}

inline VW::DOGDetector::DOGDetector(const VW::ImageMono<unsigned char>& image,
				    CornerFeature& best_feature,
				    DOGDetectorParams parameters)
  : _params(parameters)
{
  Compute(image,best_feature);
}

inline VW::DOGDetector::~DOGDetector() 
{
};

inline void VW::DOGDetector::SetParams(const DOGDetectorParams& new_params)
{
  _params = new_params;
}

inline void VW::DOGDetector::
SetMaxNumFeatures(unsigned int new_maximum)
{
  _params.max_features = new_maximum;
}

inline void VW::DOGDetector::
SetCurvatureThreshold(float max_curvature_ratio)
{
  _params.max_curvature_ratio = max_curvature_ratio;
}


#endif // _dog_detector_h_

