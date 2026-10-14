#ifdef _WIN32
#pragma warning(disable:4275) // disable C4275 warning
#endif

#include <VNL/VC6StdFix.h>
#include <complex>
#include <VNL/Algo/svd.tpp>

VNL_SVD_INSTANTIATE(float);
VNL_SVD_INSTANTIATE(double);
VNL_SVD_INSTANTIATE(std::complex<double>);




// For long double, must do something special

#ifndef HAS_FSM_PACK
#include <VNL/copy.h>

namespace VNL {

template <>
SVD<long double>::SVD(VNL::Matrix<long double> const& M, double /*zero_out_tol*/):
  m_(M.Rows()),
  n_(M.Columns()),
  U_(m_, n_),
  W_(n_),
  Winverse_(n_),
  V_(n_, n_)
{
  VNL::Matrix<double> M_(m_, n_);
  VNL::Copy(M, M_);

  VNL::SVD<double> svd(M_);

  VNL::Copy(svd.U(), U_);
  VNL::Copy(svd.W(), W_);
  VNL::Copy(svd.V(), V_);
}

}; //namespace VNL

VNL_SVD_INSTANTIATE(long double);



#endif
