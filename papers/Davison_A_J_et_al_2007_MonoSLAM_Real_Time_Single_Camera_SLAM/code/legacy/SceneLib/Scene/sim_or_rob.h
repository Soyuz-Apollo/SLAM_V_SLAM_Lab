/*  SceneLib: software for real-time SLAM

    sim_or_rob.h
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

#ifndef _scene_sim_or_rob_h_
#define _scene_sim_or_rob_h_

#include "models_base.h"
#include "identifier.h"
#include "settings.h"

/**
Базовый класс для режимов симуляции или реального робота. Эти классы
предоставляют функциональность инициализации и измерения признаков,
выполнения внутренних измерений и управления платформой.
@ingroup gScene
*/
class Sim_Or_Rob {
public:
  Sim_Or_Rob(const char *s_o_r_type)
    : simulation_or_robot_type(s_o_r_type) 
    {}
  virtual ~Sim_Or_Rob() {}

  /** Выполняет измерение признака.
  @param id Идентификатор этого признака
  @param z Измерение состояния, которое заполняется этой функцией
  @param h Ожидаемое измерение
  @param S Ковариация измерения
  */
  virtual bool measure_feature(Identifier id, VNL::Vector<double> &z, 
                               const VNL::Vector<double> &h,
                               const VNL::Matrix<double> &S) = 0;
  virtual bool set_control(const VNL::Vector<double> &u, 
                           const double delta_t) = 0;
  virtual bool continue_control(const VNL::Vector<double> &u, 
                                const double delta_t) = 0;
  virtual double wait_for_end_of_motion(const VNL::Vector<double> &u) = 0;
  virtual bool stop_vehicle() = 0;
  virtual Identifier initialise_known_feature(
    Feature_Measurement_Model *f_m_m,
    const VNL::Vector<double> &yi,
    const unsigned int known_feature_label) = 0;
  virtual Identifier initialise_known_feature(
    Feature_Measurement_Model *f_m_m,
    const VNL::Vector<double> &yi,
    const Settings::Section& settings) = 0;

  /** Выполняет первое измерение признака.
  @param z Положение признака, который нужно измерить
  @param id Identifier, который нужно заполнить измерениями признака
  @param f_m_m Модель измерения признака, используемая для этого
  частично инициализированного признака
  */
  virtual bool make_initial_measurement_of_feature(
    VNL::Vector<double> &z, 
    Identifier &id,
    Partially_Initialised_Feature_Measurement_Model *f_m_m) = 0;

  // Не чисто виртуальная функция: если метод не переопределён, используется NULL-версия.
  virtual bool make_internal_measurement(Internal_Measurement_Model* 
                                         internal_measurement_model,
                                         VNL::Vector<double> &zv,
                                         const VNL::Vector<double> &hv,
                                         const VNL::Matrix<double> &Sv);

  const char *simulation_or_robot_type;
};

#endif
