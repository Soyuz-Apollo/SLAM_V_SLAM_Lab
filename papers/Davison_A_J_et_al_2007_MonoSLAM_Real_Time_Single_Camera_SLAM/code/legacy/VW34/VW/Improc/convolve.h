/** 
 * @file   convolve.h
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
/**
 * @class   VW::Convolve
 * @author  ID Reid, DW Murray
 * @date    02/08/2002
 * @ Brief  A class for performing convolutions on images.
 *
 * If all you want to do is smooth an image, using VW::SmoothGaussian or VW::SmoothUniform
 * may be easier (and faster).
 */

#ifndef _convolve_h_
#define _convolve_h_

#define EVEN_SYMMETRY 1
#define NO_SYMMETRY   0
#define ODD_SYMMETRY -1

namespace VW {

  // Forward declare image classes
  template<typename valType> class ImageMono;
  template<typename valType> class ImageRGB;
  template<typename valType> class ImageYUV;




/**
 * @class   VW::ConvolutionMask1D
 * @author  ID Reid, DW Murray
 * @date    02/08/2002
 * @ Brief  A class to store the mask for use in a convolution (note that the mask is the
 *          reverse of the signal --- it's backwards!).
 */
template <class maskType>
class ConvolutionMask1D {

 private:
  int  symmetry;
  int  halfwidth; // half-width of mask => mask size is 2*hw+1
  maskType weight;    // sum of mask element weights

 public:
  ConvolutionMask1D(int n=1);
  ConvolutionMask1D(maskType x1, maskType x2, maskType x3);
  ConvolutionMask1D(maskType x1, maskType x2, maskType x3, maskType x4, maskType x5);
  ConvolutionMask1D(maskType *x, int n);
  ConvolutionMask1D(ConvolutionMask1D &c);
  ~ConvolutionMask1D();

  int GetHalfWidth() const { return halfwidth; }
  maskType GetWeight() const { return weight; }
  int Symmetry() const { return symmetry; }

  /** Set this mask to be a Gaussian kernel of standard deviation sigma. */
  void MakeGaussian(double sigma);
  /** Set this mask to be a uniform kernel (all values=1/n). */
  void MakeUniform();


  // public data
  maskType *mask;    // public since it seems to be difficult 
                 // to make templated convolution a friend
};

//////////////////////////////////////////////////////


template <class maskType>
class Convolve {
   
 public:
  // 
  Convolve(ConvolutionMask1D<maskType> &c, 
	   ConvolutionMask1D<maskType> &r) 
    : cmask(c), rmask(r) 
  {};
  
  template <class inType, class outType>
    Convolve(const ImageMono<inType> &im,
	     ConvolutionMask1D<maskType> &c,
	     ConvolutionMask1D<maskType> &r,
	     ImageMono<outType> &result,
	     const unsigned int subsampling=1) 
    : cmask(c), rmask(r) 
  { 
    Compute(im, result, subsampling); 
  };
  
  template <class inType, class outType>
    Convolve(const ImageYUV<inType> &im,
	     ConvolutionMask1D<maskType> &c,
	     ConvolutionMask1D<maskType> &r,
	     ImageYUV<outType> &result,
	     const unsigned int subsampling=1) 
    : cmask(c), rmask(r) 
  { 
    Compute(im, result, subsampling); 
  };
  
  template <class inType, class outType>
    void Compute(const ImageMono<inType> &im,
		 ImageMono<outType> &result,
		 const unsigned int subsampling=1);
  
  template <class inType, class outType>
    void Compute(const ImageYUV<inType> &im,
		 ImageYUV<outType> &result,
		 const unsigned int subsampling=1);
  
 protected:
  ConvolutionMask1D<maskType> cmask;
  ConvolutionMask1D<maskType> rmask;
  
};
 



}; // end namespace VW

#endif /* _convolve_h_ */






