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
 * @class   VW::PDF::PDFBase2D
 * @ingroup MathsStuff
 * @author  BJ Tordoff
 * @date    30/06/2003
 * @brief   an abstract class that defines functions that all 2D PDF's have.
 */

#ifndef _pdfbase2d_h_
#define _pdfbase2d_h_


#include <VNL/vectorfixed.h>
#include <VNL/matrixfixed.h>

namespace VW {
  namespace PDF {

    class PDFBase2D {
    public:
      /** Construct and set mean and standard deviation. */
      PDFBase2D(const VNL::VectorFixed<2,double>& mean, 
		const VNL::MatrixFixed<2,2,double>& covariance);

      virtual ~PDFBase2D() {};

      /** Calculate the probability density at location x. */
     virtual double EvaluateDensityForValue(const VNL::VectorFixed<2,double>&  x) const;
      
      /** Calculate the probability density at distance e from the mean. */
     virtual double EvaluateDensityForError(const VNL::VectorFixed<2,double>&  e) const = 0;
      
      /** Sample from a random variable with this PDF. */
      virtual VNL::VectorFixed<2,double> Sample() const = 0;

      // Accessors (all 1D PDF's have a mean and std dev)
      VNL::VectorFixed<2,double> GetMean() const;
      VNL::MatrixFixed<2,2,double> GetCovariance() const;

    protected:
      VNL::VectorFixed<2,double> _mu;
      VNL::MatrixFixed<2,2,double> _C;
    };

  } // end namespace PDF
} // end namespace VW



// Inlines

inline      
VW::PDF::PDFBase2D::PDFBase2D(const VNL::VectorFixed<2,double>& mean, 
			      const VNL::MatrixFixed<2,2,double>& covariance)
  : _mu(mean),
    _C(covariance)
{
}

inline VNL::VectorFixed<2,double> 
VW::PDF::PDFBase2D::GetMean() const
{
  return _mu;
}

inline VNL::MatrixFixed<2,2,double>
VW::PDF::PDFBase2D::GetCovariance() const
{
  return _C;
}

inline double 
VW::PDF::PDFBase2D::EvaluateDensityForValue(const VNL::VectorFixed<2,double>& x) const
{
  VNL::VectorFixed<2,double> e = x - _mu;
  return(EvaluateDensityForError(e));
  
}



#endif
