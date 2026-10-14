/** @file pdfbase2d.h 
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
 * @class   VW::PDF::PDFBase1D
 * @ingroup MathsStuff
 * @author  BJ Tordoff
 * @date    30/06/2003
 * @brief   an abstract class that defines functions that all 1D PDF's have.
 *
 * Note that two PDF's with different means or variances are different PDF's,
 * and so no methods are provided for changing the mean and variance 
 * post-construction. This is deliberate, as it would remove much of the 
 * utility of the classes for fast evaluation/sampling.
 */
#ifndef _pdfbase1d_h_
#define _pdfbase1d_h_

namespace VW {
  namespace PDF {

    class PDFBase1D {
    public:
      /** Construct and set mean and standard deviation. */
      PDFBase1D(double mean, 
		double variance);


      /** Calculate the probability density at location x. */
     virtual double EvaluateDensityForValue(double x) const = 0;
      
      /** Sample from a random variable with this PDF. */
      virtual double Sample() const = 0;

      // Accessors (all 1D PDF's have a mean and std dev)
      double GetMean() const;
      double GetStandardDeviation() const;
      double GetVariance() const;

    protected:
      double _mu,_sigma,_sigma2;
    };

  } // end namespace PDF
} // end namespace VW



// Inlines


inline double 
VW::PDF::PDFBase1D::GetMean() const
{
  return _mu;
}

inline double 
VW::PDF::PDFBase1D::GetStandardDeviation() const
{
  return _sigma;
}

inline double 
VW::PDF::PDFBase1D::GetVariance() const
{
  return _sigma2;
}


#endif
