/*  SceneLib: software for real-time SLAM

    improc.cpp
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

#include <iostream>
#include <VW/Utils/timer.h>
#include "correlate.h"
#include "improc.h"

#ifdef DEBUGDUMP
#undef DEBUGDUMP
#endif
#define DEBUGDUMP false


using namespace std;

/**
@addtogroup gSceneImproc
@{
*/

/*****************************Find Best Patch*********************************/

/** Simple function to find the eigenvalues of the 2*2 symmetric matrix
\f$ \begin{pmatrix}A & B \\ B & C \end{pmatrix} \nonumber \f$
*/
void find_eigenvalues(double A, double B, double C, 
		      double *eval1ptr, double *eval2ptr)
{
  double BB = sqrt( (A + C) * (A + C) - 4 * (A * C - B * B) );

  *eval1ptr = (A + C + BB) / 2.0;
  *eval2ptr = (A + C - BB) / 2.0;
}

/**
Function to scan over (a window in an) image and find the best patch by the Shi
and Tomasi criterion.
Method: as described in notes from 1/7/97. Form sums in an incremental
way to speed things up.
@param image The image to scan
@param ubest The x-co-ordinate of the best patch
@param vbest The y-co-ordinate of the best patch
@param evbest The smallest eigenvalue of the best patch (larger is better)
@param BOXSIZE The size of the patch to use
@param ustart The x-co-ordinate of the start of the search window
@param vstart The y-co-ordinate of the start of the search window
@param ufinish The x-co-ordinate of the end of the search window
@param vfinish The v-co-ordinate of the end of the search window
**/
void find_best_patch_inside_region(
  const VW::ImageMono<unsigned char> &image,
  unsigned int *ubest, unsigned int *vbest, double *evbest,
  const unsigned int BOXSIZE,
  unsigned int ustart, unsigned int vstart,
  unsigned int ufinish, unsigned int vfinish)
{
  /* Check that these limits aren't too close to the image edges.
     Note that we can't use the edge pixels because we can't form 
     gradients here. */
  if (ustart < (BOXSIZE - 1)/2 + 1) 
    ustart = (BOXSIZE - 1)/2 + 1;
  if (ufinish > image.GetWidth() - (BOXSIZE - 1)/2 - 1)
    ufinish = image.GetWidth() - (BOXSIZE - 1)/2 - 1;
  if (vstart < (BOXSIZE - 1)/2 + 1) 
    vstart = (BOXSIZE - 1)/2 + 1;
  if (vfinish > image.GetHeight() - (BOXSIZE - 1)/2 - 1)
    vfinish = image.GetHeight() - (BOXSIZE - 1)/2 - 1;

  // Is there anything left to search? If not, set the score to zero and return.
  if(vstart >= vfinish || ustart >= ufinish)
  {
    *ubest = ustart;
    *vbest = vstart;
    *evbest = 0;
    return;
  }
  
  /* Calculate the width we need to find gradients in. */
  unsigned int calc_width = ufinish - ustart + BOXSIZE - 1;

  /* Arrays which store column sums of height BOXSIZE */
  double CSgxsq[calc_width], CSgysq[calc_width], 
         CSgxgy[calc_width];

  /* For the final sums at each position (u, v) */
  double TSgxsq = 0.0, TSgysq = 0.0, TSgxgy = 0.0;

  double gx, gy;
  unsigned int u = ustart, v = vstart;
  double eval1, eval2;

  /* Initial stage: fill these sums for the first horizontal position */
  unsigned int cstart = ustart - (BOXSIZE - 1)/2;
  unsigned int cfinish = ufinish + (BOXSIZE - 1)/2;
  unsigned int rstart = vstart - (BOXSIZE - 1)/2;
  unsigned int i;
  unsigned int c, r;
  for (c = cstart, i = 0; c < cfinish; c++, i++)
  {
    CSgxsq[i] = 0; CSgysq[i] = 0; CSgxgy[i] = 0;
    for (r = rstart; r < rstart + BOXSIZE; r++)
    {
      gx = (image.GetPixel(c+1,r).Luminance() -
            image.GetPixel(c-1,r).Luminance()) / 2.0;
      gy = (image.GetPixel(c,r+1).Luminance() - 
            image.GetPixel(c,r-1).Luminance()) / 2.0;

      CSgxsq[i] += gx * gx;
      CSgysq[i] += gy * gy;
      CSgxgy[i] += gx * gy;      
    }
  }


  /* Now loop through u and v to form sums */
  *evbest = 0;
  for (v = vstart; v < vfinish; v++)
  {
    u = ustart;

    /* Form first sums for u = ustart */
    TSgxsq = 0.0, TSgysq = 0.0, TSgxgy = 0.0;
    for (i = 0; i < BOXSIZE; i++)
    {
      TSgxsq += CSgxsq[i];
      TSgysq += CSgysq[i];
      TSgxgy += CSgxgy[i];
    }

    for (u = ustart; u < ufinish; u++)
    {
      if (u != ustart)
      {
        /* Subtract old column, add new one */
        TSgxsq += CSgxsq[u - ustart + BOXSIZE - 1] - CSgxsq[u - ustart - 1];
        TSgysq += CSgysq[u - ustart + BOXSIZE - 1] - CSgysq[u - ustart - 1];
        TSgxgy += CSgxgy[u - ustart + BOXSIZE - 1] - CSgxgy[u - ustart - 1];
      }

      /* cout << "Sums for u = " << u << ", v = " << v 
              << ": TSgxsq = " << TSgxsq
              << ", TSgysq = " << TSgysq
              << ", TSgxgy = " << TSgxgy << "." << endl; */

      find_eigenvalues(TSgxsq, TSgxgy, TSgysq, &eval1, &eval2);

      /* cout << "Eigenvalues found in find_best_patch: " 
           << eval1 << ", " << eval2 << endl; */

      /* eval2 will be the smaller eigenvalue. Compare it with the one
         we've already got */
      if (eval2 > *evbest)
      {
        *ubest = u;
        *vbest = v;
        *evbest = eval2;
      }

      // evaluate_patch(image, u, v);
    }

    if (v != vfinish - 1)
    {
      /* Update the column sums for the next v */
      for (c = cstart, i = 0; c < cfinish; c++, i++)
      {
        // Subtract the old top pixel
        gx = (image.GetPixel(c+1, v - (BOXSIZE - 1)/2).Luminance() - 
              image.GetPixel(c-1, v - (BOXSIZE - 1)/2).Luminance()) / 2.0;
        gy = (image.GetPixel(c, v - (BOXSIZE - 1)/2 + 1).Luminance() - 
              image.GetPixel(c, v - (BOXSIZE - 1)/2 - 1).Luminance()) / 2.0;
        CSgxsq[i] -= gx * gx;
        CSgysq[i] -= gy * gy;
        CSgxgy[i] -= gx * gy;
            
        // Add the new bottom pixel
        gx = (image.GetPixel(c+1, v + (BOXSIZE - 1)/2 + 1).Luminance() - 
              image.GetPixel(c-1, v + (BOXSIZE - 1)/2 + 1).Luminance()) /  2.0;
        gy = (image.GetPixel(c, v + (BOXSIZE - 1)/2 + 1 + 1).Luminance() - 
              image.GetPixel(c, v + (BOXSIZE - 1)/2 + 1 - 1).Luminance()) / 2.0;
        CSgxsq[i] += gx * gx;
        CSgysq[i] += gy * gy;
        CSgxgy[i] += gx * gy;
      }     
    }
  }

  /* At the end: results */
  /* cout << "Best small eigenvalue of " << *evbest 
       << " found at u = " << *ubest << ", v = " << *vbest << endl; */
}



