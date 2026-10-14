#ifdef HAS_FSM_PACK

#include <complex>
#include <fsm_pack/fsm_svd.tpp>

FSM_SVDC_INSTANTIATE(float);
FSM_SVDC_INSTANTIATE(double);
FSM_SVDC_INSTANTIATE(long double);
FSM_SVDC_INSTANTIATE(std::complex<double>);

#endif
