/** @file nan.h
 * @ingroup MathsStuff
 * @author ID Reid
 * @date   02/08/02
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


// BJT says: What's this? Surely the VNL functions IsNaN etc do this job?


#ifndef _nan_h_
#define _nan_h_

#include <math.h>
#include <float.h>

namespace VW {
	inline bool IsNaN(double x)
	{
		#ifdef _WIN32
		return _isnan(x) ? true :false;
		#else
		return isnan(x) ? true :false;
		#endif
	}

	inline bool IsFinite(double x)
	{
		#ifdef _WIN32
		return _finite(x) ? true :false;
		#else
		return finite(x) ? true :false;
		#endif
	}
        const double NaN=0.0/0.0;

};








#endif // _nan_h_
