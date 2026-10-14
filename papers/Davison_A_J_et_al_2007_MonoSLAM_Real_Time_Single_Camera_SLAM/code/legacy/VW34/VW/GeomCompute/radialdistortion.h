/** 
 * @file   radialdistortion.h
 * @author BJ Tordoff after E Hayman
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


#ifndef _radialdistortion_h_
#define _radialdistortion_h_

// VW stuff
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/lineseg2d.h>

namespace VW {


  /**
   * @author  BJ Tordoff after E Hayman
   * @ingroup ImageProcessing
   * @date    02/08/2002
   * @brief   Methods for distorting and correcting images, points and lines 
   *          for the effects of radial distortion.
   *
   * The model used is that of Harris:
   *
   *        r_d = r_u (1 - 2*kappa*r_u*r_u)^(-0.5)    [distorting]
   *        r_u = r_d (1 + 2*kappa*r_d*r_d)^(-0.5)    [undistorting]
   *
   * Note that radial distortion/correction should occur in CCD coords, NOT
   * image pixels UNLESS aspect=1, skew=0.
   *
   * Features can either be distorted in functional form:
   *       dest_point = VW::RadialDistortion::Distort(kappa,source_point)
   * or in place (overwriting the source point):
   *       VW::RadialDistortion::DistortInPlace(kappa,point)
   *
   * For lists of points, the only option is to distort in place.
   */
  namespace RadialDistortion {

    // Manipulating features
    
    void DistortInPlace(const double kappa,
			double& x, double& y, 
			const double& distortion_centre_x = 0.0, 
			const double& distortion_centre_y = 0.0);

    // Specific VW features
    // Points
    Point2D Distort(const double kappa,
		      const Point2D& pin, 
		      const Point2D& distortion_centre = Point2D(0,0));
    Point2D Undistort(const double kappa,
			const Point2D& pin,
			const Point2D& distortion_centre = Point2D(0,0));
    void UndistortInPlace(const double kappa,
			  Point2D& point, 
			  const Point2D& distortion_centre = Point2D(0,0));
    void UndistortInPlace(const double kappa,
			  std::vector<Point2D>& points, 
			  const Point2D& distortion_centre = Point2D(0,0));
     // Line segs
    LineSeg2D Distort(const double kappa,
		      const LineSeg2D& linein, 
		      const Point2D& distortion_centre = Point2D(0,0));
    LineSeg2D Undistort(const double kappa,
			const LineSeg2D& linein,
			const Point2D& distortion_centre = Point2D(0,0));
    void UndistortInPlace(const double kappa,
			  LineSeg2D& line, 
			  const Point2D& distortion_centre = Point2D(0,0));
    void UndistortInPlace(const double kappa,
			  std::vector<LineSeg2D>& lines, 
			  const Point2D& distortion_centre = Point2D(0,0));
    
    



    // User defined point features (must have GetX, GetY functions)
    template <class FeatureType>
    FeatureType DistortPoint(const double kappa, 
		    const FeatureType& source_point,
		    const Point2D& distortion_centre = Point2D(0,0));
    template <class FeatureType>
    FeatureType UndistortPoint(const double kappa, 
		      const FeatureType& source_point,
		      const Point2D& distortion_centre = Point2D(0,0));
    template <class FeatureType>
    void UndistortPointInPlace(const double kappa, 
			       FeatureType& point,
			       const Point2D& distortion_centre = Point2D(0,0));
     template <class FeatureType>
   void UndistortPointInPlace(const double kappa, 
			       std::vector<FeatureType>& points,
			       const Point2D& distortion_centre = Point2D(0,0));
    



    
    













// INLINES


    
template <class FeatureType> inline FeatureType 
DistortPoint(const double kappa, 
	     const FeatureType& source_point,
	     const Point2D& distortion_centre)
{
  double rxu, ryu, sq;
  FeatureType dest_point = source_point;
  rxu = source_point.GetX()-distortion_centre.GetX();
  ryu = source_point.GetY()-distortion_centre.GetY();
  sq = rxu*rxu + ryu*ryu;
  dest_point.SetX(distortion_centre.GetX()+rxu/sqrt((1.0-2*kappa*sq)));
  dest_point.SetY(distortion_centre.GetY()+ryu/sqrt((1.0-2*kappa*sq)));

  return(dest_point);
}

template <class FeatureType> inline FeatureType 
UndistortPoint(const double kappa, 
	       const FeatureType& source_point,
	       const Point2D& distortion_centre)
{
  double rxu, ryu, sq;
  FeatureType dest_point = source_point;
  rxu = source_point.GetX()-distortion_centre.GetX();
  ryu = source_point.GetY()-distortion_centre.GetY();
  sq = rxu*rxu + ryu*ryu;
  dest_point.SetX(distortion_centre.GetX()+rxu/sqrt((1.0+2*kappa*sq)));
  dest_point.SetY(distortion_centre.GetY()+ryu/sqrt((1.0+2*kappa*sq)));

  return(dest_point);
}

template <class FeatureType> inline void 
UndistortPointInPlace(const double kappa, 
		      FeatureType& point,
		      const Point2D& distortion_centre)
{
  double rxu, ryu, sq;
  rxu = point.GetX()-distortion_centre.GetX();
  ryu = point.GetY()-distortion_centre.GetY();
  sq = rxu*rxu + ryu*ryu;
  point.SetX(distortion_centre.GetX()+rxu/sqrt((1.0+2*kappa*sq)));
  point.SetY(distortion_centre.GetY()+ryu/sqrt((1.0+2*kappa*sq)));
}

template <class FeatureType> inline void 
UndistortPointInPlace(const double kappa, 
		      std::vector<FeatureType>& points,
		      const Point2D& distortion_centre)
{
  double rxu, ryu, sq;
  typename std::vector<FeatureType>::iterator i;
  const typename std::vector<FeatureType>::iterator e = points.end(); 
  for (i = points.begin();
       i != e;
       i++) {
    rxu = i->GetX()-distortion_centre.GetX();
    ryu = i->GetY()-distortion_centre.GetY();
    sq = rxu*rxu + ryu*ryu;
    i->SetX(distortion_centre.GetX()+rxu/sqrt((1.0+2*kappa*sq)));
    i->SetY(distortion_centre.GetY()+ryu/sqrt((1.0+2*kappa*sq)));
  }
}


    inline void DistortInPlace(const double kappa,
			double& x, double& y, 
			const double& distortion_centre_x, 
			const double& distortion_centre_y)
    {
      const double rxu = x-distortion_centre_x;
      const double ryu = y-distortion_centre_y;
      const double alpha = 1.0/sqrt(1.0-2*kappa*(rxu*rxu + ryu*ryu));
      x = distortion_centre_x + rxu*alpha;
      y = distortion_centre_y + ryu*alpha;
    }

    inline Point2D 
    Distort(const double kappa,
	    const Point2D& pin, 
	    const Point2D& distortion_centre)
    {
      const double rxu = pin.GetX()-distortion_centre.GetX();
      const double ryu = pin.GetY()-distortion_centre.GetY();
      const double alpha = 1.0/sqrt(1.0-2*kappa*(rxu*rxu + ryu*ryu));
      const Point2D result(distortion_centre.GetX()+rxu*alpha,
			   distortion_centre.GetY()+ryu*alpha);
      
      return(result);     
   }

    inline Point2D 
   Undistort(const double kappa,
	     const Point2D& pin,
	     const Point2D& distortion_centre)
   {
     double rxu, ryu, sq;
     rxu = pin.GetX()-distortion_centre.GetX();
     ryu = pin.GetY()-distortion_centre.GetY();
     sq = rxu*rxu + ryu*ryu;
     const Point2D result(distortion_centre.GetX()+rxu/sqrt((1.0+2*kappa*sq)),
			  distortion_centre.GetY()+ryu/sqrt((1.0+2*kappa*sq)));
     
     return(result);
   }
 
 inline void 
   UndistortInPlace(const double kappa,
		    Point2D& point, 
		    const Point2D& distortion_centre)
   {
     double rxu, ryu, sq;
     rxu = point.GetX()-distortion_centre.GetX();
     ryu = point.GetY()-distortion_centre.GetY();
     sq = rxu*rxu + ryu*ryu;
     point.SetX(distortion_centre.GetX()+rxu/sqrt((1.0+2*kappa*sq)));
     point.SetY(distortion_centre.GetY()+ryu/sqrt((1.0+2*kappa*sq)));
   }
 
 inline void 
   UndistortInPlace(const double kappa,
		    std::vector<Point2D>& points, 
		    const Point2D& distortion_centre)
   {
     double rxu, ryu, sq;
     std::vector<Point2D>::iterator i;
     const std::vector<Point2D>::iterator e = points.end(); 
     for (i = points.begin();
       i != e;
       i++) {
       rxu = i->GetX()-distortion_centre.GetX();
       ryu = i->GetY()-distortion_centre.GetY();
       sq = rxu*rxu + ryu*ryu;
       i->SetX(distortion_centre.GetX()+rxu/sqrt((1.0+2*kappa*sq)));
       i->SetY(distortion_centre.GetY()+ryu/sqrt((1.0+2*kappa*sq)));
     }
   }
 
 inline LineSeg2D Distort(const double kappa,
			  const LineSeg2D& linein, 
			  const Point2D& distortion_centre)
   {
  return LineSeg2D(DistortPoint(kappa, linein.GetStartPoint(), distortion_centre),
		   DistortPoint(kappa, linein.GetEndPoint(), distortion_centre));  
}

inline LineSeg2D Undistort(const double kappa,
			 const LineSeg2D& linein, 
			 const Point2D& distortion_centre)
{
  return LineSeg2D(UndistortPoint(kappa, 
			     linein.GetStartPoint(), 
			     distortion_centre),
		   UndistortPoint(kappa, 
			     linein.GetEndPoint(),   
			     distortion_centre)
		   );
}

inline void UndistortInPlace(const double kappa,
			     LineSeg2D& line, 
			     const Point2D& distortion_centre)
{
  const Point2D s(UndistortPoint(kappa,
				line.GetStartPoint(),
				distortion_centre));
  const Point2D e(UndistortPoint(kappa,
				line.GetEndPoint(),
				distortion_centre));
  line.SetEndPoints(s,e);
}

inline void UndistortInPlace(const double kappa,
			     std::vector<LineSeg2D>& lines, 
			     const Point2D& distortion_centre)
{
  std::vector<LineSeg2D>::iterator i,e;
  e = lines.end();

  for (i = lines.begin(); i != e; i++)
    UndistortInPlace(kappa, *i, distortion_centre);

}



}; // End namespace RadialDistortion
}; // End namespace VW





#endif
