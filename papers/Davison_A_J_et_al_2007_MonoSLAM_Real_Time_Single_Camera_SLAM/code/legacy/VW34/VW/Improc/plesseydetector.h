/** 
 * @file    plesseydetector.h
 * @author  ID Reid and BJ Tordoff
 * @date    02/08/2002
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
 * @class   VW::PlesseyDetector
 * @ingroup ImageProcessing
 * @author  ID Reid and BJ Tordoff
 * @date    02/08/2002
 * @brief   Interest point detection using the method of Harris/Plessey.
 *
 *                   The Plessey Corner Detector
 *              (with Harris-Stephens scoring system)
 *
 *
 * the Plessey (or Harris) corner detection system. The main
 * detector is a function object 'Plessey' which takes an image or
 * image-pyramid as input and produces a list of interest point
 * locations (ie. vector<CornerFeature>).
 *
 * To convert these locations into useful features, this list must then
 * be passed to a descriptor finder, eg:
 *
 *   vector<CornerFeature> corners;
 *   vector<SIFTFeature> siftcorners;
 *
 *   VW::PlesseyDetector(image, corners);
 *   VW::GetSIFTDescriptor(corners, image, siftcorners);
 *
 * Although this seems wasteful, the memory overhead in calculating two lists
 * is not significant, and some descriptors (such as SIFT) can produce more
 * than one feature per location.
 */




#ifndef _plesseydetector_h_
#define _plesseydetector_h_

// System includes
#include <string>
#include <vector>

// VW includes (these had better all be above this in the dependency tree)
#include <VW/image.h>
#include <VW/iobasics.h>
#include <VW/GeomObjects/normalisation.h>

// Local includes
#include "cornerfeature.h"
#include "pyramid.h"


// These default settings appear severally, so
// are set here
namespace {
  const float DEF_THRESH = 0.25f;
  const float DEF_SIGMA = 1.0f;
  const int DEF_SIZE = 5;
  const float DEF_EDGEPHOBIA = 0.8f;
}


namespace VW {




/*
 *              Plessey
 */

class PlesseyDetector {    

public:
  /** Construct with default params. */
  PlesseyDetector();

  /** Construct with parameters only. */
  PlesseyDetector(const float thresh, 
						const float sigma, 
						const int convolution_size, 
						const float ep,
						const unsigned int max_number=0);




  // Mono image constructors
  
  /** Construct and compute from a mono image, specifying all parameters. */
  PlesseyDetector(const VW::ImageMono<unsigned char> &im, 
						std::vector<CornerFeature>& corners,
						const float thresh = DEF_THRESH, 
						const float sigma = DEF_SIGMA, 
						const int convolution_size = DEF_SIZE, 
						const float ep = DEF_EDGEPHOBIA,
						const unsigned int max_number=0,
						const VW::Normalisation& C = Normalisation());


  /** Construct and compute th single best feature from a mono image, specifying all parameters. */
  PlesseyDetector(const VW::ImageMono<unsigned char> &im, 
	  CornerFeature& best_corner,
	  const float thresh = DEF_THRESH, 
	  const float sigma = DEF_SIGMA, 
	  const int convolution_size = DEF_SIZE, 
	  const float ep = DEF_EDGEPHOBIA,
						const unsigned int max_number=0,
	  const VW::Normalisation& C = Normalisation());


  /** Construct and compute from a YUV image, specifying all parameters. */
  PlesseyDetector(const ImageYUV<unsigned char> &im, 
	  std::vector<CornerFeature>& corners,
	  const float thresh = DEF_THRESH, 
	  const float sigma = DEF_SIGMA, 
	  const int size = DEF_SIZE, 
	  const float ep = DEF_EDGEPHOBIA,
						const unsigned int max_number=0,
	  const Normalisation& C = Normalisation());



  // Image pyramid constructors

  /** Construct and compute from a pyramid of mono images */
  PlesseyDetector(const Pyramid<unsigned char> &im, 
	  std::vector<CornerFeature>& corners,
	  const float thresh = DEF_THRESH, 
	  const float sigma = DEF_SIGMA, 
	  const int size = DEF_SIZE, 
	  const float ep = DEF_EDGEPHOBIA,
						const unsigned int max_number=0,
	  const Normalisation& C = Normalisation());

