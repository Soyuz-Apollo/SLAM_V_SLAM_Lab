/** @file kalmanfilter.h
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
/**
 * @class   VW::KalmanFilter
 * @ingroup MathsStuff
 * @author  BJ Tordoff
 * @date    02/08/02
 * @brief   Fixed dimension Kalman filter without inputs.
 *
 */

#ifndef _kalmanfilter_h_
#define _kalmanfilter_h_

// System includes
#include <VNL/vectorfixed.h>
#include <VNL/matrixfixed.h>
#include <VNL/Algo/matrixinverse.h>


// Some macros to tidy things a little
#define StateVector VNL::VectorFixed<n_states,double>
#define MeasurementVector VNL::VectorFixed<n_measurements,double>

#define SMatrix VNL::MatrixFixed<n_states,n_states,double>
#define MMatrix VNL::MatrixFixed<n_measurements,n_measurements,double>

#define MSMatrix VNL::MatrixFixed<n_measurements,n_states,double>

namespace VW {



  template<unsigned int n_states, 
	   unsigned int n_measurements>
  class KalmanFilter {
  public:
    KalmanFilter() {};


    // Filter actions

    /** Make the prediction and return the predicted measurement. */
    MeasurementVector MakePrediction();

    /** Make an update using the measurment provided. */
    void MakeUpdate(const MeasurementVector& new_measurement);






    // Retrieval
    //    variables
    StateVector                      GetState() const;
    MeasurementVector                GetMeasurementPrediction() const;
    //    covariances
    SMatrix GetStateCovariance() const;
    MMatrix GetInnovationCovariance() const;
    MMatrix GetMeasuredInnovationCovariance() const;
    //    transitions
    SMatrix  GetStateStateTransition() const;
    MSMatrix GetStateMeasurementTransition() const;
    //    noise
    SMatrix GetStateNoise() const;
    MMatrix GetMeasurementNoise() const;

    // Assignment
    //    variables
    void SetState(const StateVector& new_x);
    //    covariances
    void SetStateCovariance(const SMatrix& new_P);
    //    transitions
    void SetStateStateTransition(const SMatrix& new_F);
    void SetStateMeasurementTransition(const MSMatrix& new_H);
    //    noise
    void SetStateNoise(const SMatrix& new_Q);
    void SetMeasurementNoise(const MMatrix& new_R);
   



  protected:
    // State vector
    StateVector                      x;
    StateVector                      x_pred;
    // prediction vector
    MeasurementVector                z_pred;
    // innovation vector
    MeasurementVector                v;

    // Covariances
    SMatrix P;
    SMatrix P_pred;
    MMatrix S;
    MMatrix S_pred;



    // Transition matrices
    SMatrix  F;
    MSMatrix H;
    
    // Noise matrices
    SMatrix Q;
    MMatrix R;
 

  };


} // end namespace VW

// Finally, include the template definitions
#include "kalmanfilter.tpp"

#undef MSMatrix

#undef MMatrix
#undef SMatrix

#undef MeasurementVector
#undef StateVector





#endif
