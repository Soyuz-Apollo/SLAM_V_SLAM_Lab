/** @file pdfgaussian1d.h 
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
 * @class   VW::PDF::Gaussian1D
 * @ingroup MathsStuff
 * @author  BJ Tordoff
 * @date    30/06/2003
 * @brief   a small class to hold the constants associated with a 
 *          1D Gaussian PDF.
 *
 * This PDF class allows evaluation of the probability density at any
 * location, and sampling from a random variable with this PDF. Since
 * some parts of the evaluation depend only on \mu and \sigma, storing 
 * as a class gives some computational benefits.
 *
 */

#ifndef _pdfgaussian1d_h
#define _pdfgaussian1d_h

#include <cmath> // sqrt
#include "exp.h" // For fast approximations to exp
#include "pdfbase1d.h"

namespace VW {

  namespace PDF {

    class Gaussian1D : public PDFBase1D {
    public:
      Gaussian1D(double mean=0, double variance=1);
      
      double EvaluateDensityForValue(double e) const;
      
      /** Evaluate the density at a distance from the mean. */
      double EvaluateDensityForError(double e) const;
      
      /** Evaluate the density at a distance-squared from the mean. */
      double EvaluateDensityForError2(double e_squared) const;
      
		/** calculate the approximate density using a Pade approximation to exp,
		 * which is faster, but only accurate to about e = 3*sigma
		 */
      double ApproxDensityForError2(double e_squared) const;


      double Sample() const;
      
    private:
      static const double _ROOTTWOPI;
      double _inv2sigmasquared;
      double _prefix;
    };
    
  }; // end namespace PDF
}; // end namespace VW


















// Inlines
inline
VW::PDF::Gaussian1D::Gaussian1D(double mean, 
				double variance)
  : PDFBase1D(mean,variance),
    _inv2sigmasquared(1.0 / (2*variance)),
    _prefix(1.0 / (std::sqrt(variance)*_ROOTTWOPI))
{
}

inline double 
VW::PDF::Gaussian1D::EvaluateDensityForValue(double e) const
{
  const double e_squared = (e-_mu)*(e-_mu);
  return(EvaluateDensityForError2(e_squared));
}
      
inline double 
VW::PDF::Gaussian1D::EvaluateDensityForError(double e) const
{
  const double e_squared = e*e;
  return(EvaluateDensityForError2(e_squared));
}

inline double 
VW::PDF::Gaussian1D::EvaluateDensityForError2(double e_squared) const
{
  return(_prefix * exp (- e_squared * _inv2sigmasquared));
}

inline double 
VW::PDF::Gaussian1D::ApproxDensityForError2(double e_squared) const
{
  const double exponent = e_squared * _inv2sigmasquared;
  if (exponent > 6.0) return 0.0;
  return(_prefix * VW::PadeExp44 (-exponent));
}





#endif
