/** 
 * @file   line2d.cpp
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


#ifndef _line2d_cpp_
#define _line2d_cpp_

#include <cmath> // sqrt

#include "line2d.h"
#include "point2d.h"
#include "lineseg2d.h"
#include <VW/MathBits/covariance3d.h>

VW::Vector2D VW::Line2D::GetDirection() const
{
  const double scale = 1.0 / sqrt(data[0]*data[0]+data[1]*data[1]);
  return VW::Vector2D(data[0]*scale, data[1]*scale);
}

VW::Vector2D VW::Line2D::GetPerpendicular() const
{
  const double scale = 1.0 / sqrt(data[0]*data[0]+data[1]*data[1]);
  return VW::Vector2D(data[1]*scale, -data[0]*scale);
}

double VW::Line2D::GetDistance(const double x,
			       const double y) const
{
  using namespace std; // for sqrt
  // Distance is dot product, providing a,b are normalised
  double denominator = sqrt(data[0]*data[0]+data[1]*data[1]);
  double numerator = data[0]*x + data[1]*y + data[2];

  return fabs(numerator / denominator);
}

double VW::Line2D::GetDistance(const Point2D& p) const
{
  return GetDistance(p.GetX(),p.GetY());
}


double VW::Line2D::GetDistance2(const double x,
			       const double y) const
{
  // Distance is dot product, providing a,b are normalised
  const double denominator = data[0]*data[0]+data[1]*data[1];
  const double numerator = data[0]*x + data[1]*y + data[2];

  return numerator*numerator / denominator;
}

double VW::Line2D::GetDistance2(const Point2D& p) const
{
  return GetDistance2(p.GetX(),p.GetY());
}


VW::LineSeg2D 
VW::Line2D::TruncateX(const double x0,
		     const double x1) const
{
  VW::Point2D startpoint (x0, LookupY(x0));
  VW::Point2D endpoint   (x1, LookupY(x1));
  return ( VW::LineSeg2D(startpoint,endpoint) );
}

VW::LineSeg2D 
VW::Line2D::TruncateY(const double y0,
		     const double y1) const
{
  VW::Point2D startpoint ( LookupX(y0),y0);
  VW::Point2D endpoint   ( LookupX(y1),y1);
  return ( VW::LineSeg2D(startpoint,endpoint) );
}

VW::LineSeg2D 
VW::Line2D::Truncate(const double x0, const double y0,
		     const double x1, const double y1) const
{
  VW::LineSeg2D default_result;

  if (data[0]==0) {
    // Horizontal line - find horizontal bounds and make sure they're within
    // the vertical limits
    VW::LineSeg2D result = TruncateX(x0,x1);
    double y = result.GetStartPoint().GetY();
    if ((y>y0) && (y<y1)) return result;
    else return default_result;

  } else if (data[1]==0) {
    // Vertical line - find vertical bounds and make sure they're within
    // the horizontal limits
    VW::LineSeg2D result = TruncateY(y0,y1);
    double x = result.GetStartPoint().GetX();
    if ((x>x0) && (x<x1)) return result;
    else return default_result;

  } else {
    // General line - truncate both ways and pick the mins
    VW::LineSeg2D resultX = TruncateX(x0,x1);
    VW::LineSeg2D resultY = TruncateY(y0,y1);

    VW::Point2D left,right;

    // Check if left edge was off bottom
    if (resultX.GetStartPoint().GetY() < y0)
      left = resultY.GetStartPoint();
    else 
      left = resultX.GetStartPoint();

    // Check if right edge is off top
    if (resultX.GetEndPoint().GetY() > y1)
      right = resultY.GetEndPoint();
    else 
      right = resultX.GetEndPoint();

    // Last remaining check - is it visible at all?
    if ((left.GetX()>x1) || (right.GetX()<x0)
	|| (left.GetY()>y1) || (right.GetY()<y0)) return(default_result);

    return VW::LineSeg2D(left,right);

  }

  // if all else fails, return the default
  return(default_result);
}


void VW::NormaliseLine(VW::Line2D& line,
		       VW::Covariance3D& line_covariance)
{
  /** Normalise the line so that distances are correct */
  const double a2 = line[0]*line[0];
  const double b2 = line[1]*line[1];
  const double ab = line[0]*line[1];
  const double ac = line[0]*line[2];
  const double bc = line[1]*line[2];
  const double sz = a2+b2;
  const double norm = 1.0/(sz*sz*sz); // (a^2 + b^2)^-3
  
  line = line/sqrt(sz); // ?? flops!

  // Now project around the homg point
  // This long hand version takes 59 flops, whereas full would take 75
  VW::Covariance3D& C = line_covariance;

  const double CP00 = b2*C[0] - ab*C[1];
  const double CP01 = a2*C[1] - ab*C[0];
  const double CP02 = sz*C[2] - ac*C[0] - bc*C[1];
  
  const double CP10 = b2*C[1] - ab*C[3];
  const double CP11 = a2*C[3] - ab*C[1];
  const double CP12 = sz*C[4] - ac*C[1] - bc*C[3];
  
  const double CP20 = b2*C[2] - ab*C[4];
  const double CP21 = a2*C[4] - ab*C[2];
  const double CP22 = sz*C[5] - ac*C[2] - bc*C[4];
  
  C[0] = b2*CP00 - ab*CP10;
  C[1] = b2*CP01 - ab*CP11;
  C[2] = b2*CP02 - ab*CP12;
  C[3] = a2*CP11 - ab*CP01;
  C[4] = a2*CP12 - ab*CP02;
  C[5] = sz*CP22 - ac*CP02 - bc*CP12;
  C *= norm;
}


#endif // #ifndef _line2d_cc_
