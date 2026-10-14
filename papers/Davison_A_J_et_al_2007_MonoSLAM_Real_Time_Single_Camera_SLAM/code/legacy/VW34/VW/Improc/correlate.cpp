/** 
 * @file   correlate.cpp
 * @author BJ Tordoff
 * @date   05/04/2004
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

#include "correlate.h"
#include "integralimage.h"
#include "integralimagesquared.h"

#include <iostream>
#include <math.h> // for sqrt


namespace VW {

template<class Type>
void Correlate::Compute(const VW::ImagePatch& patch, 
			const VW::ImageMono<Type>& image, 
			VW::ImageMono<float>& result) const
{
  assert(image.ROI().Width() >= patch.GetWidth());
  assert(image.ROI().Height() >= patch.GetHeight());

  static const float PRECISION = 1.0f; // to remove flat regions

  const int hw = patch.GetWidth()/2;
  const int hh = patch.GetHeight()/2;
  const unsigned int N = patch.GetWidth()*patch.GetHeight();
  const float mean_P = patch.GetMean();
  const float ssd_P = patch.GetNormalisedSumSquared();

  // Integral and Intergal^2 images are used for mean and variance
  VW::IntegralImage<unsigned int>         integral_image(image);
  VW::IntegralImageSquared<unsigned int>  integral_image2(image);


  // Alloc the result at the right size
  if (result.IsAlloced()) result.FreeImageData();
  result.AllocImageData(image.GetWidth(),
			image.GetHeight());
  //result.Clear(VW::PixelMono<unsigned char>(0));
  result.SetROI(image.ROI());
  result.ROI().ShrinkWidth(hw);
  result.ROI().ShrinkHeight(hh);

  const unsigned int c0 = result.ROI().Left();
  const unsigned int c1 = result.ROI().Right();
  const unsigned int r0 = result.ROI().Top();
  const unsigned int r1 = result.ROI().Bottom();

  for (unsigned int row=r0; row<r1; row++) {
    VW::PixelMono<float>* result_ptr = result.GetROIRowStart(row);
    const VW::PixelMono<Type>* im_ptr = image.GetRowStart(row) + c0;
    for (unsigned int col=c0; col<c1; col++, result_ptr++, im_ptr++) {

      // At each location, scan the patch
      const float sum_i = float(integral_image.GetRegionSum(col-hw, row-hh, col+hw, row+hh));
      const float sum_i2 = float(integral_image2.GetRegionSumSquared(col-hw, row-hh, 
							       col+hw, row+hh));
      float sum_ip = 0;
      for (int crow=-hh; crow <= hh; crow++) {
	const VW::PixelMono<Type>* cim_ptr = im_ptr + image.GetWidth()*crow - hw;
	std::vector<float>::const_iterator p_ptr = patch.GetRowStart(crow+hh);
	for (int ccol=-hw; ccol <= hw; ccol++,cim_ptr++,p_ptr++) {
	  sum_ip += cim_ptr->y * (*p_ptr);
	}
      }
      const float numerator = sum_ip - sum_i*mean_P;
      const float denominator2 = (sum_i2 - sum_i*sum_i/N) * ssd_P;
      if ((denominator2 < PRECISION) || ((numerator*numerator) < PRECISION)) {
	result_ptr->y = 0.0f;
      } else {
	result_ptr->y = numerator/sqrt(denominator2);
      }
    }
  }  
}


template<class Type>
void Correlate::Compute(const VW::ImagePatch& patch, 
			const VW::ImageMono<Type>& image, 
			const VW::ROI& roi,
			VW::ImagePatch& result) const
{
  assert(roi.Width() >= patch.GetWidth());
  assert(roi.Height() >= patch.GetHeight());

  // Also check that the roi lies in a valid part of the image
  assert(roi.Left() >= image.ROI().Left());
  assert(roi.Right() <= image.ROI().Right());
  assert(roi.Top() >= image.ROI().Top());
  assert(roi.Bottom() <= image.ROI().Bottom());

  static const float PRECISION = 1.0f; // to remove flat regions

  const int hw = patch.GetWidth()/2;
  const int hh = patch.GetHeight()/2;
  const unsigned int N = patch.GetWidth()*patch.GetHeight();
  const float mean_P = patch.GetMean();
  const float ssd_P = patch.GetNormalisedSumSquared();

  const unsigned int c0 = roi.Left() + hw;
  const unsigned int c1 = roi.Right() - hw;
  const unsigned int r0 = roi.Top() + hh;
  const unsigned int r1 = roi.Bottom() - hh;

  // Alloc the result at the right size
  result.Resize(c1-c0, r1-r0);


  // Integral and Intergal^2 images are used for mean and variance
  VW::IntegralImage<unsigned int>         integral_image(image);
  VW::IntegralImageSquared<unsigned int>  integral_image2(image);


  std::vector<float>::iterator result_ptr = result.GetRowStart(0);
  for (unsigned int row=r0; row<r1; row++) {
    const VW::PixelMono<Type>* im_ptr = image.GetRowStart(row) + c0;
    for (unsigned int col=c0; col<c1; col++, result_ptr++, im_ptr++) {

      // At each location, scan the patch
      const float sum_i = float(integral_image.GetRegionSum(col-hw, row-hh, 
						      col+hw, row+hh));
      const float sum_i2 = float(integral_image2.GetRegionSumSquared(col-hw, row-hh, 
							       col+hw, row+hh));
      float sum_ip = 0;
      for (int crow=-hh; crow <= hh; crow++) {
	const VW::PixelMono<Type>* cim_ptr = im_ptr + image.GetWidth()*crow - hw;
	std::vector<float>::const_iterator p_ptr = patch.GetRowStart(crow+hh);
	for (int ccol=-hw; ccol <= hw; ccol++,cim_ptr++,p_ptr++) {
	  sum_ip += cim_ptr->y * (*p_ptr);
	}
      }
      const float numerator = sum_ip - sum_i*mean_P;
      const float denominator2 = (sum_i2 - sum_i*sum_i/N) * ssd_P;
      if ((denominator2 < PRECISION) || ((numerator*numerator) < PRECISION)) {
	*result_ptr = 0.0f;
      } else {
	*result_ptr = numerator/sqrt(denominator2);
      }
    }
  }  
}

template<class Type>
void Correlate::Compute(const VW::ImagePatch& patch, 
			const VW::ImageYUV<Type>& image, 
			const VW::ROI& roi,
			VW::ImagePatch& result) const
{
  assert(roi.Width() >= patch.GetWidth());
  assert(roi.Height() >= patch.GetHeight());

  // Also check that the roi lies in a valid part of the image
  assert(roi.Left() >= image.ROI().Left());
  assert(roi.Right() <= image.ROI().Right());
  assert(roi.Top() >= image.ROI().Top());
  assert(roi.Bottom() <= image.ROI().Bottom());

  static const float PRECISION = 1.0f; // to remove flat regions

  const int hw = patch.GetWidth()/2;
  const int hh = patch.GetHeight()/2;
  const unsigned int N = patch.GetWidth()*patch.GetHeight();
  const float mean_P = patch.GetMean();
  const float ssd_P = patch.GetNormalisedSumSquared();

  const unsigned int c0 = roi.Left() + hw;
  const unsigned int c1 = roi.Right() - hw;
  const unsigned int r0 = roi.Top() + hh;
  const unsigned int r1 = roi.Bottom() - hh;

  // Alloc the result at the right size
  result.Resize(c1-c0, r1-r0);

  // Integral and Intergal^2 images are used for mean and variance
  VW::IntegralImage<unsigned int>         integral_image(image,roi);
  VW::IntegralImageSquared<unsigned int>  integral_image2(image,roi);


  std::vector<float>::iterator result_ptr = result.GetRowStart(0);
  for (unsigned int row=r0; row<r1; row++) {
    const VW::PixelYUV<Type>* im_ptr = image.GetRowStart(row) + c0;
    for (unsigned int col=c0; col<c1; col++, result_ptr++, im_ptr++) {

      // At each location, scan the patch
      const float sum_i = float(integral_image.GetRegionSum(col-hw, row-hh, 
						      col+hw, row+hh));
      const float sum_i2 = float(integral_image2.GetRegionSumSquared(col-hw, row-hh, 
							       col+hw, row+hh));
      float sum_ip = 0;
      for (int crow=-hh; crow <= hh; crow++) {
	const VW::PixelYUV<Type>* cim_ptr = im_ptr + image.GetWidth()*crow - hw;
	std::vector<float>::const_iterator p_ptr = patch.GetRowStart(crow+hh);
	for (int ccol=-hw; ccol <= hw; ccol++,cim_ptr++,p_ptr++) {
	  sum_ip += cim_ptr->y * (*p_ptr);
	}
      }
      const float numerator = sum_ip - sum_i*mean_P;
      const float denominator2 = (sum_i2 - sum_i*sum_i/N) * ssd_P;
      if ((denominator2 < PRECISION) || ((numerator*numerator) < PRECISION)) {
	*result_ptr = 0.0f;
      } else {
	*result_ptr = numerator/sqrt(denominator2);
      }
    }
  }  
}



  // Instantiations

  template Correlate::Correlate(const VW::ImagePatch&, 
				const VW::ImageMono<unsigned char>&, 
				VW::ImageMono<float>&);

  template Correlate::Correlate(const VW::ImagePatch&, 
				const VW::ImageMono<unsigned char>&, 
				const VW::ROI&,
				VW::ImagePatch&);

  template Correlate::Correlate(const VW::ImagePatch&, 
				const VW::ImageYUV<unsigned char>&, 
				const VW::ROI&,
				VW::ImagePatch&);

}; // End namespace
