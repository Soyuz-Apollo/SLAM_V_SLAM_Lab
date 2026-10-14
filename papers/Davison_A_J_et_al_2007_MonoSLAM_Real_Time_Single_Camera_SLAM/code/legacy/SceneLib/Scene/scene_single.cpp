/*  SceneLib: software for real-time SLAM

    scene_single.cpp
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
#include <VNL/matlabprint.h>
#include "scene_single.h"
#include "settings.h"
#include "exceptions.h"

using namespace std;

#ifdef DEBUGDUMP
#undef DEBUGDUMP
#endif
#define DEBUGDUMP false

/// Нужно ли печатать статусную информацию на каждом кадре?
const bool Scene_Single::STATUSDUMP = false;

/********************************Конструктор*********************************/

/**
Конструктор старого стиля.
Создаёт Scene_Single и задаёт начальное состояние робота.
@param initial_xv Начальное состояние робота
@param initial_Pxx Начальная ковариация состояния робота
@param m_m Используемая модель движения
*/
Scene_Single::Scene_Single(const VNL::Vector<double> &initial_xv, 
                           const VNL::Matrix<double> &initial_Pxx,
                           Motion_Model *m_m)
  : motion_model(m_m)
{
  scene_constructor_bookkeeping(initial_xv, initial_Pxx);
}

/**
Конструктор нового стиля: считывает всё из файла настроек.
Используйте его, если не нужны внутренние модели измерений.
*/
Scene_Single::Scene_Single(Settings& settings, 
			   Motion_Model_Creator* mm_creator)
{
  // Вызываем более общий конструктор с NULL, чтобы показать, что внутренних
  // моделей измерений нет.
  Scene_Single(settings, mm_creator, NULL);
}

/**
Используйте эту версию, если внутренние модели измерений заданы в файле
настроек.
*/
Scene_Single::Scene_Single(Settings& settings, 
			   Motion_Model_Creator *mm_creator,
			   Internal_Measurement_Model_Creator *imm_creator)
{
  // Какая модель движения используется?
  string model = settings.get_entry("Models", "MotionModel");
  
  assert(mm_creator != NULL);
  motion_model = mm_creator->create_model(model);

  if(motion_model == NULL)
  {
    ostringstream os;
    os << "Unable to create a motion model of type \"" << model
        << " as requested in the initalisation file. " << endl;
    throw Scene::InitialisationError(os.str());
  }

  // Инициализируем модель движения её настройками.
  motion_model->read_parameters(settings);
  
  // При необходимости создаём внутренние модели измерений.
  if (imm_creator != NULL) {
    unsigned int imm_number = 0;
    // Считываем и создаём возможные внутренние модели измерений.
    while(1) {
      // Список внутренних моделей измерений задаётся в секции Models настроек
      // через имена записей:
      // InternalMeasurementModel0, InternalMeasurementModel1, etc.
      ostringstream entry_name;
      entry_name << "InternalMeasurementModel" << imm_number;
      string new_imm_type = settings.get_entry("Models", entry_name.str());

      // Выходим из цикла, если внутренних моделей больше нет.
      if (new_imm_type == "")
	break;

      // Инициализируем новую модель и добавляем её в Scene.
      Internal_Measurement_Model *new_imm = 
	imm_creator->create_model(new_imm_type, motion_model);

      // На будущее: на этом этапе, возможно, нужно считывать параметры новой
      // imm. Потенциально может быть несколько внутренних измерений с одной
      // моделью, но разными параметрами, как с несколькими признаками.
      
      add_internal_measurement(new_imm);

      imm_number++;
    }
  }


  // Считываем настройки начального состояния.
  VNL::Vector<> initial_xv;
  VNL::Matrix<> initial_Pxx;
  motion_model->read_initial_state(settings, initial_xv, initial_Pxx);

  scene_constructor_bookkeeping(initial_xv, initial_Pxx);
}


/**
Общая часть всех конструкторов.
*/
void Scene_Single::
scene_constructor_bookkeeping(const VNL::Vector<double> &initial_xv, 
                              const VNL::Matrix<double> &initial_Pxx)
{
  // В будущем чтение этих констант стоит перенести в файл настроек.
  MINIMUM_ATTEMPTED_MEASUREMENTS_OF_FEATURE = 10;
  SUCCESSFUL_MATCH_FRACTION = 0.5;

  xv = initial_xv;
  Pxx = initial_Pxx;

  // Инициализация служебных полей.
  next_free_label = 0;

  marked_feature_label = -1;

  total_state_size = motion_model->STATE_SIZE;

  output_counter = 0;
}

/*****************************Управление признаками******************************/

/**
Возвращает признак с заданной меткой. Если такого признака нет, возвращается
NULL.
*/
Feature *Scene_Single::find_feature_lab(unsigned int lab)
{
  vector<Feature *>::iterator it;
  for (it = feature_list.begin(); it != feature_list.end(); it++) {
    if ((*it)->get_label() == lab)
      return *it;
  }
  // Если дошли сюда, метка не найдена.
  return NULL;
}


/** Создаёт новый частично инициализированный признак. Создаётся новый Feature
и пустой FeatureInitInfo для дополнительной информации инициализации, которую
затем должен заполнить вызывающий код.
@param id Уникальный идентификатор признака, например фрагмент изображения.
@param h Начальное измеренное состояние признака, например положение на
изображении.
@param f_m_m Модель измерения частично инициализированного признака,
применяемая к этому признаку.
@returns Указатель на объект FeatureInitInfo, который нужно заполнить
дополнительной информацией инициализации.
*/
const FeatureInitInfo* Scene_Single::
add_new_partially_initialised_feature(Identifier id, 
  const VNL::Vector<double> &h,
  Partially_Initialised_Feature_Measurement_Model *f_m_m)
{
  Feature *nf = new Feature(id, next_free_label, feature_list.size(), 
                            this, h, f_m_m);

  add_new_feature_bookeeping(nf);

  // Создаём структуру для дополнительной вероятностной информации о частично
  // инициализированном признаке.
  FeatureInitInfo feat(this, nf);
  feature_init_info_vector.push_back(feat);

  return &(feature_init_info_vector.back());
}

void Scene_Single::
add_new_known_feature(Identifier id, const VNL::Vector<double> &y_known, 
                      const VNL::Vector<double> &xp_o,
                      Feature_Measurement_Model *f_m_m,
                      unsigned int known_feature_label)
{
  Feature *nf = new Feature(id, next_free_label, feature_list.size(), 
                            this, y_known, xp_o, f_m_m, known_feature_label);
  add_new_feature_bookeeping(nf);
}

