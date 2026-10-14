/** @file pdfuniform1d.h 
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
 * @class   VW::PDF::Uniform1D
 * @ingroup MathsStuff
 * @author  BJ Tordoff
 * @date    30/06/2003
 * @brief   a small class to hold the constants associated with a 
 *          1D Uniform PDF.
 *
 * This PDF class allows evaluation of the probability density at any
 * location, and sampling from a random variable with this PDF. Since
 * some parts of the evaluation depend only on \mu and \sigma, storing 
 * as a class gives some computational benefits.
 *
 */

#ifndef _pdfuniform1d_h
#define _pdfuniform1d_h

#include <cmath> // for sqrt
#include "pdfbase1d.h"

namespace VW {

  namespace PDF {

    /** a small class to hold the constants associated with a 
     * 1D Uniform distribution;
     */
    class Uniform1D : public PDFBase1D {
    public:
      Uniform1D(double centre=0.0, double width=1.0);
      
      double EvaluateDensityForError(double e) const;
      
      double EvaluateDensityForError2(double e_squared) const;
      
      double Sample() const;
      

private:
  double _prob,_limit,_limit2;
};

  }; // end namespace PDF
}; // end namespace VW


















// Inlines
inline
VW::PDF::Uniform1D::Uniform1D(double centre, double width)
  : PDFBase1D(centre,width*width/3.0),
    _prob(1.0/width),
    _limit(0.5*width),
    _limit2(0.25*width*width)
{
};

inline double 
VW::PDF::Uniform1D::EvaluateDensityForError(double e) const
{
  if ((e < _limit) && (e > -_limit))
    return(_prob);
  else
    return(0);
}

inline double 
VW::PDF::Uniform1D::EvaluateDensityForError2(double e_squared) const
{
  if (e_squared<_limit2)
    return(_prob);
  else
    return(0);
}


#endif
