/** 
 * @file   normalisation3d.h
 * @author BJ Tordoff
 * @date   15/8/2003		   		
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
 * @class    VW::Normalisation3D
 * @ingroup  GeometricObjects
 * @author   BJ Tordoff
 * @date     15/8/2003		   		
 * @brief    A 3D normalisation object.
 *
 * This class is designed for normalising points and lines (segments) in 3D
 * euclidean space, such as must be done before calculating the various 
 * 3D transforms and 3D to 2D projections. A
 * variety of normalisations are possible, but the most common and simplest
 * is to simply provide a single distance which is assumed to be the dimension
 * of a cube containing all points.
 *
 * X, Y and Z are scaled equally.  Doing otherwise complicates things and 
 * is rarely needed.
 */


#ifndef _normalisation3d_h_
#define _normalisation3d_h_

#include <vector>

#include <VNL/matrixfixed.h>

#include "point3d.h"
#include "operations.h"

namespace VW
{



class Normalisation3D
{
public:
  // Constructors

  /** Construct and set to identity. */
  Normalisation3D ();

  /** Construct using a single dimension or
   * also specify height and depth when they are different. 
   * The scaling is then isotropic around the cuboid centre.
   */
  Normalisation3D (double width, double height = 0, double depth = 0);

  /** Construct and set to isotropic scaling around given location. */
  Normalisation3D (double scale, const VW::Point3D& centre);

  /** Construct from an existing 4x4 matrix. */
  Normalisation3D (const VNL::MatrixFixed<4,4,double>& C);

  /** Constaruct from an existing floating point matrix. */
  Normalisation3D (const VNL::MatrixFixed<4,4,float>& C);

  /** Construct based on mean and standard deviation of point feature list. */
  Normalisation3D (const std::vector<VW::Point3D>& feature_list);

  /** Construct based on mean and standard deviation of selected elements from a point feature list. */
  Normalisation3D (const std::vector<VW::Point3D>& feature_list, 
		 const std::vector<unsigned int>& which_features);

  /** Construct (copy) from an existing normalisation. */
  Normalisation3D (const Normalisation3D& C);




  // Initialisation after construction
  void Init ();
  void Init (double width, double height = 0, double depth = 0);
  void Init (double scale, const VW::Point3D& centre);
  void Init (const VNL::MatrixFixed<4,4,double>& C);
  void Init (const VNL::MatrixFixed<4,4,float>& C);
  /** Initialise based on mean and standard deviation of selected elements from a point feature list. */
  template <class PointFeature>
  void Init (const std::vector<PointFeature>& feature_list);
  /** Initialise based on mean and standard deviation of selected elements from a point feature list. */
  template <class PointFeature>
  void Init (const std::vector<PointFeature>& feature_list, 
		 const std::vector<unsigned int>& which_features);




  // Normalisation
  template <class PointFeature>
    void Normalise (std::vector<PointFeature>& feature_list, 
		    VW::Point3D point = VW::Point3D()) const;

  template <class PointFeature>
  PointFeature Normalise (const PointFeature& feature, 
			  VW::Point3D point = VW::Point3D()) const;

  /** Normalise a 3x3 double precision matrix. */
  VNL::MatrixFixed<4,4,double> Normalise (const VNL::MatrixFixed<4,4,double>& M) const;

  /** Normalise a 3x3 floating point matrix. */
  VNL::MatrixFixed<4,4,float> Normalise (const VNL::MatrixFixed<4,4,float>& M) const;




  // Unnormalisation
  template <class PointFeature>
  void UnNormalise (std::vector<PointFeature>& feature_list, 
		    VW::Point3D point = VW::Point3D()) const;

  template <class PointFeature>
  PointFeature UnNormalise (const PointFeature& feature, 
			    VW::Point3D point = VW::Point3D()) const;

  VNL::MatrixFixed<4,4,double> UnNormalise (const VNL::MatrixFixed<4,4,double>& M) const;

  VNL::MatrixFixed<4,4,float> UnNormalise (const VNL::MatrixFixed<4,4,float>& M) const;

