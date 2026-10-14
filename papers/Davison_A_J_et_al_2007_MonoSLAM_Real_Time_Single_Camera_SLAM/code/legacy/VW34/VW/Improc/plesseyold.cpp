#include <math.h>
#include <vector>

#include "VW/image.h"
#include "convolve.h"
#include "plesseyold.h"

#define VERBOSE false

/// Urggh, nasty defines

#define ABS(a) ((a) < (0) ? (-a) : (a))
#define SIGN(a) ((a) < (0) ? (-1) : (1))

//////////////////////////////////////////////////////////////////

using namespace std;
using namespace VW;


void PlesseyOld::calc_gradients (const ImageMono<unsigned char> &im,
				 ImageMono<float> &gx2arr,
				 ImageMono<float> &gy2arr,
				 ImageMono<float> &gxyarr) const
{
  
  unsigned char *pix0,*pixm1,*pixp1;
  int c0, r0, cl, rl;
  int     r, c;
  float   grady, gradx, *gx2r, *gy2r, *gxyr;

  // adjust border
  im.GetRoi(c0, r0, cl, rl);
  c0++; r0++;
  cl--; rl--;
  gx2arr.SetRoi(c0, r0, cl, rl);
  gy2arr.SetRoi(c0, r0, cl, rl);
  gxyarr.SetRoi(c0, r0, cl, rl);

  for(r=r0; r<rl; r++) {
    pixm1 = im.pixel[r-1]+c0;
    pix0 = im.pixel[r]+c0;
    pixp1 = im.pixel[r+1]+c0;
    gx2r = gx2arr.pixel[r]+c0;
    gy2r = gy2arr.pixel[r]+c0;
    gxyr = gxyarr.pixel[r]+c0;
    for(c=c0; c<cl; c++, pix0++, pixp1++, pixm1++) {
      gradx = (float) (pix0[1] - pix0[-1]);
      grady = (float) (*pixp1 - *pixm1);
      *gx2r++ = gradx*gradx;
      *gy2r++ = grady*grady;
      *gxyr++ = gradx*grady;
    }
  }  
}
void PlesseyOld::calc_gradients (const ImageYUV<unsigned char> &im,
			      ImageMono<float> &gx2arr,
			      ImageMono<float> &gy2arr,
			      ImageMono<float> &gxyarr) const
{
  
  unsigned char *pix0,*pixm1,*pixp1;
  int c0, r0, cl, rl;
  int     r, c;
  float   grady, gradx, *gx2r, *gy2r, *gxyr;

  const int bytes_per_pixel=4;

  // adjust border
  im.GetRoi(c0, r0, cl, rl);
  c0++; r0++;
  cl--; rl--;
  gx2arr.SetRoi(c0, r0, cl, rl);
  gy2arr.SetRoi(c0, r0, cl, rl);
  gxyarr.SetRoi(c0, r0, cl, rl);


  for(r=r0; r<rl; r++) {
    pixm1 = &(im.pixel[r-1]->y) + c0*bytes_per_pixel;
    pix0 = &(im.pixel[r]->y) + c0*bytes_per_pixel;
    pixp1 = &(im.pixel[r+1]->y) + c0*bytes_per_pixel;
    gx2r = gx2arr.pixel[r]+c0;
    gy2r = gy2arr.pixel[r]+c0;
    gxyr = gxyarr.pixel[r]+c0;
    for(c=c0; 
	c<(cl*bytes_per_pixel); 
	c+=bytes_per_pixel, pix0+=bytes_per_pixel, 
	  pixp1+=bytes_per_pixel, pixm1+=bytes_per_pixel) {
      gradx = float (pix0[bytes_per_pixel] - pix0[-bytes_per_pixel]);
      grady = float (*pixp1 - *pixm1);
      *gx2r++ = gradx*gradx;
      *gy2r++ = grady*grady;
      *gxyr++ = gradx*grady;
    }
  }  
}

//////////////////////////////////////////////////////////////////

