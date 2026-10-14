#ifdef _WIN32
#pragma warning(disable:4275) // disable C4275 warning
#endif

#include <complex>
#include <VNL/Algo/qr.tpp>

VNL_QR_INSTANTIATE(float);
VNL_QR_INSTANTIATE(double);
//VNL_QR_INSTANTIATE(long double);
VNL_QR_INSTANTIATE(std::complex<double>);
