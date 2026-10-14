/** 
 * @file   chamferdistance.cpp
 * @author BJ Tordoff
 * @date   26/06/2003
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

#include <cmath> // sqrt
#include <algorithm> // min

#include "chamferdistance.h"

/** take the minimum of 4 values */
unsigned int min4(unsigned int v1,
		  unsigned int v2,
		  unsigned int v3,
		  unsigned int v4)
{
  return std::min(std::min(v1,v2),std::min(v3,v4));
}
		 
/** take the minimum of 5 values */
unsigned int min5(unsigned int v1,
		  unsigned int v2,
		  unsigned int v3,
		  unsigned int v4,
		  unsigned int v5)

{
  return std::min(std::min(std::min(v1,v2),std::min(v3,v4)),v5);
}








void VW::ChamferDistance::Compute(const VW::ImageMono<bool> &inputimage,
				  VW::ImageMono<unsigned int> &result)
{
  unsigned int w = inputimage.GetWidth();
  unsigned int h = inputimage.GetHeight();

  const unsigned int maxdist16 = static_cast<unsigned int>(std::sqrt(float(w*w + h*h)*256.0));

  if(result.IsAlloced()){
    // If the result image is already allocated, it had better be the right size!
    assert( (result.GetWidth() == inputimage.GetWidth())
	    && (result.GetHeight() == inputimage.GetHeight()) );


  } else {
    result.AllocImageData(inputimage.GetWidth(),inputimage.GetHeight());
  }

  /*
   *    *** FIXME ***
   * BJT says: this should only operate in the ROI, NOT the full image, and the result
   * should have a ROI shrunk by 1.
   */


  // All operations have 4dp (in binary, that is), so values are multiplied by
  // 16. sqrt 2 is therefore about 23!

  // Chamfer Distance Transform

  // First create a binary image with 0 on features, MAX everywhere else
  for(unsigned int y=0;y<h;y++){
    VW::PixelMono<bool>* sourcepix = inputimage.GetRowStart(y);
    VW::PixelMono<unsigned int>* destpix1 = result.GetRowStart(y);
    VW::PixelMono<unsigned int>* destpix2 = result.GetRowStart(y);
    for(unsigned int x=0;
	x<w;
	x++, sourcepix++, destpix1++, destpix2++){
      if( sourcepix->y ) {
	// On a feature, so distance is zero!
	destpix1->y = 0;
	destpix2->y = 0;
      } else {
	// Empty, so set to max distance, which will be reduced later
	destpix1->y = maxdist16;
	destpix2->y = maxdist16;
      }
    }
  }

 // next a pass-forward raster scan (top-left to bottom-right)
  for(unsigned int y=1; y<(h-1); y++){

    /* Get pointers as follows (where * is the current pixel):
     *
     *  al | a | ar
     * -------------
     *   l | * |
     * -------------
     *     |   |
     *
     */
    VW::PixelMono<unsigned int>* al = result.GetRowStart(y-1);
    VW::PixelMono<unsigned int>* a  = al + 1;
    VW::PixelMono<unsigned int>* ar = a  + 1;
    VW::PixelMono<unsigned int>* l = result.GetRowStart(y);
    VW::PixelMono<unsigned int>* p = l + 1;
   
    // Loop stops one in from edge to prevent under/over-flow
    for(unsigned int x=1; x<(w-1) ; x++, al++, a++, ar++, l++, p++) {
      if(p->y == maxdist16) {
	// use 23/16 approx sqrt(2)
	unsigned int t1 = al->y + 23;
	unsigned int t2 = a->y  + 16;
	unsigned int t3 = ar->y + 23;
	unsigned int t4 = l->y  + 16;
	
	unsigned int z = min4(t1,t2,t3,t4);
	p->y = z;
      }
    }
  }

 // then a second pass - reverse raster scan (bottom-right to top-left)
  for(int y = (h-2) ; y > 0; y-- ) {

    /* Get pointers as follows (where * is the current pixel):
     *
     *     |   | 
     * -------------
     *     | * |  r
     * -------------
     *  bl | b | br
     *
     */   
    VW::PixelMono<unsigned int>* br = result.GetRowStart(y+1); br += (w-1);
    VW::PixelMono<unsigned int>* b  = br - 1;
    VW::PixelMono<unsigned int>* bl = b - 1; 
    VW::PixelMono<unsigned int>* p = result.GetRowStart(y); p += (w-2);
    VW::PixelMono<unsigned int>* r = p + 1;
  
    // Loop stops one in from edge to prevent under/over-flow
    for(unsigned int x = w-2; x>0 ; x--, bl--, b--, br--, r--, p--) {
      if(p->y > 0) {
	// use 23/16 approx sqrt(2)
	unsigned int t1 = bl->y + 23;
	unsigned int t2 = b->y  + 16;
	unsigned int t3 = br->y + 23;
	unsigned int t4 = r->y  + 16;
	
	unsigned int z = min4(t1,t2,t3,t4);
	if (z<p->y) p->y = z;

      }
    }
  }

   // Finally, normalise the values
   // we add 8 to make the values rounded rather than floored, then right shift
   // by 4 (divide by 16)
  for(unsigned int y=0;y<h;y++) {
    VW::PixelMono<unsigned int>* r = result.GetRowStart(y);
    for(unsigned int x=0;x<w;x++,r++) {
	r->y = (r->y + 8) >> 4;
    }
  }
}

/**
 * Compute the linear distance transform (chamfer distance) from the edges specified
 * in edgedata. Note that as edges don't specify an image size, the result image
 * should be resized to the desired size before use.
 */
void VW::ChamferDistance::Compute(const VW::EdgeData &edges,
				  VW::ImageMono<unsigned int> &result)
{
  assert(result.IsAlloced());
  VW::ImageMono<bool> inputimage(result.GetWidth(), result.GetHeight());

  // Set to false
  VW::PixelMono<bool>* p = inputimage.GetRowStart(0);
  const unsigned int numpix = result.GetWidth() * result.GetHeight();
  for (unsigned int i=0;i<numpix;i++,p++) p->y = false;

  // Go through edges setting to true
  std::vector<VW::Edgel *>::const_iterator e;
  for (e =  edges.edgels.begin();
       e != edges.edgels.end();
       e++) {
    inputimage.SetPixel((*e)->GetIX(),(*e)->GetIY(),VW::PixelMono<bool>(true));
  }
 
  Compute(inputimage,result);
}


/**
 * Compute the linear distance transform (chamfer distance) from a mono image
 * where any pixel with a value above the threshold is treated as a "feature".
 */
void VW::ChamferDistance::Compute(const VW::ImageMono<unsigned char> &inputimage,
		     VW::ImageMono<unsigned int> &result,
		     unsigned char threshold)
{
  if(result.IsAlloced()){
    // If the result image is already allocated, it had better be the right size!
    assert( (result.GetWidth() == inputimage.GetWidth())
	    && (result.GetHeight() == inputimage.GetHeight()) );


  } else {
    result.AllocImageData(inputimage.GetWidth(),inputimage.GetHeight());
  }
  VW::ImageMono<bool> boolimage(result.GetWidth(), result.GetHeight());

  // Set to false
  VW::PixelMono<unsigned char>* pin = inputimage.GetRowStart(0);
  VW::PixelMono<bool>* pout = boolimage.GetRowStart(0);
  const unsigned int numpix = inputimage.GetWidth() * inputimage.GetHeight();
  for (unsigned int i=0;i<numpix;i++,pin++,pout++) 
    if (pin->y > threshold) pout->y = true;
    else pout->y = false;


  Compute(boolimage,result);
}
