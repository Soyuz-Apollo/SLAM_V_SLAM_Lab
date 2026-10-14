/** @file pdfgaussian2d.h 
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
 * @class   VW::PDF::Gaussian2D
 * @ingroup MathsStuff
 * @author  BJ Tordoff
 * @date    30/06/2003
 * @brief   a small class to hold the constants associated with a 
 *          2D Gaussian PDF (note that for isotropic Gaussians th class
 *          VW::PDF::Gaussian2DIso is much faster).
 *
 * This PDF class allows evaluation of the probability density at any
 * location, and sampling from a random variable with this PDF. Since
 * some parts of the evaluation depend only on mean and covariance, storing 
 * as a class gives some computational benefits.
 *
 */

#ifndef _pdfgaussian2d_h
#define _pdfgaussian2d_h

#include "pdfbase2d.h"

#include <VNL/transpose.h>

namespace VW {

  namespace PDF {

    class Gaussian2D : PDFBase2D {
    public:
      Gaussian2D(const VNL::VectorFixed<2,double>& mean, 
		 const VNL::MatrixFixed<2,2,double>& covariance);
      
      double EvaluateDensityForError(const VNL::VectorFixed<2,double>& e) const;
     
      VNL::VectorFixed<2,double> Sample() const;
      
    private:
      static const double _TWOPI;
      VNL::MatrixFixed<2,2,double> _invC;
      VNL::MatrixFixed<2,2,double> _rootC;
      double _prefix;
      
    };
    
  }; // end namespace PDF
}; // end namespace VW


















// Inlines

inline double 
VW::PDF::Gaussian2D::EvaluateDensityForError(const VNL::VectorFixed<2,double>& e) const
{
  const double e_squared = VNL::DotProduct(e,_invC*e);
  return(_prefix * exp (- 0.5 * e_squared));
}




#endif