  PlesseyDetector(const Pyramid<float> &im, 
	  std::vector<CornerFeature>& corners,
	  const float thresh = DEF_THRESH, 
	  const float sigma = DEF_SIGMA, 
	  const int size = DEF_SIZE, 
	  const float ep = DEF_EDGEPHOBIA,
						const unsigned int max_number=0,
	  const Normalisation& C = Normalisation());

   PlesseyDetector(const Pyramid<float> &im, 
	  CornerFeature& best_corner,
	  const float thresh = DEF_THRESH, 
	  const float sigma = DEF_SIGMA, 
	  const int size = DEF_SIZE, 
	  const float ep = DEF_EDGEPHOBIA,
						const unsigned int max_number=0,
	  const Normalisation& C = Normalisation());

  ~PlesseyDetector() {}; 

  operator bool() const { return true; }

  void Compute(const ImageMono<unsigned char> &image, 
					std::vector<CornerFeature>& features,
					const Normalisation& C = Normalisation()) const;

  void Compute(const ImageMono<unsigned char> &image, 
					CornerFeature& best_corner,
					const Normalisation& C = Normalisation()) const;

  void Compute(const Pyramid<unsigned char> &image, 
					std::vector<CornerFeature>& features,
					const Normalisation& C = Normalisation()) const;

  void Compute(const Pyramid<float> &image, 
					std::vector<CornerFeature>& features,
					const Normalisation& C = Normalisation()) const;

  void Compute(const Pyramid<float> &image, 
					CornerFeature& best_corner,
					const Normalisation& C = Normalisation()) const;

  void Compute(const ImageYUV<unsigned char> &image, 
					std::vector<CornerFeature>& features,
					const Normalisation& C = Normalisation()) const;

  void SetMinStrengthThreshold(float new_threshold);
  void SetPatchSize(unsigned int new_size);
  void SetSmoothingSigma(float new_sigma);
  void SetEdgePhobia(float new_phobia);
  void SetMaxNumFeatures(unsigned int new_maximum);

  bool Verbose;

private:

  void calc_gradients(const ImageMono<unsigned char> &im,
		      ImageMono<float> &gx2arr,
		      ImageMono<float> &gy2arr,
		      ImageMono<float> &gxyarr) const;
  void calc_gradients(const ImageMono<float> &im,
		      ImageMono<float> &gx2arr,
		      ImageMono<float> &gy2arr,
		      ImageMono<float> &gxyarr) const;
  void calc_gradients(const ImageYUV<unsigned char> &im,
		      ImageMono<float> &gx2arr,
		      ImageMono<float> &gy2arr,
		      ImageMono<float> &gxyarr) const;
  void compute_corner_strength(const ImageMono<float> &gradx2,
			       const ImageMono<float> &grady2,
			       const ImageMono<float> &gradxy,
			       ImageMono<float> &corn_strength ) const;
  void compute_best_corner(const ImageMono<float> &gradx2,
			   const ImageMono<float> &grady2,
			   const ImageMono<float> &gradxy,
			   CornerFeature& best_feature) const;
  void compute_corners(const ImageMono<float> &strength, 
							  const float score_threshold,
							  const float raw_thresh, 
							  const float image_scale,
							  std::vector<CornerFeature>& features) const;

  // parameters for computation
  float _strength_threshold;
  float _smoothing_sigma;
  unsigned int _patch_size;
  float _edge_phobia;
  unsigned int _max_features;
};



}; // end of VW namespace











/// INLINES ///
inline VW::PlesseyDetector::
PlesseyDetector()
 : Verbose(false),
	_strength_threshold(DEF_THRESH), 
	_smoothing_sigma(DEF_SIGMA),
	_patch_size(DEF_SIZE), 
	_edge_phobia(DEF_EDGEPHOBIA),
	_max_features(0)
{
}

