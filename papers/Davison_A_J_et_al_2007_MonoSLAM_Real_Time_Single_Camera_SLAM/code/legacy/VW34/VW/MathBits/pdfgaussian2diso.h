/** @file pdfgaussian2diso.h 
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
 * @class   VW::PDF::Gaussian2DIso
 * @ingroup MathsStuff
 * @author  BJ Tordoff
 * @date    30/06/2003
 * @brief   a small class to hold the constants associated with a 
 *          2D Isotropic Gaussian PDF (note that this is much faster than
 *          the non-isotropic VW::PDF::Gaussian2D).
 *
 * This PDF class allows evaluation of the probability density at any
 * location, and sampling from a random variable with this PDF. Since
 * some parts of the evaluation depend only on mean and covariance, storing 
 * as a class gives some computational benefits.
 *
 */

#ifndef _pdfgaussian2diso_h
#define _pdfgaussian2diso_h

#include "pdfbase2diso.h"
#include "exp.h"

namespace VW {

  namespace PDF {

    class Gaussian2DIso : PDFBase2DIso {
    public:
      Gaussian2DIso(const VNL::VectorFixed<2,double>& mean, 
		    double covariance=1.0);
      
      Gaussian2DIso(double mean_x, double mean_y, double covariance=1.0);
      
      double EvaluateDensityForError(const VNL::VectorFixed<2,double>& e) const;
      
      double EvaluateDensityForError2(double e_squared) const;
      
		/** calculate the approximate density using a Pade approximation to exp,
		 * which is faster, but only accurate to about e = 3*sigma
		 */
      double ApproxDensityForError2(double e_squared) const;

      
      VNL::VectorFixed<2,double> Sample() const;
      
    private:
      static const double _TWOPI;
      double _inv2C;
      double _prefix;
    };
    
  }; // end namespace PDF
}; // end namespace VW


















// Inlines
inline
VW::PDF::Gaussian2DIso::Gaussian2DIso(double mean_x, double mean_y, double covariance)
  : PDFBase2DIso(VNL::VectorFixed<2,double>(mean_x,mean_y),covariance),
    _inv2C(1.0 / (2*covariance)),
    _prefix(1.0 / (covariance*_TWOPI))
{
}

inline
VW::PDF::Gaussian2DIso::Gaussian2DIso(const VNL::VectorFixed<2,double>& mean, 
				      double variance)
  : PDFBase2DIso(mean,variance),
    _inv2C(1.0 / (2*variance)),
    _prefix(1.0 / (variance*_TWOPI))
{
}

inline double 
VW::PDF::Gaussian2DIso::EvaluateDensityForError(const VNL::VectorFixed<2,double>& e) const
{
  const double e_squared = VNL::DotProduct(e,e);
  return(EvaluateDensityForError2(e_squared));
}

inline double 
VW::PDF::Gaussian2DIso::EvaluateDensityForError2(double e_squared) const
{
  return(_prefix * exp (- e_squared * _inv2C));
}

inline double VW::PDF::Gaussian2DIso::ApproxDensityForError2(double e_squared) const
{
  const double exponent = e_squared * _inv2C;
  if (exponent > 6.0) return 0.0;
  return(_prefix * VW::PadeExp44 (-exponent));
}



#endif
