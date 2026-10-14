#include <VNL/VC6WarningsFix.h>
#include <VNL/Algo/fftbase.tpp>
#include <VNL/Algo/fft1d.tpp>
#include <VNL/Algo/fft2d.tpp>
#include <VNL/Algo/fftprimefactors.tpp>

// As the 1 and 2D FFT's inherit from fftbase, it gets instantiated first


// 1D
VNL_FFT_BASE_INSTANTIATE(1, float);
VNL_FFT_1D_INSTANTIATE(float);

VNL_FFT_BASE_INSTANTIATE(1, double);
VNL_FFT_1D_INSTANTIATE(double);

// 2D
VNL_FFT_BASE_INSTANTIATE(2, float);
VNL_FFT_2D_INSTANTIATE(float);

VNL_FFT_BASE_INSTANTIATE(2, double);
VNL_FFT_2D_INSTANTIATE(double);

// Prime factors
VNL_FFT_PRIME_FACTORS_INSTANTIATE(float);
VNL_FFT_PRIME_FACTORS_INSTANTIATE(double);
