#ifdef _WIN32
#pragma warning(disable:4275) // disable C4275 warning
#endif

#include <VNL/VC6StdFix.h>
#include <VNL/Algo/cholesky.tpp>

template class VNL::Cholesky<double>;
template class VNL::Cholesky<float>;