void PlesseyOld::compute_corner_strength ( const ImageMono<float> &gradx2,
					const ImageMono<float> &grady2,
					const ImageMono<float> &gradxy,
					ImageMono<float> &corn_strength ) const
{
  int r, c;
  int c0, r0, cl, rl;
  float trace, det;

  gradx2.GetRoi(c0, r0, cl, rl);
  corn_strength.SetRoi(c0, r0, cl, rl);

  for (r=r0; r<rl; r++ ) {
    for (c=c0; c<cl; c++) {
      det = gradx2.pixel[r][c]*grady2.pixel[r][c] - gradxy.pixel[r][c]*gradxy.pixel[r][c];
      trace = gradx2.pixel[r][c] + grady2.pixel[r][c];
      corn_strength.pixel[r][c] = det - 0.25*params.GetEdgePhobia()*(trace*trace);
    }
  }
}

//////////////////////////////////////////////////////////////////

void PlesseyOld::compute_corners(const ImageMono<float> &strength, 
			      float score_normalisation,
			      float raw_threshold, 
			      vector<PlesseyCorner>& pl) const
{
  int i,j;
  int c0, r0, cl, rl;
  int cnew0, rnew0, cnewl, rnewl;
  float *si, *sim1, *sip1, *sim2, *sip2, s;

  strength.GetRoi(c0, r0, cl, rl);
  cnew0=c0+2; rnew0=r0+2;
  cnewl=cl-2; rnewl=rl-2;

  for (i=rnew0; i<rnewl; i++) {
    sim2 = strength.pixel[i-2];
    sim1 = strength.pixel[i-1];
    si   = strength.pixel[i];
    sip1 = strength.pixel[i+1];
    sip2 = strength.pixel[i+2];
    
    for(j=cnew0; j<cnewl; j++) {
      s = si[j];

      if (s>raw_threshold &&
	  //5x5 neighbourhood: Check the closest first
	  s>sim1[j]   && s>sip1[j]   && s>si[j-1]   && s>si[j+1]   &&
	  s>sim1[j-1] && s>sim1[j+1] && s>sip1[j-1] && s>sip1[j+1] &&
	  s>sim1[j-2] && s>sim1[j+2] &&
	  s>si[j-2]   && s>si[j+2]   &&
	  s>sip1[j-2] && s>sip1[j+2] &&
	  s>sim2[j-2] && s>sim2[j-1] && s>sim2[j]   && s>sim2[j+1] && s>sim2[j+2] &&
	  s>sip2[j-2] && s>sip2[j-1] && s>sip2[j]   && s>sip2[j+1] && s>sip2[j+2] ) {

	// Calculate position to sub-pixel accuracy using quadratic fit



	{ // Some variables local to this calculation
	  float x,y,st;           // Stuff for the sub-pixel corner

	  if (VERBOSE) {
	    cerr << "Raw scores:" << endl;
	    cerr << sim1[j-1] << "  " << sim1[j] << "  " << sim1[j+1] << endl;
	    cerr << si  [j-1] << "  " << s       << "  " << si  [j+1] << endl;
	    cerr << sip1[j-1] << "  " << sip1[j] << "  " << sip1[j+1] << endl;
	    cerr << "Normalisation: " << score_normalisation << endl;
	  }

	  // Edge phobia can result in negative strength, so need abs and sign
	  // defined at the top of the file
	  // Scores for minus row
	  float data[9];
	   data[0]=SIGN(sim1[j-1])*sqrt(sqrt(ABS(sim1[j-1])*score_normalisation));
	   data[1]=SIGN(sim1[j])  *sqrt(sqrt(ABS(sim1[j]  )*score_normalisation));
	   data[2]=SIGN(sim1[j+1])*sqrt(sqrt(ABS(sim1[j+1])*score_normalisation));
	  // Scores for zero row
	   data[3]=SIGN(si[j-1])*sqrt(sqrt(ABS(si[j-1])*score_normalisation));
	   data[4]=SIGN(s      )*sqrt(sqrt(ABS(s      )*score_normalisation));
	   data[5]=SIGN(si[j+1])*sqrt(sqrt(ABS(si[j+1])*score_normalisation));
	  // Scores for plus row
	   data[6]=SIGN(sip1[j+1])*sqrt(sqrt(ABS(sip1[j-1])*score_normalisation));
	   data[7]=SIGN(sip1[j]  )*sqrt(sqrt(ABS(sip1[j]  )*score_normalisation));
	   data[8]=SIGN(sip1[j+1])*sqrt(sqrt(ABS(sip1[j+1])*score_normalisation));

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


	  // Correct for image position
	  x += j;          // Array coord
	  y += i;          // Array coord

	  // Normalise the score
	  //	  s=s*score_normalisation;
	  //	  st=st*score_normalisation;

	  if (VERBOSE) {
	    cerr << "Basic corner ("<<j<<","<<i<<") strength="<<data[4]
		 <<"    sub-pixel ("<<x<<","<<y<<") strength="<<st<<endl;
	  }

	  // add corner into point list (using stl pushback routine)
	  // Note that the Plessey-Harris score is related to grad^4, so fourth root is 
	  // taken after normalization
	  pl.push_back(PlesseyCorner(x, y, sqrt(sqrt(st))));
	}
      }
    }
  }
  
  return;
}

