#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>

#ifdef _WIN32
#include <windows.h> // required before including GL
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#include <VW/IOBasics/miscioutils.h> // For GetFilePath() etc
#include <VW/GeomObjects/point3d.h>
#include <VWGL/Display/threedtoolglcommon.h>

#include "awfobject.h"

#define VERBOSE false


using namespace std;


VW::AWFObject::AWFObject()
  : _initialised(false),
    _list_id(-1)
{
}



VW::AWFObject::AWFObject(const std::string& filename, bool generate_edges)
  : _initialised(false),
    _list_id(-1)
{
  LoadFile(filename, generate_edges);
}



void VW::AWFObject::Translate(const Vector3D& delta)
{
  std::vector<Vector3D>::iterator v = _vertex.begin(), ve = _vertex.end();
  for (; v != ve ; v++) {
    *v += delta;
  }

  _minx -= delta.GetX();   _maxx -= delta.GetX();
  _miny -= delta.GetY();   _maxy -= delta.GetY();
  _minz -= delta.GetZ();   _maxz -= delta.GetZ();

  _GenList();
}



void VW::AWFObject::Rotate(const VW::Quaternion& q)
{
  std::vector<Vector3D>::iterator v = _vertex.begin(), ve = _vertex.end();
  for (; v != ve ; v++) {
    *v = q.Rotate(*v);
  }
  std::vector<Vector3D>::iterator n = _normal.begin(), ne = _normal.end();
  for (; n != ne ; n++) {
    *n = q.Rotate(*n);
  }
  // Faces store a normal, so must laso be rotated
   std::vector<AWFFace>::iterator f = _face.begin(), fe = _face.end();
  for (; f != fe ; f++) {
    f->Rotate(q);
  }
 

  _CalcBoundingBox();
  _GenList();
}



void VW::AWFObject::RotateY(const double theta)
{
  const double c = cos(theta);
  const double s = sin(theta);

  // Both vertices and normals must be rotated
  std::vector<Vector3D>::iterator v = _vertex.begin(), ve = _vertex.end();
  for (; v != ve ; v++) {
    const double x(v->GetX()), z(v->GetZ());
    v->Set(c*x-s*z,
	   v->GetY(),
	   s*x+c*z);
  }
  std::vector<Vector3D>::iterator n = _normal.begin(), ne = _normal.end();
  for (; n != ne ; n++) {
    const double x(n->GetX()), z(n->GetZ());
    n->Set(c*x-s*z,
	   n->GetY(),
	   s*x+c*z);
  }

  _CalcBoundingBox();
  _GenList();
}



void VW::AWFObject::Recentre()
{
  Vector3D centroid(-0.5*(_minx+_maxx),
		    -0.5*(_miny+_maxy),
		    -0.5*(_minz+_maxz));

  Translate(centroid);
}



void VW::AWFObject::Rescale(float scale_factor)
{
  std::vector<Vector3D>::iterator v = _vertex.begin(), ve = _vertex.end();
  for (; v != ve ; v++) {
    *v *= scale_factor;
  }

  _minx *= scale_factor;   _maxx *= scale_factor;
  _miny *= scale_factor;   _maxy *= scale_factor;
  _minz *= scale_factor;   _maxz *= scale_factor;

  _GenList();
}



void VW::AWFObject::Unitise()
{
  cout << "X lims: " << _minx << " - " << _maxx << endl;
  cout << "Y lims: " << _miny << " - " << _maxy << endl;
  cout << "Z lims: " << _minz << " - " << _maxz << endl;

  const float sx = _maxx - _minx;
  const float sy = _maxy - _miny;
  const float sz = _maxz - _minz;
  float scale = sx;
  if (sy>scale) scale=sy;
  if (sz>scale) scale=sz;
  scale =2.0f / scale;

  cout << "Scale="<<scale<<endl;

  Rescale(scale);
}



