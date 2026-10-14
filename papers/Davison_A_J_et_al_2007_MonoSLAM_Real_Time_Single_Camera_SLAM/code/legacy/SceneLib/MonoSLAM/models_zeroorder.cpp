/*  SceneLib: software for real-time SLAM

    models_zeroorder.cpp
    Copyright (C) 2005 University of Oxford

    Author
    Paul Smith
    pas@robots.ox.ac.uk

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <iostream>
#include <VNL/sample.h>
#include "models_zeroorder.h"

using namespace std;

//
// Constructor
/** 
Constructor. Sets standard deviations of linear velocity (\f$6ms^{-1}\f$) and angular acceleration (\f$4rads^{-1}\f$).
**/
ZeroOrder_ThreeD_Motion_Model::ZeroOrder_ThreeD_Motion_Model()
  : ThreeD_Motion_Model(7, 3, "ZEROORDER_THREED"),
    SD_A_component_filter(12.0), // m s^-1
    SD_A_component(6.0), // for simulation
    SD_alpha_component_filter(4.0),  // rad s^-1
    SD_alpha_component(4.0) // for simulation
{}

//
/// Destructor.
ZeroOrder_ThreeD_Motion_Model::~ZeroOrder_ThreeD_Motion_Model()
{}

//
// FUNC FV AND DFV BY DXV
/** 
Calculate the new state. Calculates \f$ f_v \f$ as a function of the old state \f$ x_v \f$, control vector 
\f$ u \f$ and time interval \f$ \Delta t \f$, and also the Jacobian \f$ \partial f_v / \partial x_v \f$ 
**/
void ZeroOrder_ThreeD_Motion_Model::func_fv_and_dfv_by_dxv(
                                     const VNL::Vector<double> &xv, 
                                     const VNL::Vector<double> &,
                                     const double )
{
  // The new state is just the old state!
  fvRES = xv;
  
  // The Jacobian is just the identity
  dfv_by_dxvRES.SetIdentity();
}

//
// FUNC Q
/** 
Form the covariance matrix \f$ Q \f$ of the process noise associated with \f$ x_v \f$.
**/
void ZeroOrder_ThreeD_Motion_Model::func_Q(const VNL::Vector<double> &xv, 
                                           const VNL::Vector<double> &,
                                           const double delta_t)
{
  // Fill noise covariance matrix Pnn: this is the covariance of 
  // the noise vector (V)
  //                  (Omega)
  // that gets added to the state. 
  // Form of this could change later, but for now assume that 
  // V and Omega are independent, and that each of their components is
  // independent... 
  double linear_velocity_noise_variance = 
     SD_A_component_filter * SD_A_component_filter * delta_t * delta_t;
  double angular_velocity_noise_variance =
     SD_alpha_component_filter * SD_alpha_component_filter * delta_t * delta_t;

  // Independence means that the matrix is diagonal
  VNL::MatrixFixed<6,6,double> Pnn;
  Pnn.Fill(0.0);
  Pnn.Put(0, 0, linear_velocity_noise_variance);
  Pnn.Put(1, 1, linear_velocity_noise_variance);
  Pnn.Put(2, 2, linear_velocity_noise_variance);
  Pnn.Put(3, 3, angular_velocity_noise_variance);
  Pnn.Put(4, 4, angular_velocity_noise_variance);
  Pnn.Put(5, 5, angular_velocity_noise_variance);

  // Form Jacobian dxnew_by_dn
  // Is like this:
  // I * delta_t     0
  // 0               dqnew_by_dOmega

  // Start by zeroing
  VNL::MatrixFixed<7,6,double> dxnew_by_dn;
  dxnew_by_dn.Fill(0.0);

  // The translation part is just I \Delta t
  VNL::MatrixFixed<3,3,double> Temp33A;
  Temp33A.SetIdentity();
  Temp33A *= delta_t;
  dxnew_by_dn.Update(Temp33A, 0, 0);

  // qnew = q x \Omega \Deltat
  // dqnew_by_d\Omega = dqnew_by_d\Omega\Delta t . d\Omega\Delta t_by_d\Omega
  
  // Get the first part
  VNL::VectorFixed<4,double> qRXYZ = xv.Extract(4, 3);
  VW::Quaternion qold;
  qold.SetRXYZ(qRXYZ);
  VNL::MatrixFixed<4,4,double> Temp44A = dq3_by_dq1(qold); 
  
  // Use function below for dqwt_by_dOmega
  VNL::VectorFixed<3, double> Omega;
  Omega.Fill(SD_alpha_component_filter);
  
  VNL::MatrixFixed<4,3,double> Temp43A;
  dqomegadt_by_domega(Omega, delta_t, Temp43A);
  // Multiply them together
  VNL::MatrixFixed<4,3,double> Temp43B = Temp44A * Temp43A;
  // And then plug into Jacobian
  dxnew_by_dn.Update(Temp43B, 3, 3);
  
  // Finally do Q = dxnew_by_dn . Pnn . dxnew_by_dnT
  QxRES.Update( dxnew_by_dn * Pnn * dxnew_by_dn.Transpose() );

}

