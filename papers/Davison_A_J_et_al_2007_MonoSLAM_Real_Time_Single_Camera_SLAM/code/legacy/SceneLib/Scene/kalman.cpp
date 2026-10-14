/*  SceneLib: software for real-time SLAM

    kalman.cpp
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
#include <VNL/Algo/cholesky.h>
#include <VNL/matlabprint.h>
#include "kalman.h"

using namespace std;

extern double time1;


/*************************Предсказать новое положение платформы************************/

    
/* Простое общее предсказание */
void Kalman::predict_filter_slow (Scene_Single *scene, VNL::Vector<double> &u, 
				 double delta_t)
{
  // cerr << "*** SLOW PREDICTION ***\n";

  /* Что нужно сделать на шаге предсказания:
	     
     Вычислить f и grad_f_x
     Вычислить Q
     Сформировать x(k+1|k) и P(k+1|k)
  */

  unsigned size = scene->get_total_state_size();

  // Сначала формируем исходные полные состояние и ковариацию.
  VNL::Vector<double> x(size);
  VNL::Matrix<double> P(size, size);
  scene->construct_total_state_and_covariance(x, P);

  // Выполняем вычисления модели: результаты сохраняются в RES-матрицах.
  // scene->get_motion_model()->jacobians_test(scene->get_xv(), u, delta_t);
  scene->get_motion_model()->func_fv_and_dfv_by_dxv(scene->get_xv(), 
						    u, delta_t);
  scene->get_motion_model()->func_Q(scene->get_xv(), u, delta_t);

  // Находим новое состояние f.
  VNL::Vector<double> f(size);

  /* Компоненты признаков в f совпадают с x. */
  f.Update(x);
  f.Update(scene->get_motion_model()->get_fvRES(), 0);

  // Находим новую P.

  /* Так как большинство элементов df_by_dx равны нулю... */
  VNL::Matrix<double> df_by_dx(size, size);
  df_by_dx.Fill(0.0);

  // Заполняем остальные элементы df_by_dx: единицы на диагонали для признаков.
  for (unsigned int i = scene->get_motion_model()->STATE_SIZE; 
       i < df_by_dx.Rows(); i++)
    df_by_dx(i,i) = 1.0;

  df_by_dx.Update(scene->get_motion_model()->get_dfv_by_dxvRES(), 0, 0);

  // Вычисляем шум процесса.
  VNL::Matrix<double> Q(size, size);
  Q.Fill(0.0);
  Q.Update(scene->get_motion_model()->get_QxRES(), 0, 0);

  P.Update(df_by_dx * P * df_by_dx.Transpose());

  P += Q;

  // cerr << "f:" << f;
  // cerr << "Pnew:" << Pnew;

  scene->fill_state_and_covariance(f, P);
}


/*************** Быстрая версия predict-фильтра *******************/

void Kalman::predict_filter_fast(Scene_Single *scene, 
				VNL::Vector<double> &u, double delta_t)
{
  // Выполняем вычисления модели: результаты сохраняются в RES-матрицах.
  scene->get_motion_model()->func_fv_and_dfv_by_dxv(scene->xv, 
						    u, delta_t);
  scene->get_motion_model()->func_Q(scene->xv, u, delta_t);

  //cout << "xv before " << endl;
  //MatlabPrint(cout, scene->xv); cout << endl;
  //cout << "Pxx before " << endl;
  //MatlabPrint(cout, scene->Pxx);

  scene->xv.Update(scene->get_motion_model()->get_fvRES());

  scene->Pxx.Update(scene->get_motion_model()->get_dfv_by_dxvRES() *
		    scene->Pxx *
		    scene->get_motion_model()->get_dfv_by_dxvRES().Transpose()
		    + scene->get_motion_model()->get_QxRES());

  //cout << "xv after " << endl;
  //MatlabPrint(cout, scene->xv); cout << endl;
  //cout << "Pxx after " << endl;
  //MatlabPrint(cout, scene->Pxx);



  /* Изменяем ковариации между состоянием платформы и состояниями признаков. */
  for (vector<Feature *>::iterator it = 
	 scene->get_feature_list_noconst().begin();
       it != scene->get_feature_list().end(); it++) {
    (*it)->Pxy.Update(scene->get_motion_model()->get_dfv_by_dxvRES() * 
		      (*it)->get_Pxy());
  }
}


