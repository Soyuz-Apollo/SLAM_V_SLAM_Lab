/** @file kalmanfilterwithinput.tpp
 *
 *              (c) Copyright University of Oxford
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


// This file provides the inline template definitions, and is included by the .h file

#ifndef _kalmanfilterwithinput_tpp_
#define _kalmanfilterwithinput_tpp_




// INLINES
#define KalmanWI VW::KalmanFilterWithInput<n_states,n_inputs,n_measurements>

template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline StateVector 
KalmanWI::GetState() const {
  return(x);
}

template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline MeasurementVector 
KalmanWI::GetMeasurementPrediction() const {
  return(z_pred);
}
    
template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline MeasurementVector 
KalmanWI::GetInnovation() const 
{
  return(v);
}
    
template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline SMatrix
KalmanWI::GetStateStateTransition() const 
{
  return(F);
}

template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline SIMatrix
KalmanWI::GetInputStateTransition() const 
{
  return(G);
}

template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline MSMatrix 
KalmanWI::GetStateMeasurementTransition() const 
{
  return(H);
}

template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline SMatrix KalmanWI::GetStateNoise() const 
{
  return(Q);
}

template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline MMatrix KalmanWI::GetMeasurementNoise() const 
{
  return(R);
}
template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline SMatrix KalmanWI::GetStateCovariance() const 
{
  return(P);
}
template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline MMatrix KalmanWI::GetInnovationCovariance() const 
{
  return(S_pred);
}
template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline void KalmanWI::SetState(const StateVector& new_x) 
{
  x=new_x;
}
template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline void KalmanWI::SetStateCovariance(const SMatrix& new_P) 
{
  P=new_P;
}
template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline void KalmanWI::SetStateStateTransition(const SMatrix& new_F) 
{
  F=new_F;
}
template  <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline void KalmanWI::SetInputStateTransition(const SIMatrix& new_G) 
{
  G=new_G;
}
template<unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline void KalmanWI::SetStateMeasurementTransition(const MSMatrix& new_H) 
{
  H=new_H;
}
template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline void KalmanWI::SetStateNoise(const SMatrix& new_Q) 
{
  Q=new_Q;
}
template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
inline void KalmanWI::SetMeasurementNoise(const MMatrix& new_R) 
{
  R=new_R;
}

template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
MeasurementVector KalmanWI::MakePrediction(const InputVector& new_input) {
    x_pred = F*x + G*new_input;
    z_pred = H*x_pred;
    P_pred = F*P*F.Transpose() + Q;
    S_pred = (H * P_pred * H.Transpose()) + R;
   return (z_pred);
}

template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
void KalmanWI::MakeUpdate(const MeasurementVector& new_measurement) {

    // Kalman gain
    VNL::MatrixFixed<n_states,n_measurements> K;

    VNL::Matrix<double> tmp=VNL::MatrixInverse<double>(S_pred);

     K = P_pred * H.Transpose()*tmp;

    // Update
    v = new_measurement - z_pred;
    x = x_pred + (K * v);
    P = P_pred - (K * S_pred * K.Transpose());

}
template <unsigned int n_states,unsigned int n_inputs,unsigned int n_measurements>
void KalmanWI::MakeUpdate() {

    // Update (note no new innovation etc)
    x = x_pred;
    P = P_pred;

}


#undef KalmanWI

#endif
