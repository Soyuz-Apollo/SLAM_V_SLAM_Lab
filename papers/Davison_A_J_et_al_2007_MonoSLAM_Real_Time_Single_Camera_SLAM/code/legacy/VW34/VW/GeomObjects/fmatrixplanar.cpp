/** 
 * @file   fmatrixplanar.cpp
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



#ifndef _fmatrixplanar_cpp_
#define _fmatrixplanar_cpp_

#include <cstdlib>

#include <VNL/vector.h>
#include <VNL/matrix.h>
#include <VNL/vectorfixed.h>
#include <VNL/matrixfixed.h>

#include <VNL/Algo/svd.h>
#include <VNL/crossproductmatrix.h>
#include <VNL/Algo/symmetriceigensystem.h>
#include <VNL/vnlmath.h>

#include "fmatrixplanar.h"

using namespace VW;
using namespace VNL;

/** FMatrixPlanar::Constrain () -- Constrain to rank 2 (singularity condition)
 *
 * Enforces the singularity conditions on the fundamental matrix using the SVD.
 */

void VW::FMatrixPlanar::Constrain ()
{
  if (AsMatrix () == MatrixFixed<3,3,double> (0.0)) return;

  _ConstrainHZ();
}

void VW::FMatrixPlanar::_ConstrainSVD()
{
  VNL::SVD<double> svdF (*this);
  svdF.W()(2,2) = 0.0;
  *this = svdF.Recompose ();

  const VNL::MatrixFixed<3,3,double> F = AsMatrix ();
  const VNL::MatrixFixed<3,3,double> Fasym = F - F.Transpose ();
  VNL::MatrixFixed<3,3,double> Fsym = F + F.Transpose ();
  VNL::SVD<double> svdFsym (Fsym);
  svdFsym.W()(2,2) = 0.0;
  Fsym = svdFsym.Recompose ();
  *this = (Fsym + Fasym) * 0.5;
}

void VW::FMatrixPlanar::_ConstrainHZ()
{
  VW::Point2D e1, e2;
  GetEpipoles (e1, e2);

  MatrixFixed<3,3,double> F = AsMatrix ();
  F /= F.FrobeniusNorm();

  SymmetricEigensystem<double>  symm_eig (F + F.Transpose());
  DiagMatrix<double> D=symm_eig.D;

  double eig0 = D (0,0);
  double eig1 = D (2,2);

  if (eig0 > 0 || eig1 < 0) {
	 // Aaargh! The eigenvalues are out of order!
    abort();
  }

  VectorFixed<3,double> v0 (symm_eig.GetEigenvector(0));
  VectorFixed<3,double> v1 (symm_eig.GetEigenvector(2));

  VectorFixed<3,double> f1 = v1*sqrt(eig1) + v0*sqrt(-eig0);
  VectorFixed<3,double> f2 = v1*sqrt(eig1) - v0*sqrt(-eig0);

  // Choose between f1 and f2 for the image of the screw axis, ls.
  // Do so by finding the epipolar line associated with the intersection of
  // f1 and f2, which will be algebraically equal to lh.
  VectorFixed<3,double> s = VNL::Cross3D (f1, f2);
  VectorFixed<3,double> lh = F * s;
  lh *= VNL::Sgn (lh[2]) / lh.Magnitude ();
  f1 *= VNL::Sgn (f1[2]) / f1.Magnitude ();
  f2 *= VNL::Sgn (f2[2]) / f2.Magnitude ();
  VectorFixed<3,double> ls;
  if ((lh - f1).Magnitude () < (lh - f2).Magnitude ()) ls = f2;
  else ls = f1;
  ls.Normalize ();

  VectorFixed<3,double> e1n(e1.GetX(), e1.GetY(), 1.0);
  VectorFixed<3,double> e2n(e2.GetX(), e2.GetY(), 1.0);
  e1n.Normalize ();
  e2n.Normalize ();

  // Reconstruct corrected F
  MatrixFixed<3,3,double> fmat = CrossProductMatrix<double> (e2n) 
    * CrossProductMatrix<double> (ls)
    * CrossProductMatrix<double> (e1n);
  *this = fmat / fmat.FrobeniusNorm ();
}

void VW::FMatrixPlanar::_ConstrainVLJK()
{
  VW::Point2D e1, e2;
  GetEpipoles (e1, e2);

  Matrix<double> F = AsMatrix ();
  F /= F.FrobeniusNorm();

  SymmetricEigensystem<double>  symm_eig (F + F.Transpose());

  double eig0 = symm_eig.D (0,0);
  double eig1 = symm_eig.D (2,2);

  if (eig0 > 0 || eig1 < 0) {
    std::cerr << "ERROR in VW::FMatrixPlanar::Constrain:   ";
    std::cerr << "vnl_symmetric_eigensystem<double> is unsorted: ";
    abort();
  }

  VectorFixed<3,double> v0 (symm_eig.GetEigenvector(0));
  VectorFixed<3,double> v1 (symm_eig.GetEigenvector(2));

  VectorFixed<3,double> f1 = v1*sqrt(eig1) + v0*sqrt(-eig0);
  VectorFixed<3,double> f2 = v1*sqrt(eig1) - v0*sqrt(-eig0);
  f1.Normalize ();
  f2.Normalize ();

  SVD<double> svdFa (F - F.Transpose ());
  VectorFixed<3,double> fa = svdFa.Nullvector ();
  fa.Normalize ();

  // Reconstruct corrected F
  MatrixFixed<3,3,double> fmat = CrossProductMatrix<double> (fa) 
                           + VNL::OuterProduct (f1, f2) + VNL::OuterProduct (f2, f1);
  *this = fmat / fmat.FrobeniusNorm ();
}



#endif // #ifndef _fmatrix_cpp_