// 
// FUNC XP
/** Extract the position and orientation from the state vector (they are the same!).
**/
void ZeroOrder_ThreeD_Motion_Model::func_xp(const VNL::Vector<double> &xv)
{
  xpRES = xv;
}

//
// FUNC DXP BY DXV
/** Calculate Jacobian for func_xp. (This is just the identity in this case.)
**/
void ZeroOrder_ThreeD_Motion_Model::func_dxp_by_dxv(const VNL::Vector<double> &)
{
  dxp_by_dxvRES.Fill(0.0);

  dxp_by_dxvRES.Put(0, 0, 1.0);
  dxp_by_dxvRES.Put(1, 1, 1.0);
  dxp_by_dxvRES.Put(2, 2, 1.0);
  dxp_by_dxvRES.Put(3, 3, 1.0);
  dxp_by_dxvRES.Put(4, 4, 1.0);
  dxp_by_dxvRES.Put(5, 5, 1.0);
  dxp_by_dxvRES.Put(6, 6, 1.0);
}

// FV NOISY
/** Not implemented yet.
**/
void ZeroOrder_ThreeD_Motion_Model::func_fv_noisy(const VNL::Vector<double> &,  
	const VNL::Vector<double> &, const double )
{
// TODO: Implement this.
}

//
// FUNC XVREDEF AND DXVREDEF BY DXV AND DXVREDEF BY DXPDEF
/** 
Redefine \f$ x_v \f$ and its Jacobian for a new co-ordinate frame. Not implemented yet!
**/
void ZeroOrder_ThreeD_Motion_Model::
func_xvredef_and_dxvredef_by_dxv_and_dxvredef_by_dxpdef(
const VNL::Vector<double> &, const VNL::Vector<double> &)
{
  cerr << "func_xvredef... not yet implemented for ZeroOrder_ThreeD_Motion_Model"
       << "... though surely it's not too hard?"
       << endl;
  assert(0);
}

//
// FUNC XVNORM AND DXVNORM BY DXV
/**  
Normalise the state vector: since quaternion is redundant we sometimes
need to enforce that it stays with magnitude 1.
**/
void ZeroOrder_ThreeD_Motion_Model::
func_xvnorm_and_dxvnorm_by_dxv(const VNL::Vector<double> &xv)
{
 // Most parts of the state vector don't change so copy as starting point
  xvnormRES.Update(xv);

  // Most parts of Jacobian are identity
  dxvnorm_by_dxvRES.SetIdentity();

  // Extract quaternion  
  func_xp(xv);
  func_q(xpRES);
  
  VW::Quaternion Tempqa = qRES;

  VW::Quaternion Tempqb = (Tempqa);
  VNL::MatrixFixed<4,4,double> Temp44a = dqnorm_by_dq(Tempqa);

  xvnormRES.Update(Tempqb.GetRXYZ(), 3);
  dxvnorm_by_dxvRES.Update(Temp44a, 3, 3);
}

