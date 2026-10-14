/** @file moments.cpp
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
 *
 * @class   VW::Moments 
 * @ingroup Improc
 * @author  ID Reid
 * @date    18/12/04
 * @brief   Compute moments of a binary or monochrome image
 *
 * Determine the 0th, 1st and 2nd moments of a binary or monochrome image
 * and provide various access methods to compute orientation, etc.
 *
 */

#include <VW/image.h>
#include "moments.h"


using namespace VW;


template <class ImType>
void VW::Moments::Compute(const ImageMono<ImType>& im, const ROI roi)
{
  double valtimesc, valtimesr;
  sum = sumx = sumy = sumxx = sumxy = sumxy = 0.0;

  for (int r=roi.Top(); r<roi.Bottom(); r++) {
    for (int c=roi.Left(); c<roi.Right(); c++) {
      double val = im[r][c].y/255.0;
      sum += val;
      valtimesc = val*c;
      valtimesr = val*r;
      sumx += valtimesc;
      sumy += valtimesr;
      sumxx += valtimesc*c;
      sumxy += valtimesc*r;
      sumyy += valtimesr*r;
    }
  }

  a = sumxx - sumx*sumx/sum;
  b = 2.0*(sumxy - sumx*sumy/sum);
  c = sumyy - sumy*sumy/sum;

  theta = -90.0/3.1415927*atan2(b, a-c);
  
  // compute gradient (y-axis taken up) to resolve axis ambiguity 
  int u0 = (roi.Right()-roi.Left())/2;
  int v0 = (roi.Bottom()-roi.Top())/2;
  int dx = int(im[v0][roi.Right()-2].y)-int(im[v0][roi.Left()+2].y);
  int dy = int(im[roi.Top()+2][u0].y)-int(im[roi.Bottom()-2][u0].y);

  if ((a-c)*dy + b*dx < 0) {
    theta += 180.0;
  }

  return;
}


template <class ImType>
void VW::Moments::Compute(const ImageMono<ImType>& im, 
		 const ROI roi,
		 const ImageMono<float>& weight)
{
  double valtimesc, valtimesr;
  sum = sumx = sumy = sumxx = sumxy = sumxy = 0.0;

  for (int r=roi.Top(); r<roi.Bottom(); r++) {
    for (int c=roi.Left(); c<roi.Right(); c++) {
      double val = im[r][c].y/255.0*weight[r][c].y;
      sum += val;
      valtimesc = val*c;
      valtimesr = val*r;
      sumx += valtimesc;
      sumy += valtimesr;
      sumxx += valtimesc*c;
      sumxy += valtimesc*r;
      sumyy += valtimesr*r;
    }
  }

  a = sumxx - sumx*sumx/sum;
  b = 2.0*(sumxy - sumx*sumy/sum);
  c = sumyy - sumy*sumy/sum;

  theta = -90.0/3.1415927*atan2(b, a-c);

  // compute gradient (y-axis taken up) to resolve axis ambiguity 
  int u0 = (roi.Right()-roi.Left())/2;
  int v0 = (roi.Bottom()-roi.Top())/2;
  int dx = int(im[v0][roi.Right()-2].y)-int(im[v0][roi.Left()+2].y);
  int dy = int(im[roi.Top()+2][u0].y)-int(im[roi.Bottom()-2][u0].y);

  if ((a-c)*dy + b*dx < 0) {
    theta += 180.0;
  }


  return;
}


template void VW::Moments::Compute<unsigned char>(const ImageMono<unsigned char>&,
						  const ROI roi);
template void VW::Moments::Compute<unsigned char>(const ImageMono<unsigned char>&,
						  const ROI roi,
						  const ImageMono<float>&);


