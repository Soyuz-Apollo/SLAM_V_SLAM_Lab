/** 
 * @file   parametricflow.cpp
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

#ifdef _WIN32
// finite is in a daft place on windows
#include <float.h>
#define finite(M) _finite(M)
#endif

#include <VNL/vector.h>
#include <VNL/matrix.h>
#include <VNL/Algo/cholesky.h>
#include <VNL/Algo/levenbergmarquardt.h>

#include "VW/image.h"

#include "convolve.h"
#include "pyramid.h"
#include "warp.h"
#include "parametricflow.h"

#define DET_THRESH 0.40

using namespace std;
using namespace VW;

// compute spatial and temporal gradients squared and crossed
template <class Type>
void ParametricFlow::calc_gradients ( const ImageMono<Type> &imnew,
                                      const ImageMono<Type> &imold,
                                      ImageMono<float> &gradx2,
                                      ImageMono<float> &grady2,
                                      ImageMono<float> &gradxy,
                                      ImageMono<float> &gradxt,
                                      ImageMono<float> &gradyt)
{
  typename ImageMono<float>::PixelPtr gxyp,  gxtp,  gytp,  gx2p,  gy2p;
  typename ImageMono<Type>::PixelPtr imnewp, imoldp, im_m1, im_p1;
  float temp;

  int     r0, c0, cl, rl, r, c;

  imnew.ROI().Get(c0,r0,cl,rl);
  c0++; r0++; cl--; rl--;      // NB!  imnew and grad get different ROI's
  gradx2.ROI().Set(c0,r0,cl,rl);
  gradxy.ROI().Set(c0,r0,cl,rl);
  grady2.ROI().Set(c0,r0,cl,rl);
  gradxt.ROI().Set(c0,r0,cl,rl);
  gradyt.ROI().Set(c0,r0,cl,rl);

  // temporal gradients
  for (r=r0; r<rl; r++) {
    imnewp = imnew.GetROIRowStart(r)+1; // +1 because ROI's are different
    imoldp = imold.GetROIRowStart(r)+1;
    gxtp = gradxt.GetROIRowStart(r);
    gytp = gradyt.GetROIRowStart(r);
    for (c=c0; c<cl; c++) {
      *gxtp++ = *gytp++ = *imnewp++ - *imoldp++;
    }
  }

  // x gradients
  for (r=r0; r<rl; r++) {
    imnewp = imnew.GetROIRowStart(r)+1;// +1 because ROI's are different
    gx2p = gradx2.GetROIRowStart(r);
    gxtp = gradxt.GetROIRowStart(r);
    gxyp = gradxy.GetROIRowStart(r);
    for (c=c0; c<cl; c++, imnewp++) {
      temp = 0.5f*(float(imnewp[1].y) - float(imnewp[-1].y));
      *gx2p++ = temp*temp;
      *gxtp++ *= temp;
      *gxyp++ = temp;
    }
  }

  // y gradients
  for (r=r0; r<rl; r++) {
    im_m1 = imnew.GetROIRowStart(r-1)+1;// +1 because ROI's are different
    im_p1 = imnew.GetROIRowStart(r+1)+1;
    gy2p = grady2.GetROIRowStart(r);
    gytp = gradyt.GetROIRowStart(r);
    gxyp = gradxy.GetROIRowStart(r);
    for (c=c0; c<cl; c++, im_m1++, im_p1++) {
      temp = 0.5f*(float(im_p1->y) - float(im_m1->y));
      *gy2p++ = temp*temp;
      *gytp++ *= temp;
      *gxyp++ *= temp;
    }
  }

}


// compute flow using lucas and kanade's method fitting overall flow to an 
// homography with 2, 5, 6 or 8 degrees of freedom
template <class Type>
void ParametricFlow::Compute( const ImageMono<Type> &imold,
			      const ImageMono<Type> &imnew,
			      VW::Homography2D &H )
{
/* NOTE: 
         x=(c+c_off)/f, y=(r+r_off)/f, w=1.0
                        .   .     .   .     .       .
         u=x/w, v=y/w,  u = x/w - x/w w/w = x/w - u w/w, etc
	 .    .  .    .
	 c = fu, r = fv
            .                  .  
	 1 /x\     /x\        /u\
	 _ |y| = T |y|   =>   |v| = T |v|
	 w \w/     \w/        \0/
*/

  //struct timeval tv1, tv2;
  double A, B, C, D, E, det, M[2][8], 
    x, y, w, 
    centre_c, centre_r, focal_length;
  int i, j, r, c, n_eqns, n_pixels;
  int c0, r0, cl, rl;
  const int gauss_size = 3;
  const double gauss_sigma = 1.0;

  //gettimeofday(&tv1, NULL);

  ImageMono<float> gradx2(imnew.GetWidth(), imnew.GetHeight());
  ImageMono<float> grady2(imnew.GetWidth(), imnew.GetHeight());
  ImageMono<float> gradxy(imnew.GetWidth(), imnew.GetHeight());
  ImageMono<float> gradxt(imnew.GetWidth(), imnew.GetHeight());
  ImageMono<float> gradyt(imnew.GetWidth(), imnew.GetHeight());

  ImageMono<float> sgradx2(imnew.GetWidth(), imnew.GetHeight());
  ImageMono<float> sgrady2(imnew.GetWidth(), imnew.GetHeight());
  ImageMono<float> sgradxy(imnew.GetWidth(), imnew.GetHeight());
  ImageMono<float> sgradxt(imnew.GetWidth(), imnew.GetHeight());
  ImageMono<float> sgradyt(imnew.GetWidth(), imnew.GetHeight());

  calc_gradients<Type> (imnew, imold, gradx2, grady2, gradxy, gradxt, gradyt);

  /* convolve gradient images with separable 2-D Gaussian */
  ConvolutionMask1D<float> gauss_mask(gauss_size);
  gauss_mask.MakeGaussian(gauss_sigma);
  //Smooth3x3(gradx2, sgradx2);
  //Smooth3x3(grady2, sgrady2);
  //Smooth3x3(gradxy, sgradxy);
  //Smooth3x3(gradxt, sgradxt);
  //Smooth3x3(gradyt, sgradyt);
  Convolve<float>(gradx2, gauss_mask, gauss_mask, sgradx2);
  Convolve<float>(grady2, gauss_mask, gauss_mask, sgrady2);
  Convolve<float>(gradxy, gauss_mask, gauss_mask,  sgradxy);
  Convolve<float>(gradxt, gauss_mask, gauss_mask, sgradxt);
  Convolve<float>(gradyt, gauss_mask, gauss_mask, sgradyt);

  VNL::Matrix<double> MTM(param_space, param_space);
  VNL::Vector<double> RHS(param_space);
  VNL::Vector<double> LHS(param_space);

  for (i=0; i<param_space; i++) { 
    RHS[i] = 0.0;
    for (j=0; j<param_space; j++)  
      MTM[i][j] = 0.0;
  }

  centre_c = imnew.GetWidth()/2.0;
  centre_r = imnew.GetHeight()/2.0;
  focal_length = imnew.GetWidth();

  sgradx2.ROI().Get(c0, r0, cl, rl);
  n_eqns = n_pixels = 0;
  for (r=r0; r<rl; r++) {
    VW::PixelMono<float>* sgradx2_ptr = sgradx2.GetROIRowStart(r0);
    VW::PixelMono<float>* sgrady2_ptr = sgrady2.GetROIRowStart(r0);
    VW::PixelMono<float>* sgradxy_ptr = sgradxy.GetROIRowStart(r0);
    VW::PixelMono<float>* sgradxt_ptr = sgradxt.GetROIRowStart(r0);
    VW::PixelMono<float>* sgradyt_ptr = sgradyt.GetROIRowStart(r0);
    for (c=c0; c<cl; 
	 c++,
	   sgradx2_ptr++,
	   sgrady2_ptr++,
	   sgradxy_ptr++,
	   sgradxt_ptr++,
	   sgradyt_ptr++) {
      if (!finite(sgradx2_ptr->y) ||
	  !finite(sgradxy_ptr->y) ||
	  !finite(sgrady2_ptr->y) ||
	  !finite(sgradxt_ptr->y) ||
	  !finite(sgradyt_ptr->y)) 
	continue;

      n_pixels++;

      A = sgradx2_ptr->y;
      B = sgradxy_ptr->y;
      C = sgrady2_ptr->y;
      det = A*C - B*B;

      D = -sgradxt_ptr->y;
      E = -sgradyt_ptr->y;

      if (det < DET_THRESH) {
	continue;
      }

      x = (c - centre_c) / focal_length;
      y = (r - centre_r) / focal_length;
      w = 1.0;

      n_eqns+=2;

      if (param_space == 8) {
	M[0][0] = A*x; M[0][1] = A*y; M[0][2] = A*w; 
	M[0][3] = 0.0; M[0][4] = 0.0; M[0][5] = 0.0;
	M[0][6] = -A*x*x - B*x*y; M[0][7] = -A*x*y - B*y*y;
		
	M[1][0] = 0.0; M[1][1] = 0.0; M[1][2] = 0.0;
	M[1][3] = C*x; M[1][4] = C*y; M[1][5] = C*w; 
	M[1][6] = -B*x*x - C*x*y; M[1][7] = -B*x*y - C*y*y;

      } else if (param_space==6) {
	M[0][0] = A*x+B*y;      M[0][1] = A*y-B*x; 
	M[0][2] = A*w;          M[0][3] = B*w;     
	M[0][4] = -A*x*x-B*x*y; M[0][5] = -A*x*y-B*y*y; 
		
	M[1][0] = B*x+C*y;      M[1][1] = B*y-C*x; 
	M[1][2] = B*w;          M[1][3] = C*w; 
	M[1][4] = -B*x*x-C*x*y; M[1][5] = -B*x*y-C*y*y;

      } else if (param_space == 5) {
	M[0][0] = A*x+B*y;      
	M[0][1] = A*w;          M[0][2] = B*w;     
	M[0][3] = -A*x*x-B*x*y; M[0][4] = -A*x*y-B*y*y; 
		
	M[1][0] = B*x+C*y;      
	M[1][1] = B*w;          M[1][2] = C*w; 
	M[1][3] = -B*x*x-C*x*y; M[1][4] = -B*x*y-C*y*y;

      } else if (param_space == 2) {
	M[0][0] = A;  M[0][1] = B;
	M[1][0] = B;  M[1][1] = C;

      } else {
	cerr << "param_space must be 2, 5, 6 or 8\n";
	std::exit(1);
      }

      for (i=0; i<param_space; i++) {
	RHS[i] += (M[0][i]*D + M[1][i]*E)/focal_length;
	for (j=0; j<param_space; j++) 
	  MTM[i][j] += (M[0][i]*M[0][j] + M[1][i]*M[1][j]);
      }
    }
  }

  cerr << "n_pixels = " << n_pixels << ", n_eqns = " << n_eqns << endl;
  cerr << "MTM = " << endl;
  cerr << MTM << endl;

  LHS = VNL::Cholesky<double>(MTM).Solve(RHS);

  if (param_space==8) {
    H[0][0] = LHS[0]; H[0][1] = LHS[1]; H[0][2] = LHS[2]; 
    H[1][0] = LHS[3]; H[1][1] = LHS[4]; H[1][2] = LHS[5]; 
    H[2][0] = LHS[6]; H[2][1] = LHS[7]; H[2][2] = 0.0;
  } else if (param_space==6) {
    H[0][0] = LHS[0]; H[0][1] = LHS[1]; H[0][2] = LHS[2]; 
    H[1][2] = LHS[3]; H[2][0] = LHS[4]; H[2][1] = LHS[5]; 
    H[1][0] = -H[0][1]; H[1][1] = H[0][0]; H[2][2] = 0.0;
  } else if (param_space==5) {
    H[0][0] = LHS[0]; H[0][1] = 0.0;    H[0][2] = LHS[1]; 
    H[1][0] = 0.0;    H[1][1] = LHS[0]; H[1][2] = LHS[2]; 
    H[2][0] = LHS[3]; H[2][1] = LHS[4]; H[2][2] = 0.0;
  } else if (param_space==2) {
    H[0][0] = H[0][1] = 
      H[1][0] = H[1][1] =
      H[2][0] = H[2][1] = H[2][2] = 0.0;
    H[0][2] = LHS[0];
    H[1][2] = LHS[1];
  } else {
    cerr << "param_space must be 2, 5, 6 or 8\n" << endl;
    std::exit(1);
  }

  cerr << "H = " << endl << H << endl;

  //gettimeofday(&tv2, NULL);
  //cerr << "flow time = " 
  //<< (tv2.tv_sec-tv1.tv_sec)*1000.0 + (tv2.tv_usec-tv1.tv_usec)/1000.0)
  //<< "ms\n";

  return;
}