void VW::AWFObject::LoadFile(const std::string& filename,
			     bool generate_edges)
{
  // First check that there is a current GL context
  if (glutGetWindow()<0) {
    cerr << "VW::AWFObject::LoadFile: No active GL context - make sure you have created a display before trying to load an object for it!" << endl;
    exit(0);
  }
  cout << "**** window="<<glutGetWindow()<<endl;

  // Split the path from the filename for use in loading materials and other 
  // files
  std::string path = GetFilePath(filename);

  if (_initialised) {
    _vertex.clear();
    _normal.clear();
    _texcoord.clear();
    _face.clear();
    _material.clear();
    _initialised = false;
  }
  std::vector<AWFMaterial> loaded_materials;

  // Add default materials
  AddDefaultAWFMaterials(loaded_materials);


  ifstream file(filename.c_str());
  char buff[1024];
  if (!file) {
    cerr << "Error: file \"" << filename << "\" not found." << endl;
    return;
  } else {
    cout << "Reading from file \"" << filename << "\":" << endl;
    bool done=false;
    unsigned int index=1;
    unsigned int matindex=1;
    string current_material("default");
    while (!done) {
      if (file.getline(buff,1024)) {
	
	// Now check the first char
	switch (buff[0]) {
	case '#':
	  if (VERBOSE) cout << "   " << index << ": Comment" << endl;
	  break;
	case 'v':
	  if (VERBOSE) cout << "   " << index << ": Vertex" << endl;
	  _ReadVertex(&buff[1]);
	  break;
	case 'f': {
	  if (VERBOSE) cout << "   " << index << ": Face" << endl;
	  AWFFace f;
	  if ((f.ReadLine(&buff[1],current_material)) 
	      && (f.Check(_vertex,_texcoord,_normal))) {
	    _face.push_back(f);
	  }
	  break;
	}
	case 'm': {
	  // Most likely this is a material file spec, but better check
	  if ((buff[1]=='t')
	      && (buff[2]=='l')
	      && (buff[3]=='l')
	      && (buff[4]=='i')
	      && (buff[5]=='b')) {
	    if (VERBOSE) 
	      cout << "   " << index << ": Material library" << endl;
	    // This bit of jiggery-pokery is just to remove leading/trailing whitespce
	    string filename;
	    {
	      istringstream stream(&buff[7]);
	      stream >> filename;
	    }
	    ReadAWFMaterialsFile(path+filename,loaded_materials);
	    matindex++;
	  } else {
	    if (VERBOSE) cout << "   " << index << ": Unknown"<< endl;
	  }
	  break;
	}
	case 'u': {
	  // Most likely this is a material file spec, but better check
	  if ((buff[1]=='s')
	      && (buff[2]=='e')
	      && (buff[3]=='m')
	      && (buff[4]=='t')
	      && (buff[5]=='l')) {
	    if (VERBOSE) 
	      cout << "   " << index << ": Use material" << endl;
	    istringstream st(&buff[6]);
	    st >> current_material;
	  } else {
	    if (VERBOSE) cout << "   " << index << ": Unknown"<< endl;
	  }
	  break;
	}
	default:
	  if (VERBOSE) cout << "   " << index << ": Unknown"<< endl;
	  break;
	}

      } else {
	done=true;
      }
      index++;
    } // End while(!done)
    file.close();
    
    // Now copy any materials actually used into the permanent store
    for (unsigned int m=0;m<loaded_materials.size();m++) {
      bool found=false;
      for (unsigned int i=0;i<_face.size();i++) {
	//cout << "Compare \""<<_face[i]._material<<"\" == \"" << _material[m]._my_name << "\"" << endl;
	if (loaded_materials[m].IsMe(_face[i]._material)) {
	  _material.push_back(loaded_materials[m]);
	  _material[_material.size()-1].SetUsed(true);
	  found = true;
	  break;
	}
      }
      if (!found) {
	cerr << "Material \"" << loaded_materials[m].GetName() << "\" not used." << endl;
      }
    }

    // And generate edges for the faces
    if (generate_edges) _GenerateEdges();

    cout << "Read a total of "<<index-1<<" lines and "<<matindex-1<<" material files, giving:"<<endl;
    cout << "   " << _vertex.size() << " vertices"<< endl;
    cout << "   " << _normal.size() << " normals" << endl;
    cout << "   " << _texcoord.size() << " texture positions" << endl;
    cout << "   " << _face.size() << " faces" << endl;
    if (generate_edges) cout << "   " << _edge.size() << " edges" << endl;
    cout << "   " << _material.size() << " materials" << endl;

    // OK, now perform some optimisations.

    // 1. Change the material strings into indices
    for (unsigned int i=0;i<_face.size();i++) {
      for (unsigned int m=0;m<_material.size();m++) {
	if (_material[m].IsMe(_face[i]._material)) {
	  _face[i]._matindex = m;
	  //cout << "Face "<<i<<" using material "<<m<<endl;
	  break;
	}
      }
    }

    // 2. measure the bounding box
    _CalcBoundingBox();


    _initialised = true;

  } // End if (openfile)

  _GenList();

}



