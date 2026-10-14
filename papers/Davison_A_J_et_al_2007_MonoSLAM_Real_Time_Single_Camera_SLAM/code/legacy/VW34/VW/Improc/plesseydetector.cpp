/** 
 * @file    plesseydetector.cpp
 * @author  ID Reid and BJ Tordoff
 * @date    02/08/2002
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

#include <VW/Image/imageyuv.h>
#include <VW/Image/imagemono.h>

#include "convolve.h"
#include "plesseydetector.h"

#ifdef VERBOSE
#   undef VERBOSE
#endif
#define VERBOSE false
/// Urggh, nasty defines

#define ABS(a) ((a) < (0) ? (-a) : (a))
#define SIGN(a) ((a) < (0) ? (-1) : (1))

//////////////////////////////////////////////////////////////////

using namespace std;
using namespace VW;


static inline float
SignedFourthRoot(float value)
{
  if (value<0) return -sqrt(sqrt(-value));
  return sqrt(sqrt(value));
}

static inline void 
FitQuadratic3x3(float& peak_value, 
		float& peak_x, float& peak_y,
		const float data[9])
{
  // Fit a bi-quadratic patch to data, which is arranged row-first:
  //  data[0] data[1] data[2]
  //  data[3]   etc...
  // The values returned are offsets from the central position, 
  // which is taken to be 0,0.

  // Quadratic is calculated from a 3x3 grid around the point:
  //
  //             j-1    j    j+1
  //           -------------------
  //       sim |     |     |     |
  //           -------------------
  //       si  |     |     |     |
  //           -------------------
  //       sip |     |     |     |
  //           -------------------
  //
  // If the corner strengths at the current pixel is S(x0,y0), 
  // and at the maximum is S(xm,ym) the equation is:
  //
  //  S(x0+u,y0+u) = 0.5 * (u v) | A  C/2 | |u| + (D E) |u| + F
  //                             | C/2  B | |v|         |v|
  //
  // where:
  //
  //       d^2(S)       d^2(S)        d^2(S) 
  //   A = ------   B = ------   C = -------- 
  //       d(x)^2       d(y)^2       d(x)d(y)
  //
  //       d(S)         d(S)
  //   D = ----     E = ----     F = S(x,y)
  //       d(x)         d(y)
  //
  // evaluated at x=x0, y=y0.
  // 
  // The solution is then obtained using the fact that the gradient 
  // is zero at the maximum, ie:
  // 
  //  |u| = -1 * | A  C/2 |^(-1) | D |
  //  |v|        | C/2  B |      | E |

  // Gradients are calculated using the 1st and second order Sobel filters
  float A,B,C,D,E;

  A =    (     data[0]  - 2*data[1]  +   data[2]
	   + 2*data[3]  - 4*data[4]  + 2*data[5]
	   +   data[6]  - 2*data[7]  +   data[8]  ) / 8.0f; // d^2I / dx^2


  B =    (  ( data[0]  + 2*data[1]  + data[2]  )
	- 2*( data[3]  + 2*data[4]  + data[5]  )
	+   ( data[6]  + 2*data[7]  + data[8]  )) / 8.0f; // d^2I / dy^2

  C = (  + data[0]  - data[2] 
	 - data[6]  + data[8]  ) / 4.0f;                // d^2I / dxdy

  D = ( - data[0]  - 2*data[3]  - data[6]
	+ data[2]  + 2*data[5]  + data[8]  ) / 8.0f;      // dI / dx

  E = ( - data[0]  - 2*data[1]  - data[2] 
	+ data[6]  + 2*data[7]  + data[8]  ) / 8.0f;      // dI / dy

  if (VERBOSE)
 cerr << "   Quadfit: (A,B,C,D,E,F)=("
		    <<A<<","<<B<<","
		    <<C<<","<<D<<","
		    <<E<<","<<data[4]<<")"<<endl;

  // Calculate the inverse of the determinant
  float detinv = 1.0f/(A*B - 0.25f*C*C);

  // Calculate peak position and value
  peak_x = -1* (B*D - 0.5f*C*E) * detinv; // X-Offset of quadratic peak
  peak_y = -1* (A*E - 0.5f*C*D) * detinv; // Y-Offset of quadratic peak


  // When the feature has weak gradient in one direction, bad shit
  // happens, but the most this fit should move the peak is half
  // way to a neighbouring pixel (otherwise it would be higher).
  if (peak_x>=0.5) peak_x=0.49999f;
  if (peak_x<-0.5) peak_x=-0.5f;
  if (peak_y>=0.5) peak_y=0.49999f;
  if (peak_y<-0.5) peak_y=-0.5f;
  peak_value =
    0.5f*(A*peak_x*peak_x + B*peak_y*peak_y) + C*peak_x*peak_y // Quadratic part
    + D*peak_x        + E*peak_y                           // Linear part
    + data[4];                                             // Constant part
  //cerr << "Peak location ("<<peak_x<<","<<peak_y<<") value " << peak_value << endl;
}



/* Find the highest achievable score given this mask
 * Because of discretization, truncation etc. it is
 * practically impossible to work out the actual limit
 * of corner strength theoretically, so instead
 * I actually do the convolution on a step corner.
 */