  // Data access
  VNL::MatrixFixed<4,4,double> operator() () const {return AsMatrix ();}
  VNL::MatrixFixed<4,4,double> AsMatrix () const;
 VNL::MatrixFixed<4,4,double> InverseMatrix () const;
  double GetScale () const {return _scale;}
  double GetCentreX () const {return _centre_x;}
  double GetCentreY () const {return _centre_y;}
  double GetCentreZ () const {return _centre_z;}
  void GetCentre (double& x, double& y, double& z) const {x = _centre_x; y = _centre_y; z = _centre_z; return;}
  bool IsIdentity () const {return (_scale == 1.0 && _centre_x == 0.0 && _centre_y == 0.0 && _centre_z == 0.0);}

protected:
  // Data
  double _scale;
  double _centre_x;
  double _centre_y;
  double _centre_z;

}; // end of class Normalisation












// Inlines

inline Normalisation3D::Normalisation3D () :
  _scale (1.0),
  _centre_x (0.0),
  _centre_y (0.0),
  _centre_z (0.0)
{}


inline Normalisation3D::Normalisation3D (double width, double height, double depth)
{
  Init(width, height, depth);
}


inline Normalisation3D::Normalisation3D (double scale, const VW::Point3D& centre) :
  _scale (scale),
  _centre_x (centre.GetX()),
  _centre_y (centre.GetY()),
  _centre_z (centre.GetZ())
{}


inline Normalisation3D::Normalisation3D (const Normalisation3D& C) :
  _scale (C._scale),
  _centre_x (C._centre_x),
  _centre_y (C._centre_y),
  _centre_z (C._centre_z)
{}


inline Normalisation3D::Normalisation3D (const VNL::MatrixFixed<4,4,float>& C) :
  _scale ((0.33333 * (C(0,0)+C(1,1)+C(2,2))) / C(3,3)),
  _centre_x (-C(0,3)/_scale),
  _centre_y (-C(1,3)/_scale),
  _centre_z (-C(2,3)/_scale)
{}


inline Normalisation3D::Normalisation3D (const VNL::MatrixFixed<4,4,double>& C) :
  _scale ((0.33333 * (C(0,0)+C(1,1)+C(2,2))) / C(3,3)),
  _centre_x (-C(0,3)/_scale),
  _centre_y (-C(1,3)/_scale),
  _centre_z (-C(2,3)/_scale)
{}


inline Normalisation3D::Normalisation3D (const std::vector<VW::Point3D>& feature_list)
{
  Init(feature_list);
}


inline Normalisation3D::Normalisation3D (const std::vector<VW::Point3D>& feature_list,
				     const std::vector<unsigned int>& which_features)
{
  Init(feature_list,which_features);
}



inline void Normalisation3D::Init ()
{
  _scale = 1.0;
  _centre_x = 0.0;
  _centre_y = 0.0;
  _centre_z = 0.0;
}


inline void Normalisation3D::Init (double width, double height, double depth)
{
  _scale = 6/(width + height + depth);
  _centre_x = width/2;

  if (height>0) 
    _centre_y = height/2;
  else 
    _centre_y = _centre_x;

  if (depth>0) 
    _centre_z = depth/2;
  else 
    _centre_z = _centre_x;
}


inline void Normalisation3D::Init (double scale, const VW::Point3D& centre)
{
  _scale = scale;
  _centre_x = centre.GetX();
  _centre_y = centre.GetY();
  _centre_z = centre.GetZ();
}


inline void Normalisation3D::Init (const VNL::MatrixFixed<4,4,float>& C)
{
  _scale = (0.33333 * (C(0,0)+C(1,1)+C(2,2))) / C(3,3);
  _centre_x = -C(0,3)/_scale;
  _centre_y = -C(1,3)/_scale;
  _centre_z = -C(2,3)/_scale;
}


inline void Normalisation3D::Init (const VNL::MatrixFixed<4,4,double>& C)
{
  _scale = (0.33333 * (C(0,0)+C(1,1)+C(2,2))) / C(3,3);
  _centre_x = -C(0,3)/_scale;
  _centre_y = -C(1,3)/_scale;
  _centre_z = -C(2,3)/_scale;
}


template <class PointFeature>
inline void Normalisation3D::Init (const std::vector<PointFeature>& feature_list)
{
  double sd;
  VW::GetPointDistribution (feature_list, _centre_x, _centre_y, _centre_z, sd);

  _scale = 1 / (1.414213562*sd);
}


template <class PointFeature>
inline void Normalisation3D::Init (const std::vector<PointFeature>& feature_list,
				   const std::vector<unsigned int>& which_features)
{
  double sd;
  VW::GetPointDistribution (feature_list, which_features, 
			    _centre_x, _centre_y, _centre_z, sd);

  _scale = 1 / (1.414213562*sd);
}



} // end of namespace VW

// Exported implementations/specialisations.  Move to cc file when keyword export is implemented
inline VNL::MatrixFixed<4,4,double> 
VW::Normalisation3D::Normalise (const VNL::MatrixFixed<4,4,double>& M) const 
{
  return this->AsMatrix() * M;
}


template <class PointFeature>
void VW::Normalisation3D::Normalise (std::vector<PointFeature>& feature_list, 
				     VW::Point3D point) const
{
  if (this->IsIdentity()) return;
  for (typename std::vector<PointFeature>::iterator it = feature_list.begin();
       it != feature_list.end(); it++) {
    *it = Normalise (*it, *it);
  }
}


template <class PointFeature>
PointFeature VW::Normalisation3D::Normalise (const PointFeature& feature, 
					     VW::Point3D point) const
{
  PointFeature pout = feature;
  pout.SetX (((feature.GetX() - _centre_x) * _scale));
  pout.SetY (((feature.GetY() - _centre_y) * _scale));
  pout.SetZ (((feature.GetZ() - _centre_z) * _scale));
  return pout;
}



template <class PointFeature>
void VW::Normalisation3D::UnNormalise (std::vector<PointFeature>& feature_list,
				       VW::Point3D point) const
{
  if (this->IsIdentity()) return;
  for (typename std::vector<PointFeature>::iterator it = feature_list.begin();
       it != feature_list.end(); it++) {
    *it = UnNormalise (*it, *it);
  }
}




template <class PointFeature> PointFeature 
VW::Normalisation3D::UnNormalise (const PointFeature& feature, VW::Point3D point) const
{
  PointFeature pout = feature;
  pout.SetX (((feature.GetX() / _scale) + _centre_x));
  pout.SetY (((feature.GetY() / _scale) + _centre_y));
  pout.SetZ (((feature.GetZ() / _scale) + _centre_z));
  return pout;
}


/**
 * @param M the matrix to be unnormalised (will not be changed)
 * @returns a new matrix which is un-normalised
 */
inline VNL::MatrixFixed<4,4,double> 
VW::Normalisation3D::UnNormalise (const VNL::MatrixFixed<4,4,double>& M) const
{
    return this->InverseMatrix() * M;
}


#endif // #ifndef _normalisation_h_
