/** 
 * @file   fmatrixplanar.h
 * @author JGH Knight
 * @date   02/08/2002		   		
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
 * @class   VW::FMatrixPlanar
 * @ingroup GeometricObjects
 * @author  JGH Knight
 * @date    02/08/02
 * @brief   A class to represent the specialisation of the
 *          fundamental matrix due to camera motion in a plane.
 *
 * When the camera only moves in a plane (translates in the plane and rotates
 * about an axis perpendicular to it), as is typical for ground vehicles, the
 * fundamental matrix has only 6 degrees of freedom. This is done by forcing 
 * the both F, and (F + F') (its symmetric part) to be rank 2.
 */


#ifndef _fmatrixplanar_h_
#define _fmatrixplanar_h_

// Local includes
#include "fmatrix.h"

namespace VW {


class FMatrixPlanar : public FMatrix
{
 public:

  FMatrixPlanar () {Constrain ();}
  FMatrixPlanar (const FMatrix& F) : FMatrix (F) {Constrain ();}
  FMatrixPlanar (const VNL::MatrixFixed<3,3,double>& M) :
    FMatrix (M) {Constrain ();}

  inline FMatrixPlanar &operator= (const FMatrix& F);
  inline FMatrixPlanar &operator= (const VNL::MatrixFixed<3,3,double> m);
  inline FMatrixPlanar &operator= (const VNL::Matrix<double> m);

  // Singularity Conditions.
  void Constrain ();

private:
  void _ConstrainSVD();
  void _ConstrainHZ();
  void _ConstrainVLJK();
};


}; // end namespace VW;

// Inline

inline VW::FMatrixPlanar& 
VW::FMatrixPlanar::operator= (const VW::FMatrix& F)
{
  *(static_cast<VW::FMatrix*> (this)) = F;
  return *this;
}



inline VW::FMatrixPlanar &
VW::FMatrixPlanar::operator= (const VNL::MatrixFixed<3,3,double> m)
{
  *(static_cast<VW::FMatrix*> (this)) = m;
  return *this;
}


inline VW::FMatrixPlanar &VW::FMatrixPlanar::operator= (const VNL::Matrix<double> m)
{
  *(static_cast<VW::FMatrix*> (this)) = m;
  return *this;
}


#endif // #ifndef _fmatrix_h_