void VW::AWFObject::SaveObject(const std::string& filename) const
{
  string mtlfilename;
  {
    string path,base,ext;
    VW::SplitFilename(filename,path,base,ext);
    mtlfilename = base+".mtl";
  }
  

  ofstream file(filename.c_str());

  if (!file) {
    cerr << "VW::AWFObject::SaveObject: Could not open file \""<<filename<<"\" for writing." << endl;
    return;
  } else {
   cout << "VW::AWFObject::SaveObject: Writing object to file \""<<filename<<"\""<<endl;

   time_t t = time(NULL);

   file << "# " << endl;
   file << "# file " << filename << endl;
   file << "# Written by VW::AWFObject on " << ctime(&t);
   file << "# " << endl;
   file << "# Contents:" << endl;
   file << "#   " << _vertex.size() << " vertices," << endl;
   file << "#   " << _texcoord.size() << " texture coordinates," << endl;
   file << "#   " << _normal.size() << " normal vectors," << endl;
   file << "#   and " << _face.size() << " faces" << endl;
   file << "#   using " << _material.size() << " materials." << endl;
   file << "# " << endl;
   file << endl;
   file << endl;

   file << "mtllib " << mtlfilename << endl;
   file << endl;

   {
     file << "# Vertices:" << endl;
     std::vector<VW::Vector3D>::const_iterator v=_vertex.begin(), ve=_vertex.end();
     for ( ; v != ve ; v++)
       file << "v   "<<*v<<endl;

     file << endl;
     file << endl;
   }

   {
     file << "# Texture coordinates:" << endl;
     std::vector<VW::Vector2D>::const_iterator t=_texcoord.begin(), te=_texcoord.end();
     for ( ; t != te ; t++)
       file << "vt  "<<*t<<endl;

     file << endl;
     file << endl;
   }

   {
     file << "# Normal vectors:" << endl;
     std::vector<VW::Vector3D>::const_iterator n=_normal.begin(), ne=_normal.end();
     for ( ; n != ne ; n++)
       file << "vn  "<<*n<<endl;

     file << endl;
     file << endl;
   }

   {
     file << "# Faces:" << endl;
     std::vector<VW::AWFFace>::const_iterator f=_face.begin(), fe=_face.end();
     std::string current_material("<none>");
     for ( ; f != fe ; f++) {
       const string mi = f->GetMaterialName();
       if (mi != current_material) {
	 file << endl;
	 file << "usemtl " << mi << endl;
 	 current_material = mi;
      }
       file << "f  "<<*f<<endl;
     }

     file << endl;
     file << endl;
   }

   file << "# End" << endl;

   file.close();
  }

}



void VW::AWFObject::SaveMaterials(const std::string& filename) const
{
  ofstream file(filename.c_str());

  if (!file) {
    cerr << "VW::AWFObject::SaveMaterials: Could not open file \""<<filename<<"\" for writing." << endl;
    return;
  } else {
   cout << "VW::AWFObject::SaveMaterials: Writing object to file \""<<filename<<"\""<<endl;

   time_t t = time(NULL);

   unsigned int numtex = 0;
     for (unsigned int m=0;m<_material.size();m++)
       if (_material[m].HasTexture()) numtex++;


   file << "# " << endl;
   file << "# file " << filename << endl;
   file << "# Written by VW::AWFObject on " << ctime(&t);
   file << "# " << endl;
   file << "# Contents:" << endl;
   file << "#   " << _material.size() << " materials," << endl;
   file << "#   using " << numtex << " textures." << endl;
   file << "# " << endl;
   file << endl;
   file << endl;

   std::vector<AWFMaterial>::const_iterator mi=_material.begin(), me=_material.end();
   for ( ; mi != me; mi++) {
     file << *mi << endl;
   }

  }
}