//////////////////////////////////////////////////////////////////



#ifdef MATCHER
//////////////////////////////////////////////////////////////////

/*
void matcher(int cn,VW_Pointmap * previouspointmap, VW_Pointmap *pointmap,int cor_c0,int cor_r0,
             float sigma_s, float sigma_d)
{
  VW_Point *pptr;
  VW_Point *pptr1;
  VW_Point *previouspptr;
  VW_Point *match;
  float cost,mincost,d_diff,xdiff,ydiff,s_diff, sigma_s2, sigma_d2,f;

  sigma_s2 = sigma_s*sigma_s;
  sigma_d2 = sigma_d*sigma_d;

  if(previouspointmap!=NULL && pointmap!=NULL) {

  // forwards matching from previous to current frame
    for(previouspptr=previouspointmap->GetFirstPoint();previouspptr!=NULL;previouspptr = previouspptr->GetNext()) {
      mincost = 1.0e12;
      match   = NULL;
      for(pptr=pointmap->GetFirstPoint();pptr!=NULL;pptr = pptr->GetNext()) {
	xdiff = pptr->GetIX()-previouspptr->GetIX();
	ydiff = pptr->GetIY()-previouspptr->GetIY();
	d_diff  = (xdiff*xdiff + ydiff*ydiff)/sigma_d2;
	f  = (pptr->GetS()-previouspptr->GetS());
        s_diff  = (f*f/sigma_s2);
        cost = d_diff;
	if(d_diff<1 && s_diff<1 && cost < mincost) {
	  mincost = cost;
          match = pptr;
	}
      }
      if(match!=NULL) {
	previouspptr->SetTNext(match);
      }
    }


    for(pptr=pointmap->GetFirstPoint();pptr!=NULL;pptr=pptr->GetNext()) {
      //           vw_gfxbox[cn].DrawSymbolBoxCoord(pptr->GetIX()+cor_c0,pptr->GetIY()+cor_r0,VW_POINT,0,0,FL_MAGENTA);
      mincost = 1.0e12;
      match   = NULL;
      for(previouspptr=previouspointmap->GetFirstPoint();previouspptr!=NULL;previouspptr = previouspptr->GetNext()) {
	xdiff = pptr->GetIX()-previouspptr->GetIX();
	ydiff = pptr->GetIY()-previouspptr->GetIY();
	d_diff  = (xdiff*xdiff + ydiff*ydiff)/sigma_d2;
	f  = (pptr->GetS()-previouspptr->GetS());
        s_diff  = (f*f/sigma_s2);
        cost = d_diff;
	if(d_diff<1 && s_diff<1 && cost < mincost) {
	  mincost = cost;
          match = previouspptr;
	}
      }
      if(match!=NULL) {
	pptr->SetTPrevious(match);
      }
    }
    
    
    
    for(pptr=pointmap->GetFirstPoint();pptr!=NULL;pptr=pptr->GetNext()) {
      pptr1=  pptr->GetTPrevious();
      if(pptr1!=NULL && pptr == pptr1->GetTNext() ) {
	
           vw_gfxbox[cn].DrawSymbolBoxCoord(pptr1->GetIX()+cor_c0,pptr1->GetIY()+cor_r0,VW_POINT,0,0,FL_RED);
	   vw_gfxbox[cn].DrawLineBoxCoord(
				       pptr->GetIX()+cor_c0,pptr->GetIY()+cor_r0,
				       pptr1->GetIX()+cor_c0,pptr1->GetIY()+cor_r0,
				       1,FL_RED);
	
	  }

	  
	  }
    
  }
  
}
*/

