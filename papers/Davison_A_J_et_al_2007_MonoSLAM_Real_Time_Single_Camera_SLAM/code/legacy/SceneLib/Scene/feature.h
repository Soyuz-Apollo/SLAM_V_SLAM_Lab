/*  Scene: software for real-time SLAM

    feature.h
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

#ifndef _scene_feature_h_
#define _scene_feature_h_

#include <vector>
#include "models_base.h"
#include "identifier.h"
#include "init_feature.h"


/**
Класс для хранения информации о признаке. В нём лежат Identifier признака, его
состояние, ковариации, текущее измерение и предсказание измерения. Этот класс
используется и для полностью, и для частично инициализированных признаков. Если
признак инициализирован только частично, дополнительная информация о состоянии
(например, частицы для свободных параметров) хранится в отдельном классе
FeatureInitInfo, который указывает на этот Feature. Класс также умеет
преобразовывать частично инициализированный признак в полностью
инициализированный.
@ingroup gScene
*/
class Feature
{
  friend class Scene_Single;
  friend class Kalman;

  /*******************************Конструкторы********************************/
 public:
  // Частично инициализированный признак. При первом измерении он задаётся
  // в более простой форме: например, точка, увиденная монокулярной камерой,
  // сначала инициализируется как луч/линия наблюдения.
  Feature(Identifier id, unsigned int lab, unsigned int list_pos, 
          Scene_Single *scene,  const VNL::Vector<double> &h,
          Partially_Initialised_Feature_Measurement_Model *partial_init_fmm);
  // Известный признак: инициализируется сразу заданным состоянием.
  Feature(Identifier id, unsigned int lab, unsigned int list_pos, 
          Scene_Single *scene, const VNL::Vector<double> &y_known,
          const VNL::Vector<double> &xp_o,
          Feature_Measurement_Model *f_m_m, unsigned int k_f_l);
  /// Деструктор. Ничего не делает.
  ~Feature() {};

 protected:
  // Общая часть инициализации, используемая несколькими конструкторами.
  void feature_constructor_bookeeping();

  /**************************Управление признаками******************************/
 public:
  // Удаляет матричные блоки, когда из списка удаляется другой признак.
  void feature_is_removed(unsigned int list_pos);

  // Преобразует частично инициализированный признак в полностью
  // инициализированный. На вход передаются найденный вектор свободных
  // параметров lambda и его ковариация.
  void convert_from_partially_to_fully_initialised(
    const VNL::Vector<double> &lambda,
    const VNL::Matrix<double> &Plambda,
    Scene_Single *scene);

  /*****************************Функции доступа******************************/
 public:
  /// Возвращает модель измерения для этого признака.
  Feature_Measurement_Model* get_feature_measurement_model()
                                       {return feature_measurement_model;}
  /** Возвращает модель измерения полностью инициализированного признака.
  Если признак сейчас инициализирован только частично, здесь будет NULL.*/
  Fully_Initialised_Feature_Measurement_Model
    *get_fully_initialised_feature_measurement_model() 
    {return fully_initialised_feature_measurement_model;}
  /** Возвращает модель измерения частично инициализированного признака.
  Если признак уже полностью инициализирован, здесь будет NULL.*/
  Partially_Initialised_Feature_Measurement_Model 
    *get_partially_initialised_feature_measurement_model() 
    {return partially_initialised_feature_measurement_model;}

