/** @file pdfbase2diso.h 
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
 * @class   VW::PDF::PDFBase2DIso
 * @ingroup MathsStuff
 * @author  BJ Tordoff
 * @date    30/06/2003
 * @brief   an abstract class that defines functions that all 2D isotropic PDF's have.
 */

#ifndef _pdfbase2diso_h_
#define _pdfbase2diso_h_

#include "pdfbase2d.h"

namespace VW {
  namespace PDF {

    class PDFBase2DIso : public PDFBase2D {
    public:
      /** Construct and set mean and (co)variance. For isotropic PDF's, one
       * variance is sufficient, since the covariance matrix is just scaled 
       * identity.
       */
      PDFBase2DIso(const VNL::VectorFixed<2,double>& mean, 
		   double variance);


      /** Calculate the probability density at squared-distance from 
       * the mean \f$(x - \mu)^{\rm T}(x - \mu)\f$. */
     virtual double EvaluateDensityForError2(double e_squared) const = 0;
     

    protected:
      double _sigma;
 
    };

  } // end namespace PDF
} // end namespace VW



// Inlines



#endif