#endif

//////////////////////////////////////////////////////////////////

void PlesseyOld::Compute(const ImageMono<unsigned char> &im,  
		      vector<PlesseyCorner>& pl,
		      const VW::Normalisation& C) const
{
  ConvolutionMask1D<float> gauss_mask(params.GetPatchSize());
  gauss_mask.MakeGaussian(params.GetGaussianSigma());

  int imwidth = im.GetWidth(),
      imheight = im.GetHeight(); 


  // Conversion from ordinary score to normalized one (0<=score<=1)
  const float strength_normalising_factor = 1.0/get_max_strength(gauss_mask);
  // Use conversion to find the raw threshold, using the fact that Harris-Plessey operates on grad^4
  float t4=params.GetStrengthThreshold();
  t4=t4*t4*t4*t4;
  float raw_threshold = t4/strength_normalising_factor;

  ImageMono<float> gx2arr(imwidth, imheight);
  ImageMono<float> gy2arr(imwidth, imheight);
  ImageMono<float> gxyarr(imwidth, imheight);
  ImageMono<float> sgx2arr(imwidth, imheight);
  ImageMono<float> sgy2arr(imwidth, imheight);
  ImageMono<float> sgxyarr(imwidth, imheight);
  ImageMono<float> corn_strength(imwidth, imheight);

  // Clear the point list (just in case)
  pl.clear();

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


  compute_corners(corn_strength, strength_normalising_factor, raw_threshold, pl);
  C.Normalise (pl);

  return;
}

//////////////////////////////////////////////////////////////////

void PlesseyOld::Compute(const ImageYUV<unsigned char> &im,  
		      vector<PlesseyCorner>& pl,
		      const VW::Normalisation& C) const
{
  ConvolutionMask1D<float> gauss_mask(params.GetPatchSize());
  gauss_mask.MakeGaussian(params.GetGaussianSigma());

  int imwidth = im.GetWidth(),
      imheight = im.GetHeight(); 


  // Conversion from ordinary score to normalized one (0<=score<=1)
  const float strength_normalising_factor = 1.0/get_max_strength(gauss_mask);
  // Use conversion to find the raw threshold, using the fact that Harris-Plessey operates on grad^4
  float t4=params.GetStrengthThreshold();
  t4=t4*t4*t4*t4;
  float raw_threshold = t4/strength_normalising_factor;

  ImageMono<float> gx2arr(imwidth, imheight);
  ImageMono<float> gy2arr(imwidth, imheight);
  ImageMono<float> gxyarr(imwidth, imheight);
  ImageMono<float> sgx2arr(imwidth, imheight);
  ImageMono<float> sgy2arr(imwidth, imheight);
  ImageMono<float> sgxyarr(imwidth, imheight);
  ImageMono<float> corn_strength(imwidth, imheight);

  // Clear the point list (just in case)
  pl.clear();

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


  compute_corners(corn_strength, strength_normalising_factor, raw_threshold, pl);
  C.Normalise (pl);

  return;
}



// Find the highest achievable score given this mask
//   Because of discretization, truncation etc. it is
//   practically impossible to work out the actual limit
//   of corner strength theoretically, so instead
//   I actually do the convolution on a step corner.
//
// {{{ float PlesseyOld::get_max_strength(const ConvolutionMask1D<float> gauss_mask)