static inline float 
GetMaxStrength(const ConvolutionMask1D<float> gauss_mask,
	       const float edge_phobia)
{
  const float gradient_max=255.0; // for unsigned char
  //  const float gradient_max=1.0; // for float
  //  const float gradient_max=1.0; // for double

  // Do a quick convolution of a step corner
  // Using the fact that the gradient response to a step corner is:
  //  I = 0 0 0 0 0 0  Gxx = 0 0 0 0 0 0  Gyy = 0 0 0 0 0 0  Gxy = 0 0 0 0 0 0
  //      0 0 0 0 0 0        0 0 0 0 0 0        0 0 0 0 0 0        0 0 0 0 0 0
  //      0 0 0 0 0 0        0 0 0 0 0 0        0 0 0 1 1 1        0 0 0 0 0 0
  //      0 0 0 1 1 1        0 0 1 1 0 0        0 0 0 1 1 1        0 0 0 1 0 0
  //      0 0 0 1 1 1        0 0 1 1 0 0        0 0 0 0 0 0        0 0 0 0 0 0
  //      0 0 0 1 1 1        0 0 1 1 0 0        0 0 0 0 0 0        0 0 0 0 0 0
  //
  // the convolution can be written out to save computation. The peak strength
  // is gauranteed to lie within half_width of the corner position, so only a
  // small patch need be considered.
  // The summations below calculate the the smoothed version starting at
  // the (3x3) position of the above.

  float colsum=0,rowsum=0;
  const int halfwidth=gauss_mask.GetHalfWidth()+1;
  const int arrsize=halfwidth+1;
  float* kernel = new float[2*halfwidth+1]; // Need to create a copy to pad with zeros
  for (int i=-gauss_mask.GetHalfWidth();i<=gauss_mask.GetHalfWidth();i++) kernel[i+halfwidth]=gauss_mask.mask[i];
  kernel[0]=0;kernel[2*halfwidth]=0;
  float* gyy = new float[arrsize*arrsize];
  float* gxy = new float[arrsize*arrsize];
  float* score = new float[arrsize*arrsize];

  float* gyyptr=&(gyy[0]);
  float* gxyptr=&(gxy[0]);
  float* scoreptr=&(score[0]);

  // Get dy * dy (=transpose(dx * dx))
  for (int y=0;y<arrsize;y++) {
    // Sum the kernel down the column (all elements that are multiplied by 1)
    colsum=0;
    for (int k=0;k<halfwidth+y;k++) colsum+=kernel[k];
    for (int x=0;x<arrsize;x++) {
      // Only two elements across the row lie on top of the ones
      rowsum = kernel[x+halfwidth-1] + kernel[x+halfwidth];
      *gyyptr = rowsum * colsum;
      gyyptr++;
    }
  }

  // Get dx * dy
  for (int y=0;y<arrsize;y++) {
    // Only one element lies over the 1
    colsum = kernel[y+halfwidth-1];
    for (int x=0;x<arrsize;x++) {
      // Only one element lies over the 1
      rowsum = kernel[x+halfwidth-1];
      *gxyptr = rowsum * colsum;
      gxyptr++;
    }
  }


  // Now calc raw scores
  scoreptr=&(score[0]);
  float max_score=0;
  int maxx=0,maxy=0;
  for (int y=0;y<arrsize;y++) {
    for (int x=0;x<arrsize;x++) {
      const float det = (gyy[y*arrsize+x] * gyy[x*arrsize+y]) - (gxy[y*arrsize+x] * gxy[y*arrsize+x]);
      const float trace = gyy[y*arrsize+x] + gyy[x*arrsize+y];
      *scoreptr = det - 0.25f*edge_phobia*trace*trace;
      if (*scoreptr > max_score) {
	max_score=*scoreptr;
	maxx=x;maxy=y;
      }
      scoreptr++;
    }
  }


  // If not on the edge, fit the quadratic
  if ((maxx!=0) && (maxx!=arrsize-1) && (maxy!=0) && (maxy!=arrsize-1) && (max_score>0)) {

    float data[9];
    data[0]=SignedFourthRoot(score[(maxy-1)*arrsize+(maxx-1)]);
    data[1]=SignedFourthRoot(score[(maxy-1)*arrsize+(maxx)]);
    data[2]=SignedFourthRoot(score[(maxy-1)*arrsize+(maxx+1)]);
    data[3]=SignedFourthRoot(score[(maxy)*arrsize+(maxx-1)]);
    data[4]=SignedFourthRoot(score[(maxy)*arrsize+(maxx)]);
    data[5]=SignedFourthRoot(score[(maxy)*arrsize+(maxx+1)]);
    data[6]=SignedFourthRoot(score[(maxy+1)*arrsize+(maxx-1)]);
    data[7]=SignedFourthRoot(score[(maxy+1)*arrsize+(maxx)]);
    data[8]=SignedFourthRoot(score[(maxy+1)*arrsize+(maxx+1)]);

//     cerr << "   Score Data: "<<endl;
//     cerr << "     " << data[0] <<", "<<data[1]<<", "<<data[2]<<endl;
//     cerr << "     " << data[3] <<", "<<data[4]<<", "<<data[5]<<endl;
//     cerr << "     " << data[6] <<", "<<data[7]<<", "<<data[8]<<endl;

    // Fit quadratic patch
    float peak_value=0,peak_x=0,peak_y=0;

    FitQuadratic3x3(peak_value,peak_x,peak_y,data);

    max_score=peak_value;
  } else {
    // This means that something odd has happened. If the score is
    // zero, it is likely that no smoothing is being used, so the max
    // gradient is 1, otherwise who knows?
    if (max_score==0) {
      max_score=1.0;
    } else {
      cerr << "PlesseyDetector: ** Error in computing max score - somthing odd going down!" << endl;
    float data[9];
    data[0]=sqrt(sqrt(score[(maxy-1)*arrsize+(maxx-1)]));
    data[1]=sqrt(sqrt(score[(maxy-1)*arrsize+(maxx)]));
    data[2]=sqrt(sqrt(score[(maxy-1)*arrsize+(maxx+1)]));
    data[3]=sqrt(sqrt(score[(maxy)*arrsize+(maxx-1)]));
    data[4]=sqrt(sqrt(score[(maxy)*arrsize+(maxx)]));
    data[5]=sqrt(sqrt(score[(maxy)*arrsize+(maxx+1)]));
    data[6]=sqrt(sqrt(score[(maxy+1)*arrsize+(maxx-1)]));
    data[7]=sqrt(sqrt(score[(maxy+1)*arrsize+(maxx)]));
    data[8]=sqrt(sqrt(score[(maxy+1)*arrsize+(maxx+1)]));
    cerr << "   Score Data: "<<endl;
    cerr << "     " << data[0] <<", "<<data[1]<<", "<<data[2]<<endl;
    cerr << "     " << data[3] <<", "<<data[4]<<", "<<data[5]<<endl;
    cerr << "     " << data[6] <<", "<<data[7]<<", "<<data[8]<<endl;
    }
  }
  
  const float result=gradient_max * max_score;
  
//   cerr << "Grad max = " << gradient_max << endl;
//   cerr << "Max strength = " << max_score << endl;
//   cerr << "Result = " << result << endl;

  delete[] score;
  delete[] gxy;
  delete[] gyy;
 delete[] kernel;



  return (result*result*result*result);
}


