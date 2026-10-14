/*  SceneLib: software for real-time SLAM

    internal_measurement.cpp
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

#include <iostream>
#include "internal_measurement.h"

#ifdef DEBUGDUMP
#undef DEBUGDUMP
#endif
#define DEBUGDUMP false

using namespace std;

Internal_Measurement::Internal_Measurement(Internal_Measurement_Model *i_m_m)
  : internal_measurement_model(i_m_m)
{
  hv.Resize(internal_measurement_model->MEASUREMENT_SIZE);
  zv.Resize(internal_measurement_model->MEASUREMENT_SIZE);
  nuv.Resize(internal_measurement_model->MEASUREMENT_SIZE);
  dhv_by_dxv.Resize(internal_measurement_model->MEASUREMENT_SIZE, 
	    internal_measurement_model->get_motion_model()->STATE_SIZE);
  Rv.Resize(internal_measurement_model->MEASUREMENT_SIZE,
	    internal_measurement_model->MEASUREMENT_SIZE);
  Sv.Resize(internal_measurement_model->MEASUREMENT_SIZE,
	    internal_measurement_model->MEASUREMENT_SIZE);

  if (DEBUGDUMP) cout << "Adding Internal Measurement type " 
		    << internal_measurement_model->internal_type << endl;
}

void Internal_Measurement::
predict_internal_measurement(const VNL::Vector<double> &xv,
			     const VNL::Matrix<double> &Pxx)
{
  internal_measurement_model->func_hv_and_dhv_by_dxv(xv);

  hv.Update(internal_measurement_model->get_hvRES());
  dhv_by_dxv.Update(internal_measurement_model->get_dhv_by_dxvRES());

  internal_measurement_model->func_Rv(hv);
  Rv.Update(internal_measurement_model->get_RvRES());

  internal_measurement_model->func_Sv(Pxx, dhv_by_dxv, Rv);
  Sv.Update(internal_measurement_model->get_SvRES());

  if (DEBUGDUMP) cout << "Internal measurement prediction: hv " << endl 
		    << hv << endl;
}

void Internal_Measurement::
successful_internal_measurement(const VNL::Vector<double> &zv_in)
{
  assert(zv.Size() == internal_measurement_model->MEASUREMENT_SIZE);
  zv.Update(zv_in);

  if (DEBUGDUMP) cout << "Internal measurement made      : zv " << endl 
		    << zv << endl;

  successful_internal_measurement_flag = true;

  internal_measurement_model->func_nuv(hv, zv);
  nuv.Update(internal_measurement_model->get_nuvRES());
}

void Internal_Measurement::failed_internal_measurement()
{
  successful_internal_measurement_flag = false;
}

void Internal_Measurement::construct_total_internal_measurement_stuff(
				       VNL::Vector<double> &nu_tot, 
				       VNL::Matrix<double> &dh_by_dx_tot,
				       VNL::Matrix<double> &R_tot,
				       unsigned int total_state_size)
{
  unsigned int size = internal_measurement_model->MEASUREMENT_SIZE;

  assert (nu_tot.Size() == size && 
	  dh_by_dx_tot.Rows() == size && 
	  dh_by_dx_tot.Cols() == total_state_size &&
	  R_tot.Rows() == size && R_tot.Cols() == size);

  assert(successful_internal_measurement_flag);
  
  nu_tot.Fill(0.0);
  dh_by_dx_tot.Fill(0.0);
  R_tot.Fill(0.0);

  nu_tot.Update(nuv, 0);

  dh_by_dx_tot.Update(dhv_by_dxv, 0, 0);

  R_tot.Update(Rv, 0, 0);

}
