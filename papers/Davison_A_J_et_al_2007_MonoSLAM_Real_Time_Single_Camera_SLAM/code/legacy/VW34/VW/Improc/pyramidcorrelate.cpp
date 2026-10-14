/** 
 * @file   pyramidcorrelate.cc
 * @author ID Reid, JGH Knight
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

#ifndef _pyramidcorrelate_cc
#define _pyramidcorrelate_cc

#include <iostream>

#include "pyramidcorrelate.h"
#include "improc_misc.h"

using namespace std;
using namespace VW;

////   Constructors, Destructors   /////////////////////////////////////

static const double DEFAULT_CORREL_THRESH = 0.2;


template <class T>
VW::PyramidCorrelate::PyramidCorrelate (const ImageMono<T>& image1, const ImageMono<T>& image2) :
_cthresh (DEFAULT_CORREL_THRESH)
{
  assert (image1.IsAlloced ());
  assert (image2.IsAlloced ());

  unsigned int imwidth, imheight, imwidth2, imheight2;
  image1.GetSize (imwidth, imheight);
  image2.GetSize (imwidth2, imheight2);
  assert (imwidth2 == imwidth && imheight2 == imheight);     // For the moment

  _ppyr1 = new Pyramid<float>(image1, 30);
  _ppyr2 = new Pyramid<float>(image2, 30);

  assert (_ppyr1->NumLevels () == _ppyr2->NumLevels ());
}

// Instantiations
template VW::PyramidCorrelate::PyramidCorrelate (const ImageMono<unsigned char>&, const ImageMono<unsigned char>&);
template VW::PyramidCorrelate::PyramidCorrelate (const ImageMono<float>&, const ImageMono<float>&);


VW::PyramidCorrelate::~PyramidCorrelate ()
{
  delete _ppyr1;
  delete _ppyr2;
}


bool VW::PyramidCorrelate::Compute ()
{
  int ilevels = _ppyr1->NumLevels ();

  // For each image in pyramid from top to bottom search +-1 pixel about best
  // match in images at previous level (at top level try every pixel) and
  // find best match at this level.
  _x = _y = 0;
  unsigned int w, h;
  double best_correl=100.0;
  for (int lev = ilevels - 1; lev >= 0; lev--) {
    // Width and height of current image
    (*_ppyr1)[lev].GetSize (w, h);

    // Region on first image over which to correlate second
    int xstart, ystart, xend, yend;
    if (lev == ilevels - 1) {
      int c0, r0, c1, r1;
      (*_ppyr1)[lev].ROI().Get(c0, r0, c1, r1);
      xstart = c0;
      ystart = r0;
      xend = c1 - 1;
      yend = r1 - 1;
    }
    else {
      xstart = int (_x * 2) - 1;
      xend   = int (_x * 2) + 1;
      ystart = int (_y * 2) - 1;
      yend   = int (_y * 2) + 1;
    }

    // Loop over region and correlate
    best_correl = 100.0;
    for (int x = xstart; x <= xend; x++) {
      for (int y = ystart; y <= yend; y++) {
	double this_correl =
	  VW::Misc::CorrelationScoreAJD ((*_ppyr2)[lev].GetImageBuffer(), w, h,
					 (*_ppyr1)[lev].GetImageBuffer(),
					 w, h, x, y);
	assert (this_correl >= 0.0);
	if (this_correl < best_correl) {
	  best_correl = this_correl;
	  _x = x;
	  _y = y;
	}
      }
    }

  }

  // Fail if we didn't correlate well enough
  if (best_correl > _cthresh) return false;

  // Adjust result to be offsets from centre
  _x -= w/2;
  _y -= h/2;
  cerr << "Final correlation = " << best_correl << ", t = (" << _x << ", " << _y << ")\n";

  return true;
}



#endif
