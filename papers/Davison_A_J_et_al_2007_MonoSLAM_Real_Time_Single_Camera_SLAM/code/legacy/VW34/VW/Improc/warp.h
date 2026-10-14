/** 
 * @file   warp.h
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
 * @class   VW::Warp
 * @ingroup ImageProcessing
 * @author  ID Reid
 * @date    02/08/2002
 * @brief   Warp a 2D image according to homography H.
 *
 *   #include <VW/Improc/warp.h>
 * 
 * Given an invertible 2D projective transformation (ie. non-singular 
 * 3x3 matrix) H, every point in the target image can be calculated from
 * the input image.
 *
 * Note that H must NOT be normalised - it must be in image coordinates,
 * as must the distortion (if any). If you only want to correct distortion,
 * use the DistortImage class.
 *
 * Usage:
 *   VW::Warp w(H);
 *   w.Compute(input_image, output_image);
 * or
 *   VW::Warp(input_image, H, output_image);
 * 
 *
 */


#ifndef _warp_h_
#define _warp_h_

#include <VW/GeomObjects/homography2d.h>
#include "interpolationmethod.h"

namespace VW {


class Warp {

 public:
  Warp();
  Warp(const VW::Homography2D &H);
  Warp(const VW::Homography2D &H,
       const double distortion_kappa,
       const VW::Point2D& distortion_centre);

  /** Compute assuming homography is *NOT* normalised. */
  template <class InType, class OutType>
  Warp (const InType &im, 
	const VW::Homography2D &H, 
	OutType &res,
	InterpolationMethod method=Bilinear);

  /** Compute assuming homography is *NOT* normalised. */
  template <class InType, class OutType>
  Warp (const InType &im, 
	const VW::Homography2D &H,
	const double distortion_kappa,
	const VW::Point2D& distortion_centre, 
	OutType &res,
	InterpolationMethod method=Bilinear);


  operator bool();

  template <class InType, class OutType>
    void Compute(const InType &im, OutType &res,
		 InterpolationMethod method=Bilinear);


  void SetTransformation(const VW::Homography2D &H);
  void SetDistortion(const double distortion_kappa,
		     const VW::Point2D& distortion_centre);

 private:
  VW::Homography2D _H;
  double _kappa;
  double _distortion_centre_x, _distortion_centre_y;

};

}; // end namespace VW






// Inlines

inline
VW::Warp::Warp() 
  : _kappa(0),_distortion_centre_x(0),_distortion_centre_y(0)
{
  _H.SetIdentity();
};

inline
VW::Warp::Warp(const VW::Homography2D &H) 
  : _H(H), _kappa(0),_distortion_centre_x(0),_distortion_centre_y(0)
{
};

inline  
VW::Warp::Warp(const VW::Homography2D &H,
	       const double distortion_kappa,
	       const VW::Point2D& distortion_centre)
  : _H(H), 
    _kappa(distortion_kappa),
    _distortion_centre_x(distortion_centre.GetX()),
    _distortion_centre_y(distortion_centre.GetY())
{
}

template <class InType, class OutType> inline
VW::Warp::Warp (const InType &im, const VW::Homography2D &H, OutType &res,
		InterpolationMethod method) 
  : _H(H), _kappa(0),_distortion_centre_x(0),_distortion_centre_y(0)
{ 
  Compute(im,res,method); 
}


template <class InType, class OutType> inline
VW::Warp::Warp (const InType &im, const VW::Homography2D &H, 
		const double distortion_kappa,
		const VW::Point2D& distortion_centre,
		OutType &res,
		InterpolationMethod method) 
  : _H(H), _kappa(distortion_kappa),_distortion_centre_x(distortion_centre.GetX()),_distortion_centre_y(distortion_centre.GetY())
{ 
  Compute(im,res,method); 
}



inline
VW::Warp::operator bool() 
{ 
  return true; 
}

inline void 
VW::Warp::SetTransformation(const VW::Homography2D &H) 
{ 
  _H=H; 
}

inline void 
VW::Warp::SetDistortion(const double distortion_kappa,
			const VW::Point2D& distortion_centre)
{
  _kappa = distortion_kappa;
  _distortion_centre_x = distortion_centre.GetX();
  _distortion_centre_y = distortion_centre.GetY();
}

#endif // _warp_h_
