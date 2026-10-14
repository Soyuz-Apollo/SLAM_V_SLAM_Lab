/*  SceneLib: software for real-time SLAM

    control_general.cpp
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

// Набор общих функций, работающих с основными классами. Эти функции
// используются во многих приложениях, поэтому они сгруппированы здесь.

#include "VW/Utils/timer.h"
#include "control_general.h"
#include <iostream>

#include "exceptions.h"

using namespace std;

/**
@addtogroup gScene
@{
*/

#if 0
/****************************Инициализировать симуляцию****************************/
void initialise_simulation(Simulation *simulation, const char *true_data_file,
                           Motion_Model *motion_model,
                           std::vector<Feature_Measurement_Model*>
                           feature_measurement_model_array);
{
  ifstream infile(true_data_file, ios::in);
  assert(infile != (void *) NULL);

  char type_buf[100];

  // Считываем начальное состояние платформы.
  infile >> type_buf;
  assert(strcmp(type_buf, motion_model->motion_model_type) == 0);

  VNL::Vector<double> initial_xv(motion_model->STATE_SIZE);
  for (unsigned int i = 0; i < motion_model->STATE_SIZE; i++)
    infile >> initial_xv(i);
  simulation = new Simulation(motion_model, initial_xv);

  // Считываем из файла точки сцены.
  while (infile)
  {
    infile >> type_buf;
    if (!infile) break;

    Feature_Measurement_Model *f_m_m = find_feature_measurement_model(type_buf,
                                  feature_measurement_model_array);

    VNL::Vector<double> initial_yi(f_m_m->FEATURE_STATE_SIZE);
    for (unsigned int i = 0; i < f_m_m->FEATURE_STATE_SIZE; i++)
      infile >> initial_yi(i);

    simulation->add_new_true_feature(f_m_m, initial_yi);
  }

}
#endif

/******************************Инициализировать сцену*******************************/


/******************************Выполнить измерения******************************/

/**
Выполняет измерения всех выбранных сейчас признаков. Признаки можно выбрать
через Scene_Single::auto_select_n_features() или вручную через
Scene_Single::select_feature(). Функция вызывает
Scene_Single::starting_measurements(), а затем Sim_Or_Rob::measure_feature()
для каждого выбранного признака. Для каждого признака, для которого была
сделана попытка измерения, обновляются счётчики
Feature::attempted_measurements_of_feature и
Feature::successful_measurements_of_feature.
@param scene Используемая SLAM-карта
@param sim_or_rob Класс, используемый для измерения признаков
@returns Количество успешно измеренных признаков.
*/
int make_measurements(Scene_Single *scene, Sim_Or_Rob *sim_or_rob)
{
  int count = 0;
  if(scene->get_no_selected() == 0)
  {
    cerr << "No features selected." << endl;
    return 0;
  }

  scene->starting_measurements(); 

  for(vector<Feature *>::const_iterator it =
      scene->get_selected_feature_list().begin();
      it != scene->get_selected_feature_list().end(); it++)
  {
    if(sim_or_rob->measure_feature((*it)->get_identifier(),
       (*it)->get_z_noconst(),
       (*it)->get_h(), (*it)->get_S()) == false)
    {
      scene->failed_measurement_of_feature((*it));
    }
    else
    {
      scene->successful_measurement_of_feature((*it)); 
      count ++;
    }
  }

  return count;
}

/**************************Инициализировать известные признаки**************************/


/** 
Инициализирует класс Scene_Single набором известных признаков, считанных из
Settings. Каждый известный признак имеет собственную секцию, начиная с
<code>[KnownFeature1]</code> и далее по возрастанию. Тип признака задаётся
записью <code>FeatureMeasurementModel=</code>. Остальные настройки загружает
сама модель измерения признака.
**/
unsigned int initialise_known_features(const Settings& settings,
                              Feature_Measurement_Model_Creator* model_creator,
                              Sim_Or_Rob *sim_or_rob,
                              Scene_Single *scene)
{
  unsigned int feature_no = 1;
  unsigned int num_features = 0;
  const Settings::Section* section = 0;
  do
  {
    // Проходим по именам секций.
    ostringstream section_name;
    section_name << "KnownFeature" << feature_no;
    section = settings.get_section(section_name.str());
    // Существует ли такая секция?
    if(section == 0)
    {
      return num_features;
    }

    string type = section->get_entry("FeatureMeasurementModel");
    if(type == "")
    {
      cerr << "No FeatureMeasurementModel entry under the section ["
           << section_name.str() << "] in initalisation file." << endl;
    }
    else
    {
      Feature_Measurement_Model *f_m_m =
        model_creator->create_model(type, scene->get_motion_model());
      if(f_m_m == 0)
      {
        cerr << "Unable to create a feature measurement model of type \"" 
            << type << "\" as requested in initalisation file." << endl;
      }
      else
      {
        // Инициализируем модель измерения признака её настройками.
        f_m_m->read_parameters(settings);
        // Считываем состояние признака.
        VNL::Vector<> yi;
        VNL::Vector<> xp_orig;
        f_m_m->read_initial_state(*section, yi, xp_orig);
  
        // Инициализируем признак.
        Identifier identifier = 
          sim_or_rob->initialise_known_feature(f_m_m, yi, *section);
        if (identifier == NULL)
        {
          cerr << "Trouble reading known feature " 
              << section_name.str() << " : skipping." << endl;
        }
        else
        {
          scene->add_new_known_feature(identifier, yi, xp_orig, f_m_m, 
                                      feature_no);
          cout << "Added known feature " << feature_no << endl;
          num_features++;
        }
      }
    }
    feature_no++;
  }
  while(section != 0);

  return num_features;
}

/**
@}
*/
