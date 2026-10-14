/**
 * @file operations.h
 * @ingroup GeometricObjects
 * @author  ID Reid, JGH Knight and BJ Tordoff
 * @date    02/08/02
 * @brief   Operations on and with geometric objects
 */
/*
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
#ifndef _operations_h_
#define _operations_h_


#include <vector>
#include <list>

#include <VNL/matrixfixed.h>
#include "point2d.h"
#include "point3d.h"
#include "line2d.h"
#include "lineseg2d.h"
#include "lineseg3d.h"

namespace VW
{
	
  /** Round coordinates to integer values */
  VW::Point2D Round(const VW::Point2D& p);

  /** Round coordinates to integer values */
  VW::Point3D Round(const VW::Point3D& p);


  /** Distance between 2 points. */
  double GetDistance (const VW::Point2D & p1, const VW::Point2D & p2);
	
  /** Distance between a point and a line. */
  double GetDistance (const VW::Point2D & p, const VW::Line2D & l);
	
  /** Distance between a point and a line segment. 
   * Note that this is the distance to the closest end if the point does not
   * project onto the segment.
   */
  double GetDistance (const VW::Point2D & p, const VW::LineSeg2D & l);
	
  /** Find the line that passes through two points. */
  VW::Line2D LineThroughPoints (const VW::Point2D & p1,
				const VW::Point2D & p2);
	
	
  /** Find the point of intersection of two lines. */
  VW::Point2D Intersect (const VW::Line2D & l1,
			 const VW::Line2D & l2);
	
  /** Get the angle (in radians) between two line segments. */
  double GetAngleBetween (const VW::LineSeg2D & l1,
			  const VW::LineSeg2D & l2);
	
	
  /** Recover the derivative matrix of a rotation with respect to the angle of rotation. */
  VNL::MatrixFixed<3,3,double> GetDerivativeRotationMatrix(const VW::Vector3D& axis,
							const double angle);


  /** Is the point to the left of the line-segment (only meaningful for
   * lines with a sense of direction)
   */
  bool IsToLeft (const VW::Point2D & point, const VW::LineSeg2D & line);
	
  /** Is the point to the right of the line-segment (only meaningful for
   * lines with a sense of direction)
   */
  bool IsToRight (const VW::Point2D & point,
		  const VW::LineSeg2D & line);
	
  /** Project a 3D point onto a 2D point. */
  VW::Point2D Project (const VW::Point3D & worldpoint,
		       const VNL::MatrixFixed < 3, 4,
		       double >projectionmatrix);
	
  /** Project a 3D line-segment onto a 2D line-segment. */
  VW::LineSeg2D Project (const VW::LineSeg3D & worldline,
			 const VNL::MatrixFixed < 3, 4,
			 double >projectionmatrix);
	
  /** Find the point that is closest to a 2D location. */
  template <class Feature>
  unsigned int
  FindNearestPoint(const VW::Point2D & testpoint,
		   const std::vector <Feature>& points);
	
  /** Find the N nearest points to a 2D location. 
   * @param testpoint the location that is searched around
   * @param points    the list of points to be scanned
   * @param n         the maximum number of points to find
   * @param indices   the indices of the nearest points
   * @param max_ratio points are only included if their distance is less than
   *                  max_ratio times the best distance. If zero, all points are
   *                  considered.
   */
  template <class Feature>
  void
  FindNNearestPoints(const VW::Point2D & testpoint,
		     const std::vector <Feature>& points,
		     unsigned int n,
		     std::vector<unsigned int>& indices,
		     double max_ratio = 0.0);
	
  /** Find the line segment that is closest to a 2D location. */
  template <class Feature>
  typename std::list < Feature >::iterator 
  FindNearestLineSeg(const VW::Point2D & testpoint,
		     std::list <Feature>& lines);
	
  /** Calculate the mean position and standard deviation distance of a 
   * 2D point set. This is used for optimal normalisation of point data,
   * amoungst others.
   */
  template < class PointFeature >
  void GetPointDistribution (const std::vector < PointFeature >
			     &feature_list, double &mean_x,
			     double &mean_y, double &sd);
	
  /** Calculate the mean position and standard deviation distance of a 
   * 3D point set. This is used for optimal normalisation of point data,
   * amoungst others.
   */
  template < class PointFeature >
  void GetPointDistribution (const std::vector < PointFeature >
			     &feature_list, 
			     double &mean_x, double &mean_y, double &mean_z, 
			     double &sd);
	
  /** Calculate the mean position and standard deviation distance of 
   * particular members of a 
   * 2D point set. This is used for optimal normalisation of inlier point data,
   * amoungst others.
   */
  template < class PointFeature >
  void GetPointDistribution (const std::vector < PointFeature >
			     &feature_list,
			     const std::vector <
			     unsigned int >&which_features,
			     double &mean_x, double &mean_y,
			     double &sd);
	
  /** Calculate the mean position and standard deviation distance of 
   * particular members of a 
   * 3D point set. This is used for optimal normalisation of inlier point data,
   * amoungst others.
   */
  template < class PointFeature >
  void GetPointDistribution (const std::vector < PointFeature >
			     &feature_list,
			     const std::vector <
			     unsigned int >&which_features,
			     double &mean_x, double &mean_y, double &mean_z,
			     double &sd);
	
	
	
	




