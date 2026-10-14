/** 
 * @file   warp.cpp
 * @author ID Reid
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


#include <iostream>
#include <math.h>

#include <VNL/matrix.h>
#include <VNL/vector.h>
#include <VNL/Algo/matrixinverse.h>

#include <VW/MathBits/nan.h>
#include <VW/GeomCompute/radialdistortion.h>
#include <VW/image.h>

#include "warp.h"


using namespace std;
using namespace VW;

template <class InType, class OutType>
static void WarpDistortBicubic(const InType &im, 
			    const VNL::MatrixFixed<3,3,double>& M,
			     const double kappa,
			     const double centre_x, const double centre_y,
			    OutType &res)
{
  int c0, r0, cl, rl;
  im.ROI().Get(c0, r0, cl, rl);

  const unsigned int width = res.GetWidth();
  const unsigned int height = res.GetHeight();

  // Can't process first or last two rows/columns, so shrink roi
  c0++; r0++; cl-=2; rl-=2;
  for (unsigned int r=0; r<height; r++) {
    const double rd = double(r);
    typename OutType::PixelPtr dest_ptr = res.GetRowStart(r);
    
    for (unsigned int c=0; c<width; c++, dest_ptr++) {
      const double cd = double(c);
      
      const double x = M[0][0]*cd + M[0][1]*rd + M[0][2];
      const double y = M[1][0]*cd + M[1][1]*rd + M[1][2];
      const double z = M[2][0]*cd + M[2][1]*rd + M[2][2];
      
      double c1 = x/z;
      double r1 = y/z;

      VW::RadialDistortion::DistortInPlace(kappa,
					   c1, r1, 
					   centre_x, centre_y);
	
      const int int_c1 = int(c1);
      const int int_r1 = int(r1);
	
	
      if (int_c1<c0 || int_c1>=cl ||
	  int_r1<r0 || int_r1>=rl) {
	dest_ptr->Set(typename OutType::DataType(VW::NaN));
	continue;
      }
      
      
      const double dx = (c1 - int_c1);
      const double dy = (r1 - int_r1);
      const double sx0 = dx*(-0.33333333f + dx*(0.5f - dx*0.1666667f));
      const double sx1 = 1.0f - dx*(0.5f + dx*(1.0f - dx*0.5f));
      const double sx2 = dx*(1.0f + dx*(0.5f - dx*0.5f));
      const double sx3 = 0.1666667f*dx*(dx*dx - 1.0f);
      const double sy0 = dy*(-0.33333333f + dy*(0.5f - dy*0.1666667f));
      const double sy1 = 1.0f - dy*(0.5f + dy*(1.0f - dy*0.5f));
      const double sy2 = dy*(1.0f + dy*(0.5f - dy*0.5f));
      const double sy3 = 0.1666667f*dy*(dy*dy - 1.0f);
      
      // Get the pixels around this position
      typename InType::PixelPtr* rptr = im.GetRowPointer(int_r1);
      const typename InType::PixelType& p00 = rptr[-1][int_c1-1];
      const typename InType::PixelType& p01 = rptr[-1][int_c1];
      const typename InType::PixelType& p02 = rptr[-1][int_c1+1];
      const typename InType::PixelType& p03 = rptr[-1][int_c1+2];
      const typename InType::PixelType& p10 = rptr[ 0][int_c1-1];
      const typename InType::PixelType& p11 = rptr[ 0][int_c1];
      const typename InType::PixelType& p12 = rptr[ 0][int_c1+1];
      const typename InType::PixelType& p13 = rptr[ 0][int_c1+2];
      const typename InType::PixelType& p20 = rptr[ 1][int_c1-1];
      const typename InType::PixelType& p21 = rptr[ 1][int_c1];
      const typename InType::PixelType& p22 = rptr[ 1][int_c1+1];
      const typename InType::PixelType& p23 = rptr[ 1][int_c1+2];
      const typename InType::PixelType& p30 = rptr[ 2][int_c1-1];
      const typename InType::PixelType& p31 = rptr[ 2][int_c1];
      const typename InType::PixelType& p32 = rptr[ 2][int_c1+1];
      const typename InType::PixelType& p33 = rptr[ 2][int_c1+2];
      
      // Bilinear interp on this row, column and +1.
      typename OutType::PixelType val;
      
      val = (sx0*p00 + sx1*p01 + sx2*p02 + sx3*p03) * sy0
	+ (sx0*p10 + sx1*p11 + sx2*p12 + sx3*p13) * sy1 
	+ (sx0*p20 + sx1*p21 + sx2*p22 + sx3*p23) * sy2 
	+ (sx0*p30 + sx1*p31 + sx2*p32 + sx3*p33) * sy3; 
      *dest_ptr = val;
      
    }
  }
}

template <class InType, class OutType>
static void WarpDistortBiquadratic(const InType &im, 
			    const VNL::MatrixFixed<3,3,double>& M,
			     const double kappa,
			     const double centre_x, const double centre_y,
			    OutType &res)
{
  int c0, r0, cl, rl;
  im.ROI().Get(c0, r0, cl, rl);

  const unsigned int width = res.GetWidth();
  const unsigned int height = res.GetHeight();
    // Can't process first or last row/column, so move ROI in 1
    c0++; r0++; cl--; rl--;
    for (unsigned int r=0; r<height; r++) {
      const double rd = double(r);
      typename OutType::PixelPtr dest_ptr = res.GetRowStart(r);

      for (unsigned int c=0; c<width; c++, dest_ptr++) {
	const double cd = double(c);

	const double x = M[0][0]*cd + M[0][1]*rd + M[0][2];
	const double y = M[1][0]*cd + M[1][1]*rd + M[1][2];
	const double z = M[2][0]*cd + M[2][1]*rd + M[2][2];

      double c1 = x/z;
      double r1 = y/z;

      VW::RadialDistortion::DistortInPlace(kappa,
					   c1, r1, 
					   centre_x, centre_y);

      const int int_c1 = int(c1);
      const int int_r1 = int(r1);

      if (int_c1<c0 || int_c1>=cl ||
	  int_r1<r0 || int_r1>=rl) {
	dest_ptr->Set(typename OutType::DataType(VW::NaN));
	continue;
      }


      const double dx = 0.5*(c1 - int_c1);
      const double dy = 0.5*(r1 - int_r1);
      const double dx2 = 2.0*dx*dx;
      const double dy2 = 2.0*dy*dy;
      const double sx0 = dx2 - dx;
      const double sx1 = 1.0 - 2.0*dx2;
      const double sx2 = dx + dx2;

      // Get the pixels around this position
      typename InType::PixelPtr* rptr = im.GetRowPointer(int_r1);
      const typename InType::PixelType& p00 = rptr[-1][int_c1-1];
      const typename InType::PixelType& p01 = rptr[-1][int_c1];
      const typename InType::PixelType& p02 = rptr[-1][int_c1+1];
      const typename InType::PixelType& p10 = rptr[ 0][int_c1-1];
      const typename InType::PixelType& p11 = rptr[ 0][int_c1];
      const typename InType::PixelType& p12 = rptr[ 0][int_c1+1];
      const typename InType::PixelType& p20 = rptr[ 1][int_c1-1];
      const typename InType::PixelType& p21 = rptr[ 1][int_c1];
      const typename InType::PixelType& p22 = rptr[ 1][int_c1+1];

    // Bilinear interp on this row, column and +1.
      typename OutType::PixelType val;

      val = (sx2*p22 + sx1*p21 + sx0*p20) * (dy+dy2) 
	+ (sx2*p12 + sx1*p11 + sx0*p10) * (1.0f -2.0f*dy2)
	+ (sx2*p02 + sx1*p01 + sx0*p00) * (dy2 - dy);
      *dest_ptr = val;
		
    }
  }
}

template <class InType, class OutType>
static void WarpDistortBilinear(const InType &im, 
				 const VNL::MatrixFixed<3,3,double>& M,
			     const double kappa,
			     const double centre_x, const double centre_y,
				 OutType &res)
{
  int c0, r0, cl, rl;
  im.ROI().Get(c0, r0, cl, rl);

  const unsigned int width = res.GetWidth();
  const unsigned int height = res.GetHeight();

  // Can't process last row/column, so move ROI in 1
  cl--; rl--;
  for (unsigned int r=0; r<height; r++) {
    const double rd = double(r);
    typename OutType::PixelPtr dest_ptr = res.GetRowStart(r);

    for (unsigned int c=0; c<width; c++, dest_ptr++) {
      const double cd = double(c);
	
      const double x = M[0][0]*cd + M[0][1]*rd + M[0][2];
      const double y = M[1][0]*cd + M[1][1]*rd + M[1][2];
      const double z = M[2][0]*cd + M[2][1]*rd + M[2][2];

      double c1 = x/z;
      double r1 = y/z;

      VW::RadialDistortion::DistortInPlace(kappa,
					   c1, r1, 
					   centre_x, centre_y);

      const int int_c1 = int(c1);
      const int int_r1 = int(r1);
      const double frac_c = c1 - int_c1;
      const double frac_r = r1 - int_r1;

      // Get a pointer to this row
      typename InType::PixelPtr* rptr = im.GetRowPointer(int_r1);
  

      if (int_c1<c0 || int_c1>=cl ||
	  int_r1<r0 || int_r1>=rl) {
	dest_ptr->Set(typename OutType::DataType(VW::NaN));
	continue;
      }

      // Bilinear interp on this row, column and +1.
      typename OutType::PixelType val;
      //   Note that it is (marginally) faster to access pixels on the same row
      val =
	(1.0-frac_c)*(1.0-frac_r)*rptr[0][int_c1] +
	(frac_c)*(1.0-frac_r)    *rptr[0][int_c1+1] +
	(1.0-frac_c)*(frac_r)    *rptr[1][int_c1] +
	(frac_c)*(frac_r)        *rptr[1][int_c1+1];

      *dest_ptr = val;
		
    }
  }
}

template <class InType, class OutType>
static void WarpDistortNearestNeighbour(const InType &im, 
				 const VNL::MatrixFixed<3,3,double>& M,
			     const double kappa,
			     const double centre_x, const double centre_y,
				 OutType &res)
{
  int c0, r0, cl, rl;
  im.ROI().Get(c0, r0, cl, rl);

  const unsigned int width = res.GetWidth();
  const unsigned int height = res.GetHeight();

  for (unsigned int r=0; r<height; r++) {
    const double rd = double(r);
    typename OutType::PixelPtr dest_ptr = res.GetRowStart(r);
    
    for (unsigned int c=0; c<width; c++, dest_ptr++) {
      const double cd = double(c);
      
      const double x = M[0][0]*cd + M[0][1]*rd + M[0][2];
      const double y = M[1][0]*cd + M[1][1]*rd + M[1][2];
      const double z = M[2][0]*cd + M[2][1]*rd + M[2][2];
      
      double c1 = x/z;
      double r1 = y/z;
      
      VW::RadialDistortion::DistortInPlace(kappa,
					   c1, r1, 
					   centre_x, centre_y);

     const int int_c1 = int(c1+0.5);
      const int int_r1 = int(r1+0.5);
		
      if (int_c1<c0 || int_c1>=cl ||
	  int_r1<r0 || int_r1>=rl) {
	dest_ptr->Set(typename OutType::DataType(VW::NaN));
	continue;
      }
      
      // Just get the source pixel.
      *dest_ptr = im[int_r1][int_c1];
    }
  }
}

template <class InType, class OutType>
static void WarpBicubic(const InType &im, 
			    const VNL::MatrixFixed<3,3,double>& M,
			    OutType &res)
{
  int c0, r0, cl, rl;
  im.ROI().Get(c0, r0, cl, rl);

  const unsigned int width = res.GetWidth();
  const unsigned int height = res.GetHeight();

  // Can't process first or last two rows/columns, so shrink roi
  c0++; r0++; cl-=2; rl-=2;
  for (unsigned int r=0; r<height; r++) {
    const double rd = double(r);
    typename OutType::PixelPtr dest_ptr = res.GetRowStart(r);
    
    for (unsigned int c=0; c<width; c++, dest_ptr++) {
      const double cd = double(c);
      
      const double x = M[0][0]*cd + M[0][1]*rd + M[0][2];
      const double y = M[1][0]*cd + M[1][1]*rd + M[1][2];
      const double z = M[2][0]*cd + M[2][1]*rd + M[2][2];
      
      const double c1 = x/z;
      const double r1 = y/z;
	
      const int int_c1 = int(c1);
      const int int_r1 = int(r1);
	
	
      if (int_c1<c0 || int_c1>=cl ||
	  int_r1<r0 || int_r1>=rl) {
	dest_ptr->Set(typename OutType::DataType(VW::NaN));
	continue;
      }
      
      
      const double dx = (c1 - int_c1);
      const double dy = (r1 - int_r1);
      const double sx0 = dx*(-0.33333333f + dx*(0.5f - dx*0.1666667f));
      const double sx1 = 1.0f - dx*(0.5f + dx*(1.0f - dx*0.5f));
      const double sx2 = dx*(1.0f + dx*(0.5f - dx*0.5f));
      const double sx3 = 0.1666667f*dx*(dx*dx - 1.0f);
      const double sy0 = dy*(-0.33333333f + dy*(0.5f - dy*0.1666667f));
      const double sy1 = 1.0f - dy*(0.5f + dy*(1.0f - dy*0.5f));
      const double sy2 = dy*(1.0f + dy*(0.5f - dy*0.5f));
      const double sy3 = 0.1666667f*dy*(dy*dy - 1.0f);
      
      // Get the pixels around this position
      typename InType::PixelPtr* rptr = im.GetRowPointer(int_r1);
      const typename InType::PixelType& p00 = rptr[-1][int_c1-1];
      const typename InType::PixelType& p01 = rptr[-1][int_c1];
      const typename InType::PixelType& p02 = rptr[-1][int_c1+1];
      const typename InType::PixelType& p03 = rptr[-1][int_c1+2];
      const typename InType::PixelType& p10 = rptr[ 0][int_c1-1];
      const typename InType::PixelType& p11 = rptr[ 0][int_c1];
      const typename InType::PixelType& p12 = rptr[ 0][int_c1+1];
      const typename InType::PixelType& p13 = rptr[ 0][int_c1+2];
      const typename InType::PixelType& p20 = rptr[ 1][int_c1-1];
      const typename InType::PixelType& p21 = rptr[ 1][int_c1];
      const typename InType::PixelType& p22 = rptr[ 1][int_c1+1];
      const typename InType::PixelType& p23 = rptr[ 1][int_c1+2];
      const typename InType::PixelType& p30 = rptr[ 2][int_c1-1];
      const typename InType::PixelType& p31 = rptr[ 2][int_c1];
      const typename InType::PixelType& p32 = rptr[ 2][int_c1+1];
      const typename InType::PixelType& p33 = rptr[ 2][int_c1+2];
      
      // Bilinear interp on this row, column and +1.
      typename OutType::PixelType val;
      
      val = (sx0*p00 + sx1*p01 + sx2*p02 + sx3*p03) * sy0
	+ (sx0*p10 + sx1*p11 + sx2*p12 + sx3*p13) * sy1 
	+ (sx0*p20 + sx1*p21 + sx2*p22 + sx3*p23) * sy2 
	+ (sx0*p30 + sx1*p31 + sx2*p32 + sx3*p33) * sy3; 
      *dest_ptr = val;
      
    }
  }
}

template <class InType, class OutType>
static void WarpBiquadratic(const InType &im, 
			    const VNL::MatrixFixed<3,3,double>& M,
			    OutType &res)
{
  int c0, r0, cl, rl;
  im.ROI().Get(c0, r0, cl, rl);

  const unsigned int width = res.GetWidth();
  const unsigned int height = res.GetHeight();
    // Can't process first or last row/column, so move ROI in 1
    c0++; r0++; cl--; rl--;
    for (unsigned int r=0; r<height; r++) {
      const double rd = double(r);
      typename OutType::PixelPtr dest_ptr = res.GetRowStart(r);

      for (unsigned int c=0; c<width; c++, dest_ptr++) {
	const double cd = double(c);

	const double x = M[0][0]*cd + M[0][1]*rd + M[0][2];
	const double y = M[1][0]*cd + M[1][1]*rd + M[1][2];
	const double z = M[2][0]*cd + M[2][1]*rd + M[2][2];

      const double c1 = x/z;
      const double r1 = y/z;

      const int int_c1 = int(c1);
      const int int_r1 = int(r1);


      if (int_c1<c0 || int_c1>=cl ||
	  int_r1<r0 || int_r1>=rl) {
	dest_ptr->Set(typename OutType::DataType(VW::NaN));
	continue;
      }


      const double dx = 0.5*(c1 - int_c1);
      const double dy = 0.5*(r1 - int_r1);
      const double dx2 = 2.0*dx*dx;
      const double dy2 = 2.0*dy*dy;
      const double sx0 = dx2 - dx;
      const double sx1 = 1.0 - 2.0*dx2;
      const double sx2 = dx + dx2;

      // Get the pixels around this position
      typename InType::PixelPtr* rptr = im.GetRowPointer(int_r1);
      const typename InType::PixelType& p00 = rptr[-1][int_c1-1];
      const typename InType::PixelType& p01 = rptr[-1][int_c1];
      const typename InType::PixelType& p02 = rptr[-1][int_c1+1];
      const typename InType::PixelType& p10 = rptr[ 0][int_c1-1];
      const typename InType::PixelType& p11 = rptr[ 0][int_c1];
      const typename InType::PixelType& p12 = rptr[ 0][int_c1+1];
      const typename InType::PixelType& p20 = rptr[ 1][int_c1-1];
      const typename InType::PixelType& p21 = rptr[ 1][int_c1];
      const typename InType::PixelType& p22 = rptr[ 1][int_c1+1];

    // Bilinear interp on this row, column and +1.
      typename OutType::PixelType val;

      val = (sx2*p22 + sx1*p21 + sx0*p20) * (dy+dy2) 
	+ (sx2*p12 + sx1*p11 + sx0*p10) * (1.0f -2.0f*dy2)
	+ (sx2*p02 + sx1*p01 + sx0*p00) * (dy2 - dy);
      *dest_ptr = val;
		
    }
  }
}

template <class InType, class OutType>
static void WarpBilinear(const InType &im, 
				 const VNL::MatrixFixed<3,3,double>& M,
				 OutType &res)
{
  int c0, r0, cl, rl;
  im.ROI().Get(c0, r0, cl, rl);

  const unsigned int width = res.GetWidth();
  const unsigned int height = res.GetHeight();

  // Can't process last row/column, so move ROI in 1
  cl--; rl--;
  for (unsigned int r=0; r<height; r++) {
    const double rd = double(r);
    typename OutType::PixelPtr dest_ptr = res.GetRowStart(r);

    for (unsigned int c=0; c<width; c++, dest_ptr++) {
      const double cd = double(c);
	
      const double x = M[0][0]*cd + M[0][1]*rd + M[0][2];
      const double y = M[1][0]*cd + M[1][1]*rd + M[1][2];
      const double z = M[2][0]*cd + M[2][1]*rd + M[2][2];

      const double c1 = x/z;
      const double r1 = y/z;

      const int int_c1 = int(c1);
      const int int_r1 = int(r1);
      const double frac_c = c1 - int_c1;
      const double frac_r = r1 - int_r1;

      // Get a pointer to this row
      typename InType::PixelPtr* rptr = im.GetRowPointer(int_r1);
  

      if (int_c1<c0 || int_c1>=cl ||
	  int_r1<r0 || int_r1>=rl) {
	dest_ptr->Set(typename OutType::DataType(VW::NaN));
	continue;
      }

      // Bilinear interp on this row, column and +1.
      typename OutType::PixelType val;
      //   Note that it is (marginally) faster to access pixels on the same row
      val =
	(1.0-frac_c)*(1.0-frac_r)*rptr[0][int_c1] +
	(frac_c)*(1.0-frac_r)    *rptr[0][int_c1+1] +
	(1.0-frac_c)*(frac_r)    *rptr[1][int_c1] +
	(frac_c)*(frac_r)        *rptr[1][int_c1+1];

      *dest_ptr = val;
		
    }
  }
}

template <class InType, class OutType>
static void WarpNearestNeighbour(const InType &im, 
				 const VNL::MatrixFixed<3,3,double>& M,
				 OutType &res)
{
  int c0, r0, cl, rl;
  im.ROI().Get(c0, r0, cl, rl);

  const unsigned int width = res.GetWidth();
  const unsigned int height = res.GetHeight();

  for (unsigned int r=0; r<height; r++) {
    const double rd = double(r);
    typename OutType::PixelPtr dest_ptr = res.GetRowStart(r);
    
    for (unsigned int c=0; c<width; c++, dest_ptr++) {
      const double cd = double(c);
      
      const double x = M[0][0]*cd + M[0][1]*rd + M[0][2];
      const double y = M[1][0]*cd + M[1][1]*rd + M[1][2];
      const double z = M[2][0]*cd + M[2][1]*rd + M[2][2];
      
      const double c1 = x/z;
      const double r1 = y/z;
      
      const int int_c1 = int(c1+0.5);
      const int int_r1 = int(r1+0.5);
		
      if (int_c1<c0 || int_c1>=cl ||
	  int_r1<r0 || int_r1>=rl) {
	dest_ptr->Set(typename OutType::DataType(VW::NaN));
	continue;
      }
      
      // Just get the source pixel.
      *dest_ptr = im[int_r1][int_c1];
    }
  }
}
















// NOW the compute function that calls all the previous versions




template <class InType, class OutType>
void Warp::Compute(const InType &im, OutType &res,
		   InterpolationMethod method)
{
  if (!res.IsAlloced()) {
    res.AllocImageData(im.GetWidth(), im.GetHeight());
  }

  // Clear result image
  for (unsigned int r=0; r<res.GetHeight(); r++) {
    typename OutType::PixelPtr ptr = res.GetRowStart(r);
    for (unsigned int c=0; c<res.GetWidth(); c++,ptr++) 
      *ptr = typename OutType::DataType(0);
  }
  const VNL::MatrixInverse<double> mi(_H);
  const VNL::MatrixFixed<3,3,double> M(mi);
  

  if (_kappa==0.0) {

  switch(method) {

  case NearestNeighbour:
    WarpNearestNeighbour(im,M,res);
    break;

  default:
  case Bilinear:
    WarpBilinear(im,M,res);
    break;

  case Biquadratic:
    WarpBiquadratic(im,M,res);
    break;

  case Bicubic:
    WarpBicubic(im,M,res);
    break;

  };

  } else {

    switch(method) {
      
    case NearestNeighbour:
      WarpDistortNearestNeighbour(im,M,
				  _kappa, 
				  _distortion_centre_x, _distortion_centre_y, 
				  res);
      break;
      
    default:
    case Bilinear:
      WarpDistortBilinear(im,M,				  
			  _kappa, 
			  _distortion_centre_x, _distortion_centre_y, 
			  res);
      break;
      
    case Biquadratic:
      WarpDistortBiquadratic(im,M,				  
			     _kappa, 
			     _distortion_centre_x, _distortion_centre_y, 
			     res);
      break;
      
    case Bicubic:
      WarpDistortBicubic(im,M,				  
			 _kappa, 
			 _distortion_centre_x, _distortion_centre_y, 
			 res);
      break;
      
    };

  }

  return;
}

// Instantiate some versions
#ifdef INSTANTIATE
#  undef INSTANTIATE
#endif
# define INSTANTIATE(M,N) \
  template void Warp::Compute(const M &, N &, InterpolationMethod);

INSTANTIATE(ImageMono<unsigned char>, ImageMono<unsigned char>); 
INSTANTIATE(ImageMono<unsigned char>, ImageMono<float>); 
INSTANTIATE(ImageMono<float>, ImageMono<float>); 
INSTANTIATE(ImageRGB<unsigned char>, ImageRGB<float>); 
INSTANTIATE(ImageRGB<float>, ImageRGB<float>); 
INSTANTIATE(ImageYUV<unsigned char>, ImageYUV<unsigned char>); 

#undef INSTANTIATE