template void ParametricFlow::Compute<unsigned char>( const ImageMono<unsigned char>  &imnew,
					      const ImageMono<unsigned char>  &imold,
					      VW::Homography2D &H );
template void ParametricFlow::Compute<float>( const ImageMono<float>  &imnew,
					      const ImageMono<float>  &imold,
					      VW::Homography2D &H );


// compute wide baseline flow using lucas and kanade's method on a gaussian
// pyramid
template <class Type>
void ParametricFlowWide::Compute( const ImageMono<Type> &imold,
				  const ImageMono<Type> &imnew,
				  VW::Homography2D &H )
{
  int i;
  int c0, r0, cl, rl;

  Pyramid<float> pyrold(imold);
  Pyramid<float> pyrnew(imnew);

  VNL::Matrix<double> I(3,3,VNL::MatrixIdentity);
  VW::Homography2D Hdot;

  H = I;

  for (i=pyrold.NumLevels()-3; i>=0; i--) {

    ImageMono<float> wrp;
    Warp(*pyrold.Level()[i], H, wrp);
    pyrold[i].ROI().Get(c0, r0, cl, rl);
    wrp.ROI().Set(c0, r0, cl, rl);

    wrp.WriteImage("old.pgm");
    pyrnew[i].WriteImage("new.pgm");



    ParametricFlow(wrp, pyrnew[i], param_space, Hdot);
    H *= (Hdot + I);
  }

  return;
}