/**
Общая служебная часть для add_new_partially_initialised_feature() и
add_new_known_feature().
*/
void Scene_Single::add_new_feature_bookeeping(Feature *nf)
{
  feature_list.push_back(nf);

  next_free_label++; // Потенциальная проблема в стиле "миллениума", когда
                     // счётчик переполнится. Привет, если читаете это в 3000.

  nf->set_position_in_total_state_vector(total_state_size);
  total_state_size += nf->get_feature_measurement_model()->FEATURE_STATE_SIZE;
}

/**
Удаляет текущий помеченный признак. Признаки можно помечать через
mark_feature_by_lab(). Функция также освобождает идентификатор.
*/
bool Scene_Single::delete_feature()
{
  if (marked_feature_label == -1) {
    cerr << "No feature marked to delete." << endl;
    return false;
  }

  //  Feature *f_ptr, *prev_ptr = NULL;
  
  vector<Feature *>::iterator it_to_delete;
  for (it_to_delete = feature_list.begin(); it_to_delete != feature_list.end();
       it_to_delete++) {
    if ((int) ((*it_to_delete)->get_label()) == marked_feature_label)
      break;
  }

  if (it_to_delete == feature_list.end()) {
    cerr << "Error: marked feature not found in list." << endl;
    return false;
  }

  // Удаляем элементы ковариации, связанные с этим признаком, из последующих
  // признаков.
  for (vector<Feature *>::iterator it = it_to_delete + 1; 
       it != feature_list.end(); it++) {
    (*it)->feature_is_removed((*it_to_delete)->get_position_in_list());
    (*it)->increment_position_in_total_state_vector( 
    -(*it_to_delete)->get_feature_measurement_model()->FEATURE_STATE_SIZE);
  }    

  if ((*it_to_delete)->get_selected_flag())
    deselect_feature(*it_to_delete);

  total_state_size -= (*it_to_delete)->get_feature_measurement_model()->
    FEATURE_STATE_SIZE;

  // Удаляем дополнительные данные, связанные с этим признаком.
  delete (*it_to_delete)->identifier;
  delete (*it_to_delete);
  feature_list.erase(it_to_delete);

  marked_feature_label = -1;  

  return true;
}

// Удалить все признаки с установленным scheduled_for_termination_flag.
void Scene_Single::exterminate_features()
{
  vector<Feature *>::iterator it;
  for (it = feature_list.begin(); it != feature_list.end(); ) {
    if ((*it)->get_scheduled_for_termination_flag()) {
      vector<Feature *>::iterator it_to_delete = it;
      it++;
      // Если удаляется последний признак, нужна особая обработка.
      bool deleting_last_feature_flag = false;
      if (it == feature_list.end())
        deleting_last_feature_flag = true;
      // Сохраняем текущий помеченный признак, чтобы пометить признак,
      // назначенный к удалению. delete_feature удаляет помеченный признак.
      int currently_marked_feature = get_marked_feature_label();
      // Кроме случая, когда это тот самый признак, который сейчас удаляем.
      if (currently_marked_feature == (int) ((*it_to_delete)->get_label()))
        currently_marked_feature = -1;
      mark_feature_by_lab((*it_to_delete)->get_label());
      delete_feature();
      // Теперь помечаем ранее помеченный признак обратно.
      if (currently_marked_feature != -1)
        mark_feature_by_lab(currently_marked_feature);
      if (deleting_last_feature_flag) {
        // Выходим из цикла.
        break;
      }
    }
    else
      it++;
  }
}

/**
Удаляет признаки, у которых постоянно не удаются измерения. Признак удаляется,
если было больше заданного числа попыток сопоставления
(Scene_Single::MINIMUM_ATTEMPTED_MEASUREMENTS_OF_FEATURE), а доля успешных
сопоставлений слишком мала (Scene_Single::SUCCESSFUL_MATCH_FRACTION).
*/
void Scene_Single::delete_bad_features()
{
  vector<Feature *>::iterator it;
  for (it = feature_list.begin(); it != feature_list.end();
       it++) {
    /* Сначала проверяем, нужно ли удалить этот признак. */
    if((*it)->get_attempted_measurements_of_feature() >=
        MINIMUM_ATTEMPTED_MEASUREMENTS_OF_FEATURE &&
        double(((*it)->get_successful_measurements_of_feature())) /
        double(((*it)->get_attempted_measurements_of_feature())) <
          SUCCESSFUL_MATCH_FRACTION)
    {
      if (DEBUGDUMP) cout << "DELETING FEATURE with label " 
        << (*it)->get_label() << ": match rate only "
        << double(((*it)->get_successful_measurements_of_feature())) /
           double(((*it)->get_attempted_measurements_of_feature()))
        << endl;
      (*it)->set_scheduled_for_termination_flag(true);
      continue;
    }
  }

  // Удаляем плохие признаки.
  exterminate_features();
}

/**
Задаёт текущий помеченный признак. Пометка нужна, чтобы указать признак для
удаления через delete_feature(), либо перед вызовом print_marked_feature_state(),
get_marked_feature_state() или get_feature_measurement_model_for_marked_feature().
@param lab Метка признака, начиная с нуля. Значение -1 означает, что ничего не
выбрано.
*/
void Scene_Single::mark_feature_by_lab(int lab)
{
  // Проверяем корректность.
  // Можем ли найти такой признак?
  vector<Feature *>::const_iterator found = feature_list.begin();
  if(lab > 0)
  {
    for( ; found != feature_list.end(); found++)
    {
      if((*found)->get_label() ==
          static_cast<unsigned int>(marked_feature_label));
        break;
    }
  }
  if(found == feature_list.end() && lab != -1)
  {
    cerr << "Attempted to mark feature with unknown label: " << lab << endl;
    return;
  }

  marked_feature_label = lab;
}

/**
Помечает первый признак в списке признаков.
*/
bool Scene_Single::mark_first_feature()
{
  if (feature_list.size() == 0)
    return false;

  mark_feature_by_lab((*feature_list.begin())->get_label());
  return true;
}

/*******************Выбор признаков и измерения************************/

/**
Добавляет этот признак в список выбранных.
@param fp Добавляемый признак.
*/
bool Scene_Single::select_feature(Feature *fp)
{
  if (fp->get_selected_flag() == true)
  {
    if (DEBUGDUMP) cout << "Feature with label " << fp->get_label() 
		      << " is already selected." << endl;
    return true;
  }

  fp->set_selected_flag(true);

  selected_feature_list.push_back(fp);

  return true;
}

