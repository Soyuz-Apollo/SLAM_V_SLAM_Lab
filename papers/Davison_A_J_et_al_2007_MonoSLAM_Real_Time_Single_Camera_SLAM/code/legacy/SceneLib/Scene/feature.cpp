/*  SceneLib: software for real-time SLAM

    feature.cpp
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

#include "models_base.h"
#include "feature.h"
#include "scene_single.h"

using namespace std;

/********************************Конструкторы*********************************/

/**
Конструктор для частично инициализированных признаков.
*/
Feature::Feature(Identifier id, unsigned int lab, unsigned int list_pos, 
                 Scene_Single *scene,  const VNL::Vector<double> &h,
                 Partially_Initialised_Feature_Measurement_Model *p_i_f_m_m)
  : feature_measurement_model(p_i_f_m_m),
    partially_initialised_feature_measurement_model(p_i_f_m_m),
    fully_initialised_feature_measurement_model(NULL)
{
  // Код ниже раньше был вынесен в Feature_common.
  //   Feature_common(id, lab, list_pos, scene, h);

  feature_constructor_bookeeping();

  identifier = id;
  label = lab;
  position_in_list = list_pos;   // Позиция нового признака в списке.
  position_in_total_state_vector = 0; /* Должна быть корректно задана
                                         при добавлении признака. */

  /* Сохраняем положение платформы, из которого был получен этот признак. */
  scene->get_motion_model()->func_xp(scene->get_xv());
  xp_orig = scene->get_motion_model()->get_xpRES();

  // Вызываем функции модели, чтобы вычислить состояние признака, шум измерения
  // и соответствующие якобианы. Результаты сохраняются в RES-матрицах.

  // Сначала вычисляем "позиционное состояние" и якобиан.
  scene->get_motion_model()->func_xp(scene->get_xv());
  scene->get_motion_model()->func_dxp_by_dxv(scene->get_xv());

  // Затем просим модель инициализировать вектор состояния и вычислить
  // якобианы, чтобы дальше посчитать ковариационные матрицы.
  partially_initialised_feature_measurement_model->
    func_ypi_and_dypi_by_dxp_and_dypi_by_dhi_and_Ri(h, 
				       scene->get_motion_model()->get_xpRES());

  // Состояние y.
  y = partially_initialised_feature_measurement_model->get_ypiRES();

  // В Temp_FS1 хранится dypi_by_dxv.
  VNL::Matrix<double> Temp_FS1 = 
                     partially_initialised_feature_measurement_model->
                                get_dypi_by_dxpRES() *  
		                scene->get_motion_model()->get_dxp_by_dxvRES();

  // Pxy.
  Pxy = scene->get_Pxx() * Temp_FS1.Transpose();

  // Pyy.
  Pyy = Temp_FS1 * scene->get_Pxx() * Temp_FS1.Transpose()
    + partially_initialised_feature_measurement_model->get_dypi_by_dhiRES()
      * partially_initialised_feature_measurement_model->get_RiRES()
      * partially_initialised_feature_measurement_model->
             get_dypi_by_dhiRES().Transpose();

  // Ковариации этого признака с другими признаками.
  vector<Feature *>::iterator it = scene->get_feature_list_noconst().begin();
  for (unsigned int j = 0; j < position_in_list; j++) {
    // Новый Pypiyj = dypi_by_dxv . Pxyj.
    // Размер: FEATURE_STATE_SIZE(new) на FEATURE_STATE_SIZE(old).
    VNL::Matrix<double> newPyjypi_to_store = 
               (Temp_FS1 * (*it)->get_Pxy()).Transpose();

    matrix_block_list.push_back(newPyjypi_to_store);

    it++;
  }  

  known_feature_label = -1;
}
 