inline VW::PlesseyDetector::
PlesseyDetector(const float thresh, 
					 const float sigma, 
					 const int convolution_size, 
					 const float ep,
					 const unsigned int max_number)
 : Verbose(false),
	_strength_threshold(thresh), 
	_smoothing_sigma(sigma),
	_patch_size(convolution_size), 
	_edge_phobia(ep),
	_max_features(max_number)
{
}

inline VW::PlesseyDetector::
PlesseyDetector(const ImageMono<unsigned char> &im, 
					 std::vector<CornerFeature>& corners,
					 const float thresh, 
					 const float sigma, 
					 const int size, 
					 const float ep,
					 const unsigned int max_number,
					 const VW::Normalisation& C)
  : Verbose(false),
	_strength_threshold(thresh), 
	_smoothing_sigma(sigma),
	_patch_size(size), 
	_edge_phobia(ep),
	_max_features(max_number)
{
  Compute(im, corners, C);
}


inline VW::PlesseyDetector::PlesseyDetector(const VW::ImageMono<unsigned char> &im, 
			    CornerFeature& best_corner,
			    const float thresh, 
			    const float sigma, 
			    const int convolution_size, 
			    const float ep,
														  const unsigned int max_number,
			    const VW::Normalisation& C) 
  : Verbose(false),
    _strength_threshold(thresh), 
	 _smoothing_sigma(sigma),
    _patch_size(convolution_size), 
	 _edge_phobia(ep),
	 _max_features(max_number)
{
  Compute(im, best_corner, C);
}

inline VW::PlesseyDetector::PlesseyDetector(const Pyramid<unsigned char> &im, 
			    std::vector<CornerFeature>& corners,
			    float thresh, float sigma, 
			    int size, float ep,
														  const unsigned int max_number,
			    const VW::Normalisation& C)
  : Verbose(false),
  _strength_threshold(thresh), 
  _smoothing_sigma(sigma),
  _patch_size(size), 
  _edge_phobia(ep),
  _max_features(max_number)
{
  Compute(im, corners, C);
}

inline VW::PlesseyDetector::PlesseyDetector(const Pyramid<float> &im, 
			    std::vector<CornerFeature>& corners,
			    float thresh, float sigma, 
			    int size, float ep,
															  const unsigned int max_number,
		    const VW::Normalisation& C)
  : Verbose(false),
  _strength_threshold(thresh), 
  _smoothing_sigma(sigma),
  _patch_size(size), 
  _edge_phobia(ep),
  _max_features(max_number)
{
  Compute(im, corners, C);
}

inline VW::PlesseyDetector::PlesseyDetector(const Pyramid<float> &im, 
			    CornerFeature& best_corner,
			    float thresh, float sigma, 
			    int size, float ep,
															  const unsigned int max_number,
		    const VW::Normalisation& C)
  : Verbose(false),
  _strength_threshold(thresh), 
  _smoothing_sigma(sigma),
  _patch_size(size), 
  _edge_phobia(ep),
  _max_features(max_number)
{
  Compute(im, best_corner, C);
}

inline VW::PlesseyDetector::PlesseyDetector(const ImageYUV<unsigned char> &im, 
			    std::vector<CornerFeature>& corners,
			    float thresh, float sigma, 
			    int size, float ep,
															  const unsigned int max_number,
		    const VW::Normalisation& C)
  : Verbose(false),
	 _strength_threshold(thresh), 
	 _smoothing_sigma(sigma),
	 _patch_size(size), 
	 _edge_phobia(ep),
	 _max_features(max_number)
{
  Compute(im, corners, C);
}

inline void VW::PlesseyDetector::
SetMinStrengthThreshold(float new_threshold)
{
  _strength_threshold = new_threshold;
}

inline void VW::PlesseyDetector::
SetPatchSize(unsigned int new_size)
{
  _patch_size = new_size;
}

inline void VW::PlesseyDetector::
SetMaxNumFeatures(unsigned int new_maximum)
{
  _max_features = new_maximum;
}

inline void VW::PlesseyDetector::
SetSmoothingSigma(float new_sigma)
{
  _smoothing_sigma = new_sigma;
}

inline void VW::PlesseyDetector::
SetEdgePhobia(float new_phobia)
{
  _edge_phobia = new_phobia;
}


#endif /* _plessey_h_ */