/**
   Version to find the best n patches within an image.
   Not very efficiently implemented I'm afraid.
   Now we expect the arguments ubest, vbest, evbest to be arrays of
   size n.
*/
void find_best_n_patches_inside_region(
                                    const VW::ImageMono<unsigned char> &image, 
				    const unsigned int n, 
				    unsigned int *ubest, unsigned int *vbest, 
				    double *evbest,
				    const unsigned int BOXSIZE,
				    unsigned int ustart, unsigned int vstart, 
				    unsigned int ufinish, unsigned int vfinish)
{
  /* Check that these limits aren't too close to the image edges.
     Note that we can't use the edge pixels because we can't form 
     gradients here. */
  if (ustart < (BOXSIZE - 1)/2 + 1) 
    ustart = (BOXSIZE - 1)/2 + 1;
  if (ufinish > image.GetWidth() - (BOXSIZE - 1)/2 - 1)
    ufinish = image.GetWidth() - (BOXSIZE - 1)/2 - 1;
  if (vstart < (BOXSIZE - 1)/2 + 1) 
    vstart = (BOXSIZE - 1)/2 + 1;
  if (vfinish > image.GetHeight() - (BOXSIZE - 1)/2 - 1)
    vfinish = image.GetHeight() - (BOXSIZE - 1)/2 - 1;

  /* Calculate the width we need to find gradients in. */
  unsigned int calc_width = ufinish - ustart + BOXSIZE - 1;

  /* Arrays which store column sums of height BOXSIZE */
  double CSgxsq[calc_width], CSgysq[calc_width], 
         CSgxgy[calc_width];

  /* For the final sums at each position (u, v) */
  double TSgxsq = 0.0, TSgysq = 0.0, TSgxgy = 0.0;

  double gx, gy;
  unsigned int u = ustart, v = vstart;
  double eval1, eval2;

  /* Initial stage: fill these sums for the first horizontal position */
  unsigned int cstart = ustart - (BOXSIZE - 1)/2;
  unsigned int cfinish = ufinish + (BOXSIZE - 1)/2;
  unsigned int rstart = vstart - (BOXSIZE - 1)/2;
  unsigned int i;
  unsigned int c, r;
  for (c = cstart, i = 0; c < cfinish; c++, i++)
  {
    CSgxsq[i] = 0; CSgysq[i] = 0; CSgxgy[i] = 0;
    for (r = rstart; r < rstart + BOXSIZE; r++)
    {
      gx = (image.GetPixel(c+1, r).Luminance() - image.GetPixel(c-1, r).Luminance()) / 2.0;
      gy = (image.GetPixel(c, r+1).Luminance() - image.GetPixel(c, r-1).Luminance()) / 2.0;

      CSgxsq[i] += gx * gx;
      CSgysq[i] += gy * gy;
      CSgxgy[i] += gx * gy;      
    }
  }

  double evarray[vfinish - vstart][ufinish - ustart];

  /* Now loop through u and v to form sums */
  for (v = vstart; v < vfinish; v++)
  {
    u = ustart;

    /* Form first sums for u = ustart */
    TSgxsq = 0.0, TSgysq = 0.0, TSgxgy = 0.0;
    for (i = 0; i < BOXSIZE; i++)
    {
      TSgxsq += CSgxsq[i];
      TSgysq += CSgysq[i];
      TSgxgy += CSgxgy[i];
    }

    for (u = ustart; u < ufinish; u++)
    {
      if (u != ustart)
      {
	/* Subtract old column, add new one */
	TSgxsq += CSgxsq[u - ustart + BOXSIZE - 1] - CSgxsq[u - ustart - 1]; 
	TSgysq += CSgysq[u - ustart + BOXSIZE - 1] - CSgysq[u - ustart - 1];
	TSgxgy += CSgxgy[u - ustart + BOXSIZE - 1] - CSgxgy[u - ustart - 1];
      }

      /* cout << "Sums for u = " << u << ", v = " << v 
	   << ": TSgxsq = " << TSgxsq 
	   << ", TSgysq = " << TSgysq
	   << ", TSgxgy = " << TSgxgy << "." << endl; */	   

      find_eigenvalues(TSgxsq, TSgxgy, TSgysq, &eval1, &eval2);

      /* cout << "Eigenvalues found in find_best_patch: " 
           << eval1 << ", " << eval2 << endl; */

      /* eval2 will be the smaller eigenvalue. Store in the array */
      evarray[v - vstart][u - ustart] = eval2;
      
      // evaluate_patch(image, u, v);
    }

    if (v != vfinish - 1)
    {
      /* Update the column sums for the next v */
      for (c = cstart, i = 0; c < cfinish; c++, i++)
      {
	// Subtract the old top pixel
	gx = (image.GetPixel(c+1, v - (BOXSIZE - 1)/2).Luminance() 
	    - image.GetPixel(c-1, v - (BOXSIZE - 1)/2).Luminance()) / 2.0;
	gy = (image.GetPixel(c, v - (BOXSIZE - 1)/2 + 1).Luminance()  
            - image.GetPixel(c, v - (BOXSIZE - 1)/2 - 1).Luminance()) / 2.0;
	CSgxsq[i] -= gx * gx;
	CSgysq[i] -= gy * gy;
	CSgxgy[i] -= gx * gy;      
      
	// Add the new bottom pixel
	gx = (image.GetPixel(c+1, v + (BOXSIZE - 1)/2 + 1).Luminance() 
            - image.GetPixel(c-1, v + (BOXSIZE - 1)/2 + 1).Luminance()) / 2.0;
	gy = (image.GetPixel(c, v + (BOXSIZE - 1)/2 + 1 + 1).Luminance()  
            - image.GetPixel(c, v + (BOXSIZE - 1)/2 + 1 - 1).Luminance()) / 2.0;
	CSgxsq[i] += gx * gx;
	CSgysq[i] += gy * gy;
	CSgxgy[i] += gx * gy; 
      }     
    }
  }

  /* Now: work out the best n patches which don't overlap each other */
  double best_so_far;
  int xdist, ydist;
  bool OKflag;
  double next_highest = 1000000000000.0;
  for (i = 0; i < n; i++)
  {
    best_so_far = 0.0;

    for (unsigned int y = 0; y < vfinish - vstart; y++)
      for (unsigned int x = 0; x < ufinish - ustart; x++)
      {

	if (evarray[y][x] > best_so_far && evarray[y][x] < next_highest)
	{
	  // We've found a high one: check it doesn't overlap with higher ones

	  // cout << "At x = " << x << ", y = " << y << ": evarray = " 
	  //    << evarray[y][x] << endl;

	  OKflag = true;
	  for (unsigned int j = 0; j < i; j++)
	  {
	    xdist = x + ustart - ubest[j];
	    ydist = y + vstart - vbest[j];
	    xdist = (xdist >= 0 ? xdist : -xdist);
	    ydist = (ydist >= 0 ? ydist : -ydist);
	    // cout << "i = " << i << ", j = " << j << " xdist = " << xdist
	    //	 << ", ydist = " << ydist << endl;
	    if (xdist < int(BOXSIZE) && ydist < int(BOXSIZE))
	    {
	      // cout << "Overlap with patch " << j << endl;
	      OKflag = false;
	      break;
	    }
	  }
	  if (OKflag)
	  {
	    ubest[i] = x + ustart;
	    vbest[i] = y + vstart;
	    evbest[i] = evarray[y][x];
	    best_so_far = evarray[y][x];
	    // cout << "Setting to patch " << i << endl;
	  }

	}

      }
    next_highest = evbest[i];
  }

}

