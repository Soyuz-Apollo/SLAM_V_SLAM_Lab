/** @file normaliseintensity.h
 *
 *              (c) Copyright University of Cambridge
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
#include "normaliseintensity.h"
#include <VW/Image/imagemono.h>
#include <VW/Image/imageyuv.h>

#include <iostream>
#include <math.h>

using namespace std;

namespace VW {
template <class Type>
void NormaliseIntensity(VW::ImageMono<Type>& image,
			double desired_stddev)
{
  double sum=0, sum2=0;

  const VW::ROI roi = image.ROI();
  const int n = roi.Width() * roi.Height();
  for (int y = roi.Top(); y < roi.Bottom(); y++) {
    typename VW::ImageMono<Type>::PixelPtr p = image.GetROIRowStart(y);
    
    for (int x = roi.Left(); x < roi.Right(); x++,p++) {
      sum += p->y;
      sum2 += (p->y * p->y);
    }
  }

  double mean = sum / n;
  double variance = ( sum2 / n ) - (mean*mean);

  // Ideal is for mean of 127

  double shift = 127;
  double scale = sqrt( desired_stddev * desired_stddev / variance );

  cout << "Shift = " << shift - mean << "   Scale = " << scale << endl;

  for (int y = roi.Top(); y < roi.Bottom(); y++) {
    typename VW::ImageMono<Type>::PixelPtr p = image.GetROIRowStart(y);
    
    for (int x = roi.Left(); x < roi.Right(); x++,p++) {
      double val = shift + scale * (p->y - mean);
      if (val>255) val=255;
      if (val<0) val=0;
      p->y = static_cast<Type>( val );

    }
  }


}

template <class Type>
void NormaliseIntensity(VW::ImageYUV<Type>& image,
			double desired_stddev)
{
  double sum=0, sum2=0;

  const VW::ROI roi = image.ROI();
  const int n = roi.Width() * roi.Height();
  for (int y = roi.Top(); y < roi.Bottom(); y++) {
    typename VW::ImageYUV<Type>::PixelPtr p = image.GetROIRowStart(y);
    
    for (int x = roi.Left(); x < roi.Right(); x++,p++) {
      sum += p->y;
      sum2 += (p->y * p->y);
    }
  }

  double mean = sum / n;
  double variance = ( sum2 / n ) - (mean*mean);

  // Ideal is for mean of 127

  double shift = 127;
  double scale = sqrt( desired_stddev * desired_stddev / variance );

  cout << "Shift = " << shift - mean << "   Scale = " << scale << endl;

  for (int y = roi.Top(); y < roi.Bottom(); y++) {
    VW::PixelYUV<Type>* p = image.GetROIRowStart(y);
    
    for (int x = roi.Left(); x < roi.Right(); x++,p++) {
      double val = shift + scale * (p->y - mean);
      if (val>255) val=255;
      if (val<0) val=0;
      p->y = static_cast<Type>( val );

    }
  }


}


template void NormaliseIntensity(VW::ImageMono<unsigned char>&,double);
template void NormaliseIntensity(VW::ImageYUV<unsigned char>&,double);
}
