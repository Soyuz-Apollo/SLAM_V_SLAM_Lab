/*  SceneLib: software for real-time SLAM

    internal_measurement.h
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

#ifndef _scene_internal_measurement_h_
#define _scene_internal_measurement_h_

#include "models_base.h"

/**
@addtogroup gScene
@{
*/

/** Класс, который хранит информацию, необходимую Scene для обработки
внутреннего измерения. В Scene может быть массив таких объектов для разных
типов внутренних измерений. */
class Internal_Measurement
{
  friend class Scene_Single;
  friend class Kalman;

 public:
  Internal_Measurement(Internal_Measurement_Model *i_m_m);

  Internal_Measurement_Model *get_internal_measurement_model() 
    {return internal_measurement_model;}

  void predict_internal_measurement(const VNL::Vector<double> &xv,
				    const VNL::Matrix<double> &Pxx);
  void successful_internal_measurement(const VNL::Vector<double> &zv_in);
  void failed_internal_measurement();
  void construct_total_internal_measurement_stuff(
				       VNL::Vector<double> &nu_tot, 
				       VNL::Matrix<double> &dh_by_dx_tot,
				       VNL::Matrix<double> &R_tot,
				       unsigned int total_state_size);

 protected:

  Internal_Measurement_Model *internal_measurement_model;
  VNL::Vector<double> hv, zv, nuv;
  VNL::Matrix<double> dhv_by_dxv, Rv, Sv;
  bool successful_internal_measurement_flag;
};

/**
@}
*/

#endif
