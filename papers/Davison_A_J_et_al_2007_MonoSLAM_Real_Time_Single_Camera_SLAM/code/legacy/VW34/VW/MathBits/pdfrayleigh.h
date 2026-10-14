/** @file pdfrayleigh.h 
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
 * @class   VW::PDF::Rayleigh
 * @ingroup MathsStuff
 * @author  BJ Tordoff
 * @date    30/06/2003
 * @brief   a small class to hold the constants associated with a 
 *          Rayleigh PDF.
 *
 * This PDF class allows evaluation of the probability density at any
 * location, and sampling from a random variable with this PDF. Since
 * some parts of the evaluation depend only on \sigma, storing 
 * as a class gives some computational benefits. The functions is:
 *
 * prob(X==x) = x/(s*s) * exp(-(x*x)/(2*s*s)
 *
 * where s is the free parameter. The mean and variance of the PDF are then
 *
 *      mu = s * sqrt(pi/2)
 * sigma^2 = 0.5 * s*s * (4-pi);
 *
 */

#ifndef _pdfrayleigh_h
#define _pdfrayleigh_h

#include <cmath> // sqrt
#include "pdfbase1d.h"

namespace VW {

  namespace PDF {

    class Rayleigh : public PDFBase1D {
    public:
      Rayleigh(double s=1);
      
      double EvaluateDensityForValue(double x) const;


      double Sample() const;
      
    private:
      static const double _ROOTPIBYTWO;
      static const double _TWOMINUSPIBYTWO;
      double _s, _invssquared;
    };
    
  }; // end namespace PDF
}; // end namespace VW


















// Inlines
inline
VW::PDF::Rayleigh::Rayleigh(double s)
  : PDFBase1D(s*_ROOTPIBYTWO,s*s*_TWOMINUSPIBYTWO),
    _s(s),
    _invssquared(1.0 / (s*s))
{
}

inline double 
VW::PDF::Rayleigh::EvaluateDensityForValue(double x) const
{
  const double x_squared = x*x;
  return _invssquared * x * exp(-0.5*x_squared*_invssquared);

}


#endif