template void ParametricFlowWide::Compute<unsigned char>( const ImageMono<unsigned char>  &imnew,
						  const ImageMono<unsigned char>  &imold,
						  VW::Homography2D &H );
template void ParametricFlowWide::Compute<float>( const ImageMono<float>  &imnew,
						  const ImageMono<float>  &imold,
						  VW::Homography2D &H );


// refine a parametric flow estimate using levenberg-marquardt minimisation
// 
template <class Type>
void ParametricFlowNonLin::Compute( const ImageMono<Type> &imold,
				    const ImageMono<Type> &imnew,
				    VW::Homography2D      &H,
				    const ImageMono<Type> *mask)
{

  ParametricFlowFunc<Type> p(&imold, 
			     &imnew, 
			     imold.ROI().Width()*imold.ROI().Height(), 
			     mask);

  // copy transform data into vector
  VNL::Vector<double> x(9, 9, H.DataBlock());
  // and do minimisation
  VNL::LevenbergMarquardt lm(p);
  lm.Minimize(x);
  // copy resulting vector back into matrix
  VNL::Matrix<double> Tnew(3,3,9,x.DataBlock());
  cerr << "Tnew=\n" << Tnew << endl;
  H = Tnew;
}


template void 
ParametricFlowNonLin::Compute<unsigned char>( const ImageMono<unsigned char> &imnew,
					      const ImageMono<unsigned char> &imold,
					      VW::Homography2D      &H,
					      const ImageMono<unsigned char> *mask );
