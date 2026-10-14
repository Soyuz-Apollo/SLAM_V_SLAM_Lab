/*  SceneLib: software for real-time SLAM

    init_feature.h
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

// Фильтр частиц для инициализации признаков, у которых часть параметров плохо
// описывается гауссовым распределением.
// С каждым частично инициализированным признаком, добавленным в Scene_Single,
// связывается объект FeatureInitInfo. Сам признак при этом также находится в
// основной карте Scene_Single, где обрабатываются остальные гауссовы степени
// свободы.
// Lambda — это вектор параметров, которые представлены частицами.

#ifndef _scene_init_feature_h_
#define _scene_init_feature_h_

#include <vector>
#include <VNL/vector.h>
#include <VNL/matrix.h>
#include <VNL/Algo/cholesky.h>
#include <VNL/Algo/determinant.h>

class Scene_Single;
class Feature;


/**
Класс для взвешенных сэмплов и информации об измерениях.
@ingroup gScene
*/
class Particle
{
  friend class FeatureInitInfo;
  friend class Scene_Single;

 public:
  // Геттеры и сеттеры для использования вне Scene_Single.
  /** Возвращает значения свободных параметров \f$ \lambda \f$, связанных
  с этой частицей. */
  const VNL::Vector<double> &get_lambda() const {return lambda;}
  /** Возвращает текущее ожидаемое состояние измерения \f$ h_i \f$ для этой
  частицы, например положение на изображении. */
  const VNL::Vector<double> &get_h() const {return m_h;}
  /** Возвращает текущее измерение \f$ z_i \f$ для этой частицы, например
  положение на изображении. */
  const VNL::Vector<double> &get_z() const {return m_z;}
  /** Возвращает обратную ковариацию инновации \f$ S_i^{-1} \f$ для этой
  частицы. Ковариация инновации — это полная неопределённость измерения,
  возникающая из неопределённостей состояния робота, признака и самого
  измерения. Обратная матрица используется для задания области поиска в
  пространстве измерений, например эллипса на изображении.*/
  const VNL::Matrix<double> &get_SInv() const {return m_SInv;}
  /** Возвращает определитель текущей ковариации инновации,
   \f$ \arrowvert S_i \arrowvert \f$. */
  double get_detS() const {return m_detS;}
  /** Было ли последнее измерение этой частицы успешным? */
  bool get_successful_measurement_flag() const
    {return m_successful_measurement_flag;}
  /** Возвращает текущую вероятность этой частицы. */
  double get_probability() const {return probability;}
    
  /// Обновляет измерение состояния признака, например положение на изображении.
  void set_z(const VNL::Vector<double> &new_z) {m_z.Update(new_z);}
  /// Обновляет результат последней попытки сопоставления.
  void set_successful_measurement_flag(bool newstate) 
    {m_successful_measurement_flag = newstate;}

 protected:
  Particle(const VNL::Vector<double> &l, 
           double p,
           const unsigned int MEASUREMENT_SIZE);

  /** Задаёт текущую предсказанную позицию измерения \f$ h_i \f$ для этой
  частицы. Вызывается из
  Scene_Single::predict_partially_initialised_feature_measurements() */
  void set_h(const VNL::Vector<double>& h) {m_h.Update(h);}
  void set_S(const VNL::Matrix<double>& Si);
           
  /// Значения свободных параметров, представленных этой частицей.
  VNL::Vector<double> lambda;
  /// Вероятность этой частицы.
  double probability;
  /// Накопленная вероятность всех частиц до этой включительно.
  double cumulative_probability;

