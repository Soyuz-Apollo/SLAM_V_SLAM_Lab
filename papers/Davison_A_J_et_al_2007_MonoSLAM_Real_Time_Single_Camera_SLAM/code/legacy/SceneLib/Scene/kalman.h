/*  SceneLib: software for real-time SLAM

    kalman.h
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

/* Класс Kalman содержит функции фильтрации. */

#ifndef _scene_kalman_h_
#define _scene_kalman_h_

#include "scene_single.h"

/**
Реализует расширенный фильтр Калмана для класса Scene_Single. Это дружественный
класс Scene_Single, который извлекает из него векторы состояния и ковариации.
@ingroup gScene
*/
class Kalman
{
 public:
  // Конструкторы и деструкторы
  Kalman () {}                                           // Конструктор

  // Медленные версии: исходные реализации
  void predict_filter_slow (Scene_Single *scene, VNL::Vector<double> &u, 
			   double delta_t);
  void total_update_filter_slow (Scene_Single *scene);
  void update_filter_internal_measurement_slow (Scene_Single *scene, 
						unsigned int i);

  // Быстрые версии
  void predict_filter_fast (Scene_Single *scene, VNL::Vector<double> &u, 
			   double delta_t);
};


#endif
