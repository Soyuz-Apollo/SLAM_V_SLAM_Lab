#ifdef HAS_FSM_PACK

#include <complex>
#include <fsm_pack/fsm_qr.tpp>

FSM_QR_INSTANTIATE(float);
FSM_QR_INSTANTIATE(double);
FSM_QR_INSTANTIATE(long double);
FSM_QR_INSTANTIATE(std::complex<double>);




#endif
