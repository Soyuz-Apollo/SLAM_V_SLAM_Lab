#ifdef _WIN32
#pragma warning(disable:4275) // disable C4275 warning
#endif
#include <complex>
#include <VNL/fortrancopy.tpp>

VNL_FORTRAN_COPY_INSTANTIATE(float);
VNL_FORTRAN_COPY_INSTANTIATE(double);
VNL_FORTRAN_COPY_INSTANTIATE(long double);
VNL_FORTRAN_COPY_INSTANTIATE(std::complex<double>);
