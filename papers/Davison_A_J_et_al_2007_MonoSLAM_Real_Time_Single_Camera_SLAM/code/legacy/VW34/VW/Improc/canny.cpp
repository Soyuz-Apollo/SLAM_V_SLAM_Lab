/** 
 * @file   canny.cpp
 * @author DW Murray and ID Reid
 * @date   02/08/2003
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
 * In the following there are a few places where
 * "Fast" and "Proper" codes exist, with one set commented out
 * The Fast code avoids dividing gradients by 2, and avoids taking sqrts. 
 * The resulting edge strengths are  4*[conventional_edge_strength]^2
 * The ONLY place this makes a difference is that the position of the
 * interpolated sub-pixel edge is changed. As that quadratic interpolation
 * was always a dubious hack anyway, one can live with it! The orientations
 * are unchanged, as they are formed from ratios of gradients.
 * The speed up is around 10%, so worth having.
 *
 * DWM
 */

#define _USE_MATH_DEFINES // So that win32 has M_PI!
#include <math.h>

#include "convolve.h"
#include "edgel.h"
#include "canny.h"


///////////////////////////////////////////////////////////

// Increase this to perform edge detection on a central region
// Useful if you need to reduce computation time
#define BORDER 0 
#define ZERO_SIGMA_THRES 0.4  /* any std dev below this is set to zero */



using namespace std;
using namespace VW;

///////////////////////////////////////////////////////////

// These static LUTs used during LINKING
static char *rightlist = "453546564657675760706071017102120213231324342435";
static char *leftlist  = "017102120213231324342435453546564657675760706071";
static int xoffs[8]   = {1,1,0,-1,-1,-1,0,1};
static int yoffs[8]   = {0,1,1,1,0,-1,-1,-1};

///////////////////////////////////////////////////////////

void Canny::CalcGradients (ImageMono<float>& im,
 			    ImageMono<float>& gx,
 			    ImageMono<float>& gy,
 			    ImageMono<float>& grad) const
{
  
  ImageMono<float>::PixelPtr  pix0m1,pix0p1,pixm1,pixp1;
  int     r, c;
  int     c0, r0, cl, rl;
  float grady,gradx;
  ImageMono<float>::PixelPtr gxr,gyr,gradr;

  // adjust border
  im.ROI().Get(c0, r0, cl, rl);
  c0++; r0++;
  cl--; rl--;
  gx.ROI().Set(c0, r0, cl, rl);
  gy.ROI().Set(c0, r0, cl, rl);
  grad.ROI().Set(c0, r0, cl, rl);

  for (r=r0; r<rl; r++) {
    pixm1  = im.GetRowStart(r-1)+c0;
    pix0m1 = im.GetRowStart(r)+c0-1;
    pix0p1 = im.GetRowStart(r)+c0+1;
    pixp1  = im.GetRowStart(r+1)+c0;
    gxr    = gx.GetRowStart(r)+c0;
    gyr    = gy.GetRowStart(r)+c0;
    gradr  = grad.GetRowStart(r)+c0;
    for (c=c0; c<cl; c++, pix0m1++, pix0p1++, pixp1++, pixm1++) {
      // "Proper"
      //gxr++->y = gradx = (pix0p1->y - pix0m1->y)/2.0;
      //gyr++->y = grady = (pixp1->y -  pixm1->y)/2.0;
      //gradr++->y = sqrt((double)(gradx*gradx + grady*grady));
      // "Fast"
      gxr++->y = gradx = (pix0p1->y - pix0m1->y);
      gyr++->y = grady = (pixp1->y -  pixm1->y);
      gradr++->y = (gradx*gradx + grady*grady);
    }
  }  
}


//////////////////////////////////////////