/********************************Обновить фильтр********************************/

/* Простое общее обновление фильтра */
void Kalman::total_update_filter_slow(Scene_Single *scene)
{
  /* Шаги обновления полного фильтра:
     1. Сформировать h, dh_by_dx, R(k+1) и z
     2. Вычислить S(k+1)
     3. Вычислить W(k+1)
     4. Вычислить x(k+1|k+1)
     5. Вычислить P(k+1|k+1)
  */

  unsigned int size = scene->get_successful_measurement_vector_size();
	                                                  // Размер вектора измерений
  unsigned int size2 = scene->get_total_state_size();      // Размер вектора состояния

  VNL::Vector<double> x(size2);
  VNL::Matrix<double> P(size2, size2);

  scene->construct_total_state_and_covariance(x, P);

  /* 1. Формируем nu и dh_by_dx. */
  VNL::Vector<double> nu_tot(size);
  VNL::Matrix<double> dh_by_dx_tot(size, size2);
  VNL::Matrix<double> R_tot(size, size);

  scene->construct_total_measurement_stuff(nu_tot, dh_by_dx_tot, R_tot);

  /* 2. Вычисляем S(k+1). */

  VNL::Matrix<double> S = dh_by_dx_tot * P * dh_by_dx_tot.Transpose();
  S += R_tot;

  /* 3. Вычисляем W(k+1). */
  VNL::Cholesky<double> S_cholesky(S);
  VNL::Matrix<double> W = P * dh_by_dx_tot.Transpose() * S_cholesky.Inverse();

  /* 4. Вычисляем x(k+1|k+1). */
  x += W * nu_tot;

  /* 5. Вычисляем P(k+1|k+1). */
  P -= W * S * W.Transpose();

  scene->fill_state_and_covariance(x, P);
}

void Kalman::update_filter_internal_measurement_slow (Scene_Single *scene, 
						      unsigned int i)
{
  unsigned int size = scene->internal_measurement_vector[i]->
    internal_measurement_model->MEASUREMENT_SIZE;
	                                                  // Размер вектора измерений
  unsigned int size2 = scene->get_total_state_size();   // Размер вектора состояния

  VNL::Vector<double> x(size2);
  VNL::Matrix<double> P(size2, size2);

  scene->construct_total_state_and_covariance(x, P);

  // cout << "x:" << x;
  // cout << "P:" << P;

  /* 1. Формируем nu и dh_by_dx. */
  VNL::Vector<double> nu_tot(size);
  VNL::Matrix<double> dh_by_dx_tot(size, size2);
  VNL::Matrix<double> R_tot(size, size);

  scene->construct_total_internal_measurement_stuff(nu_tot, dh_by_dx_tot, 
						    R_tot, i);

  /* 2. Вычисляем S(k+1). */
  VNL::Matrix<double> S(size, size);

  VNL::Matrix<double> Tempss2(size, size2);
  VNL::Matrix<double> Temps2s(size2, size);

  VNL::Matrix<double> dh_by_dx_totT = dh_by_dx_tot.Transpose();

  S.Update(dh_by_dx_tot * P * dh_by_dx_totT);
  S += R_tot;

  // cout << "R_tot:" << R_tot;
  //  cout << "S:" << S;
  // cout << "dh_by_dx_tot:" << dh_by_dx_tot;
  // cout << "dh_by_dx_totT:" << dh_by_dx_totT;

  /* 3. Вычисляем W(k+1). */
  VNL::Cholesky<double> S_cholesky(S);
  VNL::Matrix<double> W = P * dh_by_dx_totT * S_cholesky.Inverse();

  // cout << "W:" << W;

  /* 4. Вычисляем x(k+1|k+1). */
  x += W * nu_tot;
  
  /* 5. Вычисляем P(k+1|k+1). */
  P -= W * S * W.Transpose();

  scene->fill_state_and_covariance(x, P);

  // cout << "x after update:" << x;
  // cout << "P after update:" << P;
}