//
// DQOMEGADT BY DOMEGA
/// Calculate commonly used Jacobian part \f$ \partial q(\omega * \Delta t) / \partial \omega \f$.
void ZeroOrder_ThreeD_Motion_Model::
dqomegadt_by_domega(const VW::Vector3D &omega, 
                    const double delta_t,
                    VNL::MatrixFixed<4,3,double> &dqomegadt_by_domega)
{
  // Modulus
  double omegamod = sqrt(omega.GetX() * omega.GetX() + 
                         omega.GetY() * omega.GetY() +
                         omega.GetZ() * omega.GetZ());

  // Use generic ancillary functions to calculate components of Jacobian
  dqomegadt_by_domega.Put(0, 0,
                          dq0_by_domegaA(omega.GetX(), omegamod, delta_t));
  dqomegadt_by_domega.Put(0, 1,
                          dq0_by_domegaA(omega.GetY(), omegamod, delta_t));
  dqomegadt_by_domega.Put(0, 2,
                          dq0_by_domegaA(omega.GetZ(), omegamod, delta_t));
  dqomegadt_by_domega.Put(1, 0,
                          dqA_by_domegaA(omega.GetX(), omegamod, delta_t));
  dqomegadt_by_domega.Put(1, 1,
                          dqA_by_domegaB(omega.GetX(), omega.GetY(), omegamod,
                                        delta_t));
  dqomegadt_by_domega.Put(1, 2,
                          dqA_by_domegaB(omega.GetX(), omega.GetZ(), omegamod,
                                        delta_t));
  dqomegadt_by_domega.Put(2, 0,
                          dqA_by_domegaB(omega.GetY(), omega.GetX(), omegamod,
                                        delta_t));
  dqomegadt_by_domega.Put(2, 1,
                          dqA_by_domegaA(omega.GetY(), omegamod, delta_t));
  dqomegadt_by_domega.Put(2, 2,
                          dqA_by_domegaB(omega.GetY(), omega.GetZ(), omegamod,
                                        delta_t));
  dqomegadt_by_domega.Put(3, 0,
                          dqA_by_domegaB(omega.GetZ(), omega.GetX(), omegamod,
                                        delta_t));
  dqomegadt_by_domega.Put(3, 1,
                          dqA_by_domegaB(omega.GetZ(), omega.GetY(), omegamod,
                                        delta_t));
  dqomegadt_by_domega.Put(3, 2,
                          dqA_by_domegaA(omega.GetZ(), omegamod, delta_t));}


// Ancillary functions: calculate parts of Jacobian dq_by_domega
// which are repeatable due to symmetry.
// Here omegaA is one of omegax, omegay, omegaz
// omegaB, omegaC are the other two
// And similarly with qA, qB, qC

//
// DQ0 BY DOMEGAA
/**
Ancillary function to calculate part of Jacobian \f$ \partial q / \partial \omega \f$ which is repeatable due to symmetry. Here omegaA is one of omegax, omegay, omegaz.
**/
double ZeroOrder_ThreeD_Motion_Model::dq0_by_domegaA(const double omegaA, 
                                                     const double omega,
                                                     const double delta_t)
{
  return (-delta_t / 2.0) * (omegaA / omega) * sin(omega * delta_t / 2.0);
}

//
// DQA BY DOMEGAA
/**
Ancillary function to calculate part of Jacobian \f$ \partial q / \partial \omega \f$ which is repeatable due to symmetry. Here omegaA is one of omegax, omegay, omegaz and similarly with qA.
**/
double ZeroOrder_ThreeD_Motion_Model::dqA_by_domegaA(const double omegaA, 
                                                     const double omega,
                                                     const double delta_t)
{
  return (delta_t / 2.0) * omegaA * omegaA / (omega * omega) 
    * cos(omega * delta_t / 2.0)
    + (1.0 / omega) * (1.0 - omegaA * omegaA / (omega * omega))
    * sin(omega * delta_t / 2.0);
}

//
// DQA BY DOMEGAB
/**
Ancillary function to calculate part of Jacobian \f$ \partial q / \partial \omega \f$ which is repeatable due to symmetry. Here omegaB is one of omegax, omegay, omegaz and similarly with qA.
**/
double ZeroOrder_ThreeD_Motion_Model::dqA_by_domegaB(const double omegaA, 
                                                     const double omegaB,
                                                     const double omega,
                                                     double delta_t)
{
  return (omegaA * omegaB / (omega * omega)) * 
    ( (delta_t / 2.0) * cos(omega * delta_t / 2.0)
      - (1.0 / omega) * sin(omega * delta_t / 2.0) );
}
