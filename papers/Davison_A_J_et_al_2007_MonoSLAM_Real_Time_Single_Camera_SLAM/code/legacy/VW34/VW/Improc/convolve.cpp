/** 
 * @file   convolve.cpp
 * @author ID Reid
 * @date   02/08/2002
 * @update 31/07/2003 DWM
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


#include <math.h>
#include <vector>

#include <VW/image.h>
#include <VW/MathBits/nan.h>

#include "convolve.h"

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false

using namespace std;


//////////////////////////////////////////////////////////////////
#define ZERO_SIGMA_THRES 0.1 
//////////////////////////////////////////////////////////////////

static inline char Round(const char i) { return i; }
static inline unsigned char Round(const unsigned char i) { return i; }
static inline int Round(const int i) { return i; }
static inline unsigned int Round(const unsigned int i) { return i; }
static inline long int Round(const long int i) { return i; }
static inline unsigned long int Round(const unsigned long int i) { return i; }
static inline float Round(const float i) { return (i<0) ? ceil(i-0.5f) : floor(i + 0.5f); }
static inline double Round(const double i) { return (i<0) ? ceil(i-0.5) : floor(i + 0.5); }
static inline long double Round(const long double i) { return (i<0) ? ceil(i-0.5) : floor(i + 0.5); }

static bool IsAnOddNumber(int n) {return ((n-1)/2) == (n/2); }

namespace VW {

template <class maskType>
ConvolutionMask1D<maskType>::ConvolutionMask1D(int n)
{
  assert( IsAnOddNumber(n) );

  halfwidth = n/2;
  mask = new maskType[2*halfwidth+1];
  mask += halfwidth;
  symmetry = NO_SYMMETRY;
}
  

template <class maskType>
ConvolutionMask1D<maskType>::ConvolutionMask1D(maskType x_1, maskType x0, maskType x1)
{
  halfwidth = 1;
  mask = new maskType[3];
  mask += halfwidth;
  mask[-1] = x_1; mask[0] = x0; mask[1] = x1;
  weight = x_1+x0+x1;
  if (x_1==x1) {
    symmetry=EVEN_SYMMETRY;
  } else if ( (x_1 == -x1) && x0==maskType(0) ) {
    symmetry=ODD_SYMMETRY;
  } else {
    symmetry=NO_SYMMETRY;
  }
}

template <class maskType>
ConvolutionMask1D<maskType>::ConvolutionMask1D(maskType x_2, maskType x_1, maskType x0, maskType x1, maskType x2)
{
  halfwidth = 2;
  mask = new maskType[5];
  mask += halfwidth;
  mask[-2] = x_2; mask[-1] = x_1; mask[0] = x0; mask[1] = x1; mask[2] = x2;
  weight = x_2+x_1+x0+x1+x2;
  if ( (x_2==x2) && (x_1==x1) ) {
    symmetry=EVEN_SYMMETRY;
    //    cerr << "Even Symmetry" << endl;
  } else if ( (x_2== -x2) && (x_1== -x1) && x0==maskType(0) ) {
    symmetry=ODD_SYMMETRY;
    //    cerr << "Odd Symmetry" << endl;
  } else {
    symmetry=NO_SYMMETRY;
    //    cerr << "No Symmetry" << endl;
  }
}

template <class maskType>
ConvolutionMask1D<maskType>::ConvolutionMask1D(maskType *x, int n)
{
  assert( IsAnOddNumber(n) );

  bool even, odd;
  halfwidth = n/2;
  mask = new maskType[n];
  mask += halfwidth; x += halfwidth;
  mask[0] = weight = x[0];
  // assume both are true, then disprove
  // if both remain true (all zero) then even
  even=true;
  odd=true;
  if( mask[0]==maskType(0) ) odd=true;
  for (int i=1; i<=halfwidth; i++) {
    mask[i] = x[i];   weight += x[i];
    mask[-i] = x[-i]; weight += x[-i];
    if (x[i]!=x[-i]) even = false;
    if (x[i]!=-x[-i]) odd = false;
  }
  if(even) {
    symmetry=EVEN_SYMMETRY;
  } else if (odd) {
    symmetry=ODD_SYMMETRY;
  } else {
    symmetry=NO_SYMMETRY;
  }    
}

template <class maskType>
ConvolutionMask1D<maskType>::ConvolutionMask1D(ConvolutionMask1D<maskType> &c) 
{
  halfwidth = c.halfwidth;
  symmetry = c.symmetry;
  weight = c.weight;
  mask = new maskType[2*halfwidth+1];
  mask += halfwidth;
  for (int i=-halfwidth; i<=halfwidth; i++)
    mask[i] = c.mask[i];
}


template <class maskType>
ConvolutionMask1D<maskType>::~ConvolutionMask1D()
{
  delete (maskType *)(mask-halfwidth);
}


template <class maskType>
void ConvolutionMask1D<maskType>::MakeGaussian(double sigma)
{
  int i;
  symmetry = EVEN_SYMMETRY;

  if ( sigma < ZERO_SIGMA_THRES ) { /* treat as zero: no smoothing */
    mask[0] = maskType(1.0);
    for (i=1; i<=halfwidth; i++) {
      mask[i] = mask[-i] = maskType(0.0);
    }
  } else {
    double sigma2 = 2*sigma*sigma;
    
    mask[0] = weight = maskType(1.0);
    for (i=1; i<=halfwidth; i++) {
      mask[i] = maskType(exp ( -(double)i*(double)i/sigma2 ));
      weight += 2*mask[i];
    }
     
    mask[0] /= weight;
    for (i=1; i<=halfwidth; i++) {
      mask[i] /= weight;
      mask[-i] = mask[i];
    }
  }
  weight = maskType(1.0);
}

