#include <iostream>
#include <sstream>

#include <VW/GeomObjects/quaternion.h>

#include "awfface.h"

#define VERBOSE false

using namespace std;

/// A necessary func
static VW::Vector3D GetNormal(const VW::Vector3D& v1,
			      const VW::Vector3D& v2,
			      const VW::Vector3D& v3)
{
  const double x0 = v2.GetX() - v1.GetX();
  const double y0 = v2.GetY() - v1.GetY();
  const double z0 = v2.GetZ() - v1.GetZ();
  const double x1 = v3.GetX() - v1.GetX();
  const double y1 = v3.GetY() - v1.GetY();
  const double z1 = v3.GetZ() - v1.GetZ();
 
  VW::Vector3D result(y0*z1 - y1*z0,
		      z0*x1 - z1*x0,
		      x0*y1 - x1*y0);
  result.Normalise();
  return result;
}

///




VW::AWFFace::AWFFace() 
  : _matindex(0),
    _overall_normal(0,0,0)
{
}

bool VW::AWFFace::ReadLine(const std::string& data,
		       const std::string& current_material)
{
  istringstream stream(data);
  // Possible formats are:  v v v v ...
  //                        v/t v/t v/t v/t ...
  //                        v/t/n v/t/n v/t/n v/t/n ...
  //                        v//n v//n v//n v//n ...
  // where v=vertex index, t=texcoord index, n=normal index
  bool done=false;
  int t;
  char c;
  _type=VertexOnly;


  // First numbers dictate format
  stream >> t;
  AddVertexIndex(t-1);
  stream.get(c);
  if (c=='/') {
    // Looking for another number or '/'
    stream.get(c);
    if (c=='/') {
      SetType(WithNormal);
      if (stream >> t) {
	AddNormalIndex(t-1);
     } else {
	cerr << "Couldn't read normal" << endl;
	exit(0);
      }
    } else {
      // Wasn't a '/', so put it back
      stream.putback(c);
      if (stream >> t) {
	SetType(WithTexture);
	AddTextureIndex(t-1);
	stream.get(c);
	if (c=='/') {
	  SetType(WithTextureAndNormal);
	  stream >> t;
	  AddNormalIndex(t-1);
	}
      } else {
	cerr << "Couldn't read texture" << endl;
	exit(0);
      }
    }
  } else {
    SetType(VertexOnly);
  }
  


  if (VERBOSE) cout << "          Type="<<_type<<endl;
  switch (_type) {
  case VertexOnly:
    {
      while (!done) {
	if (stream >> t) {
	  AddVertexIndex(t-1);
	} else {
	  done=true;
	}
      };
      break;
    }
  case WithTexture:
    {
      unsigned int index=2;
      while (!done) {
	if (stream >> t) {
	  AddVertexIndex(t-1);
	  c=stream.get();
	  if (c!='/') {
	    cerr << "ReadFace: Missing texture coord on vertex "<<index<<" of line \""<<data<<"\""<<endl;
	  }
	  stream >> t;
	  AddTextureIndex(t-1);
	} else {
	  done=true;
	}
	index++;
      };
      break;
    }
  case WithNormal:
    {
      unsigned int index=2;
      while (!done) {
	if (stream >> t) {
	  AddVertexIndex(t-1);
	  c=stream.get();
	  if (c!='/') {
	    cerr << "ReadFace: Missing '/' on vertex "<<index<<" of line \""<<data<<"\""<<endl;
	    exit(0);
	  }
	  c=stream.get();
	  if (c!='/') {
	    cerr << "ReadFace: Missing second '/' on vertex "<<index<<" of line \""<<data<<"\""<<endl;
	    exit(0);
	  }
	  stream >> t;
	  AddNormalIndex(t-1);
	} else {
	  done=true;
	}
	index++;
      }
      break;
    }
  case WithTextureAndNormal:
    {
      unsigned int index=2;
       while (!done) {
	if (stream >> t) {
	  AddVertexIndex(t-1);
	  c=stream.get();
	  if (c!='/') {
	    cerr << "ReadFace: Missing texture coord on vertex "<<index<<" of line \""<<data<<"\""<<endl;
	  }
	  stream >> t;
	  AddTextureIndex(t-1);
	  c=stream.get();
	  if (c!='/') {
	    cerr << "ReadFace: Missing normal vector on vertex "<<index<<" of line \""<<data<<"\""<<endl;
	  }
	  stream >> t;
	  AddNormalIndex(t-1);
	} else {
	  done=true;
	}
	index++;
       }
     break;
    }
  default:
    {
      cerr << "ReadFace: Invalid face type." << endl;
      exit(0);
    }
  }

  _material=current_material;

  // All OK
  return(true);

}