static inline float InsertFeature(std::vector<VW::CornerFeature>& features,
											 const VW::CornerFeature& feature,
											 unsigned int limit)
{
  const unsigned int num = features.size();
  const float this_score = feature.GetCornerStrength();
  if (num>0) {
	 // Find insertion point
	 float upper_score = features[0].GetCornerStrength();
	 float lower_score = features[num-1].GetCornerStrength();

	 if (this_score > upper_score) {
		// This is the new best, so add to front
		features.insert(features.begin(),feature);
	 } else if (this_score < lower_score) {
		// This is the new worst, so add to back
		if (num<limit) features.push_back(feature);
	 } else {
		// Somewhere in between, so find it
		int upper_index = 0;
		int lower_index = num-1;
		int region_size = lower_index - upper_index;
		// upper_index always points to an entry >this_score
		// lower_index always points to an entry <=this_score
		while (region_size>1) {
		  int mid_index = int(0.5*(upper_index + lower_index));
		  if (features[mid_index].GetCornerStrength() <= this_score) {
			 // We want top half
			 lower_index = mid_index;
		  } else {
			 // We want bottom half
			 upper_index = mid_index;
		  }
		  region_size = lower_index - upper_index;
		}
		features.insert(features.begin()+lower_index,feature);
	 }
  } else {
	 // Empty list, so just add
	 features.push_back(feature);
  }

  if (num>=limit) {
	 // list is overfull, so clip and return end value
	 features.resize(limit);
	 return features[num].GetCornerStrength();
  } else if(num==(limit-1)) {
	 // List is exactly filled, so return end value
	 return features[num].GetCornerStrength();
  }
  // Haven't reached limit yet, so any score is OK
  return 0.0f;
}