  /** Возвращает состояние признака \f$ \vct{y} \f$,
  например его 3D-положение.
  */
  const VNL::Vector<double> &get_y() const {return y;}
  /** Задаёт состояние признака \f$ \vct{y} \f$,
  например его 3D-положение. */
  void set_y(const VNL::Vector<double> & new_y)
    {y.Update(new_y);}
  /** Возвращает ковариацию состояния признака \f$ \mat{P}_{yy} \f$. */
  const VNL::Matrix<double> &get_Pyy() const {return Pyy;}
  /** Задаёт ковариацию состояния признака \f$ \mat{P}_{yy} \f$. */
  void set_Pyy(const VNL::Matrix<double> &new_Pyy)
    {Pyy.Update(new_Pyy);}
  /** Возвращает ковариацию \f$ \mat{P}_{xy} \f$ между состоянием признака
  и состоянием робота. */
  const VNL::Matrix<double> &get_Pxy() const {return Pxy;}
  /** Задаёт ковариацию \f$ \mat{P}_{xy} \f$ между состоянием признака
  и состоянием робота. */
  void set_Pxy(const VNL::Matrix<double> &new_Pxy)
    {Pxy.Update(new_Pxy);}
  /** Возвращает позиционную часть состояния робота \f$ \vct{x}_p \f$, из
  которой признак был впервые увиден. Это нужно, чтобы понять, можно ли ещё
  пытаться сопоставлять этот признак из текущей позиции робота. Например, в
  visual SLAM угол обзора мог измениться слишком сильно. */
  const VNL::Vector<double> &get_xp_orig() const {return xp_orig;}
  /** Задаёт позиционную часть состояния робота \f$ \vct{x}_p \f$, из которой
  признак был впервые увиден.*/
  void set_xp_orig(const VNL::Vector<double> &new_xp_orig)
    {xp_orig.Update(new_xp_orig);}

  /** Возвращает ковариационные матрицы между этим признаком и всеми
  предыдущими признаками в системе. Каждая матрица в списке представляет
  \f$ \mat{P}_{y_iy_j} \f$ между этим и другим признаком. Столбцы полной
  ковариационной матрицы состояния, соответствующие текущему признаку
  \f$ \vct{y}_n \f$, имеют вид
  \f[
  \emat{\mat{P}_{xy_n} \\ \mat{P}_{y_1y_n} \\ \mat{P}_{y_2y_n} \\ \vdots \\
  \mat{P}_{y_ny_n} }
  \f]
  где \f$ \mat{P}_{xy_n} \f$ берётся из get_Pxy(),
  \f$ \mat{P}_{y_ny_n} \f$ — из get_Pyy(), а матрицы
  \f$ \mat{P}_{y_iy_n} \f$ — из get_matrix_block_list(). Соответствующие
  строки полной ковариационной матрицы состояния получаются транспонированием
  этих блоков.*/
  std::vector<VNL::Matrix<double> > &get_matrix_block_list()
    {return matrix_block_list;}
  /** const-версия функции выше. */
  const std::vector<VNL::Matrix<double> > &get_matrix_block_list() const
    {return matrix_block_list;}