/**
Удаляет этот признак из списка выбранных.
@param fp Удаляемый признак.
*/
bool Scene_Single::deselect_feature(Feature *fp)
{ 
  if (fp->selected_flag == false)
  { 
    if (DEBUGDUMP) cout << "Feature with label " << fp->label 
		      << " is already deselected." << endl;
    return true;
  }

  vector<Feature *>::iterator it;
  for (it = selected_feature_list.begin(); it != selected_feature_list.end();
       it++) {
    if (*it == fp)
      break;
  }

  if (it != selected_feature_list.end()) {
    // Нашли признак, удаляем его из списка.
    (*it)->set_selected_flag(false);
    selected_feature_list.erase(it);
    return true;
  }
  else {
    // Не нашли этот признак в списке выбранных.
    if (DEBUGDUMP) cerr << "Error: selected feature not found in list." << endl;
    return false;
  }
}

/**
Переключает выбор признака для выполнения измерений. Вызывается для ручного
выбора или снятия выбора с признака.
@param lab Метка признака, начиная с нуля.
*/
bool Scene_Single::toggle_feature_lab(unsigned int lab)
{
  Feature *fp;

  if (!(fp = find_feature_lab(lab)))
  {
    cerr << "Feature with label " << lab << " not found." << endl;
    return false;
  }

  if (fp->get_selected_flag())
    return deselect_feature(fp);
  else
    return select_feature(fp);
}

bool Scene_Single::auto_select_feature()
{
  unsigned int cant_see_flag;  // Флаг: 0, если признак видим, и другие
			       // значения, если не видим.
  double max_score = 0.0;
  int visible_features = 0;

  Feature *fp_selected = NULL;

  vector<Feature *>::iterator it;
  for (it = feature_list.begin(); it != feature_list.end();
       it++) {
    predict_single_feature_measurements(*it);
    /* Проверяем, видим ли признак. */
    cant_see_flag = (*it)->get_feature_measurement_model()->visibility_test(
                             motion_model->get_xpRES(), (*it)->get_y(), 
			     (*it)->get_xp_orig(), (*it)->get_h());

    /* Признак прошёл проверки видимости. */ 
    if (cant_see_flag == 0)
    {
      visible_features++;
      double score = (*it)->get_feature_measurement_model()->
	selection_score((*it)->get_feature_measurement_model()->get_SiRES());
      if (score > max_score)
      { 
	max_score = score;
	fp_selected = (*it);
      }
    }
  }

  // Снимаем выбор со всех признаков.
  while(selected_feature_list.size() != 0)
    deselect_feature(*selected_feature_list.begin());

  if (DEBUGDUMP) cout << "Best score found: " << max_score << ".\n";

  // Если нашли подходящий признак, выбираем его.
  if (max_score == 0.0)
  {
    if (DEBUGDUMP) cout << "No visible features: find some new ones." << endl;
    return false;
  }
  else 
  {
    select_feature(fp_selected);
    if (DEBUGDUMP) cout << "Auto-selected feature with label " 
		      << fp_selected->label << ".\n";
    return true;
  }
}

// Локальный массив найденных оценок.
class FeatureAndScore {
public:
  FeatureAndScore() {score = 0; fp = NULL;}
  FeatureAndScore(double s, Feature *f) {score = s; fp = f;}
  double score;
  Feature *fp;
};


// Автоматически выбрать n признаков с лучшими оценками.
/**
Автоматически выбирает n признаков с лучшими оценками. Для каждого признака
предсказывается положение, вызывается
Feature_Measurement_Model::selection_score(), после чего выбираются n лучших.
@returns число видимых признаков.
*/
unsigned int Scene_Single::auto_select_n_features(unsigned int n)
{
  unsigned int cant_see_flag;  // Флаг: 0, если признак видим, и другие
                               // значения, если не видим.

  // Снимаем выбор со всех признаков.
  while(selected_feature_list.size() != 0)
    deselect_feature(*selected_feature_list.begin());

  // Вектор до n оценок; самые большие идут первыми.
  vector<FeatureAndScore> feature_and_score_vector;

  vector<Feature *>::iterator it;
  for (it = feature_list.begin(); it != feature_list.end();
       it++)
  {
    if ((*it)->feature_measurement_model->fully_initialised_flag)
    {
      predict_single_feature_measurements(*it);
      /* Проверяем, видим ли признак. */
      cant_see_flag = (*it)->visibility_test();

      /* Признак прошёл проверки видимости. */ 
      if (cant_see_flag == 0)
      {
        double score = (*it)->get_feature_measurement_model()->
          selection_score((*it)->get_feature_measurement_model()->get_SiRES());

        FeatureAndScore fas(score, (*it));

        bool already_added = false;
        for(vector<FeatureAndScore>::iterator fasit =
              feature_and_score_vector.begin();
            fasit != feature_and_score_vector.end(); fasit++)
        {
          if (score > (*fasit).score)
          {
            // Вставляем новый признак перед старым, который он превосходит.
            feature_and_score_vector.insert(fasit, fas);
            already_added = true;
            break;
          }
        }

        if (!already_added)
          feature_and_score_vector.push_back(fas);
      }
    }
  }

  // Смотрим, что получилось.
  unsigned int n_actual = 0;
  if (feature_and_score_vector.size() == 0)
  {
    cerr << "No visible features: find some new ones." << endl;
    return 0;
  }
  else
  {
    for(vector<FeatureAndScore>::iterator fasit = 
      feature_and_score_vector.begin();
      fasit != feature_and_score_vector.end(); fasit++)
    {
      if ((*fasit).score == 0.0 || n_actual == n) 
        return feature_and_score_vector.size();
      else
      {
        select_feature((*fasit).fp);
        if (DEBUGDUMP) cout << "Auto-selected feature with label "
              << (*fasit).fp->label
              << ", score " << (*fasit).score << endl;
        n_actual++;
      }
    }
  }

  // Возвращаем число видимых признаков.
  return feature_and_score_vector.size();
}

void Scene_Single::starting_measurements()
{
  successful_measurement_vector_size = 0;
}

void Scene_Single::failed_measurement_of_feature(Feature *sfp)
{
  if(STATUSDUMP) cout << "Measurement failed of feature with label " 
                      << sfp->label << endl; 
  sfp->set_successful_measurement_flag(false);
  sfp->increment_attempted_measurements_of_feature(1);    
}

