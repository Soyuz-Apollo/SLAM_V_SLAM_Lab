/** @file pdfexponential.h 
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
 * @class   VW::PDF::Exponential
 * @ingroup MathsStuff
 * @author  BJ Tordoff
 * @date    03/03/2004
 * @brief   a small class to hold the constants associated with a 
 *          one-sided exponential distribution.
 *
 * This PDF class allows evaluation of the probability density at any
 * location, and sampling from a random variable with this PDF. Since
 * some parts of the evaluation depend only on \sigma, storing 
 * as a class gives some computational benefits. The functions is:
 *
 * prob(X==x) = 1/m * exp(-x/m)
 *
 * where m is the free parameter. The mean and variance of the PDF are then
 *
 *   E[X] = m
 * E[X^2] = 2m^2
 *   V[X] = m^2
 *
 * or, 
 *
 *    mu = m
 * sigma = m;
 *
 */

#ifndef _pdfexponential_h
#define _pdfexponential_h

//#include <cmath> // exp
#include "pdfbase1d.h"

namespace VW {

  namespace PDF {

    class Exponential : public PDFBase1D {
    public:
      /** Construct a new exponential PDF.
       * @param s the exponential parameter (= mean = standard deviation)
       */
      Exponential(double s=1);
      
      double EvaluateDensityForValue(double x) const;


      double Sample() const;
      
    private:
      double _s, _invs;
    };
    
  }; // end namespace PDF
}; // end namespace VW


















// Inlines
inline
VW::PDF::Exponential::Exponential(double s)
  : PDFBase1D(s,s),
    _s(s),
    _invs(1.0 / s)
{
}

inline double 
VW::PDF::Exponential::EvaluateDensityForValue(double x) const
{
  return _invs * exp(-x*_invs);

}


#endif