/*****************************Elliptical Search*******************************/

/**
Do a search for patch in image within an elliptical region. The
search region is parameterised an inverse covariance matrix (a distance of
NO_SIGMA is used). The co-ordinates returned are those of centre of the patch.
@param image The image to search
@param patch The patch  to search for
@param centre The centre of the search ellipse 
@param PuInv The inverse covariance matrix to use
@param u The x-co-ordinate of the best patch location
@param v The y-co-ordinate of the best patch location
@param uBOXSIZE The size of the image patch (TODO: Shouldn't this be the same
as the size of patch?)
@returns <true> if the a good match is found (above CORRTHRESH2), <false>
otherwise
*/
bool elliptical_search(
  const VW::ImageMono<unsigned char> &image,
  const VW::ImageMono<unsigned char> &patch,
  const VNL::VectorFixed<2, double> centre,
  const VNL::MatrixFixed<2,2,double> &PuInv,
  unsigned int *u, unsigned int *v,
  const unsigned int uBOXSIZE)
{
  // We want to pass BOXSIZE as an unsigned int since it is,
  // but if we use it in the if statements below then C++ casts the
  // whole calculation to unsigned ints, so it is never < 0!
  // Force it to get the right answer by using an int version of BOXSIZE
  int BOXSIZE = uBOXSIZE;

  /* The dimensions of the bounding box of the ellipse we want to search in */
  unsigned int halfwidth = (unsigned int) (NO_SIGMA / 
    sqrt( PuInv(0, 0) - PuInv(0, 1) * PuInv(0, 1) / PuInv(1, 1) ));
  unsigned int halfheight = (unsigned int) (NO_SIGMA / 
    sqrt( PuInv(1, 1) - PuInv(0, 1) * PuInv(0, 1) / PuInv(0, 0) ));

  int ucentre = int(centre[0] + 0.5);
  int vcentre = int(centre[1] + 0.5);

  /* Limits of search */
  int urelstart = -halfwidth; 
  int urelfinish = halfwidth;
  int vrelstart = -halfheight;
  int vrelfinish = halfheight;

  /* Check these limits aren't outside the image */
  if(ucentre + urelstart - (BOXSIZE - 1) / 2 < 0)
    urelstart = (BOXSIZE - 1) / 2 - ucentre;
  if(ucentre + urelfinish - (BOXSIZE - 1) / 2 >
     int(image.GetWidth()) - BOXSIZE) 
    urelfinish = image.GetWidth() - BOXSIZE - ucentre + (BOXSIZE-1) / 2;
  if(vcentre + vrelstart - (BOXSIZE - 1) / 2 < 0)
    vrelstart = (BOXSIZE - 1) / 2 - vcentre;
  if(vcentre + vrelfinish - (BOXSIZE - 1) / 2 >
     int(image.GetHeight()) - BOXSIZE)
    vrelfinish = int(image.GetHeight()) - BOXSIZE - vcentre + (BOXSIZE-1) / 2;

  /* Search counters */
  int urel, vrel; 

  double corrmax = 1000000.0;
  double corr;

  // For passing to and_correlate2_warning
  double sdpatch, sdimage;

  /* Do the search */
  for(urel = urelstart; urel <= urelfinish; urel++)
  {
    for(vrel = vrelstart; vrel <= vrelfinish; vrel++)
    {
      if(PuInv(0, 0) * urel * urel
          + 2 * PuInv(0, 1) * urel * vrel
          + PuInv(1, 1) * vrel * vrel  <  NO_SIGMA * NO_SIGMA )
      {
        corr = correlate2_warning(0, 0, BOXSIZE, BOXSIZE,
              ucentre + urel - (BOXSIZE - 1) / 2,
              vcentre + vrel - (BOXSIZE - 1) / 2,
              patch, image, &sdpatch, &sdimage);

        if (corr <= corrmax)
        {
          if (sdpatch < CORRELATION_SIGMA_THRESHOLD)
            ; // cout << "Low patch sigma." << endl;
          else if (sdimage < CORRELATION_SIGMA_THRESHOLD)
            ; // cout << "Low image sigma." << endl;
          else
          {
            corrmax = corr;
            *u = urel + ucentre;
            *v = vrel + vcentre;
          }
        }
      }
    }
  }

  //  cerr << "Best match correlation: " << corrmax << endl;

  if(corrmax > CORRTHRESH2)
  {
    return false;
  }

  return true;
}