void PlesseyDetector::calc_gradients (const ImageMono<unsigned char> &im,
												  ImageMono<float> &gx2arr,
												  ImageMono<float> &gy2arr,
												  ImageMono<float> &gxyarr) const
{
    if (VERBOSE) cerr << "VW::PlesseyDetector::calc_gradients(mono)" << endl;
 
  ImageMono<unsigned char>::PixelPtr pix0,pixm1,pixp1;
  int c0, r0, cl, rl;
  int     r, c;
  float   grady, gradx;
  ImageMono<float>::PixelPtr gx2r, gy2r, gxyr;

 
  // adjust border
  VW::ROI roi = im.ROI();
  // gradients reduce size one further
  roi.Shrink();
    
  gx2arr.SetROI(roi);
  gy2arr.SetROI(roi);
  gxyarr.SetROI(roi);
    
  // Now store the limits
  roi.Get(c0, r0, cl, rl);

  for(r=r0; r<rl; r++) {
    pixm1 = im.GetRowStart(r-1)+c0;
    pix0 = im.GetRowStart(r)+c0;
    pixp1 = im.GetRowStart(r+1)+c0;
    gx2r = gx2arr.GetRowStart(r)+c0;
    gy2r = gy2arr.GetRowStart(r)+c0;
    gxyr = gxyarr.GetRowStart(r)+c0;
    for(c=c0; c<cl; c++, pix0++, pixp1++, pixm1++) {
      gradx = (float) (pix0[1].y - pix0[-1].y);
      grady = (float) (pixp1->y - pixm1->y);
      gx2r++->y = gradx*gradx;
      gy2r++->y = grady*grady;
      gxyr++->y = gradx*grady;
    }
  }  
}
void PlesseyDetector::calc_gradients (const ImageMono<float> &im,
			      ImageMono<float> &gx2arr,
			      ImageMono<float> &gy2arr,
			      ImageMono<float> &gxyarr) const
{
    if (VERBOSE) cerr << "VW::PlesseyDetector::calc_gradients(mono)" << endl;
 
  ImageMono<float>::PixelPtr pix0,pixm1,pixp1;
  int c0, r0, cl, rl;
  int     r, c;
  float   grady, gradx;
  ImageMono<float>::PixelPtr gx2r, gy2r, gxyr;

   // adjust border
    VW::ROI roi = im.ROI();
    // gradients reduce size one further
    roi.Shrink();
    
  gx2arr.SetROI(roi);
  gy2arr.SetROI(roi);
  gxyarr.SetROI(roi);
    
    // Now store the limits
    roi.Get(c0, r0, cl, rl);

  for(r=r0; r<rl; r++) {
    pixm1 = im.GetRowStart(r-1)+c0;
    pix0 = im.GetRowStart(r)+c0;
    pixp1 = im.GetRowStart(r+1)+c0;
    gx2r = gx2arr.GetRowStart(r)+c0;
    gy2r = gy2arr.GetRowStart(r)+c0;
    gxyr = gxyarr.GetRowStart(r)+c0;
    for(c=c0; c<cl; c++, pix0++, pixp1++, pixm1++) {
      gradx = (pix0[1].y - pix0[-1].y);
      grady = (pixp1->y - pixm1->y);
      gx2r++->y = gradx*gradx;
      gy2r++->y = grady*grady;
      gxyr++->y = gradx*grady;
    }
  }  
}
void PlesseyDetector::calc_gradients (const ImageYUV<unsigned char> &im,
			      ImageMono<float> &gx2arr,
			      ImageMono<float> &gy2arr,
			      ImageMono<float> &gxyarr) const
{
   if (VERBOSE) cerr << "VW::PlesseyDetector::calc_gradients(yuv)" << endl;
  
  unsigned char *pix0,*pixm1,*pixp1;
  int c0, r0, cl, rl;
  int     r, c;
  float   grady, gradx;
  ImageMono<float>::PixelPtr gx2r, gy2r, gxyr;

  const int bytes_per_pixel=4;

   // adjust border
    VW::ROI roi = im.ROI();
    // gradients reduce size one further
    roi.Shrink();
    
  gx2arr.SetROI(roi);
  gy2arr.SetROI(roi);
  gxyarr.SetROI(roi);
    
    // Now store the limits
    roi.Get(c0, r0, cl, rl);
 

  for(r=r0; r<rl; r++) {
    pixm1 = &(im.GetRowStart(r-1)->y) + c0*bytes_per_pixel;
    pix0 = &(im.GetRowStart(r)->y) + c0*bytes_per_pixel;
    pixp1 = &(im.GetRowStart(r+1)->y) + c0*bytes_per_pixel;
    gx2r = gx2arr.GetRowStart(r)+c0;
    gy2r = gy2arr.GetRowStart(r)+c0;
    gxyr = gxyarr.GetRowStart(r)+c0;
    for(c=c0; 
	c<(cl*bytes_per_pixel); 
	c+=bytes_per_pixel, pix0+=bytes_per_pixel, 
	  pixp1+=bytes_per_pixel, pixm1+=bytes_per_pixel) {
      gradx = float (pix0[bytes_per_pixel] - pix0[-bytes_per_pixel]);
      grady = float (*pixp1 - *pixm1);
      gx2r++->y = gradx*gradx;
      gy2r++->y = grady*grady;
      gxyr++->y = gradx*grady;
    }
  }  
}

//////////////////////////////////////////////////////////////////

void PlesseyDetector::compute_corner_strength ( const ImageMono<float> &gradx2,
					const ImageMono<float> &grady2,
					const ImageMono<float> &gradxy,
					ImageMono<float> &corn_strength ) const
{
   if (VERBOSE) cerr << "VW::PlesseyDetector::compute_corner_strength" << endl;
   int r, c;
  int c0, r0, cl, rl;
  float trace, det;

  gradx2.ROI().Get(c0, r0, cl, rl);
  corn_strength.ROI().Set(c0, r0, cl, rl);

  for (r=r0; r<rl; r++ ) {
    for (c=c0; c<cl; c++) {
      det = gradx2.GetPixel(c,r).y*grady2.GetPixel(c,r).y - gradxy.GetPixel(c,r).y*gradxy.GetPixel(c,r).y;
      trace = gradx2.GetPixel(c,r).y + grady2.GetPixel(c,r).y;
      corn_strength.SetPixel(c,r, det - 0.25f*_edge_phobia*(trace*trace));
    }
  }
}

//////////////////////////////////////////////////////////////////

void PlesseyDetector::compute_best_corner ( const ImageMono<float> &gradx2,
				    const ImageMono<float> &grady2,
				    const ImageMono<float> &gradxy,
				    CornerFeature& best_corner) const
{
   if (VERBOSE) cerr << "VW::PlesseyDetector::compute_corner_strength" << endl;
   int r, c;
  int c0, r0, cl, rl;
  float trace, det;

  gradx2.ROI().Get(c0, r0, cl, rl);

  float best_score = 0.0f, best_x = 0.0f, best_y = 0.0f;

  for (r=r0; r<rl; r++ ) {
    for (c=c0; c<cl; c++) {
      det = gradx2.GetPixel(c,r).y*grady2.GetPixel(c,r).y - gradxy.GetPixel(c,r).y*gradxy.GetPixel(c,r).y;
      trace = gradx2.GetPixel(c,r).y + grady2.GetPixel(c,r).y;
      const float score = det - 0.25f*_edge_phobia*(trace*trace);
      if (score>best_score) {
	best_score = score;
	best_x = float(c);
	best_y = float(r);
      }
    }
  }
  best_corner.Set(best_x,best_y,best_score,1.0);
}

