#include <iostream>

#ifdef _WIN32
#include <windows.h> // required before including GL
#endif
#include <GL/gl.h>

#include "awfedge.h"

using namespace std;

void 
VW::AWFEdge::AddFace(unsigned int index)
{
  if (_have_face1) {
    if (_have_face2) {
      cerr << "AWFEdge::AddFace: Tried to add a face, but already have both neighbouring faces!" << endl;
    } else {
      _fi2 = index;
      _have_face2 = true;
    }
  } else {
    _fi1 = index;
    _have_face1 = true;
  }
}

VW::LineSeg3D 
VW::AWFEdge::GetAsLine(const std::vector<VW::Point3D>& vertices) const
{
  return VW::LineSeg3D(vertices[_si], vertices[_ei]);
}

VW::Vector3D 
VW::AWFEdge::GetNormal1(const std::vector<AWFFace>& faces) const
{
  return faces[_fi1].GetFacetNormal();
}

VW::Vector3D 
VW::AWFEdge::GetNormal2(const std::vector<AWFFace>& faces) const
{
  return faces[_fi2].GetFacetNormal();
}

void 
VW::AWFEdge::Draw(const std::vector<VW::Point3D>& vertices) const
{
  const VW::Point3D& s = vertices[_si];
  const VW::Point3D& e = vertices[_ei];
  glBegin(GL_LINES); {
    glVertex3f(s.GetX(), s.GetY(), s.GetZ());
    glVertex3f(e.GetX(), e.GetY(), e.GetZ());
  }; glEnd();
}
