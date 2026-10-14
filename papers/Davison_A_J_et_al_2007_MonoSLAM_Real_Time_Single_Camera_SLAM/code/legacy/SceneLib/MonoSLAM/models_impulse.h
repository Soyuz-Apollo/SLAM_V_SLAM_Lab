/*  SceneLib: software for real-time SLAM

    models_impulse.h
    Copyright (C) 2005 University of Oxford

    Author
    Andrew Davison
    ajd@doc.ic.ac.uk

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// General motion model in 3D
// Assumes a random impulse changes the velocity at each time step
// State vector: 13 elements
//                 x 
//  r              y 
//                 z 
//  -              - 
//                 q0 
//  q              qx
//                 qy
//                 qz
//  -      =       - 
//                 vx
//  v              vy
//                 vz
//  -              - 
//                 omegax
//  omega          omegay
//                 omegaz

// Control vector has 3 elements, and represents a measurement of 
// acceleration if a linear accelerometer is present. Otherwise set it
// to zero.

// Noise vector n = V
//                  Omega
// Update:
// rnew     =   r + (v + V) delta_t
// qnew     =   q x q((omega + Omega) delta_t)
// vnew     =   v + V
// omeganew =   omega + Omega


#ifndef _monoslam_models_impulse_h_
#define _monoslam_models_impulse_h_

#include "Scene/models_base.h"

//
// Impulse ThreeD Motion Model
/** 
Class to represent a constant-velocity motion model. 
A constant-velocity motion model does not mean that the camera moves at a constant velocity over all time, but that the statistical model of its motion in a timestep is that on average we expect undetermined accelerations occur with a Gaussian profile. At each timestep there is a random impulse, but these are normally-distributed.

The dimension of the state vector is the 13:
\f[
x_v = \left[r | q | v | \omega \right] = \left[x y z | q_0 q_x q_y q_z | v_x v_y v_z | \omega_x \omega_y \omega_z\right]^T
\f]
The noise vector is
\f[
n = \left[V | \Omega\right]^T
\f]
and the update equations are
\f{align}
r_{new} &= r + (v+V)\Delta t
q_{new} &= q \times q(\omega + \Omega) \Delta t)
v_{new} &= v + V \omega_{new} &= \omega + \Omega \nonumber
\f}
The dimension of the state vector is 13: seven for the usual position state vector, plus three for the translational velocity and three for the rotational velocity.
@ingroup gMonoSLAM
**/
class Impulse_ThreeD_Motion_Model : public ThreeD_Motion_Model
{
 public:
  Impulse_ThreeD_Motion_Model();
  ~Impulse_ThreeD_Motion_Model();

  // Constants
  // May need a lot of tweaking!
  /// Standard deviation of linear acceleration (in \f$ms^{-2}\f$)
  double SD_A_component_filter;
  /// Standard deviation of linear acceleration, for simulation.
  double SD_A_component;
  // Standard deviation of angular acceleration
  /// Standard deviation of linear acceleration (in \f$rad s^{-2}\f$)
  double SD_alpha_component_filter;
  /// Standard deviation of angular acceleration, for simulation.
  double SD_alpha_component;

  // Redefined virtual functions
  void func_fv_and_dfv_by_dxv(const VNL::Vector<double> &xv, 
			      const VNL::Vector<double> &u, 
			      const double delta_t);

  void func_Q(const VNL::Vector<double> &xv, const VNL::Vector<double> &u, 
	      const double delta_t);

  void func_xp(const VNL::Vector<double> &xv);

  void func_dxp_by_dxv(const VNL::Vector<double> &xv);

  void func_fv_noisy(const VNL::Vector<double> &xv_true, 
		     const VNL::Vector<double> &u_true, const double delta_t);

  void func_xvredef_and_dxvredef_by_dxv_and_dxvredef_by_dxpdef  
    (const VNL::Vector<double> &xv, const VNL::Vector<double> &xpdef);

  void func_xvnorm_and_dxvnorm_by_dxv(const VNL::Vector<double> &xv);


  // Easy access to state blocks: fill matrices r, q, v, omega with
  // values based on state xv
  void extract_r_q_v_omega(const VNL::Vector<double> &xv, 
			   VW::Vector3D &r, 
			   VW::Quaternion &q,
			   VW::Vector3D &v, 
			   VW::Vector3D &omega);
  // The opposite: put r, q, v, omega back into vector xv
  void compose_xv(const VW::Vector3D &r, 
		  const VW::Quaternion &q,
		  const VW::Vector3D &v, 
		  const VW::Vector3D &omega, 
		  VNL::Vector<double> &xvnew);

 protected:
  // Calculate commonly used Jacobian part dq(omega * delta_t) by domega
  void dqomegadt_by_domega(const VW::Vector3D &omega, 
			   const double delta_t,
			   VNL::MatrixFixed<4,3,double> &dqomegadt_by_domega);

  // Ancillary functions: calculate parts of Jacobian dq_by_domega
  // which are repeatable due to symmetry.
  double dq0_by_domegaA(const double omegaA, const double omega, 
			const double delta_t);
  double dqA_by_domegaA(const double omegaA, const double omega, 
			const double delta_t);
  double dqA_by_domegaB(const double omegaA, const double omegaB, 
			const double omega, 
			const double delta_t);
};

#endif