//////////////////////////////////////////////////////////////////
void PlesseyDetector::compute_corners(const ImageMono<float> &strength, 
												  const float score_normalisation,
												  const float raw_threshold,
												  const float image_scale,
												  vector<CornerFeature>& features) const
{
  if (VERBOSE) cerr << "VW::PlesseyDetector::compute_corners" << endl;
  int i,j;
  int c0, r0, cl, rl;
  int cnew0, rnew0, cnewl, rnewl;
  ImageMono<float>::PixelPtr si, sim1, sip1, sim2, sip2;
  float s;

  strength.ROI().Get(c0, r0, cl, rl);
  cnew0=c0+2; rnew0=r0+2;
  cnewl=cl-2; rnewl=rl-2;

  // If a limit has been set, respect it
  if (_max_features>0) {
	 // If adding to an existing list, check the current min score
	 float min_score=0.0f;
	 if (features.size() >= _max_features) {
		min_score = features[_max_features-1].GetCornerStrength();
	 }
	 for (i=rnew0; i<rnewl; i++) {
		sim2 = strength.GetRowStart(i-2);
		sim1 = strength.GetRowStart(i-1);
		si   = strength.GetRowStart(i);
		sip1 = strength.GetRowStart(i+1);
		sip2 = strength.GetRowStart(i+2);
    
		for(j=cnew0; j<cnewl; j++) {
		  s = si[j].y;

		  if (s>raw_threshold &&
				//5x5 neighbourhood: Check the closest first
				s>sim1[j].y   && s>sip1[j].y   && s>si[j-1].y   && s>si[j+1].y   &&
				s>sim1[j-1].y && s>sim1[j+1].y && s>sip1[j-1].y && s>sip1[j+1].y &&
				s>sim1[j-2].y && s>sim1[j+2].y &&
				s>si[j-2].y   && s>si[j+2].y   &&
				s>sip1[j-2].y && s>sip1[j+2].y &&
				s>sim2[j-2].y && s>sim2[j-1].y && s>sim2[j].y   && s>sim2[j+1].y && s>sim2[j+2].y &&
				s>sip2[j-2].y && s>sip2[j-1].y && s>sip2[j].y   && s>sip2[j+1].y && s>sip2[j+2].y ) {

			 // Calculate position to sub-pixel accuracy using quadratic fit
			 { // Some variables local to this calculation
				float x,y,st;           // Stuff for the sub-pixel corner

				// Edge phobia can result in negative strength, so need abs and sign
				// defined at the top of the file
				// Scores for minus row
				float data[9];
				data[0]=SignedFourthRoot(sim1[j-1].y*score_normalisation);
				data[1]=SignedFourthRoot(sim1[j  ].y*score_normalisation);
				data[2]=SignedFourthRoot(sim1[j+1].y*score_normalisation);
				// Scores for zero row
				data[3]=SignedFourthRoot(si[j-1].y*score_normalisation);
				data[4]=SignedFourthRoot(s        *score_normalisation);
				data[5]=SignedFourthRoot(si[j+1].y*score_normalisation);
				// Scores for plus row
				data[6]=SignedFourthRoot(sip1[j-1].y*score_normalisation);
				data[7]=SignedFourthRoot(sip1[j  ].y*score_normalisation);
				data[8]=SignedFourthRoot(sip1[j+1].y*score_normalisation);
	  
				if (VERBOSE) {
				  cerr << "Normalised scores:" << endl;
				  cerr << data[0] << "  " << data[1] << "  " << data[2] << endl;
				  cerr << data[3] << "  " << data[4] << "  " << data[5] << endl;
				  cerr << data[6] << "  " << data[7] << "  " << data[8] << endl;
				}

				// *** DISCLAIMER ***
				// The old code fitted a quadratic surface
				// to the raw strength values (which are in grad^4). I
				// have a suspision that it should be fitted to the
				// normalised strength values (which also gives
				// better numbers), which is what I do here.
				// It does cost some more computation though.
				//                                Ben
				FitQuadratic3x3(st,x,y,data);
				const float score = sqrt(sqrt(st));
				if (score>min_score) {
				  // Find array isertion point

				  // Correct for image position
				  x += j;          // Array coord
				  y += i;          // Array coord
				  const CornerFeature f(image_scale*x,
												image_scale*y,
												score,
												image_scale);
				  if (VERBOSE) {
					 cerr << "Basic corner ("<<j<<","<<i<<") strength="<<data[4]
							<<"    sub-pixel ("<<x<<","<<y<<") strength="<<st<<endl;
				  }
				  
				  // add corner into point list (using stl pushback routine)
				  // Note that the Plessey-Harris score is related to grad^4, so fourth root is 
				  // taken after normalization
				  min_score = InsertFeature(features,f,_max_features);
				}
			 }
		  }
		}
	 }
  } else {
	 // No limit on number, so just get them
	 for (i=rnew0; i<rnewl; i++) {
		sim2 = strength.GetRowStart(i-2);
		sim1 = strength.GetRowStart(i-1);
		si   = strength.GetRowStart(i);
		sip1 = strength.GetRowStart(i+1);
		sip2 = strength.GetRowStart(i+2);
    
		for(j=cnew0; j<cnewl; j++) {
		  s = si[j].y;

		  if (s>raw_threshold &&
				//5x5 neighbourhood: Check the closest first
				s>sim1[j].y   && s>sip1[j].y   && s>si[j-1].y   && s>si[j+1].y   &&
				s>sim1[j-1].y && s>sim1[j+1].y && s>sip1[j-1].y && s>sip1[j+1].y &&
				s>sim1[j-2].y && s>sim1[j+2].y &&
				s>si[j-2].y   && s>si[j+2].y   &&
				s>sip1[j-2].y && s>sip1[j+2].y &&
				s>sim2[j-2].y && s>sim2[j-1].y && s>sim2[j].y   && s>sim2[j+1].y && s>sim2[j+2].y &&
				s>sip2[j-2].y && s>sip2[j-1].y && s>sip2[j].y   && s>sip2[j+1].y && s>sip2[j+2].y ) {

			 // Calculate position to sub-pixel accuracy using quadratic fit



			 { // Some variables local to this calculation
				float x,y,st;           // Stuff for the sub-pixel corner
				// Edge phobia can result in negative strength, so need abs and sign
				// defined at the top of the file
				// Scores for minus row
				float data[9];
				data[0]=SignedFourthRoot(sim1[j-1].y*score_normalisation);
				data[1]=SignedFourthRoot(sim1[j  ].y*score_normalisation);
				data[2]=SignedFourthRoot(sim1[j+1].y*score_normalisation);
				// Scores for zero row
				data[3]=SignedFourthRoot(si[j-1].y*score_normalisation);
				data[4]=SignedFourthRoot(s        *score_normalisation);
				data[5]=SignedFourthRoot(si[j+1].y*score_normalisation);
				// Scores for plus row
				data[6]=SignedFourthRoot(sip1[j-1].y*score_normalisation);
				data[7]=SignedFourthRoot(sip1[j  ].y*score_normalisation);
				data[8]=SignedFourthRoot(sip1[j+1].y*score_normalisation);
	  
				// *** DISCLAIMER ***
				// The old code fitted a quadratic surface
				// to the raw strength values (which are in grad^4). I
				// have a suspision that it should be fitted to the
				// normalised strength values (which also gives
				// better numbers), which is what I do here.
				// It does cost some more computation though.
				//                                Ben
				FitQuadratic3x3(st,x,y,data);
				const float score = sqrt(sqrt(st));

				// Correct for image position
				x += j;          // Array coord
				y += i;          // Array coord

				if (VERBOSE) {
				  cerr << "Basic corner ("<<j<<","<<i<<") strength="<<data[4]
						 <<"    sub-pixel ("<<x<<","<<y<<") strength="<<st<<endl;
				}

				// add corner into point list (using stl pushback routine)
				// Note that the Plessey-Harris score is related to grad^4, so fourth root is 
				// taken after normalization
				const CornerFeature f(x,y,score,image_scale);
				features.push_back(f);
			 }
		  }
		}
	 }
  } // End if(limit)
  
  return;
}


