/** @file chebyshevpolynomial.h 
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
 * @class   VW::ChebyshevPolynomial
 * @ingroup MathsStuff
 * @author  BJ Tordoff
 * @date    02/08/02
 * @brief   Some functions for fitting and evaluating Chebyshev polynomials.
 *
 */


#ifndef _chebyshevpolynomial_h_
#define _chebyshevpolynomial_h_

#include <vector>
#include <assert.h>


namespace VW {

class ChebyshevPolynomial {
 public:
  /** Construct with no data. Data must be added before use. */
  ChebyshevPolynomial(unsigned int poly_order=5);

  /** Construct with data. 
   * Data is added and the polynomial fitted, ready for use. 
   */
  ChebyshevPolynomial(const std::vector<double>& x, 
		      const std::vector<double>& y,
		      unsigned int poly_order=5);


  ~ChebyshevPolynomial() {};

  /** Remove all existing data from the internal store. */
  void Clear();

  /** Change the order of the polynomial to be fitted. */
  void SetPolynomialOrder(unsigned int poly_order);

  /** Append new data to the internal store. */
  void AddData(double x, double y);
  void AddData(const std::vector<double>& x, 
	       const std::vector<double>& y);

  /** Get the y-value corresponding to a particular X-position. */
  double GetYValue(double x) const;


  // Some other functions
  void GetLimits(double &minx, double &maxx, 
		 double &miny, double &maxy) const;

  // Access to raw data
  unsigned int GetNumberOfDataPoints() const;
  double GetDataX(unsigned int index) const;
  double GetDataY(unsigned int index) const;
  void GetCoeffs(std::vector<double>& coeffs) const;


 private:
  bool _validdata;

  // ** Note that the following may be set during a 'Get' call, and
  // hence are mutable. This is simply to enhance performance (the curve
  // is only fitted when first used).
  mutable bool _datafitted;
  mutable std::vector<double> _x,_y,_coeffs;
  mutable double _residual;   // Residual of least squares
  // **

  double  _maxx, _minx;
  double  _maxy, _miny;

  // This isn't really const - but only changes the internal data, 
  // which is mutable
  void FitPolynomial() const;
};

}; // End namespace VW




// INLINES

inline unsigned int VW::ChebyshevPolynomial::GetNumberOfDataPoints(void) const
{
  return _x.size();
};

inline double VW::ChebyshevPolynomial::GetDataX(unsigned int index) const
{
  if (index<_x.size()) {return _x[index];} else return -1;
};

inline double VW::ChebyshevPolynomial::GetDataY(unsigned int index) const
{
  if (index<_y.size()) {return _y[index];} else return -1;
};
			      

inline void VW::ChebyshevPolynomial::GetLimits(double &minx, double &maxx, 
					       double &miny, double &maxy) const
{
  minx = _minx;
  maxx = _maxx;
  miny = _miny;
  maxy = _maxy;
}

inline void VW::ChebyshevPolynomial::GetCoeffs(std::vector<double>& coeffs) const
{
  coeffs=_coeffs;
}


inline void VW::ChebyshevPolynomial::Clear()
{
  _x.clear();
  _y.clear();
  _validdata=false;
  _datafitted=false;
}

#endif
