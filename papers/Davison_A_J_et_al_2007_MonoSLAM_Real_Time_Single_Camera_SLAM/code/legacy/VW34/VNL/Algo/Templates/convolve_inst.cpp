#include <VNL/Algo/convolve.tpp>

// Single types
VNL_CONVOLVE_INSTANTIATE(int, int);
VNL_CONVOLVE_INSTANTIATE(double, double);

// Mixed types
VNL_CONVOLVE_INSTANTIATE_2(int, double);
