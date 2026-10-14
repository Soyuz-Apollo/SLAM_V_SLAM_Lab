/*  SceneLib: software for real-time SLAM

    simul.h
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

#ifndef _scene_simul_h_
#define _scene_simul_h_


#include "sim_or_rob.h"

/**
@addtogroup gScene
@{
*/

// Класс симуляции.

class Simulation_Feature { 
friend class Simulation;
 public:
  Simulation_Feature(unsigned int lab, Feature_Measurement_Model *f_m_m, 
		     const VNL::Vector<double> &yi);

  const VNL::Vector<double> &get_yi() {return yi;}
  unsigned int get_label() {return label;}
  Simulation_Feature *get_next() {return next;}
  Feature_Measurement_Model *get_feature_measurement_model() 
    {return feature_measurement_model;}

 protected:
  Feature_Measurement_Model *feature_measurement_model;
  Simulation_Feature *next;
  VNL::Vector<double> yi;
  unsigned int label;
};


class Simulation : public Sim_Or_Rob {
public:
  Simulation(Motion_Model *m_m, const VNL::Vector<double> &initial_xv);    
  ~Simulation();

  // Переопределённые виртуальные функции.
  bool measure_feature(Identifier id, VNL::Vector<double> &z, const VNL::Vector<double> &h, 
		       const VNL::Matrix<double> &S);

  bool set_control(const VNL::Vector<double> &u, const double delta_t);
  bool continue_control(const VNL::Vector<double> &u, const double delta_t);
  double wait_for_end_of_motion(const VNL::Vector<double> &u);
  bool stop_vehicle();
  Identifier initialise_known_feature(Feature_Measurement_Model *f_m_m,
				 const VNL::Vector<double> &yi,
				 const unsigned int known_feature_label);
  bool make_initial_measurement_of_feature(VNL::Vector<double> &z, 
					   Identifier &id, 
					  Feature_Measurement_Model *f_m_m);

  bool make_internal_measurement(Internal_Measurement_Model 
             *internal_measurement_model, VNL::Vector<double> &zv, 
				const VNL::Vector<double> &hv, 
				const VNL::Matrix<double> &Sv);

  void print_true_features();
  void print_true_vehicle();

  // Функции доступа.
  Motion_Model *get_motion_model() {return motion_model;}
  unsigned int get_no_true_features() {return no_features;}
  Simulation_Feature *get_first_feature() {return first_feature;}
  const VNL::Vector<double> &get_xv() {return xv;}

  Identifier find_identifier(unsigned int label);

  /* Функции для загрузки и выгрузки истинного состояния при необходимости. */
  bool construct_total_true_state(VNL::Vector<double> &V);
  bool fill_true_state(const VNL::Vector<double> &V);

  unsigned int add_new_true_feature(Feature_Measurement_Model *f_m_m, 
				    const VNL::Vector<double> &yi);

  // Обнулить оси в текущем положении.
  void zero_axes();

protected:
  unsigned int total_state_size;
  Motion_Model *motion_model;
          
  VNL::Vector<double> xv;  // "Истинные" параметры платформы в симуляции
  Simulation_Feature *first_feature;
  Simulation_Feature *last_feature;
  unsigned int no_features;
  unsigned int next_label;         // Для выделения меток истинным признакам
};

/**
@}
*/

#endif
