/** 
 * @file    shitomasidetector.h
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
/**
 * @class   VW::ShiTomasiDetector
 * @ingroup ImageProcessing
 * @author  BJ Tordoff & AJ Davison
 * @date    29/11/2004
 * @brief   Interest point detection using the method of Shi & Tomasi.
 *
 * This detector can be used to either find the single strongest feature in
 * the region of interest (image.ROI()), or the set of all maxima.
 */
#ifndef _shitomasi_detector_h_
#define _shitomasi_detector_h_

#include <vector>

namespace VW {

  template <class T> class Pyramid;
  template <class T> class ImageMono;
  class CornerFeature;


class ShiTomasiDetector 
{
    
 public:

  /** Construct blank for subsequent use of Compute(...) 
   * @param strength_threshold is the minimum corner strength (in range 0 to 1)
   * @param patchsize is the size of the correlation patch (should be odd)
   * @param max_number is the maximum number of features that will be stored (0 indicates no limit)
   */
  ShiTomasiDetector(const float strength_threshold=0.1,
						  const unsigned int patchsize=5,
						  const unsigned int max_number=0);

  /** Construct and find features */
  ShiTomasiDetector(const VW::ImageMono<unsigned char>& image,
						  std::vector<CornerFeature>& features,
						  const float strength_threshold=0.1,
						  const unsigned int patchsize=5,
						  const unsigned int max_number=0);

  // Construct and detect corners
  ShiTomasiDetector(const VW::Pyramid<float>& pyramid,
						  std::vector<CornerFeature>& features,
						  const float strength_threshold=0.1,
						  const unsigned int patchsize=5,
						  const unsigned int max_number=0);

  /** Construct and find the singe strongest feature */
  ShiTomasiDetector(const VW::ImageMono<unsigned char>& image,
						  CornerFeature& best_feature,
						  const float strength_threshold=0.1,
						  const unsigned int patchsize=5);

  /** Construct and find the singe strongest feature */
  ShiTomasiDetector(const VW::Pyramid<float>& pyramid,
						  CornerFeature& best_feature,
						  const float strength_threshold=0.1,
						  const unsigned int patchsize=5);

  void SetMinStrengthThreshold(float new_threshold);
  void SetPatchSize(unsigned int new_size);
  void SetMaxNumFeatures(unsigned int new_maximum);




  /** Compute features at each level of a pyramid. */
  void Compute(const VW::Pyramid<float>& pyramid,
					std::vector<CornerFeature>& features);

  /** Compute features in a single image. */
  template <class T>
  void Compute(const VW::ImageMono<T>& image,
					std::vector<CornerFeature>& features);

  /** Compute the single strongest feature in a pyramid. */
  void Compute(const VW::Pyramid<float>& pyramid,
					CornerFeature& best_feature);

  /** Compute the single strongest feature in an image. */
  template <class T>
  void Compute(const VW::ImageMono<T>& image,
					CornerFeature& best_feature);
  

 private:
  // Actually compute features given a particular image scale
  template <class T>
  void _DoCompute(const VW::ImageMono<T>& image,
						std::vector<CornerFeature>& features,
						const float image_Scale);

  
  unsigned int _patchsize;
  float _threshold;
  unsigned int _max_features;
};


} // End namespace VW;








inline VW::ShiTomasiDetector::
ShiTomasiDetector(const float strength_threshold,
						const unsigned int patchsize,
						const unsigned int max_number)
  : _patchsize(patchsize),
    _threshold(strength_threshold),
    _max_features(max_number)
{
};

inline VW::ShiTomasiDetector::
ShiTomasiDetector(const VW::ImageMono<unsigned char>& image,
						std::vector<CornerFeature>& features,
						const float strength_threshold,
						const unsigned int patchsize,
						const unsigned int max_number)
: _patchsize(patchsize),
  _threshold(strength_threshold),
  _max_features(max_number)
{
  Compute(image,features);
}

inline VW::ShiTomasiDetector::
ShiTomasiDetector(const VW::Pyramid<float>& pyramid,
						std::vector<CornerFeature>& features,
						const float strength_threshold,
						const unsigned int patchsize,
						const unsigned int max_number)
  : _patchsize(patchsize),
    _threshold(strength_threshold),
    _max_features(max_number)
{
  Compute(pyramid,features);
}

inline VW::ShiTomasiDetector::
ShiTomasiDetector(const VW::Pyramid<float>& pyramid,
						CornerFeature& best_feature,
						const float strength_threshold,
						const unsigned int patchsize)
  : _patchsize(patchsize),
    _threshold(strength_threshold),
    _max_features(0)
{
  Compute(pyramid,best_feature);
}

inline VW::ShiTomasiDetector::
ShiTomasiDetector(const VW::ImageMono<unsigned char>& image,
						CornerFeature& best_feature,
						const float strength_threshold,
						const unsigned int patchsize)
  : _patchsize(patchsize),
    _threshold(strength_threshold),
    _max_features(0)
{
  Compute(image,best_feature);
}

template <class T>
inline void VW::ShiTomasiDetector::
Compute(const VW::ImageMono<T>& image,
		  std::vector<CornerFeature>& features)
{
  features.clear();
 _DoCompute(image,features,1.0f);
}

inline void VW::ShiTomasiDetector::
SetMinStrengthThreshold(float new_threshold)
{
  _threshold = new_threshold;
}

inline void VW::ShiTomasiDetector::
SetPatchSize(unsigned int new_size)
{
  _patchsize = new_size;
}

inline void VW::ShiTomasiDetector::
SetMaxNumFeatures(unsigned int new_maximum)
{
  _max_features = new_maximum;
}

#endif