SearchMultipleOverlappingEllipses::SearchDatum::SearchDatum(
  const VNL::MatrixFixed<2,2,double>& _PuInv,
  const VNL::VectorFixed<2,double>& _search_centre) :
      PuInv(_PuInv),
      search_centre(_search_centre),
      result_flag(false),
      result_u(0),
      result_v(0)
{
  typedef unsigned int uint;
  halfwidth = uint(NO_SIGMA /
        sqrt(PuInv.Get(0, 0) - PuInv.Get(0, 1)
        * PuInv.Get(0, 1) / PuInv.Get(1, 1)));
  halfheight = uint(NO_SIGMA /
        sqrt(PuInv.Get(1, 1) - PuInv.Get(0, 1)
        * PuInv.Get(0, 1) / PuInv.Get(0, 0)));
}

/**
Checks whether the point \f$(u,v)\f$ is inside the bounding box of the
(three standard deviation) ellipse.
@param u The x-co-ordinate to test
@param v The x-co-ordinate to test
*/
bool SearchMultipleOverlappingEllipses::SearchDatum::inside_fast(int u, int v)
{
  u = abs(u - int(search_centre[0] + 0.5));
  if(u > int(halfwidth))
    return false;
  v = abs(v - int(search_centre[1] + 0.5));
  if(v > int(halfheight))
    return false;
  return true;
}

