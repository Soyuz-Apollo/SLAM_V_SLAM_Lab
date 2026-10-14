/**
 * @file    generatedata.h
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

#include <vector>
#include "point3d.h"

namespace VW {

  /** Cuboid centered around origin. */
  void GeneratePointsInCuboid(const double width, 
			      const double height, 
			      const double depth, 
			      const unsigned int num_points, 
			      std::vector<VW::Point3D>& worldpoints);

  /**  Generate a cloud of points on a spherical surface centered at origin. */
  void GeneratePointsOnSphereSurface(const double radius,
				     const unsigned int num_points, 
				     std::vector<VW::Point3D>& worldpoints);

  /** Generate a cloud of points filling a sphere centered at origin. */
  void GeneratePointsInSphere(const double radius, 
			      const unsigned int num_points, 
			      std::vector<VW::Point3D>& worldpoints);

  /** Generate a cloud of points in a spherical shell centered at origin. */
  void GeneratePointsInShell(const double inner_radius, 
			     const double outer_radius, 
			     const unsigned int num_points, 
			     std::vector<VW::Point3D>& worldpoints);

  /** Generate a cloud of points in a square patch on a spherical shell centered at 
   * the origin and pointing along 'z' (max_angle gives conic half-angle, and is in radians. 
   * Do not give an angle greater than PI/2, as this will spoil the distribution of points)
   */
  void GeneratePointsInShellPatch(const double inner_radius, 
				  const double outer_radius, 
				  const double max_angle, 
				  const unsigned int num_points, 
				  std::vector<VW::Point3D>& worldpoints);

}; // end namespace VW