template <class maskType>
void ConvolutionMask1D<maskType>::MakeUniform()
{
  symmetry = EVEN_SYMMETRY;
  double val = 1.0 / double(2*halfwidth+1);
  for (int i=-halfwidth; i<=halfwidth; i++) {
    mask[i] = maskType(val);
  }

  weight = maskType(1.0);
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

template <class maskType>
template <class inType, class outType>
void Convolve<maskType>::Compute(const ImageMono<inType> &im,
				 ImageMono<outType> &result,
				 const unsigned int subsampling)
{
  int c0, r0, cl, rl;
  int cnew0, rnew0, cnewl, rnewl;
  int r, c, i;
  
  // Check image sizes
  if ( (result.GetWidth()!=im.GetWidth()/subsampling)
       || (result.GetHeight()!=im.GetHeight()/subsampling)) {
    cerr << "Convolve::Compute: Image sizes incorrect - ("
	 <<result.GetWidth()<<","
	 <<result.GetHeight()<<") != ("
	 <<im.GetWidth()<<","<<im.GetHeight()<<")/"<<subsampling<<"."<<endl;
    return;
  }


  if (VERBOSE) 
    cerr << "Convolve::Compute: Set ROI" << endl;
  im.ROI().Get(c0, r0, cl, rl);
  cnew0=c0+cmask.GetHalfWidth(); rnew0=r0+rmask.GetHalfWidth();
  cnewl=cl-cmask.GetHalfWidth(); rnewl=rl-rmask.GetHalfWidth();
  if (VERBOSE) cerr << "Current ROI = (" << cnew0 <<","<< rnew0<<")-("<<cnewl<<","<<rnewl <<")"<< endl;
  // Take account of subsampling (note that high limit is 1 beyond)
  const int next_c0 = cnew0/subsampling;
  const int next_r0 = rnew0/subsampling;
  const int next_c1 = cnewl/subsampling;
  const int next_r1 = rnewl/subsampling;
//   const int next_c1 = (cnewl-1)/subsampling + 1;
//   const int next_r1 = (rnewl-1)/subsampling + 1;
  result.ROI().Set(next_c0,next_r0,next_c1,next_r1);
  if (VERBOSE) 
    cerr << "Next ROI = (" << next_c0 <<","<< next_r0 
	 <<")-("<< next_c1 <<","<< next_r1 <<")"<< endl;

  if (VERBOSE) cerr << "Convolve::Compute: Create temporary image" << endl;
  if (VERBOSE) cerr << "                     (size "<<im.GetWidth()<<"x"<<im.GetHeight()<<")" << endl;
  ImageMono<maskType> temp(im.GetWidth(), im.GetHeight());

  // Get pointers to input and output data
  typename ImageMono<inType>::PixelPtr  *imr;
  typename ImageMono<outType>::PixelPtr resultr;
  typename ImageMono<maskType>::PixelPtr tempr;
  maskType d;
  
  if (VERBOSE) cerr << "Convolve::Compute: First pass" << endl;
  // Whether or not subsampling create the intermediate image
  // at full resolution
  switch(rmask.Symmetry()) {
  case EVEN_SYMMETRY:
    if (VERBOSE) cerr << "                      (EVEN symmetry)" << endl;
    for (r=rnew0; r<rnewl; r++) {
      imr = im.GetRowPointer(r);
      tempr = temp.GetRowStart(r)+c0;
      for (c=c0; c<cl; c++, tempr++) {
	tempr->y = rmask.mask[0] * maskType(imr[0][c].y);
        // Now loop over one side
	for (i=1; i<=rmask.GetHalfWidth(); i++) {
	  tempr->y += rmask.mask[i] * ( maskType(imr[i][c].y)+maskType(imr[-i][c].y) );
	}
      }
    }
    break;
  case ODD_SYMMETRY:
    if (VERBOSE) cerr << "                      (ODD symmetry)" << endl;
    for (r=rnew0; r<rnewl; r++) {
      imr = im.GetRowPointer(r);
      tempr = temp.GetRowStart(r)+c0;
      for (c=c0; c<cl; c++, tempr++) {
	tempr->y = maskType(0.0); // yes the central value is zero!
        // Now loop over one side
	for (i=1; i<=rmask.GetHalfWidth(); i++) {
	  tempr->y += rmask.mask[i] * ( maskType(imr[i][c].y)-maskType(imr[-i][c].y) );
	}
      }
    }
    break;
  default: // NO_SYMMETRY
    if (VERBOSE) cerr << "                      (NO symmetry)" << endl;
    for (r=rnew0; r<rnewl; r++) {
      imr = im.GetRowPointer(r);
      tempr = temp.GetRowStart(r)+c0;
      for (c=c0; c<cl; c++, tempr++) {
	tempr->y = maskType(0.0);
	for (i=-rmask.GetHalfWidth(); i<=rmask.GetHalfWidth(); i++) {
	  tempr->y += rmask.mask[i] * maskType(imr[i][c].y);
	}
      }
    }
    break;
  }

  // Now the other direction
  // Here we must consider subsampling

   if (VERBOSE) cerr << "Convolve::Compute: Second pass" << endl;
 if (subsampling==1) {
   if (VERBOSE) cerr << "                      (no subsampling)" << endl;
    
    switch(cmask.Symmetry()) {
    case EVEN_SYMMETRY:
      if (VERBOSE) cerr << "                      (EVEN symmetry)" << endl;
      for (r=rnew0; r<rnewl; r++) {
	resultr = result.GetRowStart(r)+cnew0;
	tempr   = temp.GetRowStart(r)+cnew0;
	for(c=cnew0; c<cnewl; c++, resultr++, tempr++) {
	  d = cmask.mask[0] * maskType(tempr[0].y);
	  // Now loop over one side
	  for (i=1; i<=cmask.GetHalfWidth(); i++) {
	    d += cmask.mask[i] * ( maskType(tempr[i].y) + maskType(tempr[-i].y) );
	  }
	  //	  resultr->y = outType(Round(d));
	  resultr->y = outType(d)+outType(0.5f);
	}
      }
      break;
    case ODD_SYMMETRY:
      if (VERBOSE) cerr << "                      (ODD symmetry)" << endl;
      for (r=rnew0; r<rnewl; r++) {
	resultr = result.GetRowStart(r)+cnew0;
	tempr   = temp.GetRowStart(r)+cnew0;
	for(c=cnew0; c<cnewl; c++, resultr++, tempr++) {
	  d = maskType(0.0);  // yes the central value is zero!
	  // Now loop over one side
	  for (i=1; i<=cmask.GetHalfWidth(); i++) {
	    d += cmask.mask[i] * ( maskType(tempr[i].y) - maskType(tempr[-i].y) );
	  }
	  //	  resultr->y = outType(Round(d));
	  resultr->y = outType(d)+outType(0.5f);
	}
      }
      break;
    default: // NO_SYMMETRY
      if (VERBOSE) cerr << "                      (NO symmetry)" << endl;
      for (r=rnew0; r<rnewl; r++) {
	resultr = result.GetRowStart(r)+cnew0;
	tempr   = temp.GetRowStart(r)+cnew0;
	for(c=cnew0; c<cnewl; c++, resultr++, tempr++) {
	  d = maskType(0.0);
	  for (i=-cmask.GetHalfWidth(); i<=cmask.GetHalfWidth(); i++) {
	    d += cmask.mask[i] *  maskType(tempr[i].y) ;
	  }
	  //	  resultr->y = outType(Round(d));
	  resultr->y = outType(d)+outType(0.5f);
	}
      }
      break;
    } // endswitch
    
  } else {
    if (VERBOSE) cerr << "                      (with subsampling)" << endl;
   // Subsample while convolving
    switch(cmask.Symmetry()) {
    case EVEN_SYMMETRY:
      if (VERBOSE) cerr << "                      (EVEN symmetry)" << endl;
      for (r=rnew0; r<rnewl; r+=subsampling) {
	resultr = result.GetRowStart(r/subsampling)+cnew0/subsampling;
	tempr   = temp.GetRowStart(r)+cnew0;
	for(c=cnew0; c<cnewl; c+=subsampling, resultr++, tempr+=subsampling) {
	  d = cmask.mask[0] * maskType(tempr[0].y);
	  // Now loop over one side
	  for (i=1; i<=cmask.GetHalfWidth(); i++) {
	    d += cmask.mask[i] * ( maskType(tempr[i].y) + maskType(tempr[-i].y ) );
	  }
	  //	  resultr->y = outType(Round(d));
	  resultr->y = outType(d)+outType(0.5f);
	}
      }
      break;
    case ODD_SYMMETRY:
      if (VERBOSE) cerr << "                      (ODD symmetry)" << endl;
      for (r=rnew0; r<rnewl; r+=subsampling) {
	resultr = result.GetRowStart(r/subsampling)+cnew0/subsampling;
	tempr   = temp.GetRowStart(r)+cnew0;
	for(c=cnew0; c<cnewl; c+=subsampling, resultr++, tempr+=subsampling) {
	  d = maskType(0.0);  // yes the central value is zero!
	  // Now loop over one side
	  for (i=1; i<=cmask.GetHalfWidth(); i++) {
	    d += cmask.mask[i] * ( maskType(tempr[i].y) - maskType(tempr[-i].y) );
	  }
	  //	  resultr->y = outType(Round(d));
	  resultr->y = outType(d)+outType(0.5f);
	}
      }
      break;
    default: // NO_SYMMETRY
      if (VERBOSE) cerr << "                      (NO symmetry)" << endl;
      for (r=rnew0; r<rnewl; r+=subsampling) {
	resultr = result.GetRowStart(r/subsampling)+cnew0/subsampling;
	tempr   = temp.GetRowStart(r)+cnew0;
	for(c=cnew0; c<cnewl; c+=subsampling, resultr++, tempr+=subsampling) {
	  d = maskType(0.0);
	  for (i=-cmask.GetHalfWidth(); i<=cmask.GetHalfWidth(); i++) {
	    d += cmask.mask[i] * maskType(tempr[i].y);
	  }
	  //	  resultr->y = outType(Round(d));
	  resultr->y = outType(d)+outType(0.5f);
	}
      }
      break;
    } //endswitch
  } //fi
   if (VERBOSE) cerr << "Convolve::Compute: Done." << endl;
  return;
}

//////////////////////////////////////////////////////////////////
//
//  Y   Y   U   U   V   V
//  Y   Y   U   U   V   V
//   YYY    U   U   V   V
//    Y     U   U    V V
//    Y      UUU      V

template <class maskType>
template <class inType, class outType>
void Convolve<maskType>::Compute(const ImageYUV<inType> &im,
				 ImageYUV<outType> &result,
				 const unsigned int subsampling)
{
  int c0, r0, cl, rl;
  int cnew0, rnew0, cnewl, rnewl;
  int r, c, i;
  
  // Check image sizes
  if ( (result.GetWidth()!=im.GetWidth()/subsampling)
       || (result.GetHeight()!=im.GetHeight()/subsampling)) {
    cerr << "Convolve::Compute: Image sizes incorrect - ("
	 <<result.GetWidth()<<","
	 <<result.GetHeight()<<") != ("
	 <<im.GetWidth()<<","<<im.GetHeight()<<")/"<<subsampling<<"."<<endl;
    return;
  }


  if (VERBOSE) cerr << "Convolve::Compute: Set ROI" << endl;
  im.ROI().Get(c0, r0, cl, rl);
  cnew0=c0+cmask.GetHalfWidth(); rnew0=r0+rmask.GetHalfWidth();
  cnewl=cl-cmask.GetHalfWidth(); rnewl=rl-rmask.GetHalfWidth();
  result.ROI().Set(cnew0/subsampling, rnew0/subsampling, 
		      cnewl/subsampling, rnewl/subsampling);

  if (VERBOSE) cerr << "Convolve::Compute: Create temporary image" << endl;
  if (VERBOSE) cerr << "                     (size "<<im.GetWidth()<<"x"<<im.GetHeight()<<")" << endl;
  ImageYUV<maskType> temp(im.GetWidth(), im.GetHeight());

  // Get pointers to input and output data
  typename ImageYUV<inType>::PixelPtr  *imr;
  typename ImageYUV<outType>::PixelPtr resultr;
  typename ImageYUV<maskType>::PixelPtr tempr;
  maskType dy,du,dv,da;
  
  if (VERBOSE) cerr << "Convolve::Compute: First pass" << endl;
  // Whether or not subsampling create the intermediate image
  // at full resolution
  switch(rmask.Symmetry()) {
  case EVEN_SYMMETRY:
      if (VERBOSE) cerr << "                      (EVEN symmetry)" << endl;
    for (r=rnew0; r<rnewl; r++) {
      imr = im.GetRowPointer(r);
      tempr = temp.GetRowStart(r)+c0;
      for (c=c0; c<cl; c++, tempr++) {
	tempr->y = rmask.mask[0] * maskType(imr[0][c].y);
	tempr->u = rmask.mask[0] * maskType(imr[0][c].u);
	tempr->v = rmask.mask[0] * maskType(imr[0][c].v);
	tempr->alpha = rmask.mask[0] * maskType(imr[0][c].alpha);
        // Now loop over one side
	for (i=1; i<=rmask.GetHalfWidth(); i++) {
	  tempr->y += rmask.mask[i] * ( maskType(imr[i][c].y)+maskType(imr[-i][c].y) );
	  tempr->u += rmask.mask[i] * ( maskType(imr[i][c].u)+maskType(imr[-i][c].u) );
	  tempr->v += rmask.mask[i] * ( maskType(imr[i][c].v)+maskType(imr[-i][c].v) );
	  tempr->alpha += rmask.mask[i] * ( maskType(imr[i][c].alpha)+maskType(imr[-i][c].alpha) );
	}
// 	  cerr << c << ": " << tempr->y << ", " << tempr->u << ", " << tempr->v << ", " << tempr->alpha << endl;
      }
    }
    break;
  case ODD_SYMMETRY:
      if (VERBOSE) cerr << "                      (ODD symmetry)" << endl;
    for (r=rnew0; r<rnewl; r++) {
      imr = im.GetRowPointer(r);
      tempr = temp.GetRowStart(r)+c0;
      for (c=c0; c<cl; c++, tempr++) {
	tempr->y = maskType(0.0); // yes the central value is zero!
	tempr->u = maskType(0.0);
	tempr->v = maskType(0.0);
	tempr->alpha = maskType(0.0);
        // Now loop over one side
	for (i=1; i<=rmask.GetHalfWidth(); i++) {
	  tempr->y += rmask.mask[i] * ( maskType(imr[i][c].y)-maskType(imr[-i][c].y) );
	  tempr->u += rmask.mask[i] * ( maskType(imr[i][c].u)-maskType(imr[-i][c].u) );
	  tempr->v += rmask.mask[i] * ( maskType(imr[i][c].v)-maskType(imr[-i][c].v) );
	  tempr->alpha += rmask.mask[i] * ( maskType(imr[i][c].alpha)-maskType(imr[-i][c].alpha) );
	}
      }
    }
    break;
  default: // NO_SYMMETRY
      if (VERBOSE) cerr << "                      (NO symmetry)" << endl;
    for (r=rnew0; r<rnewl; r++) {
      imr = im.GetRowPointer(r);
      tempr = temp.GetRowStart(r)+c0;
      for (c=c0; c<cl; c++, tempr++) {
	tempr->y = maskType(0.0);
	tempr->u = maskType(0.0);
	tempr->v = maskType(0.0);
	tempr->alpha = maskType(0.0);
	for (i=-rmask.GetHalfWidth(); i<=rmask.GetHalfWidth(); i++) {
	  tempr->y += rmask.mask[i] * maskType(imr[i][c].y);
	  tempr->u += rmask.mask[i] * maskType(imr[i][c].u);
	  tempr->v += rmask.mask[i] * maskType(imr[i][c].v);
	  tempr->alpha += rmask.mask[i] * maskType(imr[i][c].alpha);
	}
      }
    }
    break;
  }

  // Now the other direction
  // Here we must consider subsampling

   if (VERBOSE) cerr << "Convolve::Compute: Second pass" << endl;
 if (subsampling==1) {
   if (VERBOSE) cerr << "                      (no subsampling)" << endl;
    
    switch(cmask.Symmetry()) {
    case EVEN_SYMMETRY:
      if (VERBOSE) cerr << "                      (EVEN symmetry)" << endl;
      for (r=rnew0; r<rnewl; r++) {
	resultr = result.GetRowStart(r)+cnew0;
	tempr   = temp.GetRowStart(r)+cnew0;
	for(c=cnew0; c<cnewl; c++, resultr++, tempr++) {
	  dy = cmask.mask[0] * maskType(tempr[0].y);
	  du = cmask.mask[0] * maskType(tempr[0].u);
	  dv = cmask.mask[0] * maskType(tempr[0].v);
	  da = cmask.mask[0] * maskType(tempr[0].alpha);
	  // Now loop over one side
	  for (i=1; i<=cmask.GetHalfWidth(); i++) {
	    dy += cmask.mask[i] * ( maskType(tempr[i].y) + maskType(tempr[-i].y) );
	    du += cmask.mask[i] * ( maskType(tempr[i].u) + maskType(tempr[-i].u) );
	    dv += cmask.mask[i] * ( maskType(tempr[i].v) + maskType(tempr[-i].v) );
	    da += cmask.mask[i] * ( maskType(tempr[i].alpha) + maskType(tempr[-i].alpha) );
	  }
	  //	  resultr->y = outType(Round(d));
	  resultr->y = outType(dy)+outType(0.5f);
	  resultr->u = outType(du)+outType(0.5f);
	  resultr->v = outType(dv)+outType(0.5f);
	  resultr->alpha = outType(da)+outType(0.5f);
	}
      }
      break;
    case ODD_SYMMETRY:
      if (VERBOSE) cerr << "                      (ODD symmetry)" << endl;
      for (r=rnew0; r<rnewl; r++) {
	resultr = result.GetRowStart(r)+cnew0;
	tempr   = temp.GetRowStart(r)+cnew0;
	for(c=cnew0; c<cnewl; c++, resultr++, tempr++) {
	  dy = maskType(0.0);  // yes the central value is zero!
	  du = maskType(0.0); 
	  dv = maskType(0.0); 
	  da = maskType(0.0); 
	  // Now loop over one side
	  for (i=1; i<=cmask.GetHalfWidth(); i++) {
	    dy += cmask.mask[i] * ( maskType(tempr[i].y) - maskType(tempr[-i].y) );
	    du += cmask.mask[i] * ( maskType(tempr[i].u) - maskType(tempr[-i].u) );
	    dv += cmask.mask[i] * ( maskType(tempr[i].v) - maskType(tempr[-i].v) );
	    da += cmask.mask[i] * ( maskType(tempr[i].alpha) - maskType(tempr[-i].alpha) );
	  }
	  //	  resultr->y = outType(Round(d));
	  resultr->y = outType(dy)+outType(0.5f);
	  resultr->u = outType(du)+outType(0.5f);
	  resultr->v = outType(dv)+outType(0.5f);
	  resultr->alpha = outType(da)+outType(0.5f);
	}
      }
      break;
    default: // NO_SYMMETRY
      if (VERBOSE) cerr << "                      (NO symmetry)" << endl;
      for (r=rnew0; r<rnewl; r++) {
	resultr = result.GetRowStart(r)+cnew0;
	tempr   = temp.GetRowStart(r)+cnew0;
	for(c=cnew0; c<cnewl; c++, resultr++, tempr++) {
	  dy = maskType(0.0);
	  du = maskType(0.0);
	  dv = maskType(0.0);
	  da = maskType(0.0);
	  for (i=-cmask.GetHalfWidth(); i<=cmask.GetHalfWidth(); i++) {
	    dy += cmask.mask[i] *  maskType(tempr[i].y) ;
	    du += cmask.mask[i] *  maskType(tempr[i].u) ;
	    dv += cmask.mask[i] *  maskType(tempr[i].v) ;
	    da += cmask.mask[i] *  maskType(tempr[i].alpha) ;
	  }
	  //	  resultr->y = outType(Round(d));
	  resultr->y = outType(dy)+outType(0.5f);
	  resultr->u = outType(du)+outType(0.5f);
	  resultr->v = outType(dv)+outType(0.5f);
	  resultr->alpha = outType(da)+outType(0.5f);
	}
      }
      break;
    } // endswitch
    
  } else {
    if (VERBOSE) cerr << "                      (with subsampling)" << endl;
   // Subsample while convolving
    switch(cmask.Symmetry()) {
    case EVEN_SYMMETRY:
       if (VERBOSE) cerr << "                      (EVEN symmetry)" << endl;
     for (r=rnew0; r<rnewl; r+=subsampling) {
	resultr = result.GetRowStart(r/subsampling)+cnew0/subsampling;
	tempr   = temp.GetRowStart(r)+cnew0;
	for(c=cnew0; c<cnewl; c+=subsampling, resultr++, tempr+=subsampling) {
	  dy = cmask.mask[0] * maskType(tempr[0].y);
	  du = cmask.mask[0] * maskType(tempr[0].u);
	  dv = cmask.mask[0] * maskType(tempr[0].v);
	  da = cmask.mask[0] * maskType(tempr[0].alpha);
	  // Now loop over one side
	  for (i=1; i<=cmask.GetHalfWidth(); i++) {
	    dy += cmask.mask[i] * ( maskType(tempr[i].y) + maskType(tempr[-i].y ) );
	    du += cmask.mask[i] * ( maskType(tempr[i].u) + maskType(tempr[-i].u ) );
	    dv += cmask.mask[i] * ( maskType(tempr[i].v) + maskType(tempr[-i].v ) );
	    da += cmask.mask[i] * ( maskType(tempr[i].alpha) + maskType(tempr[-i].alpha ) );
	  }
	  resultr->y = outType(dy)+outType(0.5f);
	  resultr->u = outType(du)+outType(0.5f);
	  resultr->v = outType(dv)+outType(0.5f);
	  resultr->alpha = outType(da)+outType(0.5f);
	}
      }
      break;
    case ODD_SYMMETRY:
       if (VERBOSE) cerr << "                      (ODD symmetry)" << endl;
      for (r=rnew0; r<rnewl; r+=subsampling) {
	resultr = result.GetRowStart(r/subsampling)+cnew0/subsampling;
	tempr   = temp.GetRowStart(r)+cnew0;
	for(c=cnew0; c<cnewl; c+=subsampling, resultr++, tempr+=subsampling) {
	  dy = maskType(0.0);  // yes the central value is zero!
	  du = maskType(0.0);
	  dv = maskType(0.0);
	  da = maskType(0.0);
	  // Now loop over one side
	  for (i=1; i<=cmask.GetHalfWidth(); i++) {
	    dy += cmask.mask[i] * ( maskType(tempr[i].y) - maskType(tempr[-i].y) );
	    du += cmask.mask[i] * ( maskType(tempr[i].u) - maskType(tempr[-i].u) );
	    dv += cmask.mask[i] * ( maskType(tempr[i].v) - maskType(tempr[-i].v) );
	    da += cmask.mask[i] * ( maskType(tempr[i].alpha) - maskType(tempr[-i].alpha) );
	  }
	  //	  resultr->y = outType(Round(d));
	  resultr->y = outType(dy)+outType(0.5f);
	  resultr->u = outType(du)+outType(0.5f);
	  resultr->v = outType(dv)+outType(0.5f);
	  resultr->alpha = outType(da)+outType(0.5f);
	}
      }
      break;
    default: // NO_SYMMETRY
       if (VERBOSE) cerr << "                      (NO symmetry)" << endl;
      for (r=rnew0; r<rnewl; r+=subsampling) {
	resultr = result.GetRowStart(r/subsampling)+cnew0/subsampling;
	tempr   = temp.GetRowStart(r)+cnew0;
	for(c=cnew0; c<cnewl; c+=subsampling, resultr++, tempr+=subsampling) {
	  dy = maskType(0.0);
	  du = maskType(0.0);
	  dv = maskType(0.0);
	  da = maskType(0.0);
	  for (i=-cmask.GetHalfWidth(); i<=cmask.GetHalfWidth(); i++) {
	    dy += cmask.mask[i] * maskType(tempr[i].y);
	    du += cmask.mask[i] * maskType(tempr[i].u);
	    dv += cmask.mask[i] * maskType(tempr[i].v);
	    da += cmask.mask[i] * maskType(tempr[i].alpha);
	  }
	  //	  resultr->y = outType(Round(d));
	  resultr->y = outType(dy)+outType(0.5f);
	  resultr->u = outType(du)+outType(0.5f);
	  resultr->v = outType(dv)+outType(0.5f);
	  resultr->alpha = outType(da)+outType(0.5f);
	}
      }
      break;
    } //endswitch
  } //fi
   if (VERBOSE) cerr << "Convolve::Compute: Done." << endl;
  return;
}


//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// Instantiate floating point convolution masks

// ints and float are fastest.
// Exclude doubles (too slow) and chars (too slow AND risky)
template class ConvolutionMask1D<int>;
template class ConvolutionMask1D<float>;
template class Convolve<int>;
template class Convolve<float>;




#ifdef INSTANTIATE
#  undef INSTANTIATE
#endif
// M = computation type (usually float or double)
// N = input pixel datatype
// O = output pixel datatype
# define INSTANTIATE(M, N, O)				       \
  template void Convolve<M>::Compute<N,O>(const ImageMono<N> &, \
					  ImageMono<O> &, \
					  const unsigned int); \
  template void Convolve<M>::Compute<N,O>(const ImageYUV<N> &, \
					  ImageYUV<O> &, \
					  const unsigned int);

//////////////////////////////////////////////////////////
// Instantiate float->float,  
//             unsigned char->float and 
//             unsigned char->unsigned char convolution.
// Note that the last truncates rather than rounds, but a floating point 
// version is available.

INSTANTIATE(float,float,float); 
INSTANTIATE(float,unsigned char,float); 
INSTANTIATE(int,unsigned char,unsigned char); 
INSTANTIATE(float,unsigned char,unsigned char); 

#undef INSTANTIATE


}