void Canny::NonMaximalSuppression(ImageMono<float>& grad, 
				   ImageMono<float>& gx, 
				   ImageMono<float>& gy,
				   float low_thresh, 
				   ImageMono<Edgel *>& emap,
				   vector<Edgel *>& edgels) const
{
  int i,j;
  ImageMono<float>::PixelPtr gradi,gradim,gradip;
  ImageMono<float>::PixelPtr gxi,gyi;
  float strength, orient;
  float a,b,delx,dely,del;
  float gxij,gyij;
  float gradratio,fabsgradratio;
  ImageMono<Edgel *>::PixelPtr edgeli;
  Edgel *e;
  int c0, r0, cl, rl;
  int iorient;
  float theta;

  grad.ROI().Get(c0,r0,cl,rl);
  c0++; r0++; cl--; rl--;
  emap.ROI().Set(c0,r0,cl,rl);

  for (i=r0; i<rl; i++) {
    
    gradi   = grad.GetRowStart(i);
    gradim  = grad.GetRowStart(i-1);
    gradip  = grad.GetRowStart(i+1);
    gxi    = gx.GetRowStart(i);
    gyi    = gy.GetRowStart(i);
    edgeli = emap.GetRowStart(i);

    for (j=c0; j<cl; j++) {

      edgeli[j].Set(NULL);
      
      // "Proper"
      //if ((strength=gradi[j].y) < low_thresh) continue; 
      // "Fast"
      if ((strength=gradi[j].y) < 4*low_thresh*low_thresh) continue; 
      
      // potential edge at ij
      // determine orientation using gradient ratios
      gxij = gxi[j].y;
      gyij = gyi[j].y;
      if(gyij == 0.0) {
	fabsgradratio = gradratio = 1000; // Large number
      } else {
	gradratio = gxij/gyij;
	fabsgradratio = fabs(gradratio);
      }
      
      if (fabsgradratio < 1.50 && fabsgradratio > 0.67) {
	// It is one or other diagonal
	// This check is important for any diagonal
	// DWM once deleted it. Never again
	
	
	if (gradratio > 0) { 

	if( ( strength <= gradim[j].y || strength < gradip[j].y ) &&
	    ( strength <= gradi[j-1].y || strength < gradi[j+1].y ) ) continue;
	  /* the edge goes from botleft to topright
           * so were are concerned with pixels   a..
           *                                     ./.
           *                                     ..b
           */
	  a = gradim[j-1].y;
	  b = gradip[j+1].y;
	  if ( strength < b || strength <= a ) continue;

          theta = atan2(gyij,gxij);
	  a = gradi[j-1].y; b = gradi[j+1].y;
	  del = (a-b)/((a+b-strength*2.0f)*2.0f);
          if(fabs(del)>0.5) {
	    // dodgy result: a+b close to 2*strength
	    a = gradim[j].y; b = gradip[j].y;
	    del = (a-b)/((a+b-strength*2.0f)*2.0f);
            if(fabs(del)>0.5) {
            cerr << "Dodgy 2\n";
	      delx=dely=0.5;
	    } else {
	      delx = 0.5f+sin(theta)*cos(theta)*del;
	      dely = 0.5f+sin(theta)*sin(theta)*del;
	    }
	  } else {
	    delx = 0.5f+cos(theta)*cos(theta)*del;
	    dely = 0.5f+sin(theta)*cos(theta)*del;
	  }
	} else { // left diagonal 
	  /* the edge goes from botleft to topright
           * so were are concerned with pixels   ..b
           *                                     .\.
           *                                     a..
           */
	if( ( strength < gradim[j].y || strength <= gradip[j].y ) &&
	    ( strength <= gradi[j-1].y || strength < gradi[j+1].y ) ) continue;

	  a = gradip[j-1].y;
	  b = gradim[j+1].y;
	  if (strength < b || strength <= a ) continue;


          theta = atan2(gyij,gxij);
	  a = gradi[j-1].y; b = gradi[j+1].y;
	  del = (a-b)/((a+b-strength*2.0f)*2.0f);
          if(fabs(del)>0.5) {
	    // dodgy result: a+b close to 2*strength
	    a = gradim[j].y; b = gradip[j].y;
	    del = (a-b)/((a+b-strength*2.0f)*2.0f);
            if(fabs(del)>0.5) {
            cerr << "Dodgy 4\n";
	      delx=dely=0.5;
	    } else {
	      delx = 0.5f+sin(theta)*cos(theta)*del;
	      dely = 0.5f+sin(theta)*sin(theta)*del;
	    }
	  } else {
	    delx = 0.5f+cos(theta)*cos(theta)*del;
	    dely = 0.5f+sin(theta)*cos(theta)*del;
	  }
	  
	}
	
      } else if ( fabsgradratio > 1.0 ) { // vertical edge
	// Strictly this should be >= 1.5 but the
	// overlap avoids equality testing.
	// In hor_canny the test was differently stated as
	// if( fabs(rowc[c]) > fabs(rowr[c]) )
	// 
	a = gradi[j-1].y; 
	b = gradi[j+1].y;
	if (strength < b || strength <= a) continue;
	del = (a-b)/((a+b-strength*2.0f)*2.0f);
	if(fabs(del)>0.5) {
            cerr << "Really Dodgy stuff in vertical edge\n";
            continue;
	}
        theta = atan2(gyij,gxij);
	delx = 0.5f+del*cos(theta)*cos(theta);
	dely = 0.5f+del*cos(theta)*sin(theta);
	
      } else { // horizontal
	a = gradim[j].y;
	b = gradip[j].y;
	if (strength < b || strength <= a) continue;
	del = (a-b)/((a+b-strength*2.0f)*2.0f);
	if(fabs(del)>0.5) {
            cerr << "Really Dodgy in horizontal\n";
            continue;
	}
        theta = atan2(gyij,gxij);
	delx = 0.5f+del*cos(theta)*sin(theta);
	dely = 0.5f+del*sin(theta)*sin(theta);
      } // endif direction tests

      // As any failure to find edge results in a continue,
      // Hence at this point we have a valid edgel
      orient = atan2( gxij, -gyij);
      iorient = 8+(int)floor(orient*8.0/M_PI);
      if (iorient==16) iorient=0;
      e = new Edgel(strength,
		    orient,(float)j+delx, (float)i+dely,
		    iorient,j,i);
      edgeli[j].Set(e);
      edgels.push_back(e);

    } // endfor j
  } // endfor i

  return;
}


