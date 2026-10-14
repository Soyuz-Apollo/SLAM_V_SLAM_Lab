/** 
 * @file   normalisation.h
 * @author JGH Knight
 * @date   02/11/2002		   		
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
 * @class    VW::Normalisation
 * @ingroup  GeometricObjects
 * @author   JGH Knight
 * @date     02/11/2002
 * @brief    A 2D normalisation object.
 *
 * This class is designed for normalising points and lines on the 2D plane,
 * such as must be done before calculating the various image transforms. A
 * variety of normalisations are possible, but the most common and simplest
 * is to simply provide the image width (assuming a 4x3 ratio image), giving
 * a normalisation to coords (-1<x<1, -0.75<y<0.75).
 *
 * X and Y are scaled equally.  Doing otherwise complicates things and is rarely
 * needed.
 */


#ifndef _normalisation_h_
#define _normalisation_h_

#include <vector>

#include <VNL/matrixfixed.h>

#include "point2d.h"
#include "line2d.h"
#include "operations.h"

namespace VW
{

  // Some classes we need to know about
  class CalibrationMatrix;


class Normalisation
{
public:
  // Constants and types
  static const float DEFAULT_WIDTH_HEIGHT_RATIO;

  // Constructors

  /** Construct and set to identity. */
  Normalisation ();

  /** Construct using an image width (assumes 4:3 width:height ratio). 
   * The scaling is then isotropic around the image centre.
   */
  Normalisation (double width);

  /** Construct using an image width and height. The scaling is then isotropic
   * around the image centre.
   */
  Normalisation (double width, double height);

  /** Construct and set to isotropic scaling around given location. */
  Normalisation (double scale, double centre_x, double centre_y);

  /** Construct from an existing 3x3 matrix. */
  Normalisation (const VNL::MatrixFixed<3,3,double>& C);

  /** Construct from an existing floating point matrix. */
  Normalisation (const VNL::MatrixFixed<3,3,float>& C);

  /** Construct based on mean and standard deviation of point feature list. */
  template <class PointFeature>
  Normalisation (const std::vector<PointFeature>& feature_list, 
		 VW::Point2D point = VW::Point2D())
  {
  Init(feature_list,VW::Point2D());
};

  /** Construct based on mean and standard deviation of selected elements from a point feature list. */
  template <class PointFeature>
  Normalisation (const std::vector<PointFeature>& feature_list, 
		 const std::vector<unsigned int>& which_features,
		 VW::Point2D point = VW::Point2D())
  {
  Init(feature_list,which_features,VW::Point2D());
}


  /** Construct based on mean and standard deviation of line feature list. */
  template <class LineFeature>
  Normalisation (const std::vector<LineFeature>& feature_list, 
		   VW::Line2D line)
  {
	  std::cerr << "Normalisation::Normalisation (line_list): not implemented" << std::endl;
  assert (false);
}

  /** Construct (copy) from an existing normalisation. */
  Normalisation (const Normalisation& C);




  // Initialisation after construction
  void Init ();
  void Init (double width, double height = 0);
  void Init (double scale, double centre_x, double centre_y);
  void Init (const VNL::MatrixFixed<3,3,double>& C);
  void Init (const VNL::MatrixFixed<3,3,float>& C);
  /** Initialise based on mean and standard deviation of selected elements from a point feature list. */
  template <class PointFeature>
  void Init (const std::vector<PointFeature>& feature_list, 
	     VW::Point2D point = VW::Point2D())
		 {
  double sd;
  VW::GetPointDistribution (feature_list, _centre_x, _centre_y, sd);

  _scale = 1 / (1.414213562*sd);
}
  /** Initialise based on mean and standard deviation of selected elements from a point feature list. */
  template <class PointFeature>
  void Init (const std::vector<PointFeature>& feature_list, 
		 const std::vector<unsigned int>& which_features,
	     VW::Point2D point = VW::Point2D())
		 {
  double sd;
  VW::GetPointDistribution (feature_list, which_features, 
				  _centre_x, _centre_y, sd);

  _scale = 1 / (1.414213562*sd);
}

  template <class LineFeature>
  void Init (const std::vector<LineFeature>& feature_list, 
	     VW::Line2D line)
		 {
			 std::cerr << "Normalisation::Init (line_list): not implemented" << std::endl;
  assert (false);
}





  // Normalisation

