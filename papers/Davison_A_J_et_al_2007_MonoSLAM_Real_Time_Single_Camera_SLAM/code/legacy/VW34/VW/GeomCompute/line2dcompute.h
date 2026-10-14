/** 
 * @file   line2dcompute.h
 * @author BJ Tordoff
 * @date   10/07/2003		   		
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

#ifndef _line2dcompute_h_
#define _line2dcompute_h_

#ifdef VERBOSE
#   undef VERBOSE
#endif
#define VERBOSE false

#include <VW/MathBits/pdfgaussian1d.h>
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/line2d.h>

namespace VW { 
  namespace Line2DCompute {


    /** Get the probability of the observed error for one point */
    double CalcErrorProbabilityPoint(const VW::Point2D& x,
												 const VW::Line2D& l,
												 const VW::PDF::Gaussian1D& g)
    {
      double error = l.GetDistance(x);
      
      double prob = g.EvaluateDensityForError(error);
      if (VERBOSE) std::cerr << "Error = " << error 
			     << " prob = " << prob << std::endl;
      
      // Use the error to estimate the probability assuming gaussian noise
      return(prob);
    }
    
    
  }; // end namespace Line2DCompute
}; // end namespace VW


#endif
