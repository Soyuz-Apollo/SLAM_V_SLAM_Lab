/**
 * @file    generatedata.cpp
 * @ingroup GeometricObjects
 * @author  BJ Tordoff
 * @date    04/05/2004
 * @brief   Creating random or systematic sets of data (ie. synthetic data).
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


#include "generatedata.h"

#include <VW/MathBits/random.h>
#include "point3d.h"

#define PI    3.14159265358979304759259
#define TWOPI 6.28318530717958665604786


void VW::GeneratePointsInCuboid(const double width, 
			  const double height, 
			  const double depth, 
			  const unsigned int num_points, 
			  std::vector<VW::Point3D>& worldpoints)
{
  worldpoints.reserve(worldpoints.size() + num_points);
  for (unsigned int i=0;i<num_points;i++) {
    const double x=VW::Rand(width)  - 0.5*width;
    const double y=VW::Rand(height) - 0.5*height;
    const double z=VW::Rand(depth)  - 0.5*depth;
    worldpoints.push_back(VW::Point3D(x,y,z));
  }
}

void VW::GeneratePointsOnSphereSurface(const double radius,
				       const unsigned int num_points, 
				       std::vector<VW::Point3D>& worldpoints)
{
  // Note that:
  //   azimuth is uniformly distributed (-PI <--> PI)
  //   elevation is therefore cosinusoidally distributed (-PI/2 <--> PI/2)
  worldpoints.reserve(worldpoints.size() + num_points);
  for (unsigned int i=0;i<num_points;i++) {
    const double azimuth = VW::Rand(TWOPI)-PI;
    const double elevation = -asin(VW::Rand(2.0)-1.0);
    worldpoints.push_back(VW::Point3D(radius*cos(elevation)*sin(azimuth),
				      radius*sin(elevation),
				      radius*cos(elevation)*cos(azimuth)));
  }
}


void VW::GeneratePointsInSphere(const double radius, 
			       const unsigned int num_points, 
			       std::vector<VW::Point3D>& worldpoints)
{
  // Note that:
  //   azimuth is uniformly distributed (-PI <--> PI)
  //   elevation is therefore cosinusoidally distributed (-PI/2 <--> PI/2)
  //   r is distributed according to sqrt
  const double r2 = radius*radius;
  worldpoints.reserve(worldpoints.size() + num_points);
  for (unsigned int i=0;i<num_points;i++) {
    const double azimuth = VW::Rand(TWOPI)-PI;
    const double elevation = -asin(VW::Rand(2.0)-1.0);
    const double r = sqrt(VW::Rand(r2));

    worldpoints.push_back(VW::Point3D(r*cos(elevation)*sin(azimuth),
				      r*sin(elevation),
				      r*cos(elevation)*cos(azimuth)));
  }
}

void VW::GeneratePointsInShell(const double inner_radius, 
			       const double outer_radius, 
			       const unsigned int num_points, 
			       std::vector<VW::Point3D>& worldpoints)
{
  // Note that:
  //   azimuth is uniformly distributed (-PI <--> PI)
  //   elevation is therefore cosinusoidally distributed (-PI/2 <--> PI/2)
  //   r is distributed according to sqrt (inner_radius <--> outer_radius)
  const double ir2 = inner_radius*inner_radius;
  const double or2 = outer_radius*outer_radius;
  worldpoints.reserve(worldpoints.size() + num_points);
  for (unsigned int i=0;i<num_points;i++) {
    const double azimuth = VW::Rand(TWOPI)-PI;
    const double elevation = -asin(VW::Rand(2.0)-1.0);
    const double r = sqrt(VW::Rand(or2-ir2) + ir2);

    worldpoints.push_back(VW::Point3D(r*cos(elevation)*sin(azimuth),
				      r*sin(elevation),
				      r*cos(elevation)*cos(azimuth)));
  }
}

void VW::GeneratePointsInShellPatch(const double inner_radius, 
				const double outer_radius, 
				const double max_angle, 
				const unsigned int num_points, 
				std::vector<VW::Point3D>& worldpoints)
{
  // Note that:
  //   azimuth is uniformly distributed (-PI <--> PI)
  //   elevation is therefore cosinusoidally distributed (-PI/2 <--> PI/2)
  //   r is distributed according to sqrt (inner_radius <--> outer_radius)
  const double ir2 = inner_radius*inner_radius;
  const double or2 = outer_radius*outer_radius;
  worldpoints.reserve(worldpoints.size() + num_points);
  for (unsigned int i=0;i<num_points;i++) {
    const double azimuth = VW::Rand(2.0*max_angle)-max_angle;
    const double elevation = -asin(VW::Rand(2.0*max_angle)-max_angle);
    const double r = sqrt(VW::Rand(or2-ir2) + ir2);

    worldpoints.push_back(VW::Point3D(r*cos(elevation)*sin(azimuth),
				      r*sin(elevation),
				      r*cos(elevation)*cos(azimuth)));
  }
}