std::ostream& VW::operator<<(std::ostream& s, const AWFFace& f)
{

  // Note that the obj definition specifies that indices start from 1!

  switch (f._type) {
  case VertexOnly:
    for (unsigned int i=0;i<f._vindex.size();i++) {
      s << f._vindex[i]+1 << " \t";
    }
    break;
  case WithTexture:
    for (unsigned int i=0;i<f._vindex.size();i++) {
      s << f._vindex[i]+1 << "/" 
	<< f._tindex[i]+1 << " \t";
    }
    break;
  case WithNormal:
    for (unsigned int i=0;i<f._vindex.size();i++) {
      s << f._vindex[i]+1  << "//" 
	<< f._nindex[i]+1 << " \t";
    }
    break;
  case WithTextureAndNormal:
    for (unsigned int i=0;i<f._vindex.size();i++) {
      s << f._vindex[i]+1 << "/" 
	<< f._tindex[i]+1 << "/" 
	<< f._nindex[i]+1 << " \t";
    }
    break;
  default:
    cerr << "Invalid type" << endl;
    exit(0);
  }

  return s;
}


bool VW::AWFFace::Check(const std::vector<Vector3D>& vert,
		    const std::vector<Vector2D>& tex,
		    const std::vector<Vector3D>& norm)
{
  // At least three points are required for a valid polygon
  if (_vindex.size()<3) {
    cerr << "AWFFace::Check: " 
	 << _vindex.size() << " vertices are not enough to define a polygon." << endl;
    return false;
  }

  // Check that this face doesn't reference non-existant vertices
  const unsigned int max_v = vert.size()-1;
  const unsigned int max_t = tex.size()-1;
  const unsigned int max_n = norm.size()-1;
  for (unsigned int i=0;i<_vindex.size();i++) {
    if (_vindex[i]>int(max_v)) {
      cerr << "AWFFace::Check: Face references vertex "<<_vindex[i]<<" but there are only "<<max_v<<endl;
      return false;
    }
  }
  for (unsigned int i=0;i<_tindex.size();i++) {
    if (_tindex[i]>int(max_t)) {
      cerr << "AWFFace::Check: Face references texture location "<<_tindex[i]<<" but there are only "<<max_t<<endl;
      return false;
    }
  }
  for (unsigned int i=0;i<_nindex.size();i++) {
    if (_nindex[i]>int(max_n)) {
      cerr << "AWFFace::Check: Face references normal "<<_nindex[i]<<" but there are only "<<max_n<<endl;
      return false;
    }
  }



  switch (_type) {
  case VertexOnly:
    break;
  case WithTexture:
    if (_tindex.size() != _vindex.size()) {
      cerr << "AWFFace::Check: " 
	   << " number of vertices (" << _vindex.size()
	   << ") and textures (" << _tindex.size() << ") must be the same." << endl;
      return false;
    }
    break;
  case WithNormal:
    if (_nindex.size() != _vindex.size()) {
      cerr << "AWFFace::Check: " 
	   << " number of vertices (" << _vindex.size()
	   << ") and normals (" << _nindex.size() << ") must be the same." << endl;
      return false;
    }
    break;
  case WithTextureAndNormal:
    if ((_nindex.size() != _vindex.size()) || (_tindex.size() != _vindex.size())) {
      cerr << "AWFFace::Check: " 
	   << " number of vertices (" << _vindex.size()
	   << "), textures (" << _tindex.size()
	   << ") and normals (" << _nindex.size() << ") must be the same." << endl;
      return false;
    }
    break;
  default:
    // unknown type
      cerr << "AWFFace::Check: " 
	   << " face type does not appear to have been set." << endl;
    return false;
  }
  // Always calculate a facet normal
  _overall_normal = GetNormal(vert[_vindex[0]],
			      vert[_vindex[1]],
			      vert[_vindex[2]]);

  // If all is OK
  return true;
}

void VW::AWFFace::SetType(AWFFaceType newtype)
{
  _type = newtype;
}


void VW::AWFFace::AddVertexIndex(int vindex)
{
  _vindex.push_back(vindex);
}


void VW::AWFFace::AddTextureIndex(int tindex)
{
  if ((_type==WithTexture) || (_type==WithTextureAndNormal)) {
    _tindex.push_back(tindex);
  } else {
    cerr << "AWFFace::AddTexture: Attempted to add texture coord to a face that doesn't have them (type="<<_type<<")!" << endl;
    exit(0);
  }
}


void VW::AWFFace::AddNormalIndex(int nindex)
{
  if ((_type==WithNormal) || (_type==WithTextureAndNormal)) {
    _nindex.push_back(nindex);
  } else {
    cerr << "AWFFace::AddNormal: Attempted to add normal vector to a face that doesn't have them (type="<<_type<<")!" << endl;
    exit(0);
  }
}

void VW::AWFFace::Rotate(const VW::Quaternion& q)
{
  _overall_normal = q.Rotate(_overall_normal);
}

std::string VW::AWFFace::GetAsCommaSeparatedList() const
{
  std::ostringstream s;
  unsigned int nv = _vindex.size();
  s << _vindex[0];
  for (unsigned int i=1;i<nv;i++) {
    s << ", " << _vindex[i];
  }
  return s.str();
}
