/*
  fsm@robots.ox.ac.uk
*/
#include <VNL/VC6WarningsFix.h>
#include <assert.h>

// Local stuff
#include "cpolyroots.h"
#include "complexeigensystem.h"

void VNL::CPolyRoots::_Compute(VNL::Vector<std::complex<double> > const &a) {
  // construct companion matrix
  VNL::Matrix<std::complex<double> > comp(N, N);
  comp.Fill(0);
  for (unsigned i=0; i<N-1; ++i)
    comp(i+1, i) = 1;
  for (unsigned j=0; j<N; ++j)
	  comp(j, N-1) = -a[N-1-j];

  // the eigenvalues of the companion matrix are the roots of the polynomial
  solns = VNL::ComplexEigensystem(comp,
                                  false,    // we only want
                                  false).W; // the eigenvalues.
}

VNL::CPolyRoots::CPolyRoots(VNL::Vector<std::complex<double> > const & a)
  : solns(a.size())
  , N(a.size()) // degree
{
  _Compute(a);
}

VNL::CPolyRoots::CPolyRoots(VNL::Vector<double> const & a_real,
                                 VNL::Vector<double> const & a_imag)
  : solns(a_real.size())
  , N(a_real.size()) // degree
{
  assert(a_real.size() == a_imag.size());
  VNL::Vector<std::complex<double> > a(N);
  for (unsigned i=0; i<N; ++i)
    a[i] = std::complex<double>(a_real[i], a_imag[i]);

  //std::cerr << "a = " << a << std::endl;
  _Compute(a);
  //std::cerr << "s = " << solns << std::endl;
}
