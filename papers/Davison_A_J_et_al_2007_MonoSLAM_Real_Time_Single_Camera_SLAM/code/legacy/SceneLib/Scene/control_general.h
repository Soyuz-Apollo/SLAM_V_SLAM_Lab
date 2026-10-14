/*  SceneLib: software for real-time SLAM

    control_general.h
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

#ifndef _scene_control_general_h_
#define _scene_control_general_h_


#include "scene_single.h"
#include "sim_or_rob.h"

// Несколько управляющих функций верхнего уровня.

#if 0
/****************************Инициализировать симуляцию****************************/

void initialise_simulation(Simulation *simulation, const char *true_data_file,
			   Motion_Model *motion_model,
			   std::vector<Feature_Measurement_Model*> 
			     feature_measurement_model_array);
#endif


/******************************Выполнить измерения******************************/

int make_measurements(Scene_Single *scene, Sim_Or_Rob *sim_or_rob);

/**************************Инициализировать известные признаки**************************/

unsigned int initialise_known_features(const Settings& settings,
                              Feature_Measurement_Model_Creator* model_creator,
                              Sim_Or_Rob *sim_or_rob,
                              Scene_Single *scene);
#endif
