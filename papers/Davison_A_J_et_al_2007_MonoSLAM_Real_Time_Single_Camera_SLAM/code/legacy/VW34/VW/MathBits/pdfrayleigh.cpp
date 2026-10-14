/** 
 * @file     pdfrayleigh.cpp
 * @author   BJ Tordoff
 * @date     30/06/2003
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
#include "pdfrayleigh.h"

const double VW::PDF::Rayleigh::_ROOTPIBYTWO = 1.253314137;
const double VW::PDF::Rayleigh::_TWOMINUSPIBYTWO = 0.429203673;


// Sampling uses the cumulative distribution:
// CDF(r) = 1 - exp( -0.5 * (r*r) / (s*s) );
// we sample the CDF uniformly (CDF(r)=u), then calculate
// r = s * sqrt( -2*log(1-u) )
double VW::PDF::Rayleigh::Sample() const
{
   double u     = VW::Rand(1.0);

   return _s * sqrt( -2.0*log(1.0-u) );
 
}
