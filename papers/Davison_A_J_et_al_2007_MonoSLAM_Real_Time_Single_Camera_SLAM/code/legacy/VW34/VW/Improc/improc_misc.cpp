/** 
 * @file   improc_misc.cpp
 * @author JGH Knight
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


#ifndef _improc_misc_cc_
#define _improc_misc_cc_

#include <cmath>
#include <cassert>
#include <VW/MathBits/nan.h>

#include "improc_misc.h"


/**
 * CorrelationScoreNMSD (patch_address, its_width, its_height,
 *                       image_address, its_width, its_height,
 *                       correlate_on_image_at_x, ..._at_y)
 *
 * Correlate image patch at patch_address (which may be a whole image)
 * with the region under it at the specified point on the image at
 * image_address, using normalised mean-squared-difference correlation
 * method.  Handles parts of the patch being off the image.
 * x and y specify the centre of the patch on the image.  Returns -1.0
 * if point specified is so far off image the patch does not overlap
 * with it.
 */

template <class T>
double VW::Misc::CorrelationScoreNMSD (T *ppatch,
				       unsigned patch_width, unsigned patch_height,
				       T *pimage,
				       unsigned image_width, unsigned image_height,
				       int x, int y)
{
  assert (ppatch != 0);
  assert (pimage != 0);

  // Get bounds
  int xstart = x - patch_width/2;
  int ystart = y - patch_height/2;

  // With n we count number of comparisons.  With i we step through patch
  unsigned int n = 0, i = 0;
  double sum_squared_diff = 0.0;
  for (int v = ystart; v < ystart + signed (patch_height); v++) {
    for (int u = xstart; u < xstart + signed (patch_width); u++, i++) {
      // Avoid out-of-bounds
      if (v < 0 || v >= signed (image_height)) continue;
      if (u < 0 || u >= signed (image_width)) continue;

      T g1 = pimage [v*image_width + u];
      T g2 = ppatch [i];
      // NaN is a legitimate value in VW meaning no data -- check for it
	  if (VW::IsNaN(g1) || VW::IsNaN(g2)) continue;

      sum_squared_diff += (g1 - g2) * (g1 - g2);
      n++;
    }
  }
  if (n == 0) return -1.0;

  // Division by 255*255 is to give 1 for exact correlation for a typical
  // mono image which was (originally) an array of unsigned chars.
  // If the pixel value range is different, this should be taken into account
  // by the calling function.
  return ((sum_squared_diff / n) / (255.0 * 255.0));
}

typedef unsigned char uchar;

// Instantiation with standard types
namespace VW {
namespace Misc {
template
double CorrelationScoreNMSD<uchar> (uchar*, unsigned, unsigned, uchar*,
				    unsigned, unsigned, int, int);
template
double CorrelationScoreNMSD<int> (int*, unsigned, unsigned, int*,
				  unsigned, unsigned, int, int);
template
double CorrelationScoreNMSD<float> (float*, unsigned, unsigned, float*,
				    unsigned, unsigned, int, int);
template
double CorrelationScoreNMSD<double> (double*, unsigned, unsigned, double*,
				     unsigned, unsigned, int, int);
}
}


/**
 * CorrelationScoreAJD (patch_address, its_width, its_height,
 *                      image_address, its_width, its_height,
 *                      correlate_on_image_at_x, ..._at_y)
 *
 * Correlate image patch at patch_address (which may be a whole image)
 * with the region under it at the specified point on the image at
 * image_address, according to Andy Davison's mean-no.-s.d.s-from-mean
 * method.  Handles parts of the patch being off the image.
 * x and y specify the centre of the patch on the image.  Returns -1.0
 * if point specified is so far off image the patch does not overlap
 * with it.
 */

template <class T>
double VW::Misc::CorrelationScoreAJD (T *ppatch,
				      unsigned patch_width, unsigned patch_height,
				      T *pimage,
				      unsigned image_width, unsigned image_height,
				      int x, int y)
{
  assert (ppatch != 0);
  assert (pimage != 0);

  // Get bounds
  int xstart = x - patch_width/2;
  int ystart = y - patch_height/2;

  // With n we count number of comparisons.  i steps through patch.
  unsigned int n = 0, i = 0;
  double sum1 = 0.0, sum2 = 0.0, sum1x2 = 0.0, sum1sq = 0.0, sum2sq = 0.0;
  for (int v = ystart; v < ystart + signed (patch_height); v++) {
    for (int u = xstart; u < xstart + signed (patch_width); u++, i++) {
      // Avoid out-of-bounds
      if (v < 0 || v >= signed (image_height)) continue;
      if (u < 0 || u >= signed (image_width)) continue;

      T g1 = pimage [v*image_width + u];
      T g2 = ppatch [i];
      // NaN is a legitimate value in VW meaning no data -- check for it
	  if (VW::IsNaN(g1) || VW::IsNaN(g2)) continue;

      sum1 += g1;
      sum2 += g2;
      sum1x2 += g1 * g2;
      sum1sq += g1 * g1;
      sum2sq += g2 * g2;

      n++;
    }
  }
  if (n == 0) return -1.0;

  double mean1  = sum1 / n;
  double mean2  = sum2 / n;
  double var1   = (sum1sq / n) - (mean1 * mean1);
  double var2   = (sum2sq / n) - (mean2 * mean2);
  double sigma1 = sqrt (var1);
  double sigma2 = sqrt (var2);

  // Get round division by zero
  if (sigma1 == 0.0) {
    if (sigma2 == 0.0) return 0.0;
    else return 1.0;
  }
  if (sigma2 == 0.0) return 1.0;

  // Sorry, I don't know what this is doing!
  double k = (mean1 / sigma1) - (mean2 / sigma2);
  double C = (sum1sq / var1) + (sum2sq / var2) + (n * k * k) -
    (sum1x2 * 2.0 / (sigma1 * sigma2)) -
    (sum1 * 2.0 * k / sigma1) + (sum2 * 2.0 * k / sigma2);

  // Apparently this is the mean square no. of s.d. from the mean of the pixel values
  return fabs (C / n);
}

// Instantiation with standard types
namespace VW {
namespace Misc {

template
double CorrelationScoreAJD<uchar> (uchar*, unsigned, unsigned, uchar*,
				   unsigned, unsigned, int, int);
template
double CorrelationScoreAJD<int> (int*, unsigned, unsigned, int*,
				 unsigned, unsigned, int, int);
template
double CorrelationScoreAJD<float> (float*, unsigned, unsigned, float*,
				   unsigned, unsigned, int, int);
template
double CorrelationScoreAJD<double> (double*, unsigned, unsigned, double*,
				    unsigned, unsigned, int, int);

}
}


#endif // #ifndef _improc_misc_cc_
