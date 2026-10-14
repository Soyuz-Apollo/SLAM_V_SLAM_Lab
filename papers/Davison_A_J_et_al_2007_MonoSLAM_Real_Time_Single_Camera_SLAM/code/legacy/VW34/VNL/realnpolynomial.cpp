// This is vxl/VNL/real_npolynomial.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
*  \file
*  \brief a degree n real polynomial

*  \author Marc Pollefeys, ESAT-VISICS, K.U.Leuven, 12-08-97
*
*  Implements a polynomial with N variables
*/

#include <assert.h>
#include <cmath>    // fabs()
#include <iostream>
#include "realnpolynomial.h"

using namespace VNL;

/** Constructor.
*<PRE>
* coeffs = VNL::Vector<double>(nterms)
* polyn = VNL::Matrix<int>(nterms,nvar)
* Example: A*x^3 + B*x*y + C*y^2 + D*x*y^2
* nvar = 2;
* nterms = 4;
* coeffs = [A B C D]';
* polyn = [3 0]
*         [1 1]
*         [0 2]
*         [1 2];
*</PRE>
*/

VNL::RealNPolynomial::RealNPolynomial(const VNL::Vector<double>& c, const VNL::Matrix<int>& p)
  : coeffs_(c)
  , polyn_(p)
  , nvar_(p.Cols())
  , nterms_(p.Rows())
  , ideg_(p.MaxValue())
{
  assert(c.size() == p.Rows());
  _Simplify();
}

/** Combine terms with idential exponents (i.e., identical rows in polyn_).
* Remove terms with zero coefficient.
*/
void VNL::RealNPolynomial::_Simplify()
{
  for (int row1=0; row1<nterms_; ++row1)
    for (int row2=row1+1; row2<nterms_; ++row2) {
      int col=0;
      while (col<nvar_ && polyn_(row1,col) == polyn_(row2,col)) ++col;
      if (col < nvar_) continue; // not all exponents are identical
      coeffs_(row1) += coeffs_(row2); coeffs_(row2) = 0;
    }
  for (int row=0; row<nterms_; ++row)
    if (coeffs_(row) == 0) {
      --nterms_; // decrement nterms, and move last element to vacant place:
      coeffs_(row) = coeffs_(nterms_);
      coeffs_(nterms_) = 0; // not really necessary; to keep coeffs_ consistent
      for (int i=0; i<nvar_; ++i)
        polyn_(row,i) = polyn_(nterms_,i);
    }
}

double VNL::RealNPolynomial::_Eval(const VNL::Matrix<double>& xn)
{
  double s=0;
  for (int i=0; i<nterms_; i++){
    double t=coeffs_(i);
    for (int j=0; j<nvar_; j++)
      t*=xn(j,polyn_(i,j));
    s+=t;
  }
  return s;
}

double VNL::RealNPolynomial::Eval(const VNL::Vector<double>& x)
{
  VNL::Matrix<double> xn(nvar_,ideg_+1);

  for (int j=0; j<nvar_; j++){
    xn(j,0)=1;
    for (int i=1; i<ideg_+1; i++)
      xn(j,i)=xn(j,i-1)*x(j);
  }
  return _Eval(xn);
}


/** Set the coefficients and degree of variable.
*/
void VNL::RealNPolynomial::Set(const VNL::Vector<double>& c, const VNL::Matrix<int>& p)
{
  coeffs_= c;
  polyn_ = p;
  nvar_ = p.Cols();
  nterms_ = p.Rows();
  ideg_ = p.MaxValue();
}


int VNL::RealNPolynomial::Degree()
{
  int d=0;
  for (int i=0; i<nterms_; i++){
    int dt=0;
    for (int j=0; j<nvar_; j++)
      dt+=polyn_(i,j);
    if (dt>d) d=dt;
  }
  return d;
}

RealNPolynomial VNL::RealNPolynomial::operator-() const
{
  VNL::Vector<double> coef(nterms_);
  for (int i=0; i<nterms_; ++i) coef(i) = - coeffs_(i);

  VNL::Matrix<int> poly = polyn_;

  return RealNPolynomial(coef, poly);
}

RealNPolynomial VNL::RealNPolynomial::operator+(RealNPolynomial const& P) const
{
  assert(nvar_ == P.nvar_); // both polynomials must have the same variables

  VNL::Vector<double> coef(nterms_+P.nterms_);
  int i = 0; for (; i<nterms_; ++i) coef(i) = coeffs_(i);
  for (int j=0; j<P.nterms_; ++i,++j) coef(i) = P.coeffs_(j);

  VNL::Matrix<int> poly(nterms_+P.nterms_,nvar_);
  for (i=0; i<nterms_; ++i) for (int k=0; k<nvar_; ++k) poly(i,k) = polyn_(i,k);
  {
  for (int j=0; j<P.nterms_; ++i,++j) for (int k=0; k<nvar_; ++k) poly(i,k) = P.polyn_(j,k);
  }
  return RealNPolynomial(coef, poly);
}

