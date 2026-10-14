// This is vxl/VNL/real_polynomial.cxx
#ifdef _WIN32
#pragma warning(disable:4275) // disable C4275 warning
#endif

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \brief Evaluation of real polynomials - the implemenation

* \author Andrew W. Fitzgibbon, Oxford RRG 23 Aug 96
*
* Modifications
* IMS (Manchester) 14/03/2001: Added Manchester IO scheme
*/

#include "realpolynomial.h"
#include <iostream>
#include <complex>
#include <cmath>

// This is replacing a member template...
namespace VNL {
template <class T>
T RealPolynomialEvaluate(double const *a, int n, T const& x)
{
  --n;
  T acc = a[n];
  T xn = x;
  do {
    acc += a[--n] * xn;
    xn *= x;
  } while (n);

  return acc;
}

#ifdef _WIN32
#define SELECT(T) <T >
#else
#define SELECT(T)
#endif
}; // ENd namespace VNL

  /* Function templates do not need to be instanciated before used

  * this block has been comented by jmmm on July 21 2005

// Instantiate templates before use
template double VNL::RealPolynomialEvaluate SELECT(double )
      (double const*,int,double const&);
template std::complex<double> VNL::RealPolynomialEvaluate SELECT(std::complex<double>)
      (double const*,int,std::complex<double> const&);


end of jmmm July 2005 commet
*/

/** Evaluate polynomial at value x.
*/

/* Modified by jmmm on July 21 2005 this is the old version 
double VNL::RealPolynomial::Evaluate(double x) const
{
return VNL::RealPolynomialEvaluate SELECT(double)(coeffs_.DataBlock(), coeffs_.size(), x);
}

end of old version
*/

double VNL::RealPolynomial::Evaluate(double x) const
{
return VNL::RealPolynomialEvaluate <double>(coeffs_.DataBlock(), coeffs_.size(), x);
}



/** Evaluate polynomial at complex value x.
*/

/* Modified by jmmm on July 21 2005 this is the old version 
std::complex<double> VNL::RealPolynomial::Evaluate(std::complex<double> const& x) const
{
return VNL::RealPolynomialEvaluate SELECT(std::complex<double>)
     (coeffs_.DataBlock(), coeffs_.size(), x);
}
end of old version
*/

std::complex<double> VNL::RealPolynomial::Evaluate(std::complex<double> const& x) const
{
  return VNL::RealPolynomialEvaluate <std::complex<double> >
     (coeffs_.DataBlock(), coeffs_.size(), x);
}

/** Evaluate derivative at value x.
*/
double VNL::RealPolynomial::Devaluate(double x) const
{
  return Derivative().Evaluate(x);
}


/** Evaluate derivative at complex value x.\ Not implemented.
*/
std::complex<double> VNL::RealPolynomial::Devaluate(std::complex<double> const& x) const
{
  return Derivative().Evaluate(x);
}

/** Evaluate integral at x (assuming constant of integration is zero).
*/
double VNL::RealPolynomial::EvaluateIntegral(double x) const
{
  int d = coeffs_.size()-1;
  const double* f = coeffs_.DataBlock();
  double sum = 0.0;
  int di=1;
  double xi=x;
  for (int i=d;i>=0;--i)
  {
    sum += f[i]*xi/di;
    xi*=x;
    di++;
  }

  return sum;
}

/** Evaluate integral between x1 and x2.
*/
double VNL::RealPolynomial::EvaluateIntegral(double x1, double x2) const
{
  return EvaluateIntegral(x2)-EvaluateIntegral(x1);
}

/** Returns sum of two polynomials f1(x)+f2(x).
*/
VNL::RealPolynomial VNL:: operator+(const VNL::RealPolynomial& f1, const VNL::RealPolynomial& f2)
{
  // Degree of result is highest of the two inputs
  int d1=f1.Degree();
  int d2=f2.Degree();
  int d = d1;
  if (d2>d) d=d2;

  VNL::RealPolynomial sum(d);

  // Coefficients are stored such that f(i) is coef. on x^(d-i)
  for (int i=0;i<=d;++i)
  {
    sum[d-i]=0.0;
    if (i<=d1) sum[d-i]+=f1[d1-i];
    if (i<=d2) sum[d-i]+=f2[d2-i];
  }

  return sum;
}

