/** 
 * @file   parametricflow.h
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


#ifndef _parametricflow_h_
#define _parametricflow_h_

#include <VNL/leastsquaresfunction.h>
#include <VW/GeomObjects/homography2d.h>

namespace VW {

// Compute mapping from old to new store in H
class ParametricFlow {

 public:

  ParametricFlow( int p=8 ) : param_space(p) { assert(p>1); }

  template <class Type>
    ParametricFlow( const ImageMono<Type> &imold,
		    const ImageMono<Type> &imnew,
		    const int p,
		    VW::Homography2D &H);
  
  template <class Type>
    void Compute(const ImageMono<Type> &imold,
		 const ImageMono<Type> &imnew,
		 VW::Homography2D &H);
  
  operator bool () { return true; }

  void SetParamSpace(const int p) { assert(p>1); param_space=p; }
  int GetParamSpace() { return param_space; }

 private:

  int param_space;

  template <class Type>
    void calc_gradients ( const ImageMono<Type> &imnew,
			  const ImageMono<Type> &imold,
			  ImageMono<float> &gradx2,
			  ImageMono<float> &grady2,
			  ImageMono<float> &gradxy,
			  ImageMono<float> &gradxt,
			  ImageMono<float> &gradyt);

};


template <class Type>
class ParametricFlowFunc : public VNL::LeastSquaresFunction {

 public:
  ParametricFlowFunc(const ImageMono<Type> *im1, const ImageMono<Type> *im2,
		     int n_pixels,
		     const ImageMono<Type> *mask=NULL) 
    : VNL::LeastSquaresFunction(9, n_pixels, no_gradient),
      im1_(im1), im2_(im2), mask_(mask)
  {
    centre_c = im1->GetWidth()/2.0;
    centre_r = im1->GetHeight()/2.0;
    focal_length = im1->GetWidth();
  };
  
  ~ParametricFlowFunc() {};
  
  void F(VNL::Vector<double> const & x, VNL::Vector<double> & fx);

 private:
		       
  const ImageMono<Type> *im1_;
  const ImageMono<Type> *im2_;
  const ImageMono<Type> *mask_;

  double centre_c, centre_r, focal_length;
};


class ParametricFlowNonLin {

 public:

  ParametricFlowNonLin() {}

  template <class Type>
  ParametricFlowNonLin( const ImageMono<Type> &imold,
			const ImageMono<Type> &imnew,
			VW::Homography2D      &H,
			const ImageMono<Type> *mask=NULL ) 
  { 
    Compute(imold, imnew, H, mask); 
  }

  template <class Type>
  void Compute( const ImageMono<Type> &imold,
		const ImageMono<Type> &imnew,
		VW::Homography2D      &H,
		const ImageMono<Type> *mask=NULL );

 private:

};
  

class ParametricFlowWide {

 public:

  ParametricFlowWide( int p=8 ) : param_space(p) { assert(p>1); }

  template <class Type>
    ParametricFlowWide( const ImageMono<Type> &imold,
			const ImageMono<Type> &imnew,
			const int p,
			VW::Homography2D      &H) : param_space(p) { 
    assert(p>1); 
    Compute(imold, imnew, H); 
  };
  
  template <class Type>
    void Compute(const ImageMono<Type> &imold,
		 const ImageMono<Type> &imnew,
		 VW::Homography2D      &H);
  
  operator bool () { return true; }

  void SetParamSpace(const int p) { assert(p>1); param_space=p; }
  int GetParamSpace() { return param_space; }

 private:

  int param_space;

};



}; // end namespace VW













// Inlines

template <class Type> inline
VW::ParametricFlow::ParametricFlow( const ImageMono<Type> &imold,
				    const ImageMono<Type> &imnew,
				    const int p,
				    VW::Homography2D      &H) 
  : param_space(p) 
{ 
  assert(p>1); 
  Compute(imold, imnew, H); 
};


#endif // _parametricflow_h_