/**
Конструктор для известных признаков. Другое число аргументов отличает его от
конструктора для частично инициализированных признаков.
*/
Feature::Feature(Identifier id, unsigned int lab, unsigned int list_pos, 
		 Scene_Single *scene, const VNL::Vector<double> &y_known, 
		 const VNL::Vector<double> &xp_o, 
		 Feature_Measurement_Model *f_m_m, unsigned int k_f_l)
  : feature_measurement_model(f_m_m)
{
  feature_constructor_bookeeping();

  identifier = id;
  label = lab;
  position_in_list = list_pos;   // Позиция нового признака в списке.

  /* Сохраняем положение платформы, из которого был получен этот признак. */
  xp_orig = xp_o;

  // Прямая инициализация состояния и ковариаций.
  y = y_known;
  Pxy.Resize(scene->get_motion_model()->STATE_SIZE, 
	     feature_measurement_model->FEATURE_STATE_SIZE);
  Pxy.Fill(0.0);
  Pyy.Resize(feature_measurement_model->FEATURE_STATE_SIZE,
	     feature_measurement_model->FEATURE_STATE_SIZE);
  Pyy.Fill(0.0);

  vector<Feature *>::iterator it = scene->get_feature_list_noconst().begin();
  // Заполняем взаимные ковариационные блоки нулями.
  for (unsigned int i = 0; i < position_in_list; i++) {
    VNL::Matrix<double> newPyjyi_to_store(
		    (*it)->get_feature_measurement_model()->FEATURE_STATE_SIZE,
                    feature_measurement_model->FEATURE_STATE_SIZE, 0.0);

    matrix_block_list.push_back(newPyjyi_to_store);    

    it++;
  }    

  known_feature_label = k_f_l;

  if (feature_measurement_model->fully_initialised_flag) {
    partially_initialised_feature_measurement_model = NULL;
    fully_initialised_feature_measurement_model = 
      (Fully_Initialised_Feature_Measurement_Model *)
      feature_measurement_model;
  }
  else {
    fully_initialised_feature_measurement_model = NULL;
    partially_initialised_feature_measurement_model = 
      (Partially_Initialised_Feature_Measurement_Model *)
      feature_measurement_model;
  }
}

// Функция объединяет общую часть конструкторов ниже.
/// Общая часть инициализации, используемая несколькими конструкторами.
void Feature::feature_constructor_bookeeping()
{
  selected_flag = false;        // При первом обнаружении признак не выбран.
  scheduled_for_termination_flag = 0;
  attempted_measurements_of_feature = 0;
  successful_measurements_of_feature = 0;

  // Выделяем матрицы/векторы для предсказанных и реальных измерений.
  h.Resize(feature_measurement_model->MEASUREMENT_SIZE);
  z.Resize(feature_measurement_model->MEASUREMENT_SIZE);
  nu.Resize(feature_measurement_model->MEASUREMENT_SIZE);

  dh_by_dxv.Resize(feature_measurement_model->MEASUREMENT_SIZE, 
		    feature_measurement_model->get_motion_model()->STATE_SIZE);
  dh_by_dy.Resize(feature_measurement_model->MEASUREMENT_SIZE, 
			   feature_measurement_model->FEATURE_STATE_SIZE);
  R.Resize(feature_measurement_model->MEASUREMENT_SIZE,
		    feature_measurement_model->MEASUREMENT_SIZE);
  S.Resize(feature_measurement_model->MEASUREMENT_SIZE,
		    feature_measurement_model->MEASUREMENT_SIZE);
}


/*****************************Управление признаками******************************/

// Эта функция вызывается, когда удаляется признак, стоящий раньше в списке.
void Feature::feature_is_removed(unsigned int list_pos)
{
  position_in_list--;

  vector<VNL::Matrix<double> >::iterator it = 
                                 matrix_block_list.begin() + list_pos;
  matrix_block_list.erase(it);
}