template void ParametricFlowNonLin::Compute<float>( const ImageMono<float> &imnew,
						    const ImageMono<float> &imold,
						    VW::Homography2D      &H, 
						    const ImageMono<float> *mask );


template <class Type>
void ParametricFlowFunc<Type>::F(VNL::Vector<double> const & x, VNL::Vector<double> &fx)
{
  int c01, r01, cl1, rl1;
  int c02, r02, cl2, rl2;
  int i, c, r, int_c2, int_r2;
  double val;
  double sum, c2, r2, frac_c2, frac_r2;
  double uprime, vprime;
  VNL::Matrix<double> T(3, 3, 9, x.DataBlock());

  cerr << "Function evaluation...\n";

  // now compute the sum of squared differences between the two images 
  // im1_ and im2_ using the transformation T and considering only points
  // given by mask_ (relative to im1_)

  im1_->ROI().Get(c01,r01,cl1,rl1);
  im2_->ROI().Get(c02,r02,cl2,rl2);

  sum = 0.0;
  i = 0;
  for (r=r01; r<rl1; r++) {
    for (c=c01; c<cl1; c++, i++) {

      fx[i] = 0.0;

      if (mask_!=NULL && mask_->GetPixel(c,r).y!=0)
	continue;

      VNL::Vector<double> x((c-centre_c) / focal_length, 
			   (r-centre_r) / focal_length,
			   1.0);
      VNL::Vector<double> xprime = T*x;

      uprime = xprime[0]/xprime[2];
      vprime = xprime[1]/xprime[2];

      c2 = focal_length*uprime + centre_c;
      r2 = focal_length*vprime + centre_r;

      int_c2 = (int)c2;
      int_r2 = (int)r2;
      frac_c2 = c2 - int_c2;
      frac_r2 = r2 - int_r2;

      // Get a pointer to this row
      typename VW::PixelMono<Type>** rptr = im2_->GetRowPointer(int_r2);

      if (int_c2<c02 || int_c2>=cl2-1 ||
	  int_r2<r02 || int_r2>=rl2-1)
	continue;

      val = ((1.0-frac_c2)*(1.0-frac_r2)*rptr[0][int_c2].y +
	     (1.0-frac_c2)*(frac_r2)    *rptr[1][int_c2].y +
	     (frac_c2)*(frac_r2)        *rptr[1][int_c2+1].y +
	     (frac_c2)*(1.0-frac_r2)    *rptr[0][int_c2+1].y);
      
      const Type p = im1_->GetPixel(c,r).y;
      if (finite(val) && finite(p)) {
	fx[i] = (val-p) * (val-p); 
      }
    }
  }

  cerr << "RMS error = " << fx.RMS() << endl;
  return;
}