void VW::AWFObject::SaveAll(const std::string& filename) const
{
  std::string path, base, ext;
  VW::SplitFilename(filename,path,base,ext);

  // Determine the obj and mtl filenames
  string objname,mtlname;
  if (ext=="obj") {
    objname = filename;
    if (path.size()>0) {
      mtlname = path+"/"+base+".mtl";
    } else {
      mtlname = base+".mtl";
    }
  } else {
     objname = filename+".obj";   
     objname = filename+".mtl";   
  }

  vector<string> sourcefiles,destfiles;
  for (unsigned int m=0;m<_material.size();m++) {
    if (_material[m].HasTexture()) {
      string mpath,mbase,mext;
      VW::SplitFilename(_material[m].GetTextureFilename(),mpath,mbase,mext);
      if (mpath != path) {
	string newmfile = VW::ReformFilename(path,mbase,mext);
	sourcefiles.push_back(_material[m].GetTextureFilename());
	destfiles.push_back(newmfile);
      }
    }
  }

  // OK, now do it!
  this->SaveObject(objname);
  this->SaveMaterials(mtlname);

  for (unsigned int i=0;i<sourcefiles.size();i++) {
    cout << "VW::AWFObject::SaveAll: Copy file \""<<sourcefiles[i]<<"\" to \""<<destfiles[i]<<"\""<<endl;
    VW::CopyFile( sourcefiles[i], destfiles[i] );
  }
}


void VW::AWFObject::SaveAsVRML(const std::string& filename) const
{
  std::string path, base, ext;
  VW::SplitFilename(filename,path,base,ext);

  // Determine the wrl filename
  string wrlname;
  if (ext=="wrl") {
    wrlname = filename;
  } else {
    wrlname = filename+".wrl";   
  }

  vector<string> sourcefiles,destfiles;
  for (unsigned int m=0;m<_material.size();m++) {
    if (_material[m].HasTexture()) {
      string mpath,mbase,mext;
      VW::SplitFilename(_material[m].GetTextureFilename(),mpath,mbase,mext);
      if (mpath != path) {
	string newmfile = VW::ReformFilename(path,mbase,mext);
	sourcefiles.push_back(_material[m].GetTextureFilename());
	destfiles.push_back(newmfile);
      }
    }
  }

  // OK, now do it!
  ofstream file(wrlname.c_str());

  if (!file) {
    cerr << "VW::AWFObject::SaveAsVRML: Could not open file \""<<wrlname<<"\" for writing." << endl;
    return;
  } else {
   cout << "VW::AWFObject::SaveAsVRML: Writing object to file \""<<wrlname<<"\""<<endl;

   time_t t = time(NULL);

   const unsigned int nummat = _material.size();
   unsigned int numtex = 0;
   for (unsigned int m=0;m<nummat;m++)
     if (_material[m].HasTexture()) numtex++;

     file << "#VRML V2.0 utf8" << endl;
     file << "# " << endl;
     file << "# file " << wrlname << endl;
     file << "# Written by VW::AWFObject on " << ctime(&t);
     file << "# " << endl;
     file << "# Contents:" << endl;
     file << "#   " << _vertex.size() << " vertices," << endl;
     file << "#   " << _texcoord.size() << " texture coordinates," << endl;
     file << "#   " << _normal.size() << " normal vectors," << endl;
     file << "#   and " << _face.size() << " faces" << endl;
     file << "#   using " << _material.size() << " materials" << endl;
     file << "#        (" << numtex << " textures)." << endl;
     file << "# " << endl;
     file << endl;
     file << endl;


     file << "Transform {\n";
     file << "  children [\n";


     // For each material the list of points is duplicated
     for (unsigned int m=0 ; m < nummat; m++) {

       const std::string matname = _material[m].GetName();

       file << "    Shape {\n";
       file << "      appearance Appearance {\n";
       file << _material[m].GetVRMLString() << endl;
       file << "      }\n";
       
       file << "      geometry IndexedFaceSet {\n";
       
       // First the vertex list
       file << "        coord Coordinate {\n";
       file << "          point [\n";
       const unsigned int nv = _vertex.size();
       if (nv>0) {
	 file << "            "<<_vertex[0].GetX()
	      <<" "<<_vertex[0].GetY()
	      <<" "<<_vertex[0].GetZ();
	 // For multiple lines, add a comma
	 for (unsigned int i=1;i<nv;i++) {
	   file << ",\n            "<<_vertex[i].GetX()
		<<" "<<_vertex[i].GetY()
		<<" "<<_vertex[i].GetZ();
	 }
       }
       file << "\n          ]\n";
       file << "        }\n";
       file << "        coordIndex [\n";
       
       // Find faces using this material
       std::vector<unsigned int> face_index;
       for (unsigned int f=0;f<_face.size();f++) {
	 if (_face[f].GetMaterialName() == matname) face_index.push_back(f);
       }
       
       const unsigned int nf = face_index.size();
       if (nf>0) {
	 file << "          "<<_face[face_index[0]].GetAsCommaSeparatedList()<<", -1";
	 // For multiple lines, add a comma
	 for (unsigned int i=1;i<nf;i++)
	   file << ",\n          "<<_face[face_index[i]].GetAsCommaSeparatedList()<<", -1";
       }
       file << "\n        ]\n";
       file << "      }\n"; // End - indexed face set
       file << "    }"; // End - shape
       if ((m+1)<nummat) file << ",";
       file << endl;
     } // End for material
       file << "  ]\n"; // End - Children
       file << "}"; // End - Transform
     file.close();
  }
  
  // And copy the textures
  for (unsigned int i=0;i<sourcefiles.size();i++) {
    cout << "VW::AWFObject::SaveAll: Copy file \""<<sourcefiles[i]<<"\" to \""<<destfiles[i]<<"\""<<endl;
    VW::CopyFile( sourcefiles[i], destfiles[i] );
  }
}



