#ifdef _WIN32
#pragma warning(disable:4275) // disable C4275 warning
#endif

#include <complex>
#include <VNL/Algo/determinant.tpp>

VNL_DETERMINANT_INSTANTIATE(float);
VNL_DETERMINANT_INSTANTIATE(double);
//VNL_DETERMINANT_INSTANTIATE(long double);
VNL_DETERMINANT_INSTANTIATE(std::complex<double>);