  // Информация об измерении, используемая для обновления частицы.
  /** Измерение состояния признака \f$ z_i \f$, например положение на
  изображении.*/
  VNL::Vector<double> m_z;
  /** Текущее предсказанное состояние измерения \f$ h_i \f$ для этой частицы,
  например положение на изображении. */
  VNL::Vector<double> m_h;
  /** Текущая обратная ковариация инновации \f$ S_i^{-1} \f$ для этой частицы.
  */
  VNL::Matrix<double> m_SInv;
  /** Определитель текущей ковариации инновации \f$ |S_i| \f$. */
  double m_detS;
  /** Было ли последнее измерение этой частицы успешным? */
  bool m_successful_measurement_flag;
};


/** Задаёт текущую обратную ковариацию инновации \f$ S_i^{-1} \f$ для этой
частицы. Вызывается из
Scene_Single::predict_partially_initialised_feature_measurements() */
inline void Particle::set_S(const VNL::Matrix<double>& Si)
{
  VNL::Cholesky<double>local_Si_cholesky(Si);
  m_SInv.Update(local_Si_cholesky.Inverse());
  m_detS = Determinant(Si);
}


/**
Класс для информации о частично инициализированном признаке. Он хранит список
частиц, представляющих свободные параметры признака \f$ \lambda \f$.

Частицы добавляются пользователем этого класса через add_particle(); изначально
частиц нет. Их вероятности обновляются снаружи в Scene_Single. После этого
текущую оценку \f$ \lambda \f$ можно получить вызовами
normalise_particle_vector_and_calculate_cumulative(), а затем
calculate_mean_and_covariance(). Дополнительно, и это рекомендуется, можно
вызвать prune_particle_vector(), чтобы удалить частицы с малыми вероятностями.
@ingroup Scene
*/
class FeatureInitInfo
{
  friend class Scene_Single;

public:
  typedef std::vector<Particle> ParticleVector;

public:
  FeatureInitInfo(Scene_Single *scene, Feature *fp);

  // Добавляет новую частицу. После этого распределение нужно нормализовать
  // заново.
  void add_particle(const VNL::Vector<double> &lambda,
                    double probability);

  // Возвращает false, если pdf нельзя нормализовать: все элементы нулевые.
  bool normalise_particle_vector_and_calculate_cumulative();
  // Удаляет частицы с вероятностью ниже
  // prune_probability_threshold * (1/N) 
  void prune_particle_vector(const double prune_probability_threshold);

  // Предполагает нормализованное распределение и вычисляет среднее
  // и ковариацию.
  void calculate_mean_and_covariance();
  void print_probability_distribution();
  void print_probability_distribution_matlab();

  Particle sample_from_cumulative_distribution(
    unsigned int *position_in_vector);

  // Геттеры и сеттеры для использования вне Scene_Single.
  const Feature *get_fp() const {return fp;}
  Feature *get_fp_noconst() {return fp;}
  const ParticleVector& get_particle_vector() const
    {return particle_vector;}
  ParticleVector &get_particle_vector_noconst()
    {return particle_vector;}
  bool get_making_measurement_on_this_step_flag() const
    {return making_measurement_on_this_step_flag;}

  /** Возвращает последнее значение среднего \f$ \lambda \f$, вычисленное
  через calculate_mean_and_covariance(). */
  const VNL::Vector<double> &get_mean() const {return mean;}
  /** Возвращает последнее значение ковариации \f$ \lambda \f$, вычисленное
  через calculate_mean_and_covariance(). */
  const VNL::Matrix<double> &get_covariance() const {return covariance;}

protected:
  // Так как частицы могут жить в пространстве размерности PARTICLE_DIMENSION,
  // среднее распределения является вектором, а ковариация — матрицей.
  VNL::Vector<double> mean;
  VNL::Matrix<double> covariance;

  // Указатели на связанные объекты сцены и признака.
  Scene_Single *scene;
  Feature *fp;

  // Вектор частиц, представляющий вероятностное распределение.
  ParticleVector particle_vector;

  unsigned int number_of_match_attempts;
  bool making_measurement_on_this_step_flag;
  // Размерность пространства, в котором живёт распределение частиц.
  unsigned int PARTICLE_DIMENSION;
};

#endif