float PlesseyOld::get_max_strength(const ConvolutionMask1D<float> gauss_mask) const {
  const float gradient_max=255.0; // for unsigned char
  //  const float gradient_max=1.0; // for float
  //  const float gradient_max=1.0; // for double

  // Do a quick convolution of a step corner
  // Using the fact that the gradient response to a step edge is:
  //  I = 0 0 0 0 0 0  Gxx = 0 0 0 0 0 0  Gyy = 0 0 0 0 0 0  Gxy = 0 0 0 0 0 0
  //      0 0 0 0 0 0        0 0 0 0 0 0        0 0 0 0 0 0        0 0 0 0 0 0
  //      0 0 0 0 0 0        0 0 0 0 0 0        0 0 0 1 1 1        0 0 0 0 0 0
  //      0 0 0 1 1 1        0 0 1 1 0 0        0 0 0 1 1 1        0 0 0 1 0 0
  //      0 0 0 1 1 1        0 0 1 1 0 0        0 0 0 0 0 0        0 0 0 0 0 0
  //      0 0 0 1 1 1        0 0 1 1 0 0        0 0 0 0 0 0        0 0 0 0 0 0
  //
  // the convolution can be written out to save computation. The peak strength
  // is gauranteed to lie with half_width of the corner position, so only a
  // small patch need be considered.
  // The summations below calculate the the smoothed version starting at
  // the (3x3) position of the above.

  float colsum=0,rowsum=0;
  const int halfwidth=gauss_mask.GetHalfWidth()+1;
  const int arrsize=halfwidth+1;
  float kernel[2*halfwidth+1]; // Need to create a copy to pad with zeros
  for (int i=-gauss_mask.GetHalfWidth();i<=gauss_mask.GetHalfWidth();i++) kernel[i+halfwidth]=gauss_mask.mask[i];
  kernel[0]=0;kernel[2*halfwidth]=0;
  float gyy[arrsize*arrsize];
  float gxy[arrsize*arrsize];
  float score[arrsize*arrsize];

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
      *scoreptr = (gyy[y*arrsize+x] * gyy[x*arrsize+y]) - (gxy[y*arrsize+x] * gxy[y*arrsize+x]);
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
    data[0]=sqrt(sqrt(score[(maxy-1)*arrsize+(maxx-1)]));
    data[1]=sqrt(sqrt(score[(maxy-1)*arrsize+(maxx)]));
    data[2]=sqrt(sqrt(score[(maxy-1)*arrsize+(maxx+1)]));
    data[3]=sqrt(sqrt(score[(maxy)*arrsize+(maxx-1)]));
    data[4]=sqrt(sqrt(score[(maxy)*arrsize+(maxx)]));
    data[5]=sqrt(sqrt(score[(maxy)*arrsize+(maxx+1)]));
    data[6]=sqrt(sqrt(score[(maxy+1)*arrsize+(maxx-1)]));
    data[7]=sqrt(sqrt(score[(maxy+1)*arrsize+(maxx)]));
    data[8]=sqrt(sqrt(score[(maxy+1)*arrsize+(maxx+1)]));

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
      cerr << "PlesseyOld: ** Error in computing max score - somthing odd going down!" << endl;
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
  
  float result=gradient_max * max_score;
  
  return (result*result*result*result);
}

// }}}