  /** Возвращает предсказанное состояние измерения признака \f$ \vct{h} \f$,
  например его положение на изображении. */
  const VNL::Vector<double> &get_h() const {return h;}
  /** Задаёт предсказанное состояние измерения признака \f$ \vct{h} \f$,
  например его положение на изображении. */
  void set_h(const VNL::Vector<double> &new_h)
    {h.Update(new_h);}
  /** Возвращает последнее реальное измерение этого признака \f$ \vct{z} \f$,
  например найденное положение на изображении. */
  const VNL::Vector<double> &get_z() const {return z;}
  /** Неконстантная версия функции выше. */
  VNL::Vector<double> &get_z_noconst() {return z;}
  /** Задаёт последнее реальное измерение этого признака \f$ \vct{z} \f$,
  например найденное положение на изображении. */
  void set_z(const VNL::Vector<double> &new_z)
    {z.Update(new_z);}
  /** Возвращает инновацию \f$ \vct{\nu} \f$: разницу между предсказанным
  измерением \f$ \vct{h} \f$ и реальным измерением \f$ \vct{z} \f$. */
  const VNL::Vector<double> &get_nu() const {return nu;}  
  void set_nu(const VNL::Vector<double> & new_nu)
    {nu.Update(new_nu);}
  /** Возвращает якобиан \f$ \partfrac{\vct{h}}{\vct{x}_v} \f$ между
  состоянием измерения и состоянием робота. */
  const VNL::Matrix<double> &get_dh_by_dxv() const {return dh_by_dxv;}
  /** Задаёт якобиан \f$ \partfrac{\vct{h}}{\vct{x}_v} \f$ между состоянием
  измерения и состоянием робота. */
  void set_dh_by_dxv(const VNL::Matrix<double> &new_dh_by_dxv)
    {dh_by_dxv.Update(new_dh_by_dxv);}
  /** Возвращает якобиан \f$ \partfracv{h}{y} \f$ между состоянием измерения
  признака и состоянием самого признака. */
  const VNL::Matrix<double> &get_dh_by_dy() const {return dh_by_dy;}
  /** Задаёт якобиан \f$ \partfracv{h}{y} \f$ между состоянием измерения
  признака и состоянием самого признака. */
  void set_dh_by_dy(const VNL::Matrix<double> &new_dh_by_dy)
    {dh_by_dy.Update(new_dh_by_dy);}
  /** Возвращает ковариацию измерения \f$ \mat{R} \f$, например
  неопределённость из-за разрешения изображения. */
  const VNL::Matrix<double> &get_R() const {return R;}
  /** Задаёт ковариацию измерения \f$ \mat{R} \f$, например
  неопределённость из-за разрешения изображения. */
  void set_R(const VNL::Matrix<double> &new_R)
    {R.Update(new_R);}
  /** Возвращает ковариацию инновации \f$ \mat{S} \f$, например общую
  неопределённость положения на изображении. */
  const VNL::Matrix<double> &get_S() const {return S;}
  /** Задаёт ковариацию инновации \f$ \mat{S} \f$, например общую
  неопределённость положения на изображении. */
  void set_S(const VNL::Matrix<double> &new_S)
    {S.Update(new_S);}

  /** Возвращает true, если этот признак сейчас выбран. */
  bool get_selected_flag() const {return selected_flag;}
  /** Помечает признак как выбранный или невыбранный. */
  void set_selected_flag(bool newstate)
    {selected_flag = newstate;}
  unsigned int get_position_in_list() 
    {return position_in_list;}
  unsigned int get_position_in_total_state_vector()
    {return position_in_total_state_vector;}
  void set_position_in_total_state_vector(unsigned int newval)
    {position_in_total_state_vector = newval;}
  void increment_position_in_total_state_vector(int increment)
    {position_in_total_state_vector += increment;}
  Identifier get_identifier() const {return identifier;}
  unsigned int get_label() const {return label;}
  bool get_scheduled_for_termination_flag()
    {return scheduled_for_termination_flag;}
  void set_scheduled_for_termination_flag(bool newstate) 
    {scheduled_for_termination_flag = newstate;}

  bool get_successful_measurement_flag() {return successful_measurement_flag;}
  void set_successful_measurement_flag(bool newstate)
    {successful_measurement_flag = newstate;}
  unsigned int get_attempted_measurements_of_feature()
    {return attempted_measurements_of_feature;}
  void increment_attempted_measurements_of_feature(int inc)
    {attempted_measurements_of_feature+= inc;}
  unsigned int get_successful_measurements_of_feature()
    {return successful_measurements_of_feature;}
  void increment_successful_measurements_of_feature(int inc)
    {successful_measurements_of_feature+= inc;}

  /** Выполняет проверку видимости для этого признака. Использует
  Feature_Measurement_Model::visibility_test() и, как эта функция, возвращает
  ноль, если признак видим, и ненулевое значение иначе.*/
  unsigned int visibility_test() 
  {  return get_feature_measurement_model()->visibility_test(
       get_feature_measurement_model()->get_motion_model()->get_xpRES(),
       get_y(), get_xp_orig(), get_h());  }
  /*******************************Поля данных********************************/
 protected:
  /// Указатель на текущую модель измерения признака.
  Feature_Measurement_Model *feature_measurement_model;

