/** 
 * @file   canny.h
 * @author DW Murray and ID Reid
 * @date   02/08/2002
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
 * @class   VW::Canny
 * @ingroup ImageProcessing
 * @author  DW Murray and ID Reid
 * @date    02/08/2002
 * @brief   Class for computing image edges using the method of Canny.
 *
 * Modified 16/6/2004 BJT: Add YUV support
 */

#ifndef _canny_h_
#define _canny_h_

#include "edgel.h"
#include <VW/Image/imagemono.h>
#include <VW/Image/imageyuv.h>

namespace VW {


class Canny {
  
 public:

  /** Create the Canny object with specified parameters, but do not compute
   * anything yet.
   */
  Canny(float _sigma=1.0, int _size=5,
	float _lo=10.0, float _hi=20.0, 
	int _len=20);


  /** Create the Canny object with specified parameters, and compute
   * the edge data for an image.
   */
  Canny(const ImageMono<unsigned char>& im, EdgeData& edat,
	float _sigma=1.0, int _size=5,
	float _lo=10.0, float _hi=20.0, int _len=20);
    
  /** Create the Canny object with specified parameters, and compute
   * the edge data for an image.
   */
  Canny(const ImageYUV<unsigned char>& im, EdgeData& edat,
	float _sigma=1.0, int _size=5,
	float _lo=10.0, float _hi=20.0, int _len=20);
    
  /** Compute the edge data for an image.
   */
  void Compute(const ImageMono<unsigned char>& im, EdgeData& edat) const;

  /** Compute the edge data for the Y channel of a YUV image.
   */
  void Compute(const ImageYUV<unsigned char>& im, EdgeData& edat) const;

  operator bool() { return true; }

  /** Set a new value for the standard deviation of the Gaussian kernel. */
  void SetGaussSigma(float val);
  /** Set a new value for the size of the Gaussian kernel. */
  void SetGaussSize(int val);
  /** Set a new value for the high water mark used for hysteresis. */
  void SetHiThreshold(float val);
  /** Set a new value for the low water mark used for hysteresis. */
  void SetLoThreshold(float val);
  /** Set a new value for the minimum edge length. */
  void SetLenThreshold(int val);

 private:

  float gauss_sigma;
  int   gauss_size;
  float lo_threshold;
  float hi_threshold;
  int   len_threshold;

  // Internal func to get gradients
  void CalcGradients (ImageMono<float>& im,
		       ImageMono<float>& gx,
		       ImageMono<float>& gy,
		       ImageMono<float>& grad) const;

  void NonMaximalSuppression(ImageMono<float>& grad, 
			      ImageMono<float>& gx, 
			      ImageMono<float>& gy,
			      float lo_thresh_sq, 
			      ImageMono<Edgel *>& emap,
			      std::vector<Edgel *>& edgels) const;

  Edgel* LinkLeft(Edgel *e, const ImageMono<Edgel *>& emap) const;
  Edgel* LinkRight(Edgel *e, const ImageMono<Edgel *>& emap) const;

  void Linker(const ImageMono<Edgel *>& emap,
	      std::vector<Edgel *>& edgels, 
	      const float high_thresh_sq, const int length_thresh,
	      std::vector<EdgeString *>& strings) const;

};

}; // end namespace VW










// Inlines

/**
 * @param sigma the standard deviation of the Gaussian smoothing kernel
 * @param size the size of the convolution kernel
 * @param hi the "high water mark" edge strength kept in the hysteresis step 
 * @param lo the "low water mark" edge strength kept in the hysteresis step 
 * @param len the minimum edge length that is retained
 */
inline
VW::Canny::Canny(float _sigma, int _size,
		 float _lo, float _hi, int _len) 
    : gauss_sigma(_sigma), 
      gauss_size(_size), 
      lo_threshold(_lo), 
      hi_threshold(_hi), 
      len_threshold(_len) 
{
}

/**
 * @param im the image in which edges will be found
 * @param edat the resulting edge data
 * @param sigma the standard deviation of the Gaussian smoothing kernel
 * @param size the size of the convolution kernel
 * @param hi the "high water mark" edge strength kept in the hysteresis step 
 * @param lo the "low water mark" edge strength kept in the hysteresis step 
 * @param len the minimum edge length that is retained
 */
inline
VW::Canny::Canny(const ImageMono<unsigned char>& im, 
		 EdgeData& edat,
		 float _sigma, int _size,
		 float _lo, float _hi, 
		 int _len) 
  : gauss_sigma(_sigma), 
    gauss_size(_size), 
    lo_threshold(_lo), 
    hi_threshold(_hi), 
    len_threshold(_len) 
{
    Compute(im, edat);
}


/**
 * @param im the image in which edges will be found
 * @param edat the resulting edge data
 * @param sigma the standard deviation of the Gaussian smoothing kernel
 * @param size the size of the convolution kernel
 * @param hi the "high water mark" edge strength kept in the hysteresis step 
 * @param lo the "low water mark" edge strength kept in the hysteresis step 
 * @param len the minimum edge length that is retained
 */
inline
VW::Canny::Canny(const ImageYUV<unsigned char>& im, 
		 EdgeData& edat,
		 float _sigma, int _size,
		 float _lo, float _hi, 
		 int _len) 
  : gauss_sigma(_sigma), 
    gauss_size(_size), 
    lo_threshold(_lo), 
    hi_threshold(_hi), 
    len_threshold(_len) 
{
    Compute(im, edat);
}


inline void 
VW::Canny::SetGaussSigma(float val) 
{ 
  gauss_sigma=val; 
}

inline void 
VW::Canny::SetGaussSize(int val) 
{ 
  gauss_size=val; 
}

inline void 
VW::Canny::SetHiThreshold(float val) 
{ 
  hi_threshold=val; 
}

inline void 
VW::Canny::SetLoThreshold(float val) 
{ 
  lo_threshold=val; 
}

inline void 
VW::Canny::SetLenThreshold(int val) 
{ 
  len_threshold=val; 
}

#endif /* _canny_h_ */