void Scene_Single::successful_measurement_of_feature(Feature *sfp)
{
  sfp->set_successful_measurement_flag(true);
  successful_measurement_vector_size += 
    sfp->get_fully_initialised_feature_measurement_model()->MEASUREMENT_SIZE;

  sfp->get_fully_initialised_feature_measurement_model()->
    func_nui(sfp->get_h(), sfp->get_z());
  sfp->set_nu(sfp->get_fully_initialised_feature_measurement_model()
	      ->get_nuiRES());

  sfp->increment_successful_measurements_of_feature(1);
  sfp->increment_attempted_measurements_of_feature(1);
}

void Scene_Single::
construct_total_measurement_stuff(VNL::Vector<double> &nu_tot, 
				  VNL::Matrix<double> &dh_by_dx_tot,
				  VNL::Matrix<double> &R_tot)
{
  unsigned int size = successful_measurement_vector_size;

  assert (nu_tot.Size() == size && 
	  dh_by_dx_tot.Rows() == size && 
	  dh_by_dx_tot.Cols() == total_state_size &&
	  R_tot.Rows() == size && R_tot.Cols() == size);

  nu_tot.Fill(0.0);
  dh_by_dx_tot.Fill(0.0);
  R_tot.Fill(0.0);

  unsigned int vector_position = 0;

  for (vector<Feature *>::iterator it = selected_feature_list.begin();
       it != selected_feature_list.end(); it++) {
    if ((*it)->get_successful_measurement_flag())
    {
      nu_tot.Update((*it)->get_nu(), vector_position);

      dh_by_dx_tot.Update((*it)->get_dh_by_dxv(), vector_position, 0);
      
      dh_by_dx_tot.Update((*it)->get_dh_by_dy(), vector_position, 
			  (*it)->get_position_in_total_state_vector());

      R_tot.Update((*it)->get_R(), vector_position, vector_position);

      vector_position += (*it)->get_feature_measurement_model()->
	                                                  MEASUREMENT_SIZE;
    }
  }
}

/**
Предсказывает положение измерения и ковариацию инновации для каждого выбранного
признака. Для каждого признака из selected_feature_list вызывается
predict_single_feature_measurements().
*/
void Scene_Single::predict_measurements()
{
  for (vector<Feature *>::iterator it = selected_feature_list.begin();
       it != selected_feature_list.end(); it++)
    predict_single_feature_measurements(*it);
}


/**
Для одного признака вычисляет предсказанное измерение и якобианы по положению
платформы и положению признака. Вызывает соответствующие функции Feature, чтобы
задать измерение \f$ \vct{h} \f$, якобиан положения признака
\f$ \partfracv{h}{y} \f$, якобиан положения робота
\f$ \partfrac{\vct{h}}{\vct{x}_v} \f$, ковариацию измерения \f$ \mat{R} \f$ и
ковариацию инновации \f$ \mat{S} \f$.
*/
void Scene_Single::predict_single_feature_measurements(Feature *sfp)
{
  motion_model->func_xp(xv);
  sfp->get_fully_initialised_feature_measurement_model()->
    func_hi_and_dhi_by_dxp_and_dhi_by_dyi(sfp->get_y(),
					  motion_model->get_xpRES());  

  sfp->set_h(sfp->get_fully_initialised_feature_measurement_model()->
	     get_hiRES());
  sfp->set_dh_by_dy(sfp->get_fully_initialised_feature_measurement_model()->
		    get_dhi_by_dyiRES());

  motion_model->func_dxp_by_dxv(xv);
  
  sfp->set_dh_by_dxv(sfp->get_fully_initialised_feature_measurement_model()->
		     get_dhi_by_dxpRES() * motion_model->get_dxp_by_dxvRES());

  sfp->get_fully_initialised_feature_measurement_model()->
    func_Ri(sfp->get_h()); 
  sfp->set_R(sfp->get_fully_initialised_feature_measurement_model()->
	     get_RiRES());

  sfp->get_fully_initialised_feature_measurement_model()->func_Si(Pxx, 
		     sfp->get_Pxy(), sfp->get_Pyy(), 
		     sfp->get_dh_by_dxv(), sfp->get_dh_by_dy(), sfp->get_R());
  sfp->set_S(sfp->get_fully_initialised_feature_measurement_model()->
	     get_SiRES());
}

/*********************Частично инициализированные признаки**************************/

/**
Добавляет частицу в самый новый FeatureInitInfo, то есть в последний созданный
частично инициализированный признак. Просто вызывает
FeatureInitInfo::add_particle().
@param lambda Значения lambda, назначаемые этой частице.
@param probability Начальная вероятность этой частицы.
*/
void Scene_Single::
add_particle_to_newest_partially_init_feature(
  const VNL::Vector<double>& lambda,
  double probability)
{
  // Частица добавляется в последний, самый новый элемент
  // feature_init_info_vector.
  feature_init_info_vector.back().add_particle(lambda, probability);
}


/**
Обновляет состояния измерений частично инициализированных признаков так, чтобы
они соответствовали текущему состоянию робота. Вызывается перед их
сопоставлением в новом кадре. Для каждой Particle в каждом FeatureInitInfo
обновляется состояние измерения \f$ h_{pi} \f$, а также сохраняются обратная
ковариация инновации \f$ S_i^{-1} \f$ и определитель \f$ S_i \f$.
*/
void Scene_Single::predict_partially_initialised_feature_measurements()
{
  vector<FeatureInitInfo>::iterator feat;
  Feature *fp;

  // Получаем текущую позицию.
  motion_model->func_xp(xv);
  VNL::Vector<double> local_xp(motion_model->get_xpRES());
  motion_model->func_dxp_by_dxv(xv);
  VNL::Matrix<double> local_dxp_by_dxv(motion_model->get_dxp_by_dxvRES());

  // Проходим по разным частично инициализированным признакам.
  for (feat = feature_init_info_vector.begin(); 
       feat < feature_init_info_vector.end(); feat++)
  {
    fp = feat->fp;

    Partially_Initialised_Feature_Measurement_Model *pifmm = 
      fp->get_partially_initialised_feature_measurement_model();
    
      // Не пытаемся сопоставлять признак сразу после его инициализации.
    if(feat->number_of_match_attempts++ != 0)
    {
      feat->making_measurement_on_this_step_flag = true;

      if (DEBUGDUMP) cout 
        << "Predicting partially initialised measurements for feature label " 
        << fp->get_label() << endl;

      // Проходим по частицам конкретного признака.
      // Заполняем h, Sinv и detS для предсказанного измерения.
      for (vector<Particle>::iterator part = feat->particle_vector.begin();
           part != feat->particle_vector.end(); part++)
      {
        // Получаем и обновляем состояние измерения для текущего признака
        // с lambda этой частицы из текущей позиции.
        pifmm->func_hpi_and_dhpi_by_dxp_and_dhpi_by_dyi(
          fp->get_y(),
          local_xp,
          part->lambda);
        part->set_h(pifmm->get_hpiRES());

        // Задаём ковариацию измерения по состоянию измерения признака.
        fp->get_feature_measurement_model()->func_Ri(part->get_h());  

        // Затем вычисляем ковариацию инновации: объединяем неопределённости
        // состояния робота, состояния признака и измерения, чтобы получить
        // общую неопределённость в пространстве измерения. Это используется,
        // чтобы определить область изображения для поиска.
        fp->get_feature_measurement_model()->func_Si(
          Pxx, fp->get_Pxy(), fp->get_Pyy(), 
          pifmm->get_dhpi_by_dxpRES() * local_dxp_by_dxv, 
          pifmm->get_dhpi_by_dyiRES(), 
          fp->get_feature_measurement_model()->get_RiRES());

        part->set_S(fp->
          get_feature_measurement_model()->get_SiRES());
      }
    }
    else
    {
      feat->making_measurement_on_this_step_flag = false;
    }
  }
}