RealNPolynomial VNL::RealNPolynomial::operator+(double P) const
{
  VNL::Vector<double> coef(nterms_+1);
  for (int i=0; i<nterms_; ++i) coef(i) = coeffs_(i);
  coef(nterms_) = P;

  VNL::Matrix<int> poly(nterms_+1,nvar_);
  {for (int i=0; i<nterms_; ++i) for (int k=0; k<nvar_; ++k) poly(i,k) = polyn_(i,k);}
  {for (int k=0; k<nvar_; ++k) poly(nterms_,k) = 0;}

  return RealNPolynomial(coef, poly);
}

RealNPolynomial VNL::RealNPolynomial::operator-(RealNPolynomial const& P) const
{
  assert(nvar_ == P.nvar_); // both polynomials must have the same variables

  VNL::Vector<double> coef(nterms_+P.nterms_);
  int i = 0; for (; i<nterms_; ++i) coef(i) = coeffs_(i);
  for (int j=0; j<P.nterms_; ++i,++j) coef(i) = - P.coeffs_(j);

  VNL::Matrix<int> poly(nterms_+P.nterms_,nvar_);
  {for (i=0; i<nterms_; ++i) for (int k=0; k<nvar_; ++k) poly(i,k) = polyn_(i,k);}
  {for (int j=0; j<P.nterms_; ++i,++j) for (int k=0; k<nvar_; ++k) poly(i,k) = P.polyn_(j,k);}

  return RealNPolynomial(coef, poly);
}

RealNPolynomial VNL::RealNPolynomial::operator*(RealNPolynomial const& P) const
{
  assert(nvar_ == P.nvar_); // both polynomials must have the same variables

  VNL::Vector<double> coef(nterms_*P.nterms_);
  int k = 0;
  for (int i=0; i<nterms_; ++i)
    for (int j=0; j<P.nterms_; ++j,++k)
      coef(k) = coeffs_(i) * P.coeffs_(j);

  VNL::Matrix<int> poly(nterms_*P.nterms_,nvar_);
  k = 0;
  {
  for (int i=0; i<nterms_; ++i)
    for (int j=0; j<P.nterms_; ++j,++k)
      for (int l=0; l<nvar_; ++l)
        poly(k,l) = polyn_(i,l) + P.polyn_(j,l);
  }
  return RealNPolynomial(coef, poly);
}

RealNPolynomial VNL::RealNPolynomial::operator*(double P) const
{
  VNL::Vector<double> coef(nterms_);
  for (int i=0; i<nterms_; ++i)
    coef(i) = coeffs_(i) * P;

  VNL::Matrix<int> poly = polyn_;

  return RealNPolynomial(coef, poly);
}

std::ostream& VNL::operator<<(std::ostream& os, RealNPolynomial const& P)
{
  if (P.nvar_ <= 3) for (int i=0; i<P.nterms_; ++i) {
    os << ' ';
    if (i>0 && P.coeffs_(i) > 0) os << '+';
    if (fabs(P.coeffs_(i)) != 1) os << P.coeffs_(i) << ' ';
    int totaldeg = 0;
    if (P.nvar_ > 0 && P.polyn_(i,0) > 0)  { os << 'X'; totaldeg += P.polyn_(i,0); }
    if (P.nvar_ > 0 && P.polyn_(i,0) > 1)  os << '^' << P.polyn_(i,0);
    if (P.nvar_ > 1 && P.polyn_(i,1) > 0)  { os << 'Y'; totaldeg += P.polyn_(i,1); }
    if (P.nvar_ > 1 && P.polyn_(i,1) > 1)  os << '^' << P.polyn_(i,1);
    if (P.nvar_ > 2 && P.polyn_(i,2) > 0)  { os << 'Z'; totaldeg += P.polyn_(i,2); }
    if (P.nvar_ > 2 && P.polyn_(i,2) > 1)  os << '^' << P.polyn_(i,2);
    if (totaldeg == 0 && fabs(P.coeffs_(i)) == 1) os << P.coeffs_(i);
  }
  else for (int i=0; i<P.nterms_; ++i) {
    os << ' ';
    if (i>0 && P.coeffs_(i) > 0) os << '+';
    if (fabs(P.coeffs_(i)) != 1) os << P.coeffs_(i) << ' ';
    int totaldeg = 0;
    for (int j=0; j<P.nvar_; ++j) {
      if (P.polyn_(i,j) > 0)  os << 'X' << j;
      if (P.polyn_(i,j) > 1)  os << '^' << P.polyn_(i,j);
      totaldeg += P.polyn_(i,j);
    }
    if (totaldeg == 0 && fabs(P.coeffs_(i)) == 1) os << P.coeffs_(i);
  }
  os << std::endl; return os;
}