void VW::AWFObject::_CalcBoundingBox()
{
  _minx = _maxx = _vertex[0]._x;
  _miny = _maxy = _vertex[0]._y;
  _minz = _maxz = _vertex[0]._z;
  std::vector<Vector3D>::const_iterator v=_vertex.begin(), ve=_vertex.end();
  for (; v != ve ; v++) {
    const double x=v->GetX();;
    const double y=v->GetY();;
    const double z=v->GetZ();;
    if (x < _minx) _minx = x;
    if (x > _maxx) _maxx = x;
    if (y < _miny) _miny = y;
    if (y > _maxy) _maxy = y;
    if (z < _minz) _minz = z;
    if (z > _maxz) _maxz = z;
  }
}



void VW::AWFObject::GetBoundingBox(float& x0, float& y0, float& z0, 
				   float& x1, float& y1, float& z1) const
{
  x0 = _minx;
  y0 = _miny;
  z0 = _minz;
  x1 = _maxx;
  y1 = _maxy;
  z1 = _maxz;
}



void VW::AWFObject::_ReadVertex(const std::string& data)
{
  if (VERBOSE) cout << "   Read a vertex" << endl;

  istringstream stream(data);


  char t=stream.get();
  
  switch(t) {
  case 'n':
    { // Normal vector
    Vector3D result(0,0,0);
    stream >> result;
    _normal.push_back(result);
    break;
    }
  case 't':
    { // Texture position
    Vector2D result(0,0);
    stream >> result;
    _texcoord.push_back(result);
    break;
    }
  default:
    { // Any other vertex
    Vector3D result(0,0,0);
    stream >> result;
    _vertex.push_back(result);
    break;
    }
  }

} 

void VW::AWFObject::_GenerateEdges()
{
  const unsigned int num_faces = _face.size();
  //const unsigned int num_faces = 5;
  _edge.clear();
  _edge.reserve(num_faces*2);

  cout << "Generating edges:" << endl;
  const double scale = 100.0/double(num_faces);
  int last_update = 0;
  for (unsigned int face_index=0;face_index<num_faces;face_index++) {
    const AWFFace& f = _face[face_index];
    const unsigned int num_vert = f._vindex.size();
    
    for (unsigned int i=0;i<num_vert;i++) {
      int start_index = f._vindex[i];
      int end_index = f._vindex[(i+1)%num_vert];
      
      // Scan to see if already exists
      bool exists = false;
      unsigned int e=0;
      for (e=0;e<_edge.size();e++) {
	if (_edge[e].IsMe(start_index,end_index)) {
	  exists = true;
	  break;
	}
      }
      if (exists) {
	_edge[e].AddFace(face_index);
      } else {
	AWFEdge newedge(start_index, end_index);
	newedge.AddFace(face_index);
	_edge.push_back(newedge);
      }
    }

    // Show a status update
    const int current_update = int(face_index*scale);
    if ((current_update-last_update)>=5) {
      cout << "   " << current_update << "%" << endl;
      last_update = current_update;
    }
  }
  for (unsigned int i=0;i<_edge.size();i++) {
    if (!_edge[i].HasFaces()) {
      cout << "Edge "<<i<<" isn't complete." << endl;
    }
  }

}