// {{{ void PlesseyOld::FitQuadratic3x3(.)
void PlesseyOld::FitQuadratic3x3(float& peak_value,float& peak_x,float& peak_y,const float data[9]) const {
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
		   +   data[6]  - 2*data[7]  +   data[8]  )/8.0; // d^2I / dx^2

	  B =    (  ( data[0]  + 2*data[1]  + data[2]  )
	        - 2*( data[3]  + 2*data[4]  + data[5]  )
	        +   ( data[6]  + 2*data[7]  + data[8]  ))/8.0; // d^2I / dy^2

	  C = (  + data[0]  - data[2] 
		 - data[6]  + data[8]  )/4.0;                // d^2I / dxdy

	  D = ( - data[0]  - 2*data[3]  - data[6]
		+ data[2]  + 2*data[5]  + data[8]  )/8.0;      // dI / dx

	  E = ( - data[0]  - 2*data[1]  - data[2] 
		+ data[6]  + 2*data[7]  + data[8]  )/8.0;      // dI / dy

	  if (VERBOSE) cerr << "   Quadfit: (A,B,C,D,E,F)=("
			    <<A<<","
			    <<B<<","
			    <<C<<","
			    <<D<<","
			    <<E<<","
			    <<data[4]<<")"<<endl;

	  // Calculate the inverse of the determinant
	  float detinv = 1/(A*B - 0.25*C*C);

	  // Calculate peak position and value
	  peak_x = -1* (B*D - 0.5*C*E) * detinv; // X-Offset of quadratic peak
	  peak_y = -1* (A*E - 0.5*C*D) * detinv; // Y-Offset of quadratic peak

	  // When the feature has weak gradient in one direction, bad shit
	  // happens, but the most this fit should move the peak is half
	  // way to a neighbouring pixel (otherwise it would be higher).
	  if (peak_x>=0.5) peak_x=0.49999;
	  if (peak_x<-0.5) peak_x=-0.5;
	  if (peak_y>=0.5) peak_y=0.49999;
	  if (peak_y<-0.5) peak_y=-0.5;
	  peak_value =
	    0.5*(A*peak_x*peak_x + B*peak_y*peak_y) + C*peak_x*peak_y // Quadratic part
	       + D*peak_x        + E*peak_y                           // Linear part
	       + data[4];                                             // Constant part
}
// }}}


template <>
bool VW::XMLStuff::ReadXMLString<VW::PlesseyOldParams>(const string& str,
						VW::PlesseyOldParams& p) {
  istringstream s;
  string content;
  cerr << "ReadXMLString - String:"<<str<<endl;
  if (GetXMLBlockContents("StrengthThreshold",str,content)) {
    float f;
    ReadXMLString<float>(content,f);
    p.SetStrengthThreshold(f);
  }
  if (GetXMLBlockContents("GaussianSigma",str,content)) {
    float f;
    ReadXMLString<float>(content,f);
    p.SetGaussianSigma(f);
  }
  if (GetXMLBlockContents("PatchSize",str,content)) {
    int i;
    ReadXMLString<int>(content,i);
    p.SetPatchSize(i);
  }
  if (GetXMLBlockContents("EdgePhobia",str,content)) {
    float f;
    ReadXMLString<float>(content,f);
    p.SetEdgePhobia(f);
  }
  p.SetStrengthThreshold(0.1);
  return (true);
}
template <>
string VW::XMLStuff::GenerateXMLString<VW::PlesseyOldParams>(const VW::PlesseyOldParams& p, 
						      unsigned int indentation) {
  ostringstream s,indent;     
  for (unsigned int i=0;i<indentation;i++) indent << ' ';
  cerr << "Indent string="<<indent.str()<<"."<<endl;
  s << indent.str() << "<StrengthThreshold> " 
    << GenerateXMLString(p.GetStrengthThreshold(),0)
    << " </StrengthThreshold>" << endl;
  s << indent.str() << "<GaussianSigma> " 
    << GenerateXMLString(p.GetGaussianSigma(),0)
    << " </GaussianSigma>" << endl;
  s << indent.str() << "<PatchSize> " 
    << GenerateXMLString(p.GetPatchSize(),0)
    << " </PatchSize>" << endl;
  s << indent.str() << "<EdgePhobia> " 
    << GenerateXMLString(p.GetEdgePhobia(),0)
    << " </EdgePhobia>" << endl;
  return (s.str());
}

// Load parameters from file
void VW::PlesseyOldParams::LoadParameterFile (string paramfile = ".params") {
  ParameterParser parser ("PlesseyParams", paramfile);
  if (!parser) return;  // If file doesn't exist or is ill-formed...

  // There are a couple of ways to read the data, but this uses the 
  // ReadXMLString(...) function defined above.
  parser.GetValue(*this);
}

// Save parameters to file
void VW::PlesseyOldParams::SaveParameterFile (string paramfile = ".params") {
  VW::ParameterWriter param_writer ("PlesseyParams", paramfile);
  if (!param_writer) return;  // If file doesn't exist or is ill-formed...

  // Could add params in two ways, but this is the most convenient (IMHO)
  // it uses the GenerateXMLString function defined above.
  param_writer.AddParameter(*this);
  param_writer.WriteOut();
}
