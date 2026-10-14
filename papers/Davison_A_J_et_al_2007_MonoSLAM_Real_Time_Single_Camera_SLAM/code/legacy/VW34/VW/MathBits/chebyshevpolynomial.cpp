/** @file chebyshevpolynomial.cc
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

// System includes
#include <iostream>
#include <VNL/matrix.h>
#include <VNL/vector.h>
#include <VNL/Algo/matrixinverse.h>

// Local includes
#include "chebyshevpolynomial.h"

using namespace std;


// A useful func
static void GetArrayLimits(const std::vector<double>& A,
			   double& minval, 
			   double& maxval)
{
  assert (A.size()>0);
  maxval = A[0];
  minval = A[0];
  for(unsigned int i=1;i<A.size();i++) {
    if (A[i]>maxval) maxval = A[i];
    if (A[i]<minval) minval = A[i];
  }
};




VW::ChebyshevPolynomial::ChebyshevPolynomial(unsigned int poly_order) :
  _validdata(false),
  _datafitted(false)
{
  _coeffs.resize(poly_order+1);
  for (unsigned int i=0;i<_coeffs.size();i++) _coeffs[i] = 0.0;
}

VW::ChebyshevPolynomial::ChebyshevPolynomial(const vector<double>& x, 
					     const vector<double>& y,
					     unsigned int poly_order) :
  _validdata(false),
  _datafitted(false)
{
  _coeffs.resize(poly_order+1);
  for (unsigned int i=0;i<_coeffs.size();i++) _coeffs[i] = 0.0;

  _x.clear(); _x.reserve(x.size());
  _y.clear(); _y.reserve(y.size());

  AddData(x,y);
  FitPolynomial();
}

////////////////////////////////////////////////////

void VW::ChebyshevPolynomial::SetPolynomialOrder(unsigned int poly_order)
{
  _coeffs.resize(poly_order+1);
  for (unsigned int i=0;i<_coeffs.size();i++) _coeffs[i] = 0.0;
  _datafitted = false;

}



////////////////////////////////////////////////////

void VW::ChebyshevPolynomial::AddData(double x, double y) 
{
//   cout << "Add data ("<<x<<", "<<y<<")" << endl;
  // If there's existing data check limits, else just use this
  if (_validdata) {
    if (x<_minx) _minx=x;
    if (x>_maxx) _maxx=x;
    if (y<_miny) _miny=y;
    if (y>_maxy) _maxy=y;
  } else {
    _minx=x;_maxx=x;
    _miny=y;_maxy=y;
    _validdata = true;
  }
  // Add the point
  _x.push_back(x);
  _y.push_back(y);

  _datafitted = false;
}

////////////////////////////////////////////////////


void VW::ChebyshevPolynomial::AddData(const vector<double>& x, 
				      const vector<double>& y)
{
  // Check if data is same lengths
  assert (x.size()==y.size());

  // Get new data limits
  double new_maxx=0,new_minx=0;
  double new_maxy=0,new_miny=0;

  GetArrayLimits(x,new_minx,new_maxx);
  GetArrayLimits(y,new_miny,new_maxy);

  // If there's existing data check limits, else just use this
  if (_validdata) {
    if (new_minx<_minx) _minx=new_minx;
    if (new_maxx>_maxx) _maxx=new_maxx;
    if (new_miny<_miny) _miny=new_miny;
    if (new_maxy>_maxy) _maxy=new_maxy;
    // Add the points
    for (unsigned int i=0;i<x.size();i++) {
//       cout << "Add data ("<<x[i]<<", "<<y[i]<<")" << endl;
      _x.push_back(x[i]);
      _y.push_back(y[i]);
    }
  } else {
    _minx=new_minx;_maxx=new_maxx;
    _miny=new_miny;_maxy=new_maxy;
    // Add the points
    _x = x;
    _y = y;
//     for (unsigned int i=0;i<x.size();i++) {
//       cout << "Add data ("<<x[i]<<", "<<y[i]<<")" << endl;
//     }
    _validdata = true;
  }

  _datafitted = false;
  
}

////////////////////////////////////////////////////


void VW::ChebyshevPolynomial::FitPolynomial() const {
  if (_validdata) {
    unsigned int numdata=_x.size();
    assert (_x.size()==_y.size());
    
    // Number of unique x-vals determines the max order of fit
    unsigned int num_unique=1;
    for (unsigned int i=1;i<numdata;i++) {
      const double xi = _x[i];
      bool found=false;
      for (unsigned int j=0;j<i;j++) {
	if (xi == _x[j]) {
	  found=true;
	  break;
	}
      }
      if (!found) num_unique++;
    }

//     cout << "Num data points="<<numdata<<endl;
//     cout << "Num unique X coords="<<num_unique<<endl;
//     cout << "Limits=("<<_minx<<" - "<<_maxx<<") and ("<<_miny<<" - "<<_maxy<<")"<<endl;

    if (num_unique>_coeffs.size()) num_unique = _coeffs.size();

    // First normalise the data
    VNL::Vector<double> xnorm(numdata);
    VNL::Vector<double> ynorm(numdata);
    VNL::Vector<double> coeffs(num_unique);
    

    // Normalise X
    if (_maxx == _minx) {
      for (unsigned int i=0;i<numdata;i++) xnorm[i] = 0.0;
    } else {
      const double offset = (0.5 * (_maxx + _minx));
      const double scale = 1.0 / (0.5 * (_maxx - _minx));
      for (unsigned int i=0;i<numdata;i++) xnorm[i] = scale * (_x[i] - offset);
    }

    cout << "_minx=" << _minx <<"\t_maxx="<< _maxx << "\t_miny="<<_miny << "\t_maxy:"<<_maxy<<endl;

    // Normalise Y
    if (_maxy == _miny) {
      for (unsigned int i=0;i<numdata;i++) ynorm[i] = 0.0;
    } else {
      const double offset = (0.5 * (_maxy + _miny));
      const double scale = 1.0 / (0.5 * (_maxy - _miny));
      for (unsigned int i=0;i<numdata;i++) ynorm[i] = scale * (_y[i] - offset);
    }

    // create measurement matrix
    VNL::Matrix<double> meas(numdata,num_unique);
    
    VNL::Vector<double> T(numdata), TOld(numdata), TTemp(numdata);
    // Initailize TOld, and set first column of measurement matrix
    meas.SetColumn(0,0.5);
    TOld=1;
    if (num_unique>1) {
      meas.SetColumn(1,xnorm);
      if (num_unique>2) {
	T=xnorm;
	// Create rest of measurement matrix
	for(unsigned int c=2;c<num_unique;c++) {
	  for (unsigned int i=0;i<numdata;i++)
	    TTemp[i] = T[i] * xnorm[i] * 2 - TOld[i];
	  TOld = T; T = TTemp;
	  meas.SetColumn(c, TTemp);
	}
      }
    }
    //    cout << "meas=["<<meas<<"]"<<endl;
    // BJT: this should work since there is an operator* for inverse. Strange.
    //    coeffs = VNL::MatrixInverse<double>(meas) * ynorm;
    const VNL::MatrixInverse<double> minv(meas);
    coeffs = minv * ynorm;

    //    cout << "ynorm = [" << ynorm << "]"<<endl;

    // Store coeffs (any not solved are 0)
    for (unsigned int m=0;m<num_unique;m++) _coeffs[m]=coeffs[m];
    if (num_unique<coeffs.size()) {
      for (unsigned int n=num_unique;n<_coeffs.size();n++) _coeffs[n]=0.0;
    }

//     cout << "coeffs=";
//     for (unsigned int c=0;c<_coeffs.size();c++) cout << _coeffs[c] << ",";
//     cout << endl;
    
    // Find residual
    VNL::Vector<double> diff(ynorm.size());
    diff = meas * coeffs - ynorm;
    _residual = diff.Magnitude();
    
    // Indicate that the polynomial is fitted
    _datafitted = true;
  }
}

////////////////////////////////////////////////////

double VW::ChebyshevPolynomial::GetYValue(double x) const {
  // Retrieve the Y-Value for the given X-Value
  double y = 0.0;

  // If only one data point, then return that (ie DC fit)
  if (_x.size()==1) {
    y=_y[0];
  } else {

    // Check that the polynomial is fitted, and if not do it!
    if (!_datafitted) FitPolynomial();
    
    // If all data is at one X value _minx == _maxx, otherwise need to normalise x coord
    double xnorm = _minx;
    if (_minx != _maxx) {
      xnorm = (x - 0.5 * (_maxx + _minx)) / (0.5 * (_maxx - _minx));
      //cout << "X vals different ("<<_minx<<" != "<<_maxx<<")!" << endl;
    }

    double T, TOld;
    TOld = 1; T = xnorm;
    
    if (_coeffs.size()>0) {
      // Add DC component
      y += 0.5 * _coeffs[0];
      
      if (_coeffs.size()>1) {
	// Add linear component
	y += T * _coeffs[1];
	
	if (_coeffs.size()>2) {
	double TempT;
	// Add the rest
	for (unsigned int n=2;n<_coeffs.size();n++) {
	  TempT = T;
	  T = 2*xnorm*T - TOld;
	  TOld = TempT;
	  y += T * _coeffs[n];
	}
	}
      }  
    }
    y = 0.5 * ( (y * (_maxy - _miny)) + (_maxy + _miny) );
  }
  return y;
}