/** Обновляет вероятности частично инициализированных признаков после их
измерения. Для каждой Particle в каждом FeatureInitInfo вероятности частиц
обновляются по измерению, то есть по тому, насколько большой была инновация
\f$ \nu_i = z_i - h_i \f$ относительно ковариации инновации \f$ S_i \f$.
Затем вероятности нормализуются. Также вызывается
Feature::prune_particle_vector() для прореживания частиц, а признаки, у которых
неудачны все измерения частиц, удаляются. */
void Scene_Single::
update_partially_initialised_feature_probabilities(
  const double prune_probability_threshold)
{
  // Проходим по разным частично инициализированным признакам.
  for(vector<FeatureInitInfo>::iterator feat =
      feature_init_info_vector.begin();
      feat < feature_init_info_vector.end(); feat++)
  {
    if (feat->making_measurement_on_this_step_flag)
    {
      if (DEBUGDUMP) cout
        << "Updating partially initialised probabilities for feature label "
        << feat->get_fp()->get_label() << endl;

      // Обновляем вероятности.
      for(vector<Particle>::iterator it = feat->particle_vector.begin();
          it != feat->particle_vector.end(); it++)
      {
        double likelihood;

        if (it->get_successful_measurement_flag() == true)
        {
          // Вычисляем значение гауссова распределения.
          VNL::Vector<double> nu = it->get_z() - it->get_h();
          VNL::Vector<double> SInv_times_nu = it->get_SInv() * nu;
          // Скаляр.
          double nuT_Sinv_nu = DotProduct(nu, SInv_times_nu);
          // pux — вероятность того, что истинный признак находится
          // в этой позиции.
          double pux = (1.0 / (sqrt(2.0 * M_PI * it->get_detS())))
            * exp (-0.5 * nuT_Sinv_nu);

          likelihood = pux;
        }
        else
        {
          likelihood = 0.0;
        }

        if (DEBUGDUMP) cout << "Particle at " << it->lambda
                            << " Prior " << it->probability
                            << " Likelihood " << likelihood;

        // Затем применяем правило Байеса.
        it->probability = it->probability * likelihood;

        if (DEBUGDUMP) cout << " Posterior " << it->probability << endl;
      }

      if (feat->normalise_particle_vector_and_calculate_cumulative())
      {
        feat->prune_particle_vector(prune_probability_threshold);

        feat->calculate_mean_and_covariance();
        if (DEBUGDUMP) cout << "Particles " << feat->particle_vector.size()
                            << " Mean " << feat->mean
                            << " variance " << feat->covariance
                            << endl;
      }
      else
      {
        // normalise_particle_vector_and_calculate_cumulative возвращает false,
        // если все вероятности равны нулю, то есть все сопоставления неудачны.
        // В таком случае удаляем признак.
        delete_partially_initialised_feature(feat);
      }
    }
  }
}

/**
Проходит по списку частично инициализированных признаков и удаляет те, у
которых истёк срок ожидания. Признак считается просроченным, если он не
схлопнулся после заданного числа попыток или если у него меньше минимального
числа частиц.
@param erase_partially_init_feature_after_this_many_attempts Число попыток
сопоставления до удаления признака.
@param min_number_of_particles Минимальное число частиц; если частиц меньше,
признак удаляется.
*/
void Scene_Single::delete_partially_initialised_features_past_sell_by_date(
     const unsigned int erase_partially_init_feature_after_this_many_attempts,
     const unsigned int min_number_of_particles)
{
  vector<FeatureInitInfo>::iterator feat = feature_init_info_vector.begin();

  while (feat != feature_init_info_vector.end())
  {
    if(feat->number_of_match_attempts >
         erase_partially_init_feature_after_this_many_attempts ||
       feat->particle_vector.size() <= min_number_of_particles)
    {
      if (DEBUGDUMP) cout << "Deleting feature with label "
                          << feat->fp->get_label() << " after "
                          << feat->number_of_match_attempts
                          << " match attempts." << endl;
      delete_partially_initialised_feature(feat);
    }
    else
    {
      feat++;
    }
  }
}

void Scene_Single::delete_partially_initialised_feature(
  vector<FeatureInitInfo>::iterator feat)
{
  feat->fp->get_identifier();
  // Сохраняем текущий помеченный признак.
  int currently_marked_feature = get_marked_feature_label();
  // Помечаем признак для удаления.
  mark_feature_by_lab(feat->fp->get_label());
  delete_feature();
  feature_init_info_vector.erase(feat);
  if (currently_marked_feature != -1)
    mark_feature_by_lab(currently_marked_feature);
}


/***************************Внутренние измерения*****************************/

void Scene_Single::add_internal_measurement(Internal_Measurement_Model 
					    *internal_measurement_model)
{
  Internal_Measurement *im = 
    new Internal_Measurement(internal_measurement_model);

  internal_measurement_vector.push_back(im);
}

void Scene_Single::predict_internal_measurement(unsigned int i)
{
  Internal_Measurement *im = internal_measurement_vector[i];

  assert(im->internal_measurement_model != NULL);

  im->predict_internal_measurement(xv, Pxx);
}

void Scene_Single::construct_total_internal_measurement_stuff(
				       VNL::Vector<double> &nu_tot, 
				       VNL::Matrix<double> &dh_by_dx_tot,
				       VNL::Matrix<double> &R_tot,
				       unsigned int i)
{
  internal_measurement_vector[i]->
    construct_total_internal_measurement_stuff(nu_tot, 
					       dh_by_dx_tot,
					       R_tot,
					       total_state_size);
}