/**
Checks whether the point \f$(u,v)\f$ is inside the ellipse (within three
standard deviations).
@param u The x-co-ordinate to test
@param v The x-co-ordinate to test
*/
bool SearchMultipleOverlappingEllipses::SearchDatum::inside(int u, int v)
{
  u = abs(u - int(search_centre[0] + 0.5));
  v = abs(v - int(search_centre[1] + 0.5));
  return inside_relative(u,v);
}

/**
Checks whether the point \f$(u,v)\f$ is inside the ellipse (within three
standard deviations). This version assume that distances are relative to the
centre of the ellipse, not absolute co-ordinates.
@param u The horizontal distance from the search centre 
@param v The vertical distance from the search centre
*/
bool SearchMultipleOverlappingEllipses::SearchDatum::inside_relative(
  int u, int v)
{
  return (PuInv.Get(0, 0) * u * u
      + 2 * PuInv.Get(0, 1) * u * v
      + PuInv.Get(1, 1) * v * v <  NO_SIGMA * NO_SIGMA );
}


          
/**
Add a search ellipse to the class.
@param PuInv The inverse covariance of the feature location. The search will be
over the ellipse defined by NO_SIGMA standard deviations from the centre of the
search.
@param search_centre The centre of the search.
*/
void SearchMultipleOverlappingEllipses::add_ellipse(
  const VNL::MatrixFixed<2,2,double> &PuInv,
  const VNL::VectorFixed<2,double>& search_centre)
{
  m_searchdata.push_back(SearchDatum(PuInv, search_centre));
}

