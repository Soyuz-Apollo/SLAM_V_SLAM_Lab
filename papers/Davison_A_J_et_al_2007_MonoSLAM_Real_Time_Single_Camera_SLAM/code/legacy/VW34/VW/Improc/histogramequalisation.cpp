/** 
 * @file    histogramequalisation.cpp
 * @author  BJ Tordoff
 * @date    05/04/2005
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

#include <iostream>
#include "histogramequalisation.h"

// For now hardcoded as 256 bins
#define NUMBINS 256

using namespace std;

void VW::HistogramEqualisation::
ResetArrays()
{
  _bin_contents.resize(NUMBINS);
  _hash.resize(NUMBINS);
  for (unsigned int i=0;i<NUMBINS;i++) {
    _bin_contents[i]=0;
    _hash[i]=i;
  }
  _totalnum = 0;
}

void VW::HistogramEqualisation::
AddImage(const VW::ImageMono<unsigned char>& inputimage)
{
  // Now scan the input image adding to the bins
  const unsigned int numpix = inputimage.GetWidth()*inputimage.GetHeight();

  VW::PixelMono<unsigned char>* pixptr = inputimage.GetRowStart(0);
  for (unsigned i=0;i<numpix;i++,pixptr++) {
    _bin_contents[pixptr->y]++;
  }
  _totalnum += numpix;

  // Now recalculate the hash
  const float stepsize = float(_totalnum) / float(NUMBINS);
  float nextlim = stepsize;
  unsigned char targetbin = 0;
  unsigned int cumsum=0;
  //cerr << "Hash=[" << endl;
  for (unsigned int i=0;i<NUMBINS;i++) {
    cumsum += _bin_contents[i];
    if (float(cumsum)>nextlim) {
      targetbin = static_cast<unsigned char>(0.5f+float((NUMBINS-1)*cumsum)/float(_totalnum));
      nextlim += stepsize;
    }
    _hash[i] = targetbin;
  }

}

void VW::HistogramEqualisation::
AddImage(const VW::ImageYUV<unsigned char>& inputimage)
{
  // Now scan the input image adding to the bins
  const unsigned int numpix = inputimage.GetWidth()*inputimage.GetHeight();

  VW::PixelYUV<unsigned char>* pixptr = inputimage.GetRowStart(0);
  for (unsigned i=0;i<numpix;i++,pixptr++) {
    _bin_contents[pixptr->y]++;
  }
  _totalnum += numpix;

  // Now recalculate the hash
  const float stepsize = float(_totalnum) / float(NUMBINS);
  float nextlim = stepsize;
  unsigned char targetbin = 0;
  unsigned int cumsum=0;
  //cerr << "Hash=[" << endl;
  for (unsigned int i=0;i<NUMBINS;i++) {
    cumsum += _bin_contents[i];
    if (float(cumsum)>nextlim) {
      targetbin = static_cast<unsigned char>(0.5f+float((NUMBINS-1)*cumsum)/float(_totalnum));
      nextlim += stepsize;
    }
    _hash[i] = targetbin;
  }

}

void VW::HistogramEqualisation::
ApplyToImage(VW::ImageMono<unsigned char>& resultimage)
{
  const unsigned int numpix = resultimage.GetWidth()*resultimage.GetHeight();

  VW::PixelMono<unsigned char>* pixptr = resultimage.GetRowStart(0);
  for (unsigned i=0;i<numpix;i++,pixptr++) {
    pixptr->y = _hash[pixptr->y];
  }
}

void VW::HistogramEqualisation::
ApplyToImage(const VW::ImageMono<unsigned char>& inputimage,
	     VW::ImageMono<unsigned char>& resultimage)
{
  const unsigned int w = inputimage.GetWidth(), h = inputimage.GetHeight();
  const unsigned int numpix = w*h;

  // Make sure result is right size
  if (resultimage.IsAlloced()) {
    if ((resultimage.GetWidth() != w) || (resultimage.GetHeight() != h)) {
      resultimage.FreeImageData();
      resultimage.AllocImageData(w, h);
    }
  } else {
    resultimage.AllocImageData(w, h);
  }

  const VW::PixelMono<unsigned char>* srcptr = inputimage.GetRowStart(0);
  VW::PixelMono<unsigned char>* dstptr = resultimage.GetRowStart(0);
  for (unsigned int i=0; i<numpix;i++, srcptr++, dstptr++) {
    dstptr->y = _hash[srcptr->y];
  }
}
void VW::HistogramEqualisation::
ApplyToImage(VW::ImageYUV<unsigned char>& resultimage)
{
  const unsigned int numpix = resultimage.GetWidth()*resultimage.GetHeight();

  VW::PixelYUV<unsigned char>* pixptr = resultimage.GetRowStart(0);
  for (unsigned i=0;i<numpix;i++,pixptr++) {
    pixptr->y = _hash[pixptr->y];
    pixptr->u = pixptr->u;
    pixptr->v = pixptr->v;
    pixptr->alpha = pixptr->alpha;
  }
}

void VW::HistogramEqualisation::
ApplyToImage(const VW::ImageYUV<unsigned char>& inputimage,
	     VW::ImageYUV<unsigned char>& resultimage)
{
  const unsigned int w = inputimage.GetWidth(), h = inputimage.GetHeight();
  const unsigned int numpix = w*h;

  // Make sure result is right size
  if (resultimage.IsAlloced()) {
    if ((resultimage.GetWidth() != w) || (resultimage.GetHeight() != h)) {
      resultimage.FreeImageData();
      resultimage.AllocImageData(w, h);
    }
  } else {
    resultimage.AllocImageData(w, h);
  }

  const VW::PixelYUV<unsigned char>* srcptr = inputimage.GetRowStart(0);
  VW::PixelYUV<unsigned char>* dstptr = resultimage.GetRowStart(0);
  for (unsigned int i=0; i<numpix;i++, srcptr++, dstptr++) {
    dstptr->y = _hash[srcptr->y];
    dstptr->u = srcptr->u;
    dstptr->v = srcptr->v;
    dstptr->alpha = srcptr->alpha;
  }
}