  // Image coords can be (un)normalised independently
  double NormaliseX(double x) const;
  double NormaliseY(double y) const;
  double UnNormaliseX(double x) const;
  double UnNormaliseY(double y) const;

  // Simple classes
  VW::Point2D           Normalise(const VW::Point2D& point) const;
  VW::Line2D            Normalise(const VW::Line2D& line) const;
  VW::LineSeg2D         Normalise(const VW::LineSeg2D& lineseg) const;
  VW::CalibrationMatrix Normalise(const VW::CalibrationMatrix& K) const;

  VW::Point2D           UnNormalise(const VW::Point2D& point) const;
  VW::Line2D            UnNormalise(const VW::Line2D& line) const;
  VW::LineSeg2D         UnNormalise(const VW::LineSeg2D& lineseg) const;
  VW::CalibrationMatrix UnNormalise(const VW::CalibrationMatrix& K) const;



  // Now Joss' fancy stuff
  template <class PointFeature>
    void Normalise (std::vector<PointFeature>& feature_list, 
		    VW::Point2D point = VW::Point2D()) const
			{
  if (this->IsIdentity()) return;
  for (typename std::vector<PointFeature>::iterator it = feature_list.begin();
       it != feature_list.end(); it++) {
    *it = Normalise (*it, *it);
  }
}

  template <class PointFeature>
  PointFeature Normalise (const PointFeature& feature, 
			  VW::Point2D point = VW::Point2D()) const
			  {
  PointFeature pout = feature;
  pout.SetX (((feature.GetX() - _centre_x) * _scale));
  pout.SetY (((feature.GetY() - _centre_y) * _scale));
  return pout;
}


  template <class LineFeature>
  void Normalise (std::vector<LineFeature>& feature_list, VW::Line2D line) const
    {
  if (this->IsIdentity()) return;
  for (typename std::vector<LineFeature>::iterator it = feature_list.begin();
       it != feature_list.end(); it++) {
    *it = Normalise (*it, *it);
  }
	}
  template <class LineFeature>
  LineFeature Normalise (const LineFeature& feature, VW::Line2D line) const
  {
  // Pre-multiply by inv(C)'
  LineFeature lout = feature;
  lout[2] += lout[0]*_centre_x + lout[1]*_centre_y;
  lout[0] /= _scale;
  lout[1] /= _scale;
  return lout;
}


  /** Normalise a 3x3 double precision matrix. */
  VNL::MatrixFixed<3,3,double> Normalise (const VNL::MatrixFixed<3,3,double>& M) const;

  /** Normalise a 3x3 floating point matrix. */
  VNL::MatrixFixed<3,3,float> Normalise (const VNL::MatrixFixed<3,3,float>& M) const;




  // Unnormalisation
  template <class PointFeature>
  void UnNormalise (std::vector<PointFeature>& feature_list, 
		    VW::Point2D point = VW::Point2D()) const
			{
  if (this->IsIdentity()) return;
  for (typename std::vector<PointFeature>::iterator it = feature_list.begin();
       it != feature_list.end(); it++) {
    *it = UnNormalise (*it, *it);
  }
}

 		template <class PointFeature>
			PointFeature UnNormalise (const PointFeature& feature, 
			VW::Point2D point = VW::Point2D()) const
		{
			PointFeature pout = feature;
			pout.SetX (((feature.GetX() / _scale) + _centre_x));
			pout.SetY (((feature.GetY() / _scale) + _centre_y));
			return pout;
		}
		

		
		
		template <class LineFeature>
			void UnNormalise (std::vector<LineFeature>& feature_list, VW::Line2D line) const
		{
			if (this->IsIdentity()) return;
			for (typename std::vector<LineFeature>::iterator it = feature_list.begin();
			it != feature_list.end(); it++) {
				*it = UnNormalise (*it, *it);
			}
		}
		
		
		template <class LineFeature>
			LineFeature UnNormalise (const LineFeature& feature, VW::Line2D line) const
		{
			LineFeature lout = feature;
			lout[2] -= _scale * (lout[0]*_centre_x + lout[1]*_centre_y);
			lout[0] *= _scale;
			lout[1] *= _scale;
			return lout;
		}

  VNL::MatrixFixed<3,3,double> UnNormalise (const VNL::MatrixFixed<3,3,double>& M) const;

