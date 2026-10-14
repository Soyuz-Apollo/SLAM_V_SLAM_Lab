/*  SceneLib: software for real-time SLAM

    models_zeroorder.h
    Copyright (C) 2005 University of Oxford

    Author
    Paul Smith
    pas@robots.ox.ac.uk

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
// Assumes a random motion changes the pose at each time step
// State vector: 7 elements
//                 x 
//  r              y 
//                 z 
//  -              - 
//                 q0 
//  q              qx
//                 qy
//                 qz

// Control vector is ignored

// Noise vector n = R
//                  Q
// Update:
// rnew     =   r + R
// qnew     =   q x Q


#ifndef _monoslam_models_zeroorder_h_
#define _monoslam_models_zeroorder_h_

#include "Scene/models_base.h"

/** 
Class to represent a zero-order motion model. 
A zero-order motion model assumes that the camera stays where it was. At each timestep it moves from its location with a Gaussian profile.

The dimension of the state vector is the standard seven of position - nothing
else is stored (that's why it's zero order!)
\f[
\vct{x}_v = \evct{\vct{r} & | & \vct{q}}^T =
 \evct{x & y & z & | & q_0 & q_x & q_y & q_z}^T
\f]
The noise vector is \f$ \vct{n} = \evct{\vct{R} & | & \vct{Q}}^T \f$
And the update equations are
\f[
\begin{aligned}
\vct{r}_{new} &= \vct{r} + \vct{R} \\
\vct{q}_{new} &= \vct{q} + \vct{Q}
\end{aligned}
\f]
@ingroup gMonoSLAM
**/
class ZeroOrder_ThreeD_Motion_Model : public ThreeD_Motion_Model
{
 public:
  ZeroOrder_ThreeD_Motion_Model();
  ~ZeroOrder_ThreeD_Motion_Model();

  // Constants
  // May need a lot of tweaking!
  // Standard deviation of linear acceleration
  /// Standard deviation of position change (in \f$ms^{-1}\f$)
  double SD_A_component_filter;
  /// Standard deviation of linear velocity, for simulation.
  double SD_A_component;
  // Standard deviation of angular acceleration
  /// Standard deviation of angular velocity (in \f$rad s^{-1}\f$)
  double SD_alpha_component_filter;
  /// Standard deviation of angular velocity, for simulation.
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


private:
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