/** Returns sum of two polynomials f1(x)-f2(x).
*/
VNL::RealPolynomial VNL::operator-(const VNL::RealPolynomial& f1, const VNL::RealPolynomial& f2)
{
  // Degree of result is highest of the two inputs
  int d1=f1.Degree();
  int d2=f2.Degree();
  int d = d1;
  if (d2>d) d=d2;

  VNL::RealPolynomial sum(d);

  // Coefficients are stored such that f(i) is coef. on x^(d-i)
  for (int i=0;i<=d;++i)
  {
    sum[d-i]=0.0;
    if (i<=d1) sum[d-i]+=f1[d1-i];
    if (i<=d2) sum[d-i]-=f2[d2-i];
  }

  return sum;
}

/** Returns polynomial which is product of two polynomials f1(x)*f2(x).
*/
VNL::RealPolynomial VNL::operator*(const VNL::RealPolynomial& f1, const VNL::RealPolynomial& f2)
{
  int d1=f1.Degree();
  int d2=f2.Degree();
  int d = d1+d2;

  VNL::RealPolynomial sum(d);
  sum.Coefficients().Fill(0.0);

  for (int i=0;i<=d1;++i)
    for (int j=0;j<=d2;++j)
      sum[d-(i+j)] += (f1[d1-i]*f2[d2-j]);

  return sum;
}

/** Returns RMS difference between f1 and f2 over range [x1,x2].
* \f$\frac1{\sqrt{|x_2-x_1|}}\,\sqrt{\int_{x_1}^{x_2}\left(f_1(x)-f_2(x)\right)^2\,dx}\f$
*/
double VNL::RMSDifference(const VNL::RealPolynomial& f1, const VNL::RealPolynomial& f2,
                          double x1, double x2)
{
  using namespace std;
	double dx = fabs(x2-x1);
  if (dx==0.0) return 0;

  VNL::RealPolynomial df = f2-f1;
  VNL::RealPolynomial df2 = df*df;
  double area = fabs(df2.EvaluateIntegral(x1,x2));
  return sqrt(area/dx);
}

/** Return derivative of this polynomial.
*/
VNL::RealPolynomial VNL::RealPolynomial::Derivative() const
{
  VNL::Vector<double> c = Coefficients();
  int d = Degree();
  VNL::Vector<double> cd (d);
  for (int i=0; i<d; ++i)
    cd[i] = c[i] * (d-i);
  return VNL::RealPolynomial(cd);
}

/** Return primitive function (inverse derivative) of this polynomial.
* Since a primitive function is not unique, the one with constant = 0 is returned
*/
VNL::RealPolynomial VNL::RealPolynomial::Primitive() const
{
  VNL::Vector<double> c = Coefficients();
  int d = Degree();
  VNL::Vector<double> cd (d+2);
  for (int i=0; i<=d; ++i)
    cd[i] = c[i] / (d-i+1);
  cd[d+1] = 0.0;
  return VNL::RealPolynomial(cd);
}

void VNL::RealPolynomial::Print(std::ostream& os) const
{
  int d = Degree();
  int i = 0;
  while (i <= d && coeffs_[i] == 0) ++i;
  if (i > d) { os << "0 "; return; }
  bool b = (coeffs_[i+1] > 0); // to avoid '+' in front of equation

  for (; i <= d; ++i) {
    if (coeffs_[i] == 0) continue;
    if (coeffs_[i] > 0 && !b) os << '+';
    b = false;
    if (coeffs_[i] == -1)     os << '-';
    else if (coeffs_[i] != 1) os << coeffs_[i];

    if (i < d-1)              os << " X^" << d-i << ' ';
    else if (i == d-1)        os << " X ";
  }
}