/**
Search for the image patch over all of the ellipses registered with the class.
Since correlation is expensive we locally cache the correlation results so that
we only do it once at each image location.
*/
void SearchMultipleOverlappingEllipses::search()
{
  if (DEBUGDUMP) cout << "Search multiple overlapping ellipses." << endl;

  // Rather than working out an overall bounding box, we'll be slightly
  // lazy and make an array for correlation results which is the same size 
  // as the image
  // Pixels in this array in raster order
  VW::ImageMono<double> correlation_image(m_image->GetWidth(),
                                          m_image->GetHeight());

  // Set all positions to impossible correlation value
  fill_image(correlation_image, -1.0);

  // Now, we loop over ellipses
  unsigned int count = 0;
  for(SearchData::iterator i = m_searchdata.begin();
      i != m_searchdata.end(); i++)
  {
    /* Limits of search */
    int urelstart = -i->halfwidth;
    int urelfinish = i->halfwidth;
    int vrelstart = -i->halfheight;
    int vrelfinish = i->halfheight;

    int ucentre = int(i->search_centre(0));
    int vcentre = int(i->search_centre(1));

    /* Check these limits aren't outside the image */
    if(ucentre + urelstart - int(m_boxsize - 1) / 2 < 0)
    {
      urelstart = int(m_boxsize - 1) / 2 - ucentre;
    }
    if(ucentre + urelfinish - int(m_boxsize - 1) / 2 >
      int(m_image->GetWidth()) - int(m_boxsize))
    {
      urelfinish = int(m_image->GetWidth()) - int(m_boxsize) -
        ucentre + int(m_boxsize - 1) / 2;
    }
    if(vcentre + vrelstart - int(m_boxsize - 1) / 2 < 0)
    {
      vrelstart = int(m_boxsize - 1) / 2 - vcentre;
    }
    if(vcentre + vrelfinish - int(m_boxsize - 1) / 2 >
      int(m_image->GetHeight()) - int(m_boxsize))
    {
      vrelfinish = int(m_image->GetHeight()) - int(m_boxsize) -
        vcentre + int(m_boxsize - 1) / 2;
    }

    /* Search counters */
    int urel, vrel; 

    double corrmax = 1000000.0;
    double corr;

    // For passing to and_correlate2_warning
    double sdpatch, sdimage;

    /* Do the search */
    for(urel = urelstart; urel <= urelfinish; urel++)
    {
      for(vrel = vrelstart; vrel <= vrelfinish; vrel++)
      {
        if(i->inside_relative(urel, vrel))
        {
          if (DEBUGDUMP) cout << "Searching at "
                << ucentre + urel << ", "
                << vcentre + vrel
                << endl;
    
          // We are inside ellipse
          // Has this place been searched before?
          VW::PixelMono<double>* corr_ptr = correlation_image.GetRowStart(
            vcentre + vrel) + ucentre + urel;
          if(corr_ptr->y != -1.0)
          {
            corr = corr_ptr->y;
            if (DEBUGDUMP) cout << "Searched before, score "
                                << corr << endl;
          }
          else
          {
            corr = correlate2_warning(0, 0, m_boxsize, m_boxsize,
              ucentre + urel - (int)(m_boxsize - 1) / 2,
              vcentre + vrel - (int)(m_boxsize - 1) / 2,
              *m_patch, *m_image, &sdpatch, &sdimage);

            if (sdimage < CORRELATION_SIGMA_THRESHOLD)
            {
              if (DEBUGDUMP) cout << "Low image sigma sdimage "
                                  << sdimage << endl;
              corr += LOW_SIGMA_PENALTY;
            }

            *corr_ptr = corr;

            if (DEBUGDUMP) cout << "New search, score "
                                << corr << endl;
          }

          if (corr <= corrmax)
          {
            corrmax = corr;
            i->result_u = urel + ucentre;
            i->result_v = vrel + vcentre;
          }
        }
      }
    }

    //  cerr << "Best match correlation: " << corrmax << endl;

    // Threshold correlation score: check if good enough 
    if(corrmax > CORRTHRESH2)
    {
      // cout << "Matching correlation  not good enough." << endl;
      i->result_flag = false;
    }
    else
      i->result_flag = true;

    if (DEBUGDUMP) cout << "Search " << count++ << " correlation " << corrmax
                        << " location " << i->result_u << ", "
                        << i->result_v << endl;
  }
}


/**
@}
*/

/*****************************************************************************/
