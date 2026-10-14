/** 
 * @file   awfmaterial.h
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

#ifdef _WIN32
#include <windows.h> // required before including GL
#define GL_BGRA GL_RGBA // Hack as windows GL doesn't support little-endian bit ordering
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream> 
#include <fstream>
#include <sstream>
#include <VW/IOBasics/miscioutils.h> // For GetPath()
#include <VW/Image/imagergb.h>
#include "awfmaterial.h"


#define VERBOSE false

using namespace std;


// Get the first power of 2 greater than or equal to number
static inline unsigned int GetNextPowerOf2(const unsigned int number)
{
  // Note that the iterative version (below) is *MUCH* faster than
  // taking logs for small powers of 2 (ie. sizes and the like). For
  // a typical number like 4000 -> 4096, it's about 10x faster.


  // Log version
  //const double lognum = log(double(w+i*2)) / log(2.0);
  //result = int (pow(2.0, ceil(lognum)));


  // Iterative version
  unsigned int result=1;
  unsigned int testnum=2;
  while (testnum<number) {
    testnum *= 2;
    result++;
  }
  return testnum;
}

static int LoadTexture(const std::string& filename)
{
  if (VERBOSE) cerr << "Reading texture map \"" << filename << "\""<<endl;
  VW::ImageRGB<unsigned char> image;

  if (!image.ReadImage(filename)){
    cerr << "Unable to open image file \"" << filename << "\""<<endl;
    exit(0);
  }

  // OpenGL wants the image to be upside down!
  image.FlipVertical();

  const unsigned int w=image.GetWidth(), h=image.GetHeight();
  char* imptr = static_cast<char*>(image.GetRawBuffer());


  if (VERBOSE) cout << "Hurrah! got image OK, now load into texture mem" << endl;
  const unsigned int texture_width = GetNextPowerOf2(w);
  const unsigned int texture_height = GetNextPowerOf2(h);

  if (VERBOSE) cout << "image size ("<<w<<"x"<<h
       << "), texture size (" << texture_width << "x" << texture_height 
       << ")" << endl;
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


    if (VERBOSE) cout << "Set GL texture" << endl;
  GLuint texname;
  glGenTextures(1, &texname);
  glBindTexture(GL_TEXTURE_2D, texname);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  if ((w != texture_width) || (h != texture_height)) {
    if (VERBOSE) cout << "Size mismatch, so scale" << endl;
    if (VERBOSE) cout << "   Alloc texture mem" << endl;
    GLubyte *dummy_texture = new GLubyte [texture_width * texture_height * 4];
    if (VERBOSE) cout << "   Scale image" << endl;
    gluScaleImage(GL_BGRA, 
		  w, h, GL_UNSIGNED_BYTE, imptr,
		  texture_width, texture_height, GL_UNSIGNED_BYTE, dummy_texture);
    
    if (VERBOSE) cout << "   Alloc texture" << endl;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 
		 0, GL_BGRA, GL_UNSIGNED_BYTE, dummy_texture);
    if (VERBOSE) cout << "   Clean up" << endl;
    delete dummy_texture;
  } else {
    if (VERBOSE) cout << "Sizes same, so no scaling. Alloc texture" << endl;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 
		 0, GL_BGRA, GL_UNSIGNED_BYTE, imptr);
  }

 
    if (VERBOSE) cout << "   Done" << endl;
   return texname;


}





















VW::AWFMaterial::AWFMaterial(const std::string& name, bool is_default)
  : _my_name(name)
{
  Reset();
  // Reset will set _isdefault to false, so set it back!
  _isdefault = is_default;
}

VW::AWFMaterial::AWFMaterial(const AWFMaterial& m)
  : _my_name(m._my_name),
    _got_ka(m._got_ka),_got_kd(m._got_kd),
    _got_ks(m._got_ks),_got_ke(m._got_ke),
    _alpha(m._alpha),_shininess(m._shininess),
    _illum(m._illum),
    _texturename(m._texturename),
    _texid(m._texid),
    _has_texture(m._has_texture),
    _used(m._used),
    _isdefault(m._isdefault)
{
    _Kd[0] = m._Kd[0]; _Kd[1] = m._Kd[1]; _Kd[2] = m._Kd[2];
    _Ka[0] = m._Ka[0]; _Ka[1] = m._Ka[1]; _Ka[2] = m._Ka[2];
    _Ks[0] = m._Ks[0]; _Ks[1] = m._Ks[1]; _Ks[2] = m._Ks[2];
    _Ke[0] = m._Ke[0]; _Ke[1] = m._Ke[1]; _Ke[2] = m._Ke[2];
}

void VW::AWFMaterial::Reset()
{
  _isdefault = false;
  _got_ka = false;
  _got_kd = false;
  _got_ks = false;
  _got_ke = false;
  _alpha = 1.0;
  _shininess = 65.0;
  _illum = DiffuseMaterial;
  _has_texture = false;
  _used = false;
  _Ka[0] = 0.2f; _Ka[1] = 0.2f; _Ka[2] = 0.2f;
  _Kd[0] = 0.8f; _Kd[1] = 0.8f; _Kd[2] = 0.8f;
  _Ks[0] = 1.0f; _Ks[1] = 1.0f; _Ks[2] = 1.0f;
  _Ke[0] = 0.0f; _Ke[1] = 0.0f; _Ke[2] = 0.0f;
}

bool VW::AWFMaterial::Check()
{
  bool result=true;



  switch(_illum) {
  case 0: {
    // Illumination off - only kd must be supplied, others are set to kd
    if (!_got_kd) result=false;
    if (!_got_ks) {
      _Ks[0] = _Kd[0]; _Ks[1] = _Kd[1]; _Ks[2] = _Kd[2];
    }
    if (!_got_ka) {
      _Ka[0] = _Kd[0]; _Ka[1] = _Kd[1]; _Ka[2] = _Kd[2];
    }
    break;
  }
  case 2:
    if (!_got_ks) result=false;
  case 1:
    // Illumination on - must have at least Ka and Kd
    if ((!_got_ka) || (!_got_kd)) result=false;
    if (!_got_ks) {
      _Ks[0] = _Kd[0]; _Ks[1] = _Kd[1]; _Ks[2] = _Kd[2];
    }
    break;
  default:
    cerr << "VW::AWFMaterial::Check: Illegal illumination type ("<<_illum<<")"<<endl;
    result=false;
  };

  // If there's a Tr value, check it's in the range 0<Tr<1
  if ((_alpha<0) || (_alpha>1)) result=false;
  // Shininess goes from 0 to 128
  if ((_shininess<0) || (_shininess>128)) result=false;

  if (VERBOSE) {
  cout << "Material \"" << _my_name << "\"" << endl;
  cout << "   illum=["<<_illum<<"]"<<endl;
  cout << "   Kd("<<_got_kd<<")=["<<_Kd[0]<<","<<_Kd[1]<<","<<_Kd[2]<<"]"<<endl;
  cout << "   Ka("<<_got_ka<<")=["<<_Ka[0]<<","<<_Ka[1]<<","<<_Ka[2]<<"]"<<endl;
  cout << "   Ks("<<_got_ks<<")=["<<_Ks[0]<<","<<_Ks[1]<<","<<_Ks[2]<<"]"<<endl;
  cout << "   Ke("<<_got_ke<<")=["<<_Ke[0]<<","<<_Ke[1]<<","<<_Ke[2]<<"]"<<endl;
  cout << "   alpha=["<<_alpha<<"]"<<endl;
  cout << "   shine=["<<_shininess<<"]"<<endl;
  }

  return result;
}


void VW::AWFMaterial::SetAsCurrent() const
{
  if (VERBOSE) cout << "VW::AWFMaterial::SetAsCurrent(" << _my_name << ")" << endl;
  glDisable(GL_COLOR_MATERIAL);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, _Kd);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, _Ka);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, _Ks);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &_shininess);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, _Ke);
  if (_has_texture) {
    if (VERBOSE) cout << "bind texture " << _texid << endl;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,_texid);
    // A hack to make the textures lighter
//     glEnable(GL_COLOR_MATERIAL);
//      glColor4f(1.0,1.0,1.0,0.0);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  } else {
    glDisable(GL_TEXTURE_2D);
  }
}

bool VW::AWFMaterial::IsMe(const std::string& name) const
{
  return (name == _my_name);
}

void VW::AWFMaterial::SetUsed(bool newval) 
{
  // If this is a change to being used, better init those textures and maps
  if ((newval==true) && (_used==false)) {
    if (_has_texture) {
      _texid = LoadTexture(_texturename);
    }
  }
  _used = newval;
}

std::ostream& VW::operator<<(std::ostream& s, const VW::AWFMaterial& m)
{
  s << "newmtl " << m._my_name << endl;
  s << "   illum " << m._illum << endl;

  if (m._got_kd) 
    s << "   Kd \t"<<m._Kd[0]<<" \t"<<m._Kd[1]<<" \t"<<m._Kd[2]<<endl;

  if (m._got_ka) 
    s << "   Ka \t"<<m._Ka[0]<<" \t"<<m._Ka[1]<<" \t"<<m._Ka[2]<<endl;

  if (m._got_ks) 
    s << "   Ks \t"<<m._Ks[0]<<" \t"<<m._Ks[1]<<" \t"<<m._Ks[2]<<endl;

  if (m._got_ke) 
    s << "   Ke \t"<<m._Ke[0]<<" \t"<<m._Ke[1]<<" \t"<<m._Ke[2]<<endl;

  s << "   Tr \t"<<m._alpha<<endl;
  s << "   Ns \t"<<m._shininess<<endl;

  if (m._has_texture) {
    cerr << "Map="<<VW::StripPath(m._texturename)<<endl;
    s << "   map_Kd \t" << VW::StripPath(m._texturename) << endl;
  }
  return s;
}

std::string VW::AWFMaterial::GetVRMLString() const
{
  std::ostringstream s;
  s << "        material DEF Mat" << _my_name << " Material {" << endl;
  if (_got_kd) 
    s << "          diffuseColor "<<_Kd[0]<<" "<<_Kd[1]<<" "<<_Kd[2] << endl;

  if (_got_ka) 
    s << "#          ambientColor "<<_Ka[0]<<" "<<_Ka[1]<<" "<<_Ka[2] << endl;

  if (_got_ks) 
    s << "          specularColor "<<_Ks[0]<<" "<<_Ks[1]<<" "<<_Ks[2] << endl;

  if (_got_ke) 
    s << "          emissiveColor "<<_Ke[0]<<" "<<_Ke[1]<<" "<<_Ke[2] << endl;

  s << "          shininess "<<1.0 - _shininess/128.0<< endl;
  s << "          transparency "<<1.0-_alpha<< endl;

  if (_has_texture) {
    cerr << "Map="<<VW::StripPath(_texturename)<<endl;
    s << "          textureMap" << VW::StripPath(_texturename) << endl;
  }
  s << "        }\n";
  return s.str();
}

void VW::AWFMaterial::_ReadMapLine(const std::string& line,
				   const std::string& current_path)
{
  istringstream stream(line);

  string command;
  stream >> command;

  if (command.substr(0,4)=="map_") {
    if (VERBOSE) cout << "Found a map" << endl;
    // A Texture map of some kind:
    //  _kx is texture (x=a,d,s)
    //  _bump is a bump map
    //  _refl is a reflection map
    switch(command[4]) {
    case 'k':
    case 'K': {
      char texturetype=command[5];
      {
	string imagefilename;
	stream >> imagefilename;
	_texturename = current_path + imagefilename;
      }
      switch(texturetype) {
      case 'a': {
	cerr << "map_ka \""<<_texturename<<"\""<<endl;
	cerr << "   Ambient maps not yet supported (how to do them in GL?)"<<endl;
	break;
      }
      case 'd': {
	if (VERBOSE) cerr << "map_kd \""<<_texturename<<"\""<<endl;
	_has_texture = true;
	break;
      }
      case 's': {
	cerr << "map_ks \""<<_texturename<<"\""<<endl;
	cerr << "   Specular maps not yet supported (how to do them in GL?)"<<endl;
	break;
      }
      default:
	cerr << "map_k"<<texturetype<<" not recognised!"<<endl;
	break;
      } // End switch texture_type;
      break;
    }
    case 'b': {
      cerr << "Bump maps not supported yet" << endl;
      break;
    }
    case 'r': {
      cerr << "Reflection maps not supported yet" << endl;
      break;
    }
    default:
      cerr << command <<"' not recognised!"<<endl;
      break;
    }
  } else {
    cerr << command <<" not recognised!"<<endl;
  }
}

void VW::ReadAWFMaterialsFile(const std::string& filename,
			      std::vector<VW::AWFMaterial>& materials)
{
  string path = VW::GetFilePath(filename);


  fstream file(filename.c_str());
  if (!file) {
    cerr << "AWFObject::ReadMaterialsFile: unable to read file \""<<filename<<"\""<<endl;
  } else {
    cerr << "ReadMaterialsFile: reading from file \""<<filename<<"\""<<endl;
    char buff[1024];
    int matindex = -1; // Matindex is negative when no material is current
    while (file.getline(buff,1024)) {
      if (strlen(buff)>0) {
	istringstream stream(buff);
	string command; stream >> command;
	if (VERBOSE) cout << "Command='"<<command<<"'"<<endl;
	if (command=="newmtl") {
	  string matname;
	  stream >> matname;
	  if (VERBOSE) cout << "New material (\"" << matname << "\")" << endl;

	  // Need to check if already on list and ignore or replace
	  bool exists = false;
	  for (unsigned int i=0;i<materials.size();i++) {
	    if (materials[i].IsMe(matname)) {
	      // Found an existing entry. If it's a default one we should replace it,
	      // but if not then this material is not required
	      if (materials[i].IsDefault()) {
		cout << "   Replace default material \""<<matname<<"\"" << endl;
		materials[i].Reset();
		matindex = i;
	      } else {
		cout << "   Material \""<<matname<<"\" already exists" << endl;
		matindex = -1;
	      }
	      exists = true;
	      break;
	    }
	  }
	  if (!exists) {
	    AWFMaterial m(matname);
	    materials.push_back(m);
	    matindex = materials.size() - 1;
	  }


	} else {
	  char c=command[0];
	  char d=command[1];
	  switch(c) {
	  case '#': {
	    // Comment
	    if (VERBOSE) cout << "Comment" << endl;
	    break;
	  }
	  case 'N': {
	    // must be followed by 's'
	    if (d=='s') {
	      // Shininess
	      if (matindex >= 0) {
		stream >> materials[matindex]._shininess;
		// AWF shininess goes from 0-1000, GL from 0-128
		materials[matindex]._shininess *= 0.128f;
	      }
	    } else {
	      cerr << "N"<<c<<" not recognised!"<<endl;
	    }
	    break;
	  }
	  case 'T': {
	    // must be followed by 'r'
	    if (d=='r') {
	      // Transparency
	      if (matindex >= 0)
		stream >> materials[matindex]._alpha;
	    } else {
	      cerr << "T"<<c<<" not recognised!"<<endl;
	    }
	    break;
	  }
	  case 'd': {
	    // Transparency (old format)
	    if (matindex >= 0)
	      stream >> materials[matindex]._alpha;
	    break;
	  }
	  case 'i': {
	    // must be followed by 'llum'
	    if (command=="illum") {
	      // Illumination type
	      if (matindex >= 0) {
		int i=0;
		stream >> i; 
		switch (i) {
		default:
		  cerr << "Illegal material type '" << i << "'" << endl;
		case 0:
		  materials[matindex]._illum = DiffuseMaterial;
		  break;
		case 1:
		  materials[matindex]._illum = AmbientMaterial;
		  break;
		case 2:
		  materials[matindex]._illum = SpecularMaterial;
		  break;
		}
	      }
	    } else {
	      cerr << command<<" not recognised!"<<endl;
	    }
	    break;
	  }
	  case 'm': {
	    // Probably a map, so pass to material for reading
	    if (matindex >= 0)
	      materials[matindex]._ReadMapLine(buff,path);
	    break;
	  }
	  case 'K': {
	    // A colour command
	    switch(d) {
	    case 'a': { // ambient
	      if (matindex >= 0) {
		stream >> materials[matindex]._Ka[0];
		stream >> materials[matindex]._Ka[1];
		stream >> materials[matindex]._Ka[2];
		materials[matindex]._got_ka = true;
	      }
	      break;
	    }
	    case 'd': { // diffuse
	      if (matindex >= 0) {
	      stream >> materials[matindex]._Kd[0];
	      stream >> materials[matindex]._Kd[1];
	      stream >> materials[matindex]._Kd[2];
	      materials[matindex]._got_kd = true;
	      }
	      break;
	    }
	    case 's': { //specular
	      if (matindex >= 0) {
	      stream >> materials[matindex]._Ks[0];
	      stream >> materials[matindex]._Ks[1];
	      stream >> materials[matindex]._Ks[2];
	      materials[matindex]._got_ks = true;
	      }
	      break;
	    };
	    default:
	      cerr << "Unknown colour specifier." << endl;
	      break;
	    };
	    // Done with colour
	    break;
	  }
	  default:
	    //cerr << "Unrecognised denominator '" << c << "'" << endl;
	    break;
	  }      
	}
      }
      
    }
    file.close();
  }

  for (unsigned int i=0;i<materials.size();i++) {
    materials[i].Check();
  }

}


void VW::AddDefaultAWFMaterials(std::vector<AWFMaterial>& materials)
{
  { // Default
    AWFMaterial m("default",true);
    m.SetAmbientColour (0.2f,0.2f,0.2f);
    m.SetDiffuseColour (0.8f,0.8f,0.8f);
    m.SetSpecularColour(1.0f,1.0f,1.0f);
    m.SetType(DiffuseMaterial);
    materials.push_back(m);
  }


  // GL Precious
  {
    AWFMaterial m("emerald",true);
    m.SetAmbientColour(  0.0215f,  0.1745f,   0.0215f);
    m.SetDiffuseColour(  0.07568f, 0.61424f,  0.07568f);
    m.SetSpecularColour( 0.633f,   0.727811f, 0.633f);
    m.SetShininess(0.6f);
    m.SetType(SpecularMaterial);
    materials.push_back(m);
  }
  { // jade
    AWFMaterial m("jade",true);
    m.SetAmbientColour(  0.135,    0.2225,   0.1575);
    m.SetDiffuseColour(  0.54,     0.89,     0.63);
    m.SetSpecularColour( 0.316228, 0.316228, 0.316228);
    m.SetShininess(0.1f);
    m.SetType(SpecularMaterial);
    materials.push_back(m);
  }
  { // 
    AWFMaterial m("obsidian",true);
    m.SetAmbientColour(  0.05375,  0.05,     0.06625);
    m.SetDiffuseColour(  0.18275,  0.17,     0.22525);
    m.SetSpecularColour( 0.332741, 0.328634, 0.346435);
    m.SetShininess(0.3f);
    m.SetType(SpecularMaterial);
    materials.push_back(m);
  }
  { // 
    AWFMaterial m("pearl",true);
    m.SetAmbientColour(  0.25,     0.20725,  0.20725);
    m.SetDiffuseColour(  1,        0.829,    0.829);
    m.SetSpecularColour( 0.296648, 0.296648, 0.296648);
    m.SetShininess(0.088);
    m.SetType(SpecularMaterial);
    materials.push_back(m);
  }
  { // 
    AWFMaterial m("ruby",true);
    m.SetAmbientColour(  0.1745,   0.01175,  0.01175);
    m.SetDiffuseColour(  0.61424,  0.04136,  0.04136);
    m.SetSpecularColour( 0.727811, 0.626959, 0.626959);
    m.SetShininess(0.6);
    m.SetType(SpecularMaterial);
    materials.push_back(m);
  }
  { // 
    AWFMaterial m("turquoise",true);
    m.SetAmbientColour(  0.1,      0.18725, 0.1745);
    m.SetDiffuseColour(  0.396,    0.74151, 0.69102);
    m.SetSpecularColour( 0.297254, 0.30829, 0.306678);
    m.SetShininess(0.1);
    m.SetType(SpecularMaterial);
    materials.push_back(m);
  }


  // GL Metals
  { // 
    AWFMaterial m("brass",true);
    m.SetAmbientColour(  0.329412, 0.223529, 0.027451 );
    m.SetDiffuseColour(  0.780392, 0.568627, 0.113725 );
    m.SetSpecularColour( 0.992157, 0.941176, 0.807843 );
    m.SetShininess(0.21794872);
    m.SetType(SpecularMaterial);
    materials.push_back(m);
  }
  { // 
    AWFMaterial m("bronze",true);
    m.SetAmbientColour(  0.2125,   0.1275,   0.054 );
    m.SetDiffuseColour(  0.714,    0.4284,   0.18144 );
    m.SetSpecularColour( 0.393548, 0.271906, 0.166721);
    m.SetShininess(0.2);
    m.SetType(SpecularMaterial);
    materials.push_back(m);
  }
  { // 
    AWFMaterial m("chrome",true);
    m.SetAmbientColour(  0.25,     0.25,     0.25 );
    m.SetDiffuseColour(  0.4,      0.4,      0.4 );
    m.SetSpecularColour( 0.774597, 0.774597, 0.774597);
    m.SetShininess(0.6);
    m.SetType(SpecularMaterial);
    materials.push_back(m);
  }
  { // 
    AWFMaterial m("copper",true);
    m.SetAmbientColour(  0.19125,  0.0735,   0.0225 );
    m.SetDiffuseColour(  0.7038,   0.27048,  0.0828 );
    m.SetSpecularColour( 0.256777, 0.137622, 0.086014 );
    m.SetShininess(0.1);
    m.SetType(SpecularMaterial);
    materials.push_back(m);
  }
  { // 
    AWFMaterial m("gold",true);
    m.SetAmbientColour(  0.24725,  0.1995,   0.0745 );
    m.SetDiffuseColour(  0.75164,  0.60648,  0.22648 );
    m.SetSpecularColour( 0.628281, 0.555802, 0.366065);
    m.SetShininess(0.4);
    m.SetType(SpecularMaterial);
    materials.push_back(m);
  }
  { // 
    AWFMaterial m("silver",true);
    m.SetAmbientColour(  0.19225,  0.19225,  0.19225 );
    m.SetDiffuseColour(  0.50754,  0.50754,  0.50754 );
    m.SetSpecularColour( 0.508273, 0.508273, 0.508273);
    m.SetShininess(0.4);
    m.SetType(SpecularMaterial);
    materials.push_back(m);
  }
}