//////////////////////////////////////////////////////////////////



void PlesseyDetector::Compute(const ImageMono<unsigned char> &im,  
										vector<CornerFeature>& features,
		      const VW::Normalisation& C) const
{
  if (VERBOSE) cerr << "VW::PlesseyDetector::Compute(imagemono)" << endl;

  if (!im.IsAlloced()) {
    cerr << "VW::PlesseyDetector: Image is not allocated!" << endl;
    return;
  }

 ConvolutionMask1D<float> gauss_mask(_patch_size);
 gauss_mask.MakeGaussian(_smoothing_sigma);

  int imwidth = im.GetWidth(),
      imheight = im.GetHeight(); 


  // Conversion from ordinary score to normalized one (0<=score<=1)
  const float strength_normalising_factor = 1.0f/GetMaxStrength(gauss_mask,_edge_phobia);
  // Use conversion to find the raw threshold, using the fact that Harris-Plessey operates on grad^4
  float t4=_strength_threshold;
  t4=t4*t4*t4*t4;
  const float raw_threshold = t4/strength_normalising_factor;

  ImageMono<float> gx2arr(imwidth, imheight);
  ImageMono<float> gy2arr(imwidth, imheight);  ImageMono<float> gxyarr(imwidth, imheight);
  ImageMono<float> sgx2arr(imwidth, imheight);
  ImageMono<float> sgy2arr(imwidth, imheight);
  ImageMono<float> sgxyarr(imwidth, imheight);
  ImageMono<float> corn_strength(imwidth, imheight);

  calc_gradients (im, gx2arr, gy2arr, gxyarr);

  if (VERBOSE) {
    for (int i=-gauss_mask.GetHalfWidth();i<=gauss_mask.GetHalfWidth();i++) {
      cerr << gauss_mask.mask[i] << ",";
    }
    cerr << endl;
  }

  Convolve<float>(gx2arr, gauss_mask, gauss_mask, sgx2arr);
  Convolve<float>(gy2arr, gauss_mask, gauss_mask, sgy2arr);
  Convolve<float>(gxyarr, gauss_mask, gauss_mask, sgxyarr);

  compute_corner_strength(sgx2arr, sgy2arr, sgxyarr, corn_strength);


  // Create the point list
  features.clear();
  compute_corners(corn_strength, 
						strength_normalising_factor, 
						raw_threshold, 
						1.0f, // Single image, so scale is 1.0
						features);
  C.Normalise (features);

  return;
}

void PlesseyDetector::Compute(const ImageMono<unsigned char> &im,  
		      CornerFeature& best_corner,
		      const VW::Normalisation& C) const
{
  if (VERBOSE) cerr << "VW::PlesseyDetector::Compute(imagemono)" << endl;

  if (!im.IsAlloced()) {
    cerr << "VW::PlesseyDetector: Image is not allocated!" << endl;
    return;
  }

 ConvolutionMask1D<float> gauss_mask(_patch_size);
 gauss_mask.MakeGaussian(_smoothing_sigma);

  int imwidth = im.GetWidth(),
      imheight = im.GetHeight(); 


  // Conversion from ordinary score to normalized one (0<=score<=1)
  const float strength_normalising_factor = 1.0f/GetMaxStrength(gauss_mask,_edge_phobia);
  // Use conversion to find the raw threshold, using the fact that Harris-Plessey operates on grad^4
  float t4=_strength_threshold;
  t4=t4*t4*t4*t4;

  ImageMono<float> gx2arr(imwidth, imheight);
  ImageMono<float> gy2arr(imwidth, imheight);  ImageMono<float> gxyarr(imwidth, imheight);
  ImageMono<float> sgx2arr(imwidth, imheight);
  ImageMono<float> sgy2arr(imwidth, imheight);
  ImageMono<float> sgxyarr(imwidth, imheight);

  calc_gradients (im, gx2arr, gy2arr, gxyarr);

  if (VERBOSE) {
    for (int i=-gauss_mask.GetHalfWidth();i<=gauss_mask.GetHalfWidth();i++) {
      cerr << gauss_mask.mask[i] << ",";
    }
    cerr << endl;
  }

  Convolve<float>(gx2arr, gauss_mask, gauss_mask, sgx2arr);
  Convolve<float>(gy2arr, gauss_mask, gauss_mask, sgy2arr);
  Convolve<float>(gxyarr, gauss_mask, gauss_mask, sgxyarr);

  compute_best_corner(sgx2arr, sgy2arr, sgxyarr, best_corner);

  best_corner.SetCornerStrength(best_corner.GetCornerStrength()*strength_normalising_factor);

  C.Normalise (best_corner);

  return;
}