#ifdef ROUND
#  undef ROUND
#endif
#define ROUND(v) (v<0) ? ceil(v-0.5) : floor(v+0.5)
	
	
  // Inlines
	
  /** Round coordinates to integer values */
  inline VW::Point2D 
  Round(const VW::Point2D& p)
  {
    return VW::Point2D(ROUND(p.GetX()), 
		       ROUND(p.GetY()));
  }

  /** Round coordinates to integer values */
  inline VW::Point3D 
  Round(const VW::Point3D& p)
  {
    return VW::Point3D(ROUND(p.GetX()), 
		       ROUND(p.GetY()), 
		       ROUND(p.GetZ()));
  }

  inline double
  GetDistance (const VW::Point2D & p1, const VW::Point2D & p2)
  {
    return (p2 - p1).Norm ();
  }
	
  inline double
  GetDistance (const VW::Point2D & p, const VW::Line2D & l)
  {
    return l.GetDistance (p);
  }
	
  inline double
  GetDistance (const VW::Point2D & p, const VW::LineSeg2D & l)
  {
    return l.GetDistanceToLineSeg (p);
  }
	
  inline VW::Line2D
  LineThroughPoints (const VW::Point2D & p1, const VW::Point2D & p2)
  {
    VW::LineSeg2D l (p1, p2);
    return l.GetLine ();
  }
	
  inline VW::Point2D
  Intersect (const VW::Line2D & l1, const VW::Line2D & l2)
  {
    const double invz =
      1.0 / (l1.GetA () * l2.GetB () - l1.GetB () * l2.GetA ());
    const double x = l1.GetB () * l2.GetC () - l1.GetC () * l2.GetB ();
    const double y = l1.GetC () * l2.GetA () - l1.GetA () * l2.GetC ();
		
    return VW::Point2D (x * invz, y * invz);
  }
	
  inline double
  GetAngleBetween (const VW::LineSeg2D & l1, const VW::LineSeg2D & l2)
  {
    return l1.GetAngle () - l2.GetAngle ();
  }
	
  inline bool
  IsToLeft (const VW::Point2D & point, const VW::LineSeg2D & line)
  {
    return (line.IsToLeft (point));
  }
	
  inline bool
  IsToRight (const VW::Point2D & point, const VW::LineSeg2D & line)
  {
    return (line.IsToRight (point));
  }
	
  inline
  VW::Point2D
  Project (const VW::Point3D & worldpoint,
	   const VNL::MatrixFixed < 3, 4, double >projectionmatrix)
  {
    // This is written out in full, for max speed
    const double invz = 1.0 / (projectionmatrix[2][0] * worldpoint.GetX ()
			       +
			       projectionmatrix[2][1] *
			       worldpoint.GetY () +
			       projectionmatrix[2][2] *
			       worldpoint.GetZ () +
			       projectionmatrix[2][3]);
    const double x =
      projectionmatrix[0][0] * worldpoint.GetX () +
      projectionmatrix[0][1] * worldpoint.GetY () +
      projectionmatrix[0][2] * worldpoint.GetZ () +
      projectionmatrix[0][3];
    const double y =
      projectionmatrix[1][0] * worldpoint.GetX () +
      projectionmatrix[1][1] * worldpoint.GetY () +
      projectionmatrix[1][2] * worldpoint.GetZ () +
      projectionmatrix[1][3];
    return VW::Point2D (x * invz, y * invz);
  }
	
  inline
  VW::LineSeg2D
  Project (const VW::LineSeg3D & worldline,
	   const VNL::MatrixFixed < 3, 4, double >projectionmatrix)
  {
    // Simply project the two endpoints
		
    return VW::LineSeg2D (VW::
			  Project (worldline.GetStartPoint (),
				   projectionmatrix),
			  VW::Project (worldline.GetEndPoint (),
				       projectionmatrix));
		
  }
	
  template <class Feature>
  unsigned int
  FindNearestPoint(const VW::Point2D & testpoint,
		   const std::vector <Feature>& points)
  {
    assert(points.size() > 0);
		
    typename std::vector<Feature>::const_iterator i = points.begin();
    typename std::vector<Feature>::const_iterator e = points.end();
		
    // do the first one separately
    unsigned int index = 0, best_index=0;
    double best_distance = i->GetDistance(testpoint);
    i++; index++;
		
    for (;i != e; i++, index++) {
      const double distance = i->GetDistance(testpoint);
      //		std::cout << "("<< testpoint << ")-(" << *i << ") distance = " << distance << " (best="<<best_distance<<")"<<std::endl;
      if (distance < best_distance) {
	best_distance = distance;
	best_index = index;
      }
    }
    return best_index;
  }
	
  template <class Feature>
  void
  FindNNearestPoints(const VW::Point2D & testpoint,
		     const std::vector <Feature>& points,
		     const unsigned int n,
		     std::vector<unsigned int>& indices,
		     double max_ratio)
  {
    assert(points.size() >= n);
		
    typename std::vector<Feature>::const_iterator i = points.begin();
    typename std::vector<Feature>::const_iterator e = points.end();
		
    // do the first ones separately
    unsigned int index = 0;
    indices.clear();
    std::vector<double> best_distances;
    for (;i != e; i++, index++) {
      const double distance = i->GetDistance(testpoint);

      // work out where to insert
      unsigned int index2;
      const unsigned int nb = best_distances.size();
      for (index2 = 0;index2 < nb;index2++) {
	if (distance<best_distances[index2]) break;
      }
      if (index2 == nb) {
	// Insert at end
	if (index2 < n) {
	  best_distances.push_back(distance);
	  indices.push_back(index);
	}
      } else {
	// Shuffle down and insert
	if (nb<n) {
	  best_distances.push_back(0);
	  indices.push_back(0);
	}
	for (unsigned int ri = nb; ri>index2; ri--) {
	  best_distances[ri] = best_distances[ri-1];
	  indices[ri] = indices[ri-1];
	}
	best_distances[index2] = distance;
	indices[index2] = index;
      }    
    }

    // If a max_ratio was set, truncate the list
    if (max_ratio >= 1.0) {
      unsigned int new_size = n;
      const double best = best_distances[0];
      for (unsigned int i=n-1; i>0;i--,new_size--) {
	if ((best_distances[i]/best) < max_ratio) break;
      }
      indices.resize(new_size);
    }


  }
	
  template <class Feature>
  typename std::list < Feature >::iterator 
  FindNearestLineSeg(const VW::Point2D & testpoint,
		     std::list <Feature>& lines)
  {
    assert(lines.size() > 0);
		
    double dist, max_dist;
    typename std::list < Feature >::iterator selected_line, i, e;
    i = lines.begin ();
    selected_line = lines.begin ();
    e = lines.end ();
		
    // Get first distance for comparison
    max_dist = dist = i->GetDistanceToLineSeg(testpoint);
    i++;
		
    // Now the rest
    for (; i != e; i++)
      {
	dist = i->GetDistanceToLineSeg(testpoint);
	//	cout << "testpoint=("<<testpoint<<"), dist="<< dist <<" line=[("<<i->GetStartPoint()<<")<->("<<i->GetEndPoint()<<")"<<endl;
	if (dist < max_dist)
	  {
	    selected_line = i;
	    max_dist = dist;
	  }
      }
    //	cout << "Closet line to ("<<testpoint<<") is [("<<selected_line->GetStartPoint()<<")<->("<<selected_line->GetEndPoint()<<")"<<endl;
    return (selected_line);
		
  }
	
	
	
  // Exported implementation, move to cc file when export is implemented
	
  template < class PointFeature > void
  GetPointDistribution (const std::vector < PointFeature > &feature_list,
			double &mean_x, double &mean_y, double &sd)
  {
    // Stuff for traversing the list
    typename std::vector < PointFeature >::const_iterator it;
    typename std::vector < PointFeature >::const_iterator itend =
      feature_list.end ();
    // Find mean
    double sumx = 0.0;
    double sumy = 0.0;
    for (it = feature_list.begin (); it != itend; it++)
      {
	sumx += it->GetX ();
	sumy += it->GetY ();
      }
    const unsigned N = feature_list.size ();
    mean_x = sumx / N;
    mean_y = sumy / N;
		
    // Find standard deviation (of distance)
    double sumdist = 0.0;
    for (it = feature_list.begin (); it != itend; it++)
      {
	double xdist = it->GetX () - mean_x;
	double ydist = it->GetY () - mean_y;
	sumdist += sqrt (xdist * xdist + ydist * ydist);
      }
    sd = sumdist / N;
  }
	
  template < class PointFeature > void
  GetPointDistribution (const std::vector < PointFeature > &feature_list,
			double &mean_x, double &mean_y, double &mean_z, double &sd)
  {
    // Stuff for traversing the list
    typename std::vector < PointFeature >::const_iterator it;
    typename std::vector < PointFeature >::const_iterator itend =
      feature_list.end ();
    // Find mean
    double sumx = 0.0;
    double sumy = 0.0;
    double sumz = 0.0;
    for (it = feature_list.begin (); it != itend; it++)
      {
	sumx += it->GetX ();
	sumy += it->GetY ();
	sumz += it->GetZ ();
      }
    const unsigned N = feature_list.size ();
    mean_x = sumx / N;
    mean_y = sumy / N;
    mean_z = sumz / N;
		
    // Find standard deviation (of distance)
    double sumdist = 0.0;
    for (it = feature_list.begin (); it != itend; it++)
      {
	double xdist = it->GetX () - mean_x;
	double ydist = it->GetY () - mean_y;
	double zdist = it->GetZ () - mean_z;
	sumdist += sqrt (xdist * xdist + ydist * ydist + zdist * zdist);
      }
    sd = sumdist / N;
  }
	
  template < class PointFeature > void
  GetPointDistribution (const std::vector < PointFeature > &feature_list,
			const std::vector < unsigned int >&which_features,
			double &mean_x, double &mean_y, double &sd)
  {
    // Stuff for traversing the list
    typename std::vector < unsigned int >::const_iterator it, itend;
    itend = which_features.end ();
		
    // Find mean
    double sumx = 0.0;
    double sumy = 0.0;
    for (it = which_features.begin (); it != itend; it++)
      {
	sumx += feature_list[*it].GetX ();
	sumy += feature_list[*it].GetY ();
      }
    const unsigned int N = static_cast<unsigned int>(which_features.size());
    //std::cerr << "Sums ("<<sumx<<","<<sumy<<") from "<<N<<" points." << std::endl;
    mean_x = sumx / N;
    mean_y = sumy / N;
		
    // Find standard deviation (of distance)
    double sumdist = 0.0;
    for (it = which_features.begin (); it != itend; it++)
      {
	double xdist = feature_list[*it].GetX () - mean_x;
	double ydist = feature_list[*it].GetY () - mean_y;
	sumdist += sqrt (xdist * xdist + ydist * ydist);
      }
    sd = sumdist / N;
  }
	
  template < class PointFeature > void
  GetPointDistribution (const std::vector < PointFeature > &feature_list,
			const std::vector < unsigned int >&which_features,
			double &mean_x, double &mean_y, double &mean_z, 
			double &sd)
  {
    // Stuff for traversing the list
    typename std::vector < unsigned int >::const_iterator it, itend;
    itend = which_features.end ();
		
    // Find mean
    double sumx = 0.0;
    double sumy = 0.0;
    double sumz = 0.0;
    for (it = which_features.begin (); it != itend; it++)
      {
	sumx += feature_list[*it].GetX ();
	sumy += feature_list[*it].GetY ();
	sumz += feature_list[*it].GetZ ();
      }
    const unsigned N = which_features.size ();
    //std::cerr << "Sums ("<<sumx<<","<<sumy<<","<<sumz<<") from "<<N<<" points." << std::endl;
    mean_x = sumx / N;
    mean_y = sumy / N;
    mean_z = sumz / N;
		
    // Find standard deviation (of distance)
    double sumdist = 0.0;
    for (it = which_features.begin (); it != itend; it++)
      {
	double xdist = feature_list[*it].GetX () - mean_x;
	double ydist = feature_list[*it].GetY () - mean_y;
	double zdist = feature_list[*it].GetZ () - mean_z;
	sumdist += sqrt (xdist * xdist + ydist * ydist + zdist * zdist);
      }
    sd = sumdist / N;
  }
};				// End namespace VW


#endif