void VW::AWFObject::Draw() const
{
  assert(_initialised);
  glCallList(_list_id);
}



void VW::AWFObject::Draw(VW::ThreeDToolGLCommon& display,
			 const int name) const
{
  if (name>0) glLoadName(name);
  Draw();

  // And add the bounding box to make axes scale properly
  display.IncludeInRotationCentre(Vector3D(_minx,_miny,_minz));
  display.IncludeInRotationCentre(Vector3D(_maxx,_maxy,_maxz));

  if (name>0) glLoadName(0);    
}



void VW::AWFObject::_GenList()
{
  assert(_initialised);

  if (_list_id==-1) _list_id = glGenLists(1);

  glNewList(_list_id,GL_COMPILE);

  if (VERBOSE) 
    cout << "VW::AWFObject::_GenList..." << endl;

  // Save all attributes for later
  glPushAttrib(GL_CURRENT_BIT);

  std::vector<AWFFace>::const_iterator f = _face.begin(), fe = _face.end();
  // Go through each face
  int current_material = -1;
  for (; f != fe ; f++) {
    const unsigned int nv = f->_vindex.size();

    if (VERBOSE) cout << "   Draw a face with "<<nv<<" vertices"<<endl;

    const int this_material=f->_matindex;
    if (this_material != current_material) {
      _material[this_material].SetAsCurrent();
      current_material = this_material;
    }

    if (nv==3) {
      glBegin(GL_TRIANGLES);
    } else if (nv==4) {
      glBegin(GL_QUADS);
    } else {
      glBegin(GL_POLYGON);
    }

    switch (f->_type) {
    case VertexOnly: {
      if (VERBOSE) cout << "      vertex only" << endl;
      // Normal is same for all vertices
      glNormal3f(f->_overall_normal.GetX(),
 		 f->_overall_normal.GetY(),
 		 f->_overall_normal.GetZ());
      // Go through each face
      for (unsigned int i=0;i<nv;i++) {
	const Vector3D& v = _vertex[f->_vindex[i]];
	glVertex3f(v.GetX(), v.GetY(), v.GetZ());
      }  
      break;
    }

    case WithTexture: {
      if (VERBOSE) cout << "      vertex with texture" << endl;
      // Normal is same for all vertices
      glNormal3f(f->_overall_normal.GetX(),
		 f->_overall_normal.GetY(),
		 f->_overall_normal.GetZ());
      // Go through each face
      for (unsigned int i=0;i<nv;i++) {
	//cout << "         " << i << endl;
	const Vector3D& v = _vertex[f->_vindex[i]];
	const Vector2D& t = _texcoord[f->_tindex[i]];
	glTexCoord2f(t.GetX(), t.GetY());
	glVertex3f(v.GetX(), v.GetY(), v.GetZ());
      }
      break;
    }

    case WithNormal: {
      if (VERBOSE) cout << "      vertex with normal" << endl;
      // Go through each face
      for (unsigned int i=0;i<nv;i++) {
	const Vector3D& v = _vertex[f->_vindex[i]];
	const Vector3D& n = _normal[f->_nindex[i]];
	glNormal3f(n._x, n._y, n._z);
	glVertex3f(v._x, v._y, v._z);
      }
      break;
    }

    case WithTextureAndNormal: {
      if (VERBOSE) cout << "      vertex with texture and normal" << endl;
      // Go through each face
      for (unsigned int i=0;i<nv;i++) {
	const Vector3D& v = _vertex[f->_vindex[i]];
	const Vector3D& n = _normal[f->_nindex[i]];
	const Vector2D& t = _texcoord[f->_tindex[i]];
	glTexCoord2f(t.GetX(), t.GetY());
	glNormal3f(n.GetX(), n.GetY(), n.GetZ());
	glVertex3f(v.GetX(), v.GetY(), v.GetZ());
      }
      break;
   }


    default:
      if (VERBOSE) cerr << "Unrecognized face type - ignoring" << endl;
    }
    glEnd();
  }

  // Now also add edges
  if (_edge.size()>0) {
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
    glColor3f(1.0,1.0,1.0);
    glLineWidth(2);
    for (unsigned int i=0;i<_edge.size();i++) {
      _edge[i].Draw(_vertex);
    }
  }

  glPopAttrib();
  glEndList();

}


