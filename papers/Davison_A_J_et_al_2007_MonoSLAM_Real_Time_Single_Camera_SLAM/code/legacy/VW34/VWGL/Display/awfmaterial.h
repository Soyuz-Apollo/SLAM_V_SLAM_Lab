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
/** 
 * @class VW::AWFMaterial
 * @author BJ Tordoff
 * @date 13/01/2004
 * @brief Class to store material properties for Alias WaveFront (AWF) objects.
 */

#ifndef _vw_awfmaterial_h_
#define _vw_awfmaterial_h_

#include <vector>
#include <string>



namespace VW {

enum AWFMaterialType {
  DiffuseMaterial=0,
  AmbientMaterial,
  SpecularMaterial
};

// Forward declare friends
class AWFMaterial;
void ReadAWFMaterialsFile(const std::string& filename,
						std::vector<AWFMaterial>& materials);
std::ostream& operator<<(std::ostream& s, const AWFMaterial& m);

class AWFMaterial {

  // IO operations are my friends :-)
  friend void VW::ReadAWFMaterialsFile(const std::string& filename,
				    std::vector<AWFMaterial>& materials);
  friend std::ostream& VW::operator<<(std::ostream& s, const AWFMaterial& m);

 public:
  AWFMaterial(const std::string& name, bool is_default = false);
  AWFMaterial(const AWFMaterial& m);

  void SetType(AWFMaterialType type);
  void SetAmbientColour(float red, float green, float blue);
  void SetDiffuseColour(float red, float green, float blue);
  void SetSpecularColour(float red, float green, float blue);
  void SetEmissiveColour(float red, float green, float blue);
  void SetShininess(float shininess);

  void Reset();

  bool Check();

  bool IsMe(const std::string& name) const;

  /** Is this material part of the default set (true) or a loaded one (false). */
  bool IsDefault() const;
  std::string GetName() const;
  std::string GetVRMLString() const;

  // Indicate that this material is needed. This will cause any textures/maps
  // that have been deferred to be loaded.
  void SetUsed(bool newval);
  bool IsUsed() const {return(_used);}
  bool HasTexture() const {return(_has_texture);}
  std::string GetTextureFilename() const {return(_texturename);} 

  // Set the GL properties for this material
  void SetAsCurrent() const;

  void _ReadMapLine(const std::string& line,
		    const std::string& current_path);

protected: // all data public for now
  std::string _my_name;
  float _Ka[3]; // ambient
  float _Kd[3]; // diffuse
  float _Ks[3]; // specular
  float _Ke[3]; // emission
  bool _got_ka,_got_kd,_got_ks,_got_ke;
  float _alpha; // transparency
  float _shininess;
  AWFMaterialType _illum; // Illumination 0=off, 1=flat (no ks), 2=specular (with ks)
  std::string _texturename;
  int _texid;
  bool _has_texture;
  bool _used;
  bool _isdefault;
};


std::ostream& operator<<(std::ostream& s, const AWFMaterial& m);

/** Read a materials (.mtl) file and add the materials contained to 
 * the result vector. 
 */
void ReadAWFMaterialsFile(const std::string& filename,
			  std::vector<AWFMaterial>& materials);


  void AddDefaultAWFMaterials(std::vector<AWFMaterial>& materials);


}; // end namespace VW






// INLINES

inline std::string VW::AWFMaterial::GetName() const
{
  return _my_name;
}


inline void 
VW::AWFMaterial::SetType(AWFMaterialType new_type)
{
  _illum = new_type;
}


inline void 
VW::AWFMaterial::SetAmbientColour(float red, float green, float blue)
{
  _Ka[0] = red;
  _Ka[1] = green;
  _Ka[2] = blue;
  _got_ka = true;
}


inline void 
VW::AWFMaterial::SetDiffuseColour(float red, float green, float blue)
{
  _Kd[0] = red;
  _Kd[1] = green;
  _Kd[2] = blue;
  _got_kd = true;
}


inline void 
VW::AWFMaterial::SetSpecularColour(float red, float green, float blue)
{
  _Ks[0] = red;
  _Ks[1] = green;
  _Ks[2] = blue;
  _got_ks = true;
}


inline void 
VW::AWFMaterial::SetEmissiveColour(float red, float green, float blue)
{
  _Ke[0] = red;
  _Ke[1] = green;
  _Ke[2] = blue;
  _got_ke = true;
}

inline void 
VW::AWFMaterial::SetShininess(float shininess)
{
  _shininess = shininess;
}

inline bool 
VW::AWFMaterial::IsDefault() const
{
  return _isdefault;
}

#endif
