/** 
 * @file   awfface.h
 * @author BJ Tordoff
 * @date   13/01/2004
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


#ifndef _vw_awfface_h_
#define _vw_awfface_h_

#include <vector>
#include <string>
#include <iostream>

#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/point3d.h>

namespace VW {

  class Quaternion;

enum AWFFaceType {
  VertexOnly=0,
  WithTexture,
  WithNormal,
  WithTextureAndNormal
};


/** 
 * @author BJ Tordoff
 * @date 13/01/2004
 * @brief A class to represent faces for Alias WaveFront (AWF) objects.
 */
class AWFFace {
  friend std::ostream& operator<<(std::ostream& s, const AWFFace& f);
public:
  AWFFace();

  std::string GetMaterialName() const;

  // Try to read the indices and things froma line taken from an obj file
  bool ReadLine(const std::string& data,
		const std::string& current_material);

  // Check that all sizes, references etc are correct.
  // Also updates the overall normal if required
  bool Check(const std::vector<Vector3D>& vert,
	     const std::vector<Vector2D>& tex,
	     const std::vector<Vector3D>& norm);

  void Rotate(const VW::Quaternion& q);

  void SetType(AWFFaceType newtype);
  void AddVertexIndex(int vindex);
  void AddTextureIndex(int tindex);
  void AddNormalIndex(int nindex);
  void CalcNormal();
  unsigned int NumVertices() const;

  VW::Vector3D GetFacetNormal() const;

  std::string GetAsCommaSeparatedList() const;

  std::vector<int> _vindex, _tindex, _nindex;
  std::string _material;
  int _matindex;
  AWFFaceType _type;
  Vector3D _overall_normal;
};

std::ostream& operator<<(std::ostream& s, const AWFFace& f);

}; // end namespace VW








// INLINES

inline std::string 
VW::AWFFace::GetMaterialName() const
{
  return _material;
}

inline VW::Vector3D 
VW::AWFFace::GetFacetNormal() const
{
  return _overall_normal;
}

inline unsigned int 
VW::AWFFace::NumVertices() const
{
  return _vindex.size();
}

#endif
