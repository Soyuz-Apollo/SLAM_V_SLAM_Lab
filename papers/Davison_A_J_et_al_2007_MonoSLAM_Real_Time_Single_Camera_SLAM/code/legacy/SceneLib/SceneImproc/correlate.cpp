/*  SceneLib: software for real-time SLAM

    correlate.cpp
    Copyright (C) 2005 University of Oxford

    Author
    Andrew Davison
    ajd@doc.ic.ac.uk

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "correlate.h"

/**
Normalise sum-of-squared-difference between two patches. Assumes that the images
are continuous memory blocks in raster order. The two pointer arguments return
the standard deviations of the patches: if either of these are low, the results
can be misleading.
@param x0 Start x-co-ordinate of patch in first image
@param y0 Start y-co-ordinate of patch in first image
@param x0lim End x-co-ordinate of patch in first image
@param y0lim End y-co-ordinate of patch in first image
@param x1 Start x-co-ordinate of patch in second image
@param y1 Start y-co-ordinate of patch in second image
@param p0 First image
@param p0 Second image
@param sd0ptr Standard deviation of patch from first image
@param sd1ptr Standard deviation of patch from second image
@ingroup gSceneImproc
*/
double correlate2_warning(const int x0, const int y0,
                          const int x0lim, const int y0lim,
                          const int x1, const int y1,
                          const VW::ImageMono<unsigned char> &p0,
                          const VW::ImageMono<unsigned char> &p1,
                          double *sd0ptr, double *sd1ptr)
{
  VW::PixelMono<unsigned char> *p0_ptr, *p1_ptr;

  // Make these int rather than unsigned int for speed
  int patchwidth = x0lim - x0;
  int p0skip = p0.GetWidth() - patchwidth;
  int p1skip = p1.GetWidth() - patchwidth;
  int Sg0 = 0, Sg1 = 0, Sg0g1 = 0, Sg0sq = 0, Sg1sq = 0;

  double n = (x0lim - x0) * (y0lim - y0);     /* to hold total no of pixels */

  double varg0 = 0.0, varg1 = 0.0, sigmag0 = 0.0, sigmag1 = 0.0, g0bar = 0.0, 
         g1bar = 0.0;   
                                   /* variances, standard deviations, means */
  double Sg0doub = 0.0, Sg1doub = 0.0, Sg0g1doub = 0.0, Sg0sqdoub = 0.0, 
         Sg1sqdoub = 0.0;

  double C = 0.0;                               /* to hold the final result */
  double k = 0.0;                         /* to hold an intermediate result */

  /* at the moment the far right and bottom pixels aren't included */

  p0_ptr = p0.GetImageBuffer() + p0.GetWidth() * y0 + x0;
  p1_ptr = p1.GetImageBuffer() + p1.GetWidth() * y1 + x1;

  for(int y0copy = y0lim - 1; y0copy >= 0 ; y0copy--) {
    for(int x0copy = x0lim - 1; x0copy >=0 ; x0copy--) {
      Sg0 += p0_ptr->y;
      Sg1 += p1_ptr->y;
      Sg0g1 += p0_ptr->y * p1_ptr->y;
      Sg0sq += p0_ptr->y * p0_ptr->y;
      Sg1sq += p1_ptr->y * p1_ptr->y;

      p0_ptr++;
      p1_ptr++;
    }
    p0_ptr += p0skip;
    p1_ptr += p1skip;
  }

  Sg0doub = Sg0;
  Sg1doub = Sg1;
  Sg0g1doub = Sg0g1;
  Sg0sqdoub = Sg0sq;
  Sg1sqdoub = Sg1sq;

  g0bar = Sg0doub / n;
  g1bar = Sg1doub / n;

  varg0   = Sg0sqdoub / n  -  (g0bar * g0bar);
  varg1   = Sg1sqdoub / n  -  (g1bar * g1bar);

  sigmag0 = sqrt(varg0);
  sigmag1 = sqrt(varg1);

  *sd0ptr = sigmag0;
  *sd1ptr = sigmag1;

  if (sigmag0 == 0.0)      /* special checks for this algorithm */
  {                       /* to avoid division by zero */
    if (sigmag1 == 0.0)
      return 0.0;
    else
      return 1.0;
  }

  if (sigmag1 == 0.0)
    return 1.0;

  k = g0bar / sigmag0 - g1bar / sigmag1;

  C = Sg0sqdoub / varg0   +   Sg1sqdoub / varg1   +   n * (k * k)
      -   Sg0g1doub * 2.0 / (sigmag0 * sigmag1)   
      -   Sg0doub * 2.0 * k / sigmag0   +   Sg1doub * 2.0 * k / sigmag1;

  return C / n;    /* returns mean square no of s.d. from mean of pixels */ 
}