/** Преобразует частично инициализированный признак в полностью
инициализированный, используя информацию о свободных параметрах
\f$ \vct{\lambda} \f$.
@param lambda Среднее значение \f$ \vct{\lambda} \f$
@param Plambda Ковариация \f$ \vct{\lambda} \f$
@param scene SLAM-карта

Новое состояние \f$ \vct{y}_{fi} \f$ получается вызовом
Partially_Initialised_Feature_Measurement_Model::func_yfi_and_dyfi_by_dypi_and_dyfi_by_dlambda().
Новые ковариации признака:
\f[
\begin{aligned}
\mat{P}_{\vct{y}_{fi}\vct{y}_{fi}} &=
\partfrac{\vct{y}_{fi}}{\vct{y}_{pi}}
\mat{P}_{\vct{y}_{pi}\vct{y}_{pi}} \partfrac{\vct{y}_{fi}}{\vct{y}_{pi}}^T +
\partfrac{\vct{y}_{fi}}{\vct{\lambda}} \mat{P}_{\vct{\lambda}}
\partfrac{\vct{y}_{fi}}{\vct{\lambda}}^T \\

\mat{P}_{\vct{x}\vct{y}_{fi}} &=
\mat{P}_{\vct{x}\vct{y}_{pi}} \partfrac{\vct{y}_{fi}}{\vct{y}_{pi}}^T \\

\mat{P}_{\vct{y}_j\vct{y}_{fi}} &=
\mat{P}_{\vct{y}_j\vct{y}_{pi}} \partfrac{\vct{y}_{fi}}{\vct{y}_{pi}}^T \\

\mat{P}_{\vct{y}_{fi}\vct{y}_{j}} &=
\partfrac{\vct{y}_{pi}}{\vct{y}_{j}} \mat{P}_{\vct{y}_fi\vct{y}_{pi}} \\
\end{aligned}
\f]
где разные якобианы возвращаются вызовами
Partially_Initialised_Feature_Measurement_Model, а ковариационные матрицы
\f$ \mat{P}_{kl} \f$ уже известны и хранятся в классе. Исключение —
\f$ \mat{P}_{\vct{\lambda}} \f$, которая передаётся в эту функцию.
*/
void Feature::convert_from_partially_to_fully_initialised(
  const VNL::Vector<double> &lambda,
  const VNL::Matrix<double> &Plambda,
  Scene_Single *scene)
{
  // Всю работу делаем здесь, в feature.cc; вероятно, это работает только
  // с scene_single.
  
  // Вычисляем новое состояние yfi(ypi, lambda).
  // Новая ковариация признака:
  // Pyfiyfi = dyfi_by_dypi Pypiypi dyfi_by_dypiT + 
  //           dyfi_by_dlambda Plambda dyfi_by_dlambdaT
  // Взаимные ковариации меняются так:
  // Pxyfi = Pxypi dyfi_by_dypiT
  // Pyjyfi = Pyjypi dyfi_by_dypiT   для j < i
  // Pyfiyj = dyfi_by_dypi Pypiyj    для j > i
  // потому что хранится только верхняя правая часть ковариационной матрицы.
  
  partially_initialised_feature_measurement_model->
    func_yfi_and_dyfi_by_dypi_and_dyfi_by_dlambda(y, lambda);

  VNL::Matrix<double> dyfi_by_dypiT = 
    partially_initialised_feature_measurement_model->
                  get_dyfi_by_dypiRES().Transpose();
  VNL::Matrix<double> dyfi_by_dlambdaT =
    partially_initialised_feature_measurement_model->
    get_dyfi_by_dlambdaRES().Transpose();

  // Заменяем y.
  y = partially_initialised_feature_measurement_model->get_yfiRES();

  // Заменяем Pxy.
  Pxy = Pxy * dyfi_by_dypiT;

  // Заменяем Pyy.
  VNL::Matrix<double> Pypiypi_1 = 
    partially_initialised_feature_measurement_model->get_dyfi_by_dypiRES() * 
    Pyy * dyfi_by_dypiT;
  VNL::Matrix<double> Pypiypi_2 = 
    partially_initialised_feature_measurement_model->get_dyfi_by_dlambdaRES() * 
    Plambda * dyfi_by_dlambdaT;
  Pyy = Pypiypi_1 + Pypiypi_2;
  
  // Элементы Pyjyi для j < i: ковариация с признаками перед i в списке.
  const unsigned int i = position_in_list;
  
  for(vector<VNL::Matrix<double> >::iterator it = matrix_block_list.begin();
      it != matrix_block_list.begin() + position_in_list;
      it++) {
    *it = *it * dyfi_by_dypiT;
  }

  // Элементы Pyjyi для j > i: ковариация с признаками после i в списке.
  vector<Feature *>::iterator it = scene->get_feature_list_noconst().begin();
  // Начинаем с элемента после текущего признака.
  while(*it != this)
    it++; // теперь указывает на текущий признак.
  it++; // теперь указывает на следующий признак.

  for (; it != scene->get_feature_list().end(); it++) {
    (*it)->matrix_block_list[i] = 
      partially_initialised_feature_measurement_model->get_dyfi_by_dypiRES()
      * (*it)->matrix_block_list[i];
    (*it)->increment_position_in_total_state_vector(
                       -feature_measurement_model->FEATURE_STATE_SIZE);
  }

  // Изменяем полный размер состояния в сцене; здесь приращение отрицательное.
  scene->increment_total_state_size(
    partially_initialised_feature_measurement_model->
    more_initialised_feature_measurement_model->FEATURE_STATE_SIZE -
    partially_initialised_feature_measurement_model->FEATURE_STATE_SIZE);

  // Меняем fmm этой модели на модель полностью инициализированного признака.
  feature_measurement_model = 
    partially_initialised_feature_measurement_model->
    more_initialised_feature_measurement_model;

  partially_initialised_feature_measurement_model = NULL;
  fully_initialised_feature_measurement_model = 
    dynamic_cast<Fully_Initialised_Feature_Measurement_Model *>(
      feature_measurement_model);

  assert(fully_initialised_feature_measurement_model != NULL);
  
  // Нужно заново выделить остальные матрицы.
  // Предполагаем, что размер измерения не меняется.
  dh_by_dy.Resize(feature_measurement_model->MEASUREMENT_SIZE, 
		  feature_measurement_model->FEATURE_STATE_SIZE);
}