void Scene_Single::
successful_internal_measurement(const VNL::Vector<double> &zv_in,
				unsigned int i)
{
  internal_measurement_vector[i]->successful_internal_measurement(zv_in);
}

void Scene_Single::failed_internal_measurement(unsigned int i)
{
  internal_measurement_vector[i]->failed_internal_measurement();
}


/*************************Некоторые важные операции***************************/

void Scene_Single::zero_axes()
{
  unsigned int size = get_total_state_size();
  VNL::Vector<double> x(size);
  x.Fill(0.0);
  VNL::Matrix<double> dxnew_by_dxold(size, size);
  dxnew_by_dxold.Fill(0.0);

  // Формируем новое состояние и якобиан нового состояния по старому состоянию.
  unsigned int state_position = 0;
  
  // Часть робота.
  motion_model->func_zeroedxv_and_dzeroedxv_by_dxv(xv);
  x.Update(motion_model->get_zeroedxvRES(), 0);
  dxnew_by_dxold.Update(motion_model->get_dzeroedxv_by_dxvRES(),
			0, 0);

  state_position += motion_model->STATE_SIZE;

  // Каждый признак по очереди.
  unsigned int feature_no = 0;

  // Копируем эти значения на всякий случай.
  motion_model->func_xp(xv);
  VNL::Vector<double> local_xp = motion_model->get_xpRES();
  motion_model->func_dxp_by_dxv(xv);
  VNL::Matrix<double> local_dxp_by_dxv = motion_model->get_dxp_by_dxvRES();

  for (vector<Feature *>::iterator it = feature_list.begin();
       it != feature_list.end(); it++) {
    (*it)->get_feature_measurement_model()->
      func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi((*it)->get_y(), 
							      local_xp);
    x.Update((*it)->get_feature_measurement_model()->get_zeroedyiRES(), 
	     state_position);

    // Вычисляем dzeroedyi_by_dxv в Temp_FS1.
    VNL::Matrix<double> Temp_FS1 = (*it)->get_feature_measurement_model()->
      get_dzeroedyi_by_dxpRES() * local_dxp_by_dxv;

    dxnew_by_dxold.Update(Temp_FS1, state_position, 0);
    dxnew_by_dxold.Update((*it)->get_feature_measurement_model()->
			  get_dzeroedyi_by_dyiRES(), state_position, 
			  state_position);

    // Также нужно переопределить xp_orig для этого признака: это позиция
    // робота, из которой признак был инициализирован в карту. Она используется
    // функциями проверки видимости.
    motion_model->
      func_xpredef_and_dxpredef_by_dxp_and_dxpredef_by_dxpdef(
                                                        (*it)->get_xp_orig(),
							      local_xp);
    (*it)->set_xp_orig((*it)->get_feature_measurement_model()->
		       get_motion_model()->get_xpredefRES());   

    feature_no++;
    state_position += (*it)->get_feature_measurement_model()->
                      FEATURE_STATE_SIZE;
  }

  // Проверяем, что всё посчитано корректно.
  assert (feature_no == feature_list.size() && 
	  state_position == total_state_size);

  // Применяем якобианное преобразование, чтобы получить новую ковариацию.
  VNL::Matrix<double> P(size, size);
  construct_total_covariance(P);

  P = dxnew_by_dxold * P * dxnew_by_dxold.Transpose();

  // В конце загружаем новое состояние и ковариацию в структуры данных сцены.
  fill_states(x);
  fill_covariances(P);
}

void Scene_Single::normalise_state()
{
  // cout << "Normalising state." << endl;

  /* Нормализация состояния:

     Обрабатывает случай, когда состояние робота нужно нормализовать,
     например если оно содержит кватернион.

     Предполагается, что состояния признаков нормализовать не нужно.
  */

  motion_model->func_xvnorm_and_dxvnorm_by_dxv(xv);

  // Изменяем вектор состояния.
  xv.Update(motion_model->get_xvnormRES());

  // Изменяем ковариацию состояния платформы.
  Pxx.Update(motion_model->get_dxvnorm_by_dxvRES() * Pxx * 
	     motion_model->get_dxvnorm_by_dxvRES().Transpose(),
	     0, 0);

  /* Изменяем ковариации между состоянием платформы и состояниями признаков. */
  for (vector<Feature *>::iterator it = feature_list.begin();
       it != feature_list.end(); it++) {
    (*it)->set_Pxy(motion_model->get_dxvnorm_by_dxvRES() * (*it)->get_Pxy());
  }

}

/*******************Работа с полным состоянием и ковариацией*********************/

/**
Создаёт общий вектор состояния и ковариационную матрицу путём объединения
состояний и ковариаций робота и признаков.
@param V Вектор, заполняемый состоянием
@param M Матрица, заполняемая ковариацией
*/
void Scene_Single::
construct_total_state_and_covariance(VNL::Vector<double> &V, 
				     VNL::Matrix<double> &M)
{
  construct_total_state(V);
  construct_total_covariance(M);
}

void Scene_Single::fill_state_and_covariance(const VNL::Vector<double> &V, 
					     const VNL::Matrix<double> &M)
{
  fill_states(V);
  fill_covariances(M);
}

/**
Создаёт общий вектор состояния, объединяя состояние робота $x_v$ и все
состояния признаков $y_i$.
@param V Вектор, заполняемый состоянием
*/
void Scene_Single::construct_total_state(VNL::Vector<double> &V)
{
  assert(V.Size() == total_state_size);

  unsigned int y_position = 0;

  V.Update(xv, y_position);
  y_position += motion_model->STATE_SIZE;

  unsigned int y_feature_no = 0;

  for (vector<Feature *>::iterator it = feature_list.begin(); 
       it != feature_list.end(); it++) {
    V.Update((*it)->get_y(), y_position);
    y_feature_no++;
    y_position += (*it)->get_feature_measurement_model()->FEATURE_STATE_SIZE;
  }

  assert (y_feature_no == feature_list.size() && 
	  y_position == total_state_size);
}

