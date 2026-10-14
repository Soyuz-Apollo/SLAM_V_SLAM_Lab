/*  SceneLib: software for real-time SLAM

    simul.cpp
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

#include "simul.h"

using namespace std;

Simulation_Feature::Simulation_Feature(const unsigned int lab, 
				       Feature_Measurement_Model *f_m_m,
				       const VNL::Vector<double> &yi_passed)
  : feature_measurement_model(f_m_m)
{
  label = lab;
  yi.Update(yi_passed);
  next = NULL;
}

// Конструктор.
Simulation::Simulation(Motion_Model *m_m, const VNL::Vector<double> &initial_xv)
  : Sim_Or_Rob("SIMULATION"), motion_model(m_m)
{
  xv = initial_xv;

  total_state_size = motion_model->STATE_SIZE;

  no_features = 0;
  next_label = 0;
  first_feature = NULL;
  last_feature = NULL;
}

// Деструктор.
Simulation::~Simulation()
{

}

/*******************************Измерения**********************************/

bool Simulation::measure_feature(Identifier id, VNL::Vector<double> &z, 
				 const VNL::Vector<double> &, 
				 const VNL::Matrix<double> &)
{
  Simulation_Feature *sfp;

  for (sfp = first_feature; sfp; sfp = sfp->next)
    if (&(sfp->label) == (unsigned int *)(id))
      break;

  // h и S здесь не используются в симуляции.
  
  Fully_Initialised_Feature_Measurement_Model *fifmm = 
    (Fully_Initialised_Feature_Measurement_Model *)
    sfp->feature_measurement_model;

  motion_model->func_xp(xv);
  fifmm->func_hi_noisy(sfp->yi, motion_model->get_xpRES());

  z.Update(fifmm->get_hi_noisyRES());

  return 0;
}

/*****************************Движение платформы********************************/

bool Simulation::set_control(const VNL::Vector<double> &u, const double delta_t)
{
  motion_model->func_fv_noisy(xv, u, delta_t);
  
  xv.Update(motion_model->get_fv_noisyRES());

  return true;
}

/* В симуляции continue_control делает ровно то же самое, что и set_control. */
bool Simulation::continue_control(const VNL::Vector<double> &u, const double delta_t)
{
  set_control(u, delta_t);

  return true;
}

// Ожидание завершения движения и остановка платформы: в симуляции это
// фиктивные функции.
double Simulation::wait_for_end_of_motion(const VNL::Vector<double> &)
{
  return 0.0;
}

bool Simulation::stop_vehicle()
{
  return true;
}

Identifier Simulation::initialise_known_feature(Feature_Measurement_Model *f_m_m,
					   const VNL::Vector<double> &yi,
					   const unsigned int)
{
  unsigned int true_label = add_new_true_feature(f_m_m, yi);
  return (Identifier ) find_identifier(true_label);
}

bool Simulation::make_initial_measurement_of_feature(VNL::Vector<double> &z, 
						    Identifier &id, 
				      	  Feature_Measurement_Model *)
{
  VNL::Matrix<double> dummy_S;
  measure_feature(id, z, z, dummy_S);

  return true;
}

// Выполняем внутреннее измерение согласно модели внутреннего измерения.
bool Simulation::make_internal_measurement(Internal_Measurement_Model 
      *internal_measurement_model, VNL::Vector<double> &zv, const VNL::Vector<double> &, 
	     const VNL::Matrix<double> &)
{
  internal_measurement_model->func_hv_noisy(xv);

  zv.Update(internal_measurement_model->get_hv_noisyRES());

  return true;
}


// Находим идентификатор признака с заданной меткой.
// Identifier — это просто указатель на эту метку.
Identifier Simulation::find_identifier(unsigned int label)
{
  for (Simulation_Feature *sfp = first_feature; sfp; sfp = sfp->next)
    if (sfp->label == label)
      return (Identifier)((void *)(&(sfp->label)));
      
  cerr << "Error with Simulation::find_identifier()." << endl;
  return NULL;
}


// Добавляем признак.
unsigned int Simulation::add_new_true_feature(Feature_Measurement_Model *f_m_m,
				     const VNL::Vector<double> &yi)
{
  Simulation_Feature *s = new Simulation_Feature(next_label, 
						 f_m_m, yi);

  if(first_feature == NULL)
    first_feature = s;
  else
    last_feature->next = s;
  
  last_feature = s;
  
  no_features++;
  total_state_size += f_m_m->FEATURE_STATE_SIZE;

  return next_label++;
}

/*****************************Функции вывода********************************/

// Функция для вывода истинных точек сцены.
void Simulation::print_true_features()
{
  cout << "Number of features: " << no_features << endl;

  for (Simulation_Feature *sf = first_feature; sf; sf = sf->next)
    cout << "Feature " << sf->label << " has structure:" 
	 << sf->yi;
}

// Функция для вывода положения платформы.
void Simulation::print_true_vehicle()
{
  cout << "True vehicle state:" << xv;
}

/*******************************Обнулить оси*************************************/

/* Обнуляет систему координат так, чтобы платформа оказалась в центре. */
void Simulation::zero_axes()
{
  motion_model->func_xp(xv);
  VNL::Vector<double> local_xp = motion_model->get_xpRES();

  // Проходим по признакам.
  for (Simulation_Feature *sfp = first_feature; sfp; sfp = sfp->next)
  { 
    sfp->feature_measurement_model->
        func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi(sfp->yi, 
								local_xp);
    sfp->yi.Update(sfp->feature_measurement_model->get_zeroedyiRES());
  }

  // Состояние робота обновляем последним.
  motion_model->func_zeroedxv_and_dzeroedxv_by_dxv(xv);
  xv.Update(motion_model->get_zeroedxvRES());
}

/******************Функции для загрузки и выгрузки состояния******************/

bool Simulation::construct_total_true_state(VNL::Vector<double> &V)
{
  unsigned int y_position = 0;

  V.Update(xv, y_position);
  y_position += motion_model->STATE_SIZE;

  unsigned int y_feature_no = 0;

  for (Simulation_Feature *f = first_feature; f; f = f->next)
  {
    V.Update(f->yi, y_position);
    y_feature_no++;
    y_position += f->feature_measurement_model->FEATURE_STATE_SIZE;
  }

  assert (y_feature_no == no_features && y_position == total_state_size);

  return 0;
}

bool Simulation::fill_true_state(const VNL::Vector<double> &V)
{
  unsigned int y_position = 0;

  xv.Update(V.Extract(motion_model->STATE_SIZE, y_position));
  y_position += motion_model->STATE_SIZE;

  unsigned int y_feature_no = 0;

  for (Simulation_Feature *f = first_feature; f; f = f->next)
  {
    f->yi.Update(V.Extract(f->feature_measurement_model->FEATURE_STATE_SIZE, 
			   y_position));
    y_feature_no++;
    y_position += f->feature_measurement_model->FEATURE_STATE_SIZE;
  }

  assert (y_feature_no == no_features && y_position == total_state_size);

  return 0;
}
