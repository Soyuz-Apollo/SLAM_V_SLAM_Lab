/*  SceneLib: software for real-time SLAM

    scene_single.h
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

#ifndef _scene_scene_single_h_
#define _scene_scene_single_h_

#include "internal_measurement.h"
#include "feature.h"
#include "init_feature.h"
#include "vector.h"

class Settings;

/**
Класс для управления одной SLAM-картой со связанной ковариационной матрицей.
@ingroup gScene
**/
// Поля данных находятся в нижней части класса.
class Scene_Single 
{
  friend class Kalman;  // Некрасиво, но пока с этим живём.

  /******************************Конструктор*********************************/ 
 public:
  Scene_Single(const VNL::Vector<double> &initial_xv, 
    const VNL::Matrix<double> &initial_Pxx,
    Motion_Model *m_m);
  // Основной конструктор: считывает всё из файла настроек.
  Scene_Single(Settings& settings, Motion_Model_Creator *mm_creator);
  // Использовать, если внутренние модели измерений тоже считываются из настроек.
  Scene_Single(Settings& settings, Motion_Model_Creator *mm_creator,
	       Internal_Measurement_Model_Creator *imm_creator);
  
 protected:
  // Внутренняя функция, используемая конструкторами.
  void scene_constructor_bookkeeping(const VNL::Vector<double> &initial_xv,
    const VNL::Matrix<double> &initial_Pxx);

  /***************************Управление признаками******************************/
 public:
  // Поиск признаков в списке.
  Feature *find_feature_lab(unsigned int lab);
  const Feature *find_feature_lab(unsigned int lab) const
    {return const_cast<Scene_Single*>(this)->find_feature_lab(lab);}

  // Добавление признаков.
  const FeatureInitInfo* add_new_partially_initialised_feature(
    Identifier id, 
    const VNL::Vector<double> &h, 
    Partially_Initialised_Feature_Measurement_Model *f_m_m);
  void add_new_known_feature(
    Identifier id, 
    const VNL::Vector<double> &y_known, 
    const VNL::Vector<double> &xp_o, 
    Feature_Measurement_Model *f_m_m,
    unsigned int known_feature_label);

 protected:
  // Функция только для внутреннего использования.
  void add_new_feature_bookeeping(Feature *nf);

 public:
  // Удалить текущий помеченный признак.
  bool delete_feature();
  void exterminate_features();
  
  // Автоматически удалять признаки с неудачными измерениями.
  /** Число попыток сопоставления, после которого признак рассматривается для
  автоматического удаления через delete_bad_features() (по умолчанию 10). */
  unsigned int MINIMUM_ATTEMPTED_MEASUREMENTS_OF_FEATURE;
  /** Доля неудачных попыток сопоставления, после которой признак
  автоматически удаляется в delete_bad_features() (по умолчанию 0.5). */
  double SUCCESSFUL_MATCH_FRACTION;
  void delete_bad_features();

  void mark_feature_by_lab(int lab);
  bool mark_first_feature();

  /******************Выбор признаков и измерения***********************/
 public:
  // Выбор и снятие выбора с признаков.
  bool select_feature(Feature *fp);
  bool deselect_feature(Feature *fp);
  bool toggle_feature_lab(unsigned int lab);

  // Автоматический выбор признаков.
  bool auto_select_feature();
  // Эта функция теперь возвращает число ВИДИМЫХ признаков.
  unsigned int auto_select_n_features(unsigned int n);

  // Служебный учёт при выполнении измерений.
  void starting_measurements();
  void failed_measurement_of_feature(Feature *fp);
  void successful_measurement_of_feature(Feature *fp);

  // Формирование матриц для шага обновления по измерениям.
  void construct_total_measurement_stuff(VNL::Vector<double> &nu_tot, 
                                         VNL::Matrix<double> &dh_by_dx_tot,
                                          VNL::Matrix<double> &R);
  void predict_measurements();
  void predict_single_feature_measurements(Feature *sfp);

