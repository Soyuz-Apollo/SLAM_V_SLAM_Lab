/** 
 * @file     pdfexponential.cpp
 * @author   BJ Tordoff
 * @date     03/03/2004
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


#include <cmath> // for sqrt, log

#include "random.h"
#include "pdfexponential.h"


// Sampling uses the cumulative distribution:
// CDF(r) = 1 - exp( -r/s );
// we sample the CDF uniformly (CDF(r)=u), then calculate
// r = -s log(1-CDF(r))
double VW::PDF::Exponential::Sample() const
{
   double u     = VW::Rand(1.0);

   return - _s * log(1.0 - u) ;
 
}
