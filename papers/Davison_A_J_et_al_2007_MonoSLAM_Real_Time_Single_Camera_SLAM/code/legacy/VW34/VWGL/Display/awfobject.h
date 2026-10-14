/** 
 * @file   awfobject.h
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
/** 
 * @class VW::AWFObject
 * @author BJ Tordoff
 * @date 13/01/2004
 * @brief Class to read and store an Alias WaveFront (AWF) objects.
 */
#ifndef _awfobject_h_
#define _awfobject_h_

#include <vector>
#include <string>

#include <VW/GeomObjects/lineseg3d.h>

#include "awfedge.h"
#include "awfface.h"
#include "awfmaterial.h"    // For the materials/textures stuff


namespace VW {

  class ThreeDToolGLCommon;
  class Quaternion;

class AWFObject {
public:
  AWFObject();
  AWFObject(const std::string& filename, bool generate_edges = false);

  /** Load a new object from a file. Any existing data will be discarded. */
  void LoadFile(const std::string& filename, bool generate_edges = false);

  /** Write out the geometry data to a ".obj" file (filename should include
   * the full path and extension - none will be added).
   */
  void SaveObject(const std::string& filename) const;

  /** Write out the materials data to a ".mtl" file (filename should include
   * the full path and extension - none will be added).
   */
  void SaveMaterials(const std::string& filename) const;


  //******* EXPERIMENTAL *******//

  /** Write out all data. The geometry is saved to filename.obj, the materials
   * to filename.mtl and any texture images are copied to the same location (if
   * different from the source location).
   */
  void SaveAll(const std::string& filename) const;

  void SaveAsVRML(const std::string& filename) const;

  //******* /EXPERIMENTAL *******//


  /** Draw the object - note that you must have the right GL context set and the position
   * and orientation how you want them - this functions just draws on the current view at
   * the current position!
   */
  void Draw() const;

  /** VW version of the draw command that also takes into account sizing etc, 
   * and with optional selection (0 indicates no selection). 
   */
  void Draw(VW::ThreeDToolGLCommon& display,
	    const int name=0) const;

  void GetBoundingBox(float& x0, float& y0, float& z0, 
		      float& x1, float& y1, float& z1) const;
	
  /** Move the object with respect to its origin. */
  void Translate(const VW::Vector3D& delta);

  /** Rotate the object with respect to its origin. */
  void Rotate(const VW::Quaternion& q);

  /** Rotate the object around the Y-axis with respect to its origin. */
  void RotateY(const double theta);



  /** Scale the object uniformly around its origin. */
  void Rescale(float scale_factor);

  /** Make the object origin the centroid */
  void Recentre();
	      
  /** Rescale so that the largest dimension is 1 */
  void Unitise();
	    

  // Occasionally we want to get to the primitives:
  unsigned int GetNumVertices() const;
  unsigned int GetNumEdges() const;
  unsigned int GetNumFaces() const;

  /** Get a vertex in object coords. */
  VW::Point3D GetVertex(unsigned int index) const;

  /** Get an edge, along with the normals for the bordering faces. */
  void GetEdge(unsigned int index, VW::LineSeg3D& line, 
	       VW::Vector3D& normal1, VW::Vector3D& normal2) const;

  void GetFace(unsigned int index,
	       std::vector<VW::Point3D>& vertices, 
	       VW::Vector3D& normal) const;
  
protected:
  void _CalcBoundingBox();
  void _ReadVertex(const std::string& data);
  void _GenerateEdges();
  void _GenList();		 

  bool _initialised;
  std::vector<Vector3D> _vertex;
  std::vector<Vector3D> _normal;
  std::vector<Vector2D> _texcoord;
  std::vector<AWFFace>     _face;
  std::vector<AWFEdge>     _edge;
  std::vector<AWFMaterial> _material;
  int _list_id;
  // Bounding box
  float _minx,_maxx;
  float _miny,_maxy;
  float _minz,_maxz;

};

} // end namespace VW


// INLINES

inline unsigned int 
VW::AWFObject::GetNumVertices() const
{
  return _vertex.size();
}

inline unsigned int 
VW::AWFObject::GetNumEdges() const
{
  return _edge.size();
}

inline unsigned int 
VW::AWFObject::GetNumFaces() const
{
  return _face.size();
}

inline VW::Point3D 
VW::AWFObject::GetVertex(unsigned int index) const
{
  assert(index<_vertex.size());
  return _vertex[index];
}
inline void 
VW::AWFObject::GetEdge(unsigned int index, VW::LineSeg3D& line, 
		       VW::Vector3D& normal1, VW::Vector3D& normal2) const
{
  line = _edge[index].GetAsLine(_vertex);
  normal1 = _edge[index].GetNormal1(_face);
  normal2 = _edge[index].GetNormal2(_face);
}
inline void 
VW::AWFObject::GetFace(unsigned int index,
		       std::vector<VW::Point3D>& vertices, 
		       VW::Vector3D& normal) const
{
  vertices.clear();
  const unsigned int num_vert = _face[index]._vindex.size();
  for (unsigned int v=0;v<num_vert;v++) 
    vertices.push_back(_vertex[_face[index]._vindex[v]]);
  normal = _face[index]._overall_normal;
}

#endif