  /*******************Частично инициализированные признаки*************************/
 public:
  // Информация о частично инициализированных признаках хранится в
  // feature_init_info_vector.

  // Добавить частицу в распределение последнего добавленного частично
  // инициализированного признака.
  void add_particle_to_newest_partially_init_feature
                 (const VNL::Vector<double> &lambda, double probability);

  void predict_partially_initialised_feature_measurements();

  // Обновление после измерения.
  void update_partially_initialised_feature_probabilities(
    const double 
    prune_probability_threshold);

  void delete_partially_initialised_features_past_sell_by_date(
     const unsigned int erase_partially_init_feature_after_this_many_attempts,
     const unsigned int min_number_of_particles);

  // Служебная функция, выполняющая всё, что связано с удалением частично
  // инициализированного признака.
  void delete_partially_initialised_feature(
    std::vector<FeatureInitInfo>::iterator feat);

  /***********************Внутренние измерения******************************/
 public:
  // Внутренние измерения напрямую относятся к состоянию робота и не используют
  // признаки, например измерения от инерциальных датчиков.

  // Добавить новый тип внутреннего измерения, зарегистрировав модель.
  void add_internal_measurement(
  Internal_Measurement_Model* internal_measurement_model);
  void predict_internal_measurement(unsigned int i);
  Internal_Measurement_Model* get_internal_measurement_model(
    unsigned int i) const 
    {return internal_measurement_vector[i]->internal_measurement_model;}

  void construct_total_internal_measurement_stuff(
    VNL::Vector<double> &nu_tot,
    VNL::Matrix<double> &dh_by_dx_tot,
    VNL::Matrix<double> &R_tot,
    unsigned int i);

  void successful_internal_measurement(const VNL::Vector<double> &zv_in,
                                       unsigned int i);
  void failed_internal_measurement(unsigned int i);
  bool get_successful_internal_measurement_flag(unsigned int i) const 
    {return internal_measurement_vector[i]->
       successful_internal_measurement_flag;}

  /************************Некоторые важные операции**************************/
 public:
  // Обнулить систему координат в текущей позиции робота.
  void zero_axes();
  // Нормализовать состояние, если состояние робота задано не минимальной
  // параметризацией, например содержит кватернион.
  void normalise_state();

  /******************Работа с полным состоянием и ковариацией********************/
 public:
  void construct_total_state_and_covariance(VNL::Vector<double> &V, 
                                            VNL::Matrix<double> &M);
  void fill_state_and_covariance(const VNL::Vector<double> &V, 
                                 const VNL::Matrix<double> &M);

  void construct_total_state(VNL::Vector<double> &V);
  void construct_total_covariance(VNL::Matrix<double> &M);
  void fill_states(const VNL::Vector<double> &V);
  void fill_covariances(const VNL::Matrix<double> &M);

  /***********************Печать или вывод информации в файл***********************/
 public:
  // Вывод информации о текущих признаках.
  void print_current_features();
  void print_selected_features();
  void print_covariances();
  void print_robot_state();
  void print_marked_feature_state();
  void print_whole_state();
  void output_state_to_file(unsigned int counter, const char *filename);
  void output_state_to_file(unsigned int counter);
  void output_state_to_file();


  /*****************************Функции доступа******************************/
 public:
  Motion_Model *get_motion_model() const {return motion_model;}

  /// Возвращает состояние робота \f$ x_v \f$.
  const VNL::Vector<double> &get_xv() const {return xv;}
  /// Возвращает состояние робота \f$ x_v \f$; неконстантная версия позволяет
  /// редактировать состояние.
  VNL::Vector<double> &get_xv_noconst() {return xv;}
  /// Возвращает ковариацию состояния робота \f$ P_{xx} \f$.
  const VNL::Matrix<double> &get_Pxx() const {return Pxx;}