/////////////////////////////////////////////////////////////

Edgel *Canny::LinkLeft(Edgel *e, const ImageMono<Edgel *>& emap) const
{
  Edgel *eptr;
  int choice,ia,ix,iy,ixlink,iylink,index;
  int c0, r0, cl, rl;

  emap.ROI().Get(c0, r0, cl, rl);
  
  ia = e->GetIA();
  ix = e->GetIX();
  iy = e->GetIY();

  for (choice=0; choice<3; choice++) {
    // find the neighbour index. Note the the -48
    // is simply to convert chars into ints. Yuck.
    index = (int)(leftlist[3*ia+choice] - 48);
    ixlink = ix + xoffs[index];
    if (ixlink<c0 || ixlink >= cl) break;
    iylink = iy + yoffs[index];
    if(iylink<r0 || iylink >= rl) break;
    
    eptr = emap[iylink][ixlink].y;
    if (eptr!=NULL && !eptr->GetLinked()) {
      eptr->SetLinked();
      return(eptr);
    }
  }
  return(NULL);
}


/////////////////////////////////////////////////////////////

Edgel *Canny::LinkRight(Edgel *e, const ImageMono<Edgel *>& emap) const
{
  int choice,ia,ix,iy,ixlink,iylink,index;
  Edgel *eptr;
  int c0, r0, cl, rl;

  emap.ROI().Get(c0, r0, cl, rl);

  ia = e->GetIA();
  ix = e->GetIX();
  iy = e->GetIY();
  for (choice=0; choice<3; choice++) {
    // find the neighbour index. Note the the -48
    // is simply to convert chars into ints. Yuck.
    index = (int)(rightlist[3*ia+choice] - 48);
    // is there an edge at
    ixlink = ix + xoffs[index];
    if (ixlink<c0 || ixlink >= cl) break;
    iylink = iy + yoffs[index];
    if (iylink<r0 || iylink >= rl) break;
    eptr = emap[iylink][ixlink].y;
    if (eptr!=NULL && !eptr->GetLinked()) {
      eptr->SetLinked();
      return(eptr);
    }
  }
  return(NULL);
}