/**
Создаёт общую ковариационную матрицу, объединяя ковариацию робота $P_{xx}$ и
все ковариации признаков $P_{xy_i}$, $P_{y_iy_i}$ и $P_{y_iy_j}$.
@param M Матрица, заполняемая ковариацией
*/
void Scene_Single::construct_total_covariance(VNL::Matrix<double> &M)
{
  assert (M.Rows() == total_state_size && M.Cols() == total_state_size);

  M.Update(Pxx, 0, 0);

  unsigned int x_position = motion_model->STATE_SIZE;

  unsigned int x_feature_no = 0;
  for (vector<Feature *>::iterator it = feature_list.begin();
       it != feature_list.end(); it++)
  {
    unsigned int y_feature_no = 0;
    unsigned int y_position = 0;

    M.Update((*it)->get_Pxy(), y_position, x_position);
    M.Update((*it)->get_Pxy().Transpose(), x_position, y_position);
    y_position += motion_model->STATE_SIZE;

    for(vector<VNL::Matrix<double> >::iterator itmat =
        (*it)->matrix_block_list.begin();
        itmat != (*it)->matrix_block_list.end();
        itmat++)
    {
      assert (y_feature_no < x_feature_no);

      M.Update(*itmat, y_position, x_position);
      M.Update(itmat->Transpose(), x_position, y_position);
      y_position += itmat->Rows();
      y_feature_no++;
    }
    M.Update((*it)->get_Pyy(), y_position, x_position);

    x_feature_no++;
    x_position += (*it)->get_feature_measurement_model()->FEATURE_STATE_SIZE;
  }

  assert (x_position == total_state_size);
}

void Scene_Single::fill_states(const VNL::Vector<double> &V)
{
  assert (V.Size() == total_state_size);

  unsigned int y_position = 0;

  xv.Update(V.Extract(motion_model->STATE_SIZE, y_position));
  y_position += motion_model->STATE_SIZE;
  
  unsigned int y_feature_no = 0;

  for (vector<Feature *>::iterator it = feature_list.begin(); 
       it != feature_list.end() && y_position < V.Size(); it++) {
    (*it)->set_y(
          V.Extract((*it)->get_feature_measurement_model()->FEATURE_STATE_SIZE,
			     y_position));
    y_feature_no++;
    y_position += (*it)->get_feature_measurement_model()->FEATURE_STATE_SIZE;
  }

  assert (y_feature_no <= feature_list.size() && 
	  y_position <= total_state_size);
}

void Scene_Single::fill_covariances(const VNL::Matrix<double> &M)
{
  assert (M.Rows() == total_state_size && M.Cols() == total_state_size);

  Pxx = M.Extract(motion_model->STATE_SIZE, motion_model->STATE_SIZE, 0, 0);

  unsigned int x_position = motion_model->STATE_SIZE;

  unsigned int x_feature_no = 0;
  for (vector<Feature *>::iterator it = feature_list.begin(); 
       it != feature_list.end() && x_position < M.Cols(); it++) {
    unsigned int y_feature_no = 0;
    unsigned int y_position = 0;

    (*it)->set_Pxy(M.Extract(motion_model->STATE_SIZE, 
			  (*it)->feature_measurement_model->FEATURE_STATE_SIZE,
			     y_position, x_position));
    y_position += motion_model->STATE_SIZE;

    for(vector<VNL::Matrix<double> >::iterator itmat = 
	  (*it)->get_matrix_block_list().begin();
	itmat != (*it)->get_matrix_block_list().end();
	itmat++) {
      assert (y_feature_no < x_feature_no);

      itmat->Update(M.Extract(itmat->Rows(), itmat->Cols(), 
			      y_position, x_position));
      y_position += itmat->Rows();
      y_feature_no++;
    }
    (*it)->set_Pyy(
              M.Extract((*it)->feature_measurement_model->FEATURE_STATE_SIZE,
			(*it)->feature_measurement_model->FEATURE_STATE_SIZE,
			     y_position, x_position));

    x_feature_no++;
    x_position += (*it)->feature_measurement_model->FEATURE_STATE_SIZE;
  }

  assert (x_position <= total_state_size);
}

/************************Печать или вывод информации в файл************************/

void Scene_Single::print_current_features()
{
  cout << "\n" << feature_list.size() << " feature" 
       << (feature_list.size() == 1 ? "" : "s") 
       << " currently known about:" << endl;

  for (vector<Feature *>::iterator it = feature_list.begin();
       it != feature_list.end(); it++) {
    cout << "Feature label: " << (*it)->get_label() << endl;
    cout << "State estimate: " << (*it)->get_y();
  }  
}

void Scene_Single::print_selected_features()
{
  cout << "\n" << selected_feature_list.size() << " feature" 
       << (selected_feature_list.size() == 1 ? "" : "s")
       << " currently selected:" << endl;

  for (vector<Feature *>::iterator it = selected_feature_list.begin();
       it != selected_feature_list.end(); it++) {
    cout << "Feature label: "
         << (*it)->get_label() << ". List position: "
	 << (*it)->get_position_in_list() << "." << endl;
  }
}

void Scene_Single::print_covariances()
{
  cout << "Covariance matrix blocks:" << endl;
  cout << "(V, V): " << Pxx;

  unsigned int y_feature_no;
  unsigned int x_feature_no = 0;

  for (vector<Feature *>::iterator it = feature_list.begin();
       it != feature_list.end(); it++) {
    cout << "(V, " << x_feature_no << "): " << endl;
    MatlabPrint(cout, (*it)->get_Pxy());
    
    y_feature_no = 0;
    
    for (vector<VNL::Matrix<double> >::iterator itmat = 
	   (*it)->get_matrix_block_list().begin();
	 itmat != (*it)->get_matrix_block_list().end();
	 itmat++) {
      
      assert(y_feature_no < x_feature_no);
      
      cout << "(" << y_feature_no << ", " << x_feature_no << "): " << endl;
      MatlabPrint(cout, *itmat);
      y_feature_no++;
    }
    cout << "(" << x_feature_no << ", " << x_feature_no << "): " << endl;
    MatlabPrint(cout, (*it)->get_Pyy());

    x_feature_no++;
  }

}

/**
Печатает текущее состояние робота: вектор состояния \f$ x_v \f$ и его
ковариацию \f$ P_{xx} \f$.
*/
void Scene_Single::print_robot_state()
{
  cout << "Robot state" << xv;
  cout << "Robot covariance" << Pxx;
}

void Scene_Single::print_marked_feature_state()
{
  if (marked_feature_label == -1)
  {
    cerr << "No feature marked." << endl;
    return;
  }

  vector<Feature *>::iterator it;
  for (it = feature_list.begin(); it != feature_list.end(); it++) {
     if (int((*it)->get_label()) == marked_feature_label)
      break;
  }

  if (it == feature_list.end())
  {
    cerr << "Error: marked feature not found in list." << endl;
    return;
  }

  cout << "Feature label: "
         << (*it)->get_label() << ". List position: "
	 << (*it)->get_position_in_list() << "." << endl;

  cout << "yi" << (*it)->get_y();
  cout << "Pyiyi" << (*it)->get_Pyy();
}