//////////////////////////////////////

void PlesseyDetector::Compute(const Pyramid<unsigned char> &im,  
										vector<CornerFeature>& features,
										const VW::Normalisation& C) const
{
  if (VERBOSE) cerr << "VW::PlesseyDetector::Compute(pyramid)" << endl;

 ConvolutionMask1D<float> gauss_mask(_patch_size);
 gauss_mask.MakeGaussian(_smoothing_sigma);

 // Conversion from ordinary score to normalized one (0<=score<=1)
  const float strength_normalising_factor = 1.0f/GetMaxStrength(gauss_mask,_edge_phobia);
  // Use conversion to find the raw threshold, using the fact that Harris-Plessey operates on grad^4
  float t4=_strength_threshold;
  t4=t4*t4*t4*t4;
  const float raw_threshold = t4/strength_normalising_factor;


  features.clear();
  features.reserve(_max_features+1);
  for (unsigned int j=0; j<im.NumLevels(); j++) {

    // The current image
    const VW::ImageMono<unsigned char>& this_im=im[j];

  int imwidth = this_im.GetWidth(),
      imheight = this_im.GetHeight(); 

    // The images
    ImageMono<float> gx2arr(imwidth, imheight);
    ImageMono<float> gy2arr(imwidth, imheight);  
    ImageMono<float> gxyarr(imwidth, imheight);
    ImageMono<float> sgx2arr(imwidth, imheight);
    ImageMono<float> sgy2arr(imwidth, imheight);
    ImageMono<float> sgxyarr(imwidth, imheight);
    ImageMono<float> corn_strength(imwidth, imheight);
    
    calc_gradients (this_im, gx2arr, gy2arr, gxyarr);
    
    if (VERBOSE) {
      for (int i=-gauss_mask.GetHalfWidth();i<=gauss_mask.GetHalfWidth();i++) {
	cerr << gauss_mask.mask[i] << ",";
      }
      cerr << endl;
    }
    
    Convolve<float>(gx2arr, gauss_mask, gauss_mask, sgx2arr);
    Convolve<float>(gy2arr, gauss_mask, gauss_mask, sgy2arr);
    Convolve<float>(gxyarr, gauss_mask, gauss_mask, sgxyarr);
    
    compute_corner_strength(sgx2arr, sgy2arr, sgxyarr, corn_strength);
    
    
 	 const float image_scale = 1.0f/im.GetScale(j);
	 compute_corners(corn_strength, 
						  strength_normalising_factor, 
						  raw_threshold, 
						  image_scale, 
						  features);
  }
  
  C.Normalise (features);
  
  return;
}

//////////////////////////////////////////////////////////////////

void PlesseyDetector::Compute(const Pyramid<float> &im,  
										vector<CornerFeature>& features,
										const VW::Normalisation& C) const
{
  if (VERBOSE) cerr << "VW::PlesseyDetector::Compute(pyramid)" << endl;

 ConvolutionMask1D<float> gauss_mask(_patch_size);
 gauss_mask.MakeGaussian(_smoothing_sigma);


  // Conversion from ordinary score to normalized one (0<=score<=1)
  const float strength_normalising_factor = 1.0f/GetMaxStrength(gauss_mask,_edge_phobia);
  // Use conversion to find the raw threshold, using the fact that Harris-Plessey operates on grad^4
  float t4=_strength_threshold;
  t4=t4*t4*t4*t4;
  const float raw_threshold = t4/strength_normalising_factor;


  // Clear the point list (just in case)
  features.clear();
  for (unsigned int j=0; j<im.NumLevels(); j++) {

    // The current image
    const VW::ImageMono<float>& this_im=im[j];

  int imwidth = this_im.GetWidth(),
      imheight = this_im.GetHeight(); 

    // A temporary point list
    vector<CornerFeature> temp_points;

    // The images
    ImageMono<float> gx2arr(imwidth, imheight);
    ImageMono<float> gy2arr(imwidth, imheight);  
    ImageMono<float> gxyarr(imwidth, imheight);
    ImageMono<float> sgx2arr(imwidth, imheight);
    ImageMono<float> sgy2arr(imwidth, imheight);
    ImageMono<float> sgxyarr(imwidth, imheight);
    ImageMono<float> corn_strength(imwidth, imheight);
    
    calc_gradients (this_im, gx2arr, gy2arr, gxyarr);
    
    if (VERBOSE) {
      for (int i=-gauss_mask.GetHalfWidth();i<=gauss_mask.GetHalfWidth();i++) {
	cerr << gauss_mask.mask[i] << ",";
      }
      cerr << endl;
    }
    
    Convolve<float>(gx2arr, gauss_mask, gauss_mask, sgx2arr);
    Convolve<float>(gy2arr, gauss_mask, gauss_mask, sgy2arr);
    Convolve<float>(gxyarr, gauss_mask, gauss_mask, sgxyarr);
    
    compute_corner_strength(sgx2arr, sgy2arr, sgxyarr, corn_strength);
    
    
 	 const float image_scale = 1.0f/im.GetScale(j);
	 compute_corners(corn_strength, 
						  strength_normalising_factor, 
						  raw_threshold, 
						  image_scale, 
						  features);
  }
  
  C.Normalise(features);
  
  return;
}