/////////////////////////////////////////////////////////////


void Canny::Linker(const ImageMono<Edgel *>& emap,
		   vector<Edgel *>& edgels, 
		   const float high_thresh, 
                   const int length_thresh,
		   vector<EdgeString *>& strings) const
{
  Edgel *leftel, *rightel;
  EdgeString *estring;
  EdgeString::iterator i;
  vector<Edgel *>::iterator e;
  int length;
    
  // This new linker should be faster than earlier versions.
  // First recall that the nonmax only supplies edges that are
  // above the lo_threshold.
  // Here we only start a string from edgels above the hi_threshold
  // The edgels can then be linked without strength tests
  
  // e is a pointer to a pointer to an edgel
  // Only interested in e if it is not linked and above hi thresh
  for (e=edgels.begin(); e!=edgels.end(); e++) {

    //"Proper"
    //if (!(*e)->GetLinked() && (*e)->GetS()> high_thresh) {
    //"Fast"
     if (!(*e)->GetLinked() && (*e)->GetS()> 4*high_thresh*high_thresh) {
      (*e)->SetLinked();
      
      // Create new string
      estring = new EdgeString;
      estring->push_front(*e);
      length=1; 
      leftel=rightel=*e;
      while ((leftel = LinkLeft(leftel, emap)) != NULL) {
	estring->push_front(leftel);
	++length;
      }
      while ((rightel = LinkRight(rightel, emap)) != NULL) {
	estring->push_back(rightel);
	++length;
      }
      
      if (length >= length_thresh) {
	// formalize as a string
	strings.push_back(estring);
      } else {
	// Set the edgels as unlinked.
	// Important, as they might get linked into a later string
	for (i=estring->begin(); i!=estring->end(); i++) {
	  (*i)->UnSetLinked();
	}
	delete estring;
      }
    }
  }
  return;
}


/////////////////////////////////////////////////////////////

/**
 * @param im the image in which edges will be found
 * @param edat the resulting edge data
 */
void Canny::Compute(const ImageMono<unsigned char>& im, 
		    EdgeData& edata) const
{
  int imwidth = im.GetWidth(),
      imheight = im.GetHeight(); 

  ImageMono<float> smoothed(imwidth, imheight);
  ImageMono<float> gx(imwidth, imheight);
  ImageMono<float> gy(imwidth, imheight);
  ImageMono<float> grad(imwidth, imheight);

  ConvolutionMask1D<float> gauss_mask(gauss_size);
  gauss_mask.MakeGaussian(gauss_sigma);

  // OK TO HERE!!

  // smooth
  Convolve<float>(im, gauss_mask, gauss_mask, smoothed);
  // compute spatial gradients
  CalcGradients(smoothed, gx, gy, grad);

  // This is an array of pointers to edgels
  ImageMono<Edgel *> emap(imwidth, imheight);

  // Note that NonMax also kills off any edgels
  // below the lo threshold
  NonMaximalSuppression(grad, 
			 gx, 
			 gy, 
			 lo_threshold, 
			 emap, 
			 edata.edgels);

  // Link the result to complete hysteris
     Linker(emap, edata.edgels, 
            hi_threshold, len_threshold, edata.strings);


  return;
}

///////////////////////////////////////////////////////////

/**
 * @param im the image in which edges will be found
 * @param edat the resulting edge data
 */
void Canny::Compute(const ImageYUV<unsigned char>& im, 
		    EdgeData& edata) const
{
  // Simply extract the Y channel
  const unsigned int w = im.GetWidth();
  const unsigned int h = im.GetHeight();
  VW::ImageMono<unsigned char> y(w, h);
  for (unsigned int row = 0; row < h; row++) {
    PixelYUV<unsigned char>* src_ptr = im.GetRowStart(row);
    PixelMono<unsigned char>* dest_ptr = y.GetRowStart(row);
    for (unsigned int col = 0; col < w; col++) {
      (dest_ptr++)->y = (src_ptr++)->y;
    }
  } 

  Compute(y,edata);
}