void Scene_Single::print_whole_state()
{
  cout << "Robot state " << endl;
  MatlabPrint(cout, xv); cout << endl;

  vector<Feature *>::iterator it;
  for (it = feature_list.begin(); it != feature_list.end(); it++) {
    cout << "Feature label: "
         << (*it)->get_label() << ". List position: "
	 << (*it)->get_position_in_list() << "." << endl;

    MatlabPrint(cout, (*it)->get_y()); cout << endl;
  }

  print_covariances();
}

/* Формат: output_counter (label)
           no_features
           вектор состояния
           ковариационная матрица
   Самая общая функция вызывается перегруженными функциями ниже, где меньше
   аргументов. */
void Scene_Single::output_state_to_file(unsigned int counter, const char *filename)
{
  ofstream outfile(filename, ios::app);
  
  unsigned int size = total_state_size;

  VNL::Vector<double> x(size);
  VNL::Matrix<double> P(size, size);

  construct_total_state_and_covariance(x, P);

  outfile << "Step " << counter << endl;

  outfile << "Robot model type " << motion_model->motion_model_type
	  << ", state site " << motion_model->STATE_SIZE << endl;
  outfile << "Robot State" << endl;
  MatlabPrint(outfile, xv);
  outfile << endl << "Robot Covariance" << endl;
  MatlabPrint(outfile, Pxx);

  outfile << "Number of features " << feature_list.size() << endl;
  vector<Feature *>::iterator it;
  for (it = feature_list.begin(); it != feature_list.end();
       it++) {
    outfile << "Feature " << (*it)->get_position_in_list()
	    << " in list, label " << (*it)->get_label() 
	    << ", type " 
	    << (*it)->get_feature_measurement_model()->feature_type 
	    << ", state size " 
	    << (*it)->get_feature_measurement_model()->FEATURE_STATE_SIZE
	    << endl;
    outfile << "Feature State" << endl;
    MatlabPrint (outfile, (*it)->get_y()); 
    outfile << endl << "Feature Covariance" << endl;
    MatlabPrint (outfile, (*it)->get_Pyy());
  }

  outfile << "Total State" << endl;
  MatlabPrint (outfile, x);
  outfile << endl << "Total Covariance" << endl;
  MatlabPrint (outfile, P);

  cout << "State for step " << counter 
       << " output to file " << filename << "." << endl;
}

void Scene_Single::output_state_to_file(unsigned int counter)
{
  // Делаем имя выходного файла зависящим от выбранных признаков.
  // Автор уже не совсем помнит, зачем хотел так сделать.
  char name[100];
  char extra[2];
  strcpy (name, "state_");
  
  extra[1] = '\0';

  for (vector<Feature *>::iterator it = selected_feature_list.begin();
       it != selected_feature_list.end(); it++) {
    if ((*it)->get_position_in_list() > 9)
    {
      cout << "Problem with filename: too many features." << endl;
      break;
    }
    extra[0] = '0' + (*it)->get_position_in_list();
    strcat(name, extra);
  }

  output_state_to_file(counter, name);
}

// Вызов без аргументов использует внутренний счётчик вывода.
void Scene_Single::output_state_to_file()
{
  output_state_to_file(output_counter, "state");

  output_counter++;
}



/******************************Функции доступа*******************************/

/** Заполняет вектор состоянием \f$ \vct{y}_i \f$ текущего помеченного
признака.
@param y_to_fill Вектор, который нужно заполнить состоянием.
@returns <code>false</code>, если сейчас нет помеченного признака или метка
неизвестна; иначе <code>true</code>.
*/
bool Scene_Single::get_marked_feature_state(
  VNL::Vector<double> &y_to_fill) const
{
  if (marked_feature_label == -1)
  {
    cerr << "No feature marked." << endl;
    return false;
  }

  vector<Feature *>::const_iterator it;
  for (it = feature_list.begin(); it != feature_list.end(); it++) {
    if (int((*it)->get_label()) == marked_feature_label)
      break;
  }

  if (it == feature_list.end())
  {
    cerr << "Error: marked feature not found in list." << endl;
    return false;
  }

  y_to_fill.Update((*it)->get_y());

  return true;
}


/** Заполняет вектор состоянием измерения \f$ \vct{h}_i \f$ текущего
помеченного признака.
@param h_to_fill Вектор, который нужно заполнить состоянием.
@returns <code>false</code>, если сейчас нет помеченного признака или метка
неизвестна; иначе <code>true</code>.
*/
bool Scene_Single::get_marked_feature_measurement_state(
  VNL::Vector<double> &h_to_fill) const
{
  if (marked_feature_label == -1)
  {
    cerr << "No feature marked." << endl;
    return false;
  }

  Feature_Measurement_Model* fmm =
      get_feature_measurement_model_for_marked_feature();
  if(!fmm->fully_initialised())
  {
    cerr << "Selected feature is not a fully-initialised feature! " << endl;
    return false;
  }

  Fully_Initialised_Feature_Measurement_Model* fmm2 =
    dynamic_cast<Fully_Initialised_Feature_Measurement_Model*>(fmm);
  if(fmm2 == 0)
  {
    cerr << "Selected feature claims to be a fully-initialised feature, "
         << "but it cannot be converted to one! "
         << "Instead, it is of type " << typeid(*fmm).name();
    return false;
  }
  // Известно, что с этой fmm признаки являются 3-векторами
  // в мировой системе, а измерения признаков — 2-векторами.
  VNL::VectorFixed<3> yi;
  get_marked_feature_state(yi);

  // Где находится камера?
  fmm->get_motion_model()->func_xp(get_xv());
  VNL::Vector<> xp = fmm->get_motion_model()->get_xpRES();
  // Тогда каково положение на изображении?
  fmm2->func_hi_and_dhi_by_dxp_and_dhi_by_dyi(yi, xp);
  h_to_fill = fmm2->get_hiRES();

  return true;
}

/** Возвращает модель измерения текущего помеченного признака.
@returns Модель измерения признака или <code>NULL</code>, если сейчас нет
помеченного признака или метка неизвестна.
*/
Feature_Measurement_Model *Scene_Single::
get_feature_measurement_model_for_marked_feature() const
{
  if (marked_feature_label == -1)
  {
    cerr << "No feature marked." << endl;
    return NULL;
  }

  vector<Feature *>::const_iterator it;
  for (it = feature_list.begin(); it != feature_list.end(); it++) {
    if (int((*it)->get_label()) == marked_feature_label)
      break;
  }
  if (it == feature_list.end())
  {
    cerr << "Error: marked feature not found in list." << endl;
    return NULL;
  }

  return (*it)->feature_measurement_model;
}