//////////////////////////////////////////////////////////////////

void PlesseyDetector::Compute(const Pyramid<float> &pyramid,  
		      CornerFeature& best_corner,
		      const VW::Normalisation& C) const
{
  if (VERBOSE) cerr << "VW::PlesseyDetector::Compute(pyramid)" << endl;

 ConvolutionMask1D<float> gauss_mask(_patch_size);
 gauss_mask.MakeGaussian(_smoothing_sigma);

 // Conversion from ordinary score to normalized one (0<=score<=1)
  const float strength_norm = 1.0f/GetMaxStrength(gauss_mask,_edge_phobia);
  best_corner.Set(0,0,0,0);

  for (unsigned int j=0; j<pyramid.NumLevels(); j++) {

    // The current image
    const VW::ImageMono<float>& this_im=pyramid[j];

    const int imwidth  = this_im.GetWidth(),
              imheight = this_im.GetHeight(); 

    VW::CornerFeature this_best(0,0,0,0);

    // The images
    ImageMono<float> gx2arr(imwidth, imheight);
    ImageMono<float> gy2arr(imwidth, imheight);  
    ImageMono<float> gxyarr(imwidth, imheight);
    ImageMono<float> sgx2arr(imwidth, imheight);
    ImageMono<float> sgy2arr(imwidth, imheight);
    ImageMono<float> sgxyarr(imwidth, imheight);
    ImageMono<float> corn_strength(imwidth, imheight);
    
    calc_gradients (this_im, gx2arr, gy2arr, gxyarr);
    
    if (VERBOSE) {
      for (int i=-gauss_mask.GetHalfWidth();i<=gauss_mask.GetHalfWidth();i++) {
	cerr << gauss_mask.mask[i] << ",";
      }
      cerr << endl;
    }
    
    Convolve<float>(gx2arr, gauss_mask, gauss_mask, sgx2arr);
    Convolve<float>(gy2arr, gauss_mask, gauss_mask, sgy2arr);
    Convolve<float>(gxyarr, gauss_mask, gauss_mask, sgxyarr);
    
    compute_best_corner(sgx2arr, sgy2arr, sgxyarr, this_best);
    
    if (this_best.GetCornerStrength()>best_corner.GetCornerStrength()) {
      const float scale = 1.0f/pyramid.GetScale(j);
      best_corner.Set(float(this_best.GetX())*scale,
		      float(this_best.GetY())*scale,
		      this_best.GetCornerStrength()*strength_norm,
		      scale);
    }
  }
  
  C.Normalise (best_corner);
  
  return;
}

//////////////////////////////////////////////////////////////////

void PlesseyDetector::Compute(const ImageYUV<unsigned char> &im,  
		      vector<CornerFeature>& features,
		      const VW::Normalisation& C) const
{
  if (VERBOSE) cerr << "VW::PlesseyDetector::Compute(imageyuv)" << endl;
  if (!im.IsAlloced()) {
    cerr << "VW::PlesseyDetector: Image is not allocated!" << endl;
    return;
  }
  ConvolutionMask1D<float> gauss_mask(_patch_size);
  gauss_mask.MakeGaussian(_smoothing_sigma);

  int imwidth = im.GetWidth(),
      imheight = im.GetHeight(); 


  // Conversion from ordinary score to normalized one (0<=score<=1)
  const float strength_normalising_factor = 1.0f/GetMaxStrength(gauss_mask,_edge_phobia);
  // Use conversion to find the raw threshold, using the fact that Harris-Plessey operates on grad^4
  float t4=_strength_threshold;
  t4=t4*t4*t4*t4;
  float raw_threshold = t4/strength_normalising_factor;

  ImageMono<float> gx2arr(imwidth, imheight);
  ImageMono<float> gy2arr(imwidth, imheight);
  ImageMono<float> gxyarr(imwidth, imheight);
  ImageMono<float> sgx2arr(imwidth, imheight);
  ImageMono<float> sgy2arr(imwidth, imheight);
  ImageMono<float> sgxyarr(imwidth, imheight);
  ImageMono<float> corn_strength(imwidth, imheight);

  calc_gradients (im, gx2arr, gy2arr, gxyarr);

  if (VERBOSE) {
    for (int i=-gauss_mask.GetHalfWidth();i<=gauss_mask.GetHalfWidth();i++) {
      cerr << gauss_mask.mask[i] << ",";
    }
    cerr << endl;
  }

  Convolve<float>(gx2arr, gauss_mask, gauss_mask, sgx2arr);
  Convolve<float>(gy2arr, gauss_mask, gauss_mask, sgy2arr);
  Convolve<float>(gxyarr, gauss_mask, gauss_mask, sgxyarr);

  compute_corner_strength(sgx2arr, sgy2arr, sgxyarr, corn_strength);


  // Clear the point list (just in case)
  features.clear();

  compute_corners(corn_strength, 
						strength_normalising_factor, 
						raw_threshold, 
						1.0, // Single image, so scale is 1
						features);

  C.Normalise(features);

  return;
}