  /** Модель измерения частично инициализированного признака. Если она не
  относится к этому признаку, хранится <code>NULL</code>. */
  Partially_Initialised_Feature_Measurement_Model 
    *partially_initialised_feature_measurement_model;

  /** Модель измерения полностью инициализированного признака. Если она не
  относится к этому признаку, хранится <code>NULL</code>. */
  Fully_Initialised_Feature_Measurement_Model
    *fully_initialised_feature_measurement_model;

  // Важные геометрические данные.
  /** Оценка состояния признака \f$ y_i \f$, например его 3D-положение. */
  VNL::Vector<double> y;
  /** Ковариация этого признака \f$ P_{y_iy_i} \f$. */
  VNL::Matrix<double> Pyy;
  /** Ковариация между состоянием робота и состоянием этого признака
  \f$ P_{x_vy_i} \f$.*/
  VNL::Matrix<double> Pxy;
  /** Позиционная часть состояния робота \f$ x_p \f$, из которой этот признак
  был инициализирован. */
  VNL::Vector<double> xp_orig;

  /** Ковариации между этим признаком и другими признаками. */
  std::vector<VNL::Matrix<double> > matrix_block_list;

  // Места для хранения предсказанных (h) и реальных (z) измерений, а также
  // якобианов.
  /** Предсказанное состояние измерения признака \f$ h_i \f$. */
  VNL::Vector<double> h;
  /** Реальное состояние измерения признака \f$ z_i \f$. */
  VNL::Vector<double> z;
  /** Инновация признака \f$ \nu_i = z_i - h_i \f$. */
  VNL::Vector<double> nu; // Инновация.
  /** Якобиан между предсказанным измерением и состоянием платформы
  \f$ \frac{\partial h_i}{\partial x_v} \f$. */
  VNL::Matrix<double> dh_by_dxv;
  /** Якобиан между измерением признака и его состоянием
  \f$ \frac{\partial h_i}{\partial y_i} \f$. */
  VNL::Matrix<double> dh_by_dy;
  /** Ковариация шума измерения \f$ R_i \f$. */
  VNL::Matrix<double> R;
  /** Ковариация инновации измерения признака \f$ S_i \f$. */
  VNL::Matrix<double> S;

  // Важные служебные данные.
  /** Выбран ли этот признак сейчас? */
  bool selected_flag;
  /** Текущая позиция признака в списке признаков. */
  unsigned int position_in_list;
  /** При разных размерах состояний признаков это нужно, чтобы найти начальную
  позицию данного признака в полном векторе состояния. */
  unsigned int position_in_total_state_vector;

  /** Идентификатор признака. Identifier указывает на объект во внешнем мире,
  который однозначно задаёт этот признак. */
  Identifier identifier;
  /** Метка этого признака внутри класса сцены. */
  unsigned int label;

  /** Устанавливается, если признак нужно скоро удалить, но прямо сейчас это
  сделать нельзя. */
  bool scheduled_for_termination_flag;

  // Учитываем попытки измерения признака и успешные измерения во времени.
  /** Было ли последнее измерение этого признака успешным? */
  bool successful_measurement_flag;
  /** Сколько раз система пыталась измерить этот признак. Используется вместе
  с successful_measurements_of_feature, чтобы понять, является ли признак
  плохим и нужно ли его удалить. */
  unsigned int attempted_measurements_of_feature;
  /** Сколько раз этот признак был успешно измерен. Используется вместе с
  attempted_measurements_of_feature, чтобы понять, является ли признак плохим
  и нужно ли его удалить. */
  unsigned int successful_measurements_of_feature;

  /** Если это известный признак, здесь хранится <code>label >= 0</code>;
  иначе <code>-1</code>. */
  int known_feature_label;
};

#endif
