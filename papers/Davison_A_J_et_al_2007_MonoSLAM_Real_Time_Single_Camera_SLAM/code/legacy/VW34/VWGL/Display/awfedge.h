#ifndef _vw_awfedge_h_
#define _vw_awfedge_h_

#include <vector>
#include <VW/GeomObjects/point3d.h>
#include <VW/GeomObjects/lineseg3d.h>
#include "awfface.h"

namespace VW {

  class AWFEdge {
  public:
    AWFEdge(unsigned int startindex, unsigned int endindex);

    void AddFace(unsigned int index);

    VW::LineSeg3D GetAsLine(const std::vector<VW::Point3D>& vertices) const;

    VW::Vector3D GetNormal1(const std::vector<AWFFace>& faces) const;
    VW::Vector3D GetNormal2(const std::vector<AWFFace>& faces) const;
    void Draw(const std::vector<VW::Point3D>& vertices) const;


    bool IsMe(unsigned int index1, unsigned int index2) const;

    bool HasFaces() const { return (_have_face1 && _have_face2); }
    
  private:
    unsigned int _si, _ei;
    unsigned int _fi1, _fi2;
    bool _have_face1, _have_face2;

  };

};

inline
VW::AWFEdge::AWFEdge(unsigned int startindex, unsigned int endindex)
  : _si(startindex), _ei(endindex), _have_face1(false), _have_face2(false)
{
}

inline bool 
VW::AWFEdge::IsMe(unsigned int index1, unsigned int index2) const
{
  if ((index1==_si) && (index2==_ei)) return true;
  if ((index1==_ei) && (index2==_si)) return true;
  return false;
}


#endif