  /** Возвращает список всех признаков в системе: инициализированных и частично
  инициализированных. */
  const std::vector<Feature *> &get_feature_list() const
    {return feature_list;}
  /** Возвращает список всех признаков в системе: инициализированных и частично
  инициализированных. */
  std::vector<Feature *> &get_feature_list_noconst()
    {return feature_list;}
  /** Возвращает список выбранных признаков. */
  const std::vector<Feature *> &get_selected_feature_list() const
    {return selected_feature_list;}
  std::vector<Feature *> &get_selected_feature_list_noconst()
    {return selected_feature_list;}
  const std::vector<FeatureInitInfo> &get_feature_init_info_vector() const
    {return feature_init_info_vector;}
  std::vector<FeatureInitInfo> &get_feature_init_info_vector_noconst()
    {return feature_init_info_vector;}

  unsigned int get_no_selected() const {return selected_feature_list.size();}
  unsigned int get_no_features() const {return feature_list.size();}

  unsigned int get_total_state_size() const {return total_state_size;}
  void increment_total_state_size(int increment)
    {total_state_size += increment;}
  unsigned int get_successful_measurement_vector_size() 
                             const {return successful_measurement_vector_size;}

  void set_successful_measurement_vector_size(unsigned int smvs) 
                             {successful_measurement_vector_size = smvs;}

  /**
  Возвращает метку текущего помеченного признака. Пометка признака используется,
  чтобы указать признак для удаления через delete_feature(), либо перед вызовом
  print_marked_feature_state(), get_marked_feature_state() или
  get_feature_measurement_model_for_marked_feature().
  @returns Метка текущего помеченного признака или -1, если признак не выбран.
  */
  int get_marked_feature_label() const
    {return marked_feature_label;}

  bool get_marked_feature_state(VNL::Vector<double> &y_to_fill) const;
  bool get_marked_feature_measurement_state(VNL::Vector<double> &h_to_fill)
    const;

  Feature_Measurement_Model
    *get_feature_measurement_model_for_marked_feature() const;

  const VNL::Vector<double> &get_hv(unsigned int i) const 
    {return internal_measurement_vector[i]->hv;}
  const VNL::Vector<double> &get_zv(unsigned int i) const
    {return internal_measurement_vector[i]->zv;}
  const VNL::Matrix<double> &get_Sv(unsigned int i) const 
    {return internal_measurement_vector[i]->Sv;}


  /*******************************Поля данных********************************/
 protected:
  Motion_Model *motion_model;

  /** 
  Состояние робота \f$x_v\f$ в том виде, в котором его использует модель
  движения. Полное состояние системы, то есть робот и признаки, не хранится в
  Scene явно, а собирается при необходимости через
  construct_total_state_and_covariance().
  **/
  VNL::Vector<double> xv; 
  /// Ковариация состояния робота \f$P_{xx}\f$, используемая моделью движения.
  VNL::Matrix<double> Pxx; 

  // Списки указателей на признаки.
  // feature_list содержит все признаки.
  std::vector<Feature*> feature_list;
  // selected_feature_list содержит только признаки, выбранные сейчас для
  // измерения.
  std::vector<Feature *> selected_feature_list;

  // Отдельный список частично инициализированных признаков с дополнительной
  // вероятностной информацией. Эти признаки также есть в основном feature_list.
  std::vector <FeatureInitInfo> feature_init_info_vector;

  // Размер полного вектора состояния, включая робота и признаки.
  unsigned int total_state_size;
  // Размер последнего вектора измерений.
  unsigned int successful_measurement_vector_size;

  // Уникальные метки для новых признаков.
  unsigned int next_free_label; 

  // Помеченный признак: для удаления, обхода и т.п.
  int marked_feature_label; /* Метка последнего признака, выбранного мышью.
                               Тип signed, потому что -1 означает отсутствие
                               помеченного признака. */

  // Вектор пронумерованных моделей внутренних измерений.
  std::vector <Internal_Measurement *> internal_measurement_vector;

  // Счётчик для вывода состояния в текстовый файл.
  unsigned int output_counter;

public:
  static const bool STATUSDUMP;
};

#endif