  VNL::MatrixFixed<3,3,float> UnNormalise (const VNL::MatrixFixed<3,3,float>& M) const;

  /** The inverse of a normalisation matrix is also a normalisation matrix. */
  Normalisation Inverse() const;

  // Data access
  VNL::MatrixFixed<3,3,double> operator() () const {return AsMatrix ();}
  VNL::MatrixFixed<3,3,double> AsMatrix () const;
  VNL::MatrixFixed<3,3,double> InverseMatrix () const;
  double GetScale () const {return _scale;}
  double GetCentreX () const {return _centre_x;}
  double GetCentreY () const {return _centre_y;}
  void GetCentre (double& x, double& y) const {x = _centre_x; y = _centre_y; return;}
  bool IsIdentity () const {return (_scale == 1.0 && _centre_x == 0.0 && _centre_y == 0.0);}

protected:
  // Data
  double _scale;
  double _centre_x;
  double _centre_y;

}; // end of class Normalisation












// Inlines

inline Normalisation::Normalisation () :
  _scale (1.0),
  _centre_x (0.0),
  _centre_y (0.0)
{}


inline Normalisation::Normalisation (double width) :
  _scale (2.0/width),
  _centre_x (0.5*width),
  _centre_y (0.5 * width * DEFAULT_WIDTH_HEIGHT_RATIO)
{}


inline Normalisation::Normalisation (double width, double height) :
  _scale (2.0/width),
  _centre_x (0.5*width),
  _centre_y (0.5*height)
{}


inline Normalisation::Normalisation (double scale, double centre_x, double centre_y) :
  _scale (scale),
  _centre_x (centre_x),
  _centre_y (centre_y)
{}


inline Normalisation::Normalisation (const Normalisation& C) :
  _scale (C._scale),
  _centre_x (C._centre_x),
  _centre_y (C._centre_y)
{}


inline Normalisation::Normalisation (const VNL::MatrixFixed<3,3,float>& C) :
  _scale ((0.5 * (C(0,0)+C(1,1))) / C(2,2)),
  _centre_x (-C(0,2)/_scale),
  _centre_y (-C(1,2)/_scale)
{}


inline Normalisation::Normalisation (const VNL::MatrixFixed<3,3,double>& C) :
  _scale ((0.5 * (C(0,0)+C(1,1))) / C(2,2)),
  _centre_x (-C(0,2)/_scale),
  _centre_y (-C(1,2)/_scale)
{}







inline void Normalisation::Init ()
{
  _scale = 1.0;
  _centre_x = 0.0;
  _centre_y = 0.0;
}


inline void Normalisation::Init (double width, double)
{
  _scale = 2/width;
  _centre_x = width/2;
  _centre_y = (width * DEFAULT_WIDTH_HEIGHT_RATIO)/2;
}


inline void Normalisation::Init (double scale, double centre_x, double centre_y)
{
  _scale = scale;
  _centre_x = centre_x;
  _centre_y = centre_y;
}


inline void Normalisation::Init (const VNL::MatrixFixed<3,3,float>& C)
{
  _scale = (0.5 * (C(0,0)+C(1,1))) / C(2,2);
  _centre_x = -C(0,2)/_scale;
  _centre_y = -C(1,2)/_scale;
}


inline void Normalisation::Init (const VNL::MatrixFixed<3,3,double>& C)
{
  _scale = (0.5 * (C(0,0)+C(1,1))) / C(2,2);
  _centre_x = -C(0,2)/_scale;
  _centre_y = -C(1,2)/_scale;
}



} // end of namespace VW



// Exported implementations/specialisations.  Move to cc file when keyword export is implemented
inline VNL::MatrixFixed<3,3,double> 
VW::Normalisation::Normalise (const VNL::MatrixFixed<3,3,double>& M) const 
{
  return this->AsMatrix() * M;
}


/**
 * @param M the matrix to be unnormalised (will not be changed)
 * @returns a new matrix which is un-normalised
 */
inline VNL::MatrixFixed<3,3,double> 
VW::Normalisation::UnNormalise (const VNL::MatrixFixed<3,3,double>& M) const 
{
    return this->InverseMatrix() * M;
}

inline VW::Normalisation VW::Normalisation::Inverse() const
{
  return Normalisation(1.0 / _scale, -_scale*_centre_x, -_scale*_centre_y);
}

#endif // #ifndef _normalisation_h_
