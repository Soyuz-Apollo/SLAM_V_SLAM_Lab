/** 
 * @file   distortimage.cpp
 * @author BJ Tordoff after E Hayman
 * @date   19/08/2003
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


#include "distortimage.h"

#include <VW/GeomObjects/point2d.h>
#include <VW/GeomCompute/radialdistortion.h>

using namespace std;

static void GetResultSize(unsigned int in_width, unsigned int in_height,
			  double kappa, VW::Point2D in_centre,
			  unsigned int& result_width, unsigned int& result_height,
			  VW::Point2D& result_centre)
{
  using namespace VW::RadialDistortion;

  // Calculate the result size and make sure the ouput image can take it
  VW::Point2D t_side_pos = DistortPoint(kappa,VW::Point2D(in_centre.GetX(),0),in_centre);
  VW::Point2D b_side_pos = DistortPoint(kappa,VW::Point2D(in_centre.GetX(),in_height),in_centre);
  VW::Point2D l_side_pos = DistortPoint(kappa,VW::Point2D(0,in_centre.GetY()),in_centre);
  VW::Point2D r_side_pos = DistortPoint(kappa,VW::Point2D(in_width,in_centre.GetY()),in_centre);
  VW::Point2D tl_corner_pos = DistortPoint(kappa,VW::Point2D(0,0),in_centre);
  VW::Point2D tr_corner_pos = DistortPoint(kappa,VW::Point2D(in_width,0),in_centre);
  VW::Point2D bl_corner_pos = DistortPoint(kappa,VW::Point2D(0,in_height),in_centre);
  VW::Point2D br_corner_pos = DistortPoint(kappa,VW::Point2D(in_width,in_height),in_centre);

  double result_x0 = max(max(l_side_pos.GetX(),tl_corner_pos.GetX()), 
			 bl_corner_pos.GetX());
  double result_x1 = min(min(r_side_pos.GetX(),tr_corner_pos.GetX()), 
			 br_corner_pos.GetX());
  double result_y0 = max(max(t_side_pos.GetY(),tl_corner_pos.GetY()), 
			 tr_corner_pos.GetY());
  double result_y1 = min(min(b_side_pos.GetY(),bl_corner_pos.GetY()), 
			 br_corner_pos.GetY());

  result_width = static_cast<unsigned int>(result_x1 - result_x0);
  result_height = static_cast<unsigned int>(result_y1 - result_y0);

  result_centre.Set(in_centre.GetX()-result_x0,
		    in_centre.GetY()-result_y0);
}

void VW::RadialDistortion::DistortImage(const double kappa,
					const VW::ImageMono<unsigned char>& input_image,
					VW::ImageMono<unsigned char>& result,
					const VW::Point2D& distortion_centre)
{
  // First check that the images are OK
  assert(input_image.IsAlloced());

  const unsigned int w = input_image.GetWidth(), h = input_image.GetHeight();
  unsigned int result_width, result_height;
  VW::Point2D result_centre;
  GetResultSize(w, h,
		kappa, distortion_centre,
		result_width, result_height,
		result_centre);

  if (result.IsAlloced()) {
    if ((result.GetWidth() < result_width)
	|| (result.GetHeight() < result_height)) {
      cout << "VW::RadialDistortion::DistortImage: Result requires a "<<result_width<<"x"<<result_height
	   <<" image, but you have given me a "<<result.GetWidth()<<"x"<<result.GetHeight()<<" one."<<endl;
      exit(0);
    }
  } else {
    result.AllocImageData(result_width,result_height);
  }

  for (unsigned int y=0;y<result.GetHeight();y++) {
    VW::PixelMono<unsigned char>* ptr = result.GetRowStart(y);
    unsigned int x=0;
    for (x=0;x<result_width;x++,ptr++) {
      VW::Point2D source = VW::RadialDistortion::UndistortPoint(kappa,VW::Point2D(x,y),result_centre);

      source = source - result_centre + distortion_centre;

      if ((source.GetX() >= 0) && (source.GetX() < (w-1))
	  && (source.GetY() >= 0) && (source.GetY() < (h-1))) {

	//cout << "Destination point ("<<VW::Point2D(x,y)<<") is sourced from ("<<source<<")"<<endl;

      // Store integer positions and offsets
      const int px=int(source.GetX());
       const int py=int(source.GetY());
       const double deltax=source.GetX()-px;
       const double deltay=source.GetY()-py;

      // Coefficeints of interpolation are constant
       const double a00 = (1-deltax)*(1-deltay);
       const double a01 = (1-deltax)*(  deltay);
       const double a10 = (  deltax)*(1-deltay);
       const double a11 = (  deltax)*(  deltay);

       *ptr = (a00 * input_image.GetPixel(px  ,py  )
	       + a01 * input_image.GetPixel(px  ,py+1)
	       + a10 * input_image.GetPixel(px+1,py  )
	       + a11 * input_image.GetPixel(px+1,py+1));
      } else {
	ptr->y = 0;
      }
    }
  }

}

void VW::RadialDistortion::DistortImage(const double kappa,
					const VW::ImageYUV<unsigned char>& input_image,
					VW::ImageYUV<unsigned char>& result,
					const VW::Point2D& distortion_centre)
{
  // First check that the images are OK
  assert(input_image.IsAlloced());

  const unsigned int w = input_image.GetWidth(), h = input_image.GetHeight();
  unsigned int result_width, result_height;
  VW::Point2D result_centre;
  GetResultSize(w, h,
		kappa, distortion_centre,
		result_width, result_height,
		result_centre);

  if (result.IsAlloced()) {
    if ((result.GetWidth() < result_width)
	|| (result.GetHeight() < result_height)) {
      cout << "VW::RadialDistortion::DistortImage: Result requires a "<<result_width<<"x"<<result_height
	   <<" image, but you have given me a "<<result.GetWidth()<<"x"<<result.GetHeight()<<" one."<<endl;
      exit(0);
    }
  } else {
    result.AllocImageData(result_width,result_height);
  }

  for (unsigned int y=0;y<result.GetHeight();y++) {
   VW::PixelYUV<unsigned char>* ptr = result.GetRowStart(y);
    unsigned int x=0;
    for (x=0;x<result_width;x++,ptr++) {
      VW::Point2D source = VW::RadialDistortion::UndistortPoint(kappa,VW::Point2D(x,y),result_centre);

      source = source - result_centre + distortion_centre;

      if ((source.GetX() >= 0) && (source.GetX() < (w-1))
	  && (source.GetY() >= 0) && (source.GetY() < (h-1))) {

      //	cout << "Destination point ("<<VW::Point2D(x,y)<<") is sourced from ("<<source<<")"<<endl;

      // Store integer positions and offsets
	const int px=int(source.GetX());
	const int py=int(source.GetY());
       const double deltax=source.GetX()-px;
       const double deltay=source.GetY()-py;

      // Coefficeints of interpolation are constant
       const double a00 = (1-deltax)*(1-deltay);
       const double a01 = (1-deltax)*(  deltay);
       const double a10 = (  deltax)*(1-deltay);
       const double a11 = (  deltax)*(  deltay);

       *ptr = (  a00 * input_image.GetPixel(px  ,py  )
		 + a01 * input_image.GetPixel(px  ,py+1)
		 + a10 * input_image.GetPixel(px+1,py  )
		 + a11 * input_image.GetPixel(px+1,py+1));
      } else {
	*ptr = 0;
      }
    }
  }
  cerr << "done distorting." <<endl;

}

void VW::RadialDistortion::UndistortImage(const double kappa,
					const VW::ImageMono<unsigned char>& input_image,
					VW::ImageMono<unsigned char>& result,
					const VW::Point2D& distortion_centre)
{
  // First check that the images are OK
  assert(input_image.IsAlloced());

  const unsigned int w = input_image.GetWidth();
  const unsigned int h = input_image.GetHeight();

  // Calculate the result size and make sure the ouput image can take it
  VW::Point2D t_side_pos = VW::RadialDistortion::UndistortPoint(kappa,VW::Point2D(distortion_centre.GetX(),0),distortion_centre);
  VW::Point2D b_side_pos = VW::RadialDistortion::UndistortPoint(kappa,VW::Point2D(distortion_centre.GetX(),h),distortion_centre);
  VW::Point2D l_side_pos = VW::RadialDistortion::UndistortPoint(kappa,VW::Point2D(0,distortion_centre.GetY()),distortion_centre);
  VW::Point2D r_side_pos = VW::RadialDistortion::UndistortPoint(kappa,VW::Point2D(w,distortion_centre.GetY()),distortion_centre);
  VW::Point2D tl_corner_pos = VW::RadialDistortion::UndistortPoint(kappa,VW::Point2D(0,0),distortion_centre);
  VW::Point2D tr_corner_pos = VW::RadialDistortion::UndistortPoint(kappa,VW::Point2D(w,0),distortion_centre);
  VW::Point2D bl_corner_pos = VW::RadialDistortion::UndistortPoint(kappa,VW::Point2D(0,h),distortion_centre);
  VW::Point2D br_corner_pos = VW::RadialDistortion::UndistortPoint(kappa,VW::Point2D(w,h),distortion_centre);

  double result_x0 = max(max(l_side_pos.GetX(),tl_corner_pos.GetX()) , bl_corner_pos.GetX());
  double result_x1 = min(min(r_side_pos.GetX(),tr_corner_pos.GetX()) , br_corner_pos.GetX());
  double result_y0 = max(max(t_side_pos.GetY(),tl_corner_pos.GetY()) , tr_corner_pos.GetY());
  double result_y1 = min(min(b_side_pos.GetY(),bl_corner_pos.GetY()) , br_corner_pos.GetY());

  unsigned int result_width = static_cast<unsigned int>(result_x1 - result_x0);
  unsigned int result_height = static_cast<unsigned int>(result_y1 - result_y0);

  cout << "Result size ("<<result_width<<"x"<<result_height<<")"<<endl;

  if (result.IsAlloced()) {
    if ((result.GetWidth() < result_width)
	|| (result.GetHeight() < result_height)) {
      cout << "VW::RadialDistortion::UndistortImage: Result requires a "<<result_width<<"x"<<result_height
	   <<" image, but you have given me a "<<result.GetWidth()<<"x"<<result.GetHeight()<<" one."<<endl;
      exit(0);
    }
  } else {
    result.AllocImageData(result_width,result_height);
  }

  VW::Point2D result_centre(distortion_centre.GetX()-result_x0,
			    distortion_centre.GetY()-result_y0);

  for (unsigned int y=0;y<result.GetHeight();y++) {
    VW::PixelMono<unsigned char>* ptr = result.GetRowStart(y);
    unsigned int x=0;
    for (x=0;x<result_width;x++,ptr++) {
      VW::Point2D source = VW::RadialDistortion::DistortPoint(kappa,VW::Point2D(x,y),result_centre);

      source = source - result_centre + distortion_centre;

      const int px=int(source.GetX());
       const int py=int(source.GetY());
      if ((px >= 0)
	  && (px < int(w-1))
	  && (py >= 0)
	  && (py < int(h-1))) {

	//cout << "Destination point ("<<VW::Point2D(x,y)<<") is sourced from ("<<source<<")"<<endl;

      // Store integer positions and offsets
       const double deltax=source.GetX()-px;
       const double deltay=source.GetY()-py;

      // Coefficeints of interpolation are constant
       const double a00 = (1-deltax)*(1-deltay);
       const double a01 = (1-deltax)*(  deltay);
       const double a10 = (  deltax)*(1-deltay);
       const double a11 = (  deltax)*(  deltay);

      ptr->y = static_cast<unsigned char>(  a00 * input_image.GetPixel(px  ,py  ).y
					  + a01 * input_image.GetPixel(px  ,py+1).y
					  + a10 * input_image.GetPixel(px+1,py  ).y
					  + a11 * input_image.GetPixel(px+1,py+1).y);
      } else {
	ptr->y = 0;
      }
    }
  }
}
