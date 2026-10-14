/*  SceneLib: software for real-time SLAM

    init_feature.cpp
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
#include "init_feature.h"
#include "scene_single.h"

using namespace std;

#ifdef DEBUGDUMP
#undef DEBUGDUMP
#endif
#define DEBUGDUMP false

/** Конструктор. Он защищённый, потому что вызывается только из
FeatureInitInfo::add_particle()
@param l Значения свободных параметров \f$ \lambda \f$, представленных этой
частицей.
@param p Начальная вероятность этой частицы
@param MEASUREMENT_SIZE Число параметров, представляющих измерение признака
*/
Particle::Particle(const VNL::Vector<double> &l, 
                   double p,
                   const unsigned int MEASUREMENT_SIZE)
{
  lambda = l; 
  probability = p;

  m_z.Resize(MEASUREMENT_SIZE);
  m_h.Resize(MEASUREMENT_SIZE);
  m_SInv.Resize(MEASUREMENT_SIZE, MEASUREMENT_SIZE);
}


/** Конструктор. Класс создаётся без частиц, поэтому их нужно добавить позже
через add_particle().
@param sc Класс Scene.
@param f Признак, для которого этот класс хранит дополнительную информацию.
*/
FeatureInitInfo::FeatureInitInfo(Scene_Single *sc, Feature *f)
  : scene(sc),
    fp(f),
    PARTICLE_DIMENSION(
      f->get_partially_initialised_feature_measurement_model()->
        FREE_PARAMETER_SIZE)
{
  number_of_match_attempts = 0;

  mean.Resize(PARTICLE_DIMENSION);
  covariance.Resize(PARTICLE_DIMENSION, PARTICLE_DIMENSION);

  if(Scene_Single::STATUSDUMP) cout << "PARTICLE_DIMENSION = " 
                                    << PARTICLE_DIMENSION << endl;
}

/**
Создаёт частицу и добавляет её в список частиц этого частично
инициализированного признака.
@param lambda Значения свободных параметров, представленных этой частицей.
@param probability Начальная вероятность, назначаемая частице.
*/
void FeatureInitInfo::add_particle(const VNL::Vector<double> &lambda,
                                   double probability)
{
  assert(lambda.Size() == PARTICLE_DIMENSION);
  Particle p(lambda, probability,
             fp->get_partially_initialised_feature_measurement_model()->
             MEASUREMENT_SIZE);
  particle_vector.push_back(p);
}

/**
Нормализует вероятности частиц: делит их на полную вероятность так, чтобы сумма
стала равна единице. Функция записывает в Particle::probability нормализованное
значение, а в Particle::cumulative_probability — суммарную вероятность всех
частиц до текущей включительно после нормализации.
@returns <code>false</code>, если полная вероятность равна нулю,
<code>true</code> иначе.
*/
bool FeatureInitInfo::normalise_particle_vector_and_calculate_cumulative()
{
  double total = 0.0;
  for (vector<Particle>::iterator it = particle_vector.begin();
       it != particle_vector.end(); it++) {
    total += it->probability;
  }

  if (total == 0.0)
    return false;

  double cumulative_total = 0.0;
  for (vector<Particle>::iterator it = particle_vector.begin();
       it != particle_vector.end(); it++) {
    it->probability = it->probability / total;
    it->cumulative_probability = cumulative_total + it->probability;
    cumulative_total += it->probability;

    if (DEBUGDUMP) cout << "Particle at " << it->lambda 
                        << " normalised probability " << it->probability
                        << endl;
  }

  return true;
}

/**
Прореживает вектор частиц.
Предполагается, что перед этим вероятности частиц уже нормализованы. Функция
удаляет частицы с вероятностью ниже prune_probability_threshold * (1/N), а
затем нормализует распределение заново.
@param prune_probability_threshold Используемый порог.
*/
void FeatureInitInfo::prune_particle_vector(
  const double prune_probability_threshold)
{
  double prune_threshold = prune_probability_threshold / 
    double(particle_vector.size());

  vector<Particle>::iterator it = particle_vector.begin();
  while (it != particle_vector.end()) {
    if (it->probability < prune_threshold)
    {
      it = particle_vector.erase(it);
    }
    else
    {
      it++;
    }
  }

  normalise_particle_vector_and_calculate_cumulative();
}

/**
Вычисляет среднее и ковариацию \f$ \lambda \f$ по всем частицам:
\f{align}
  \text{mean} &= \mu = \sum_i \lambda_i p(i) \nonumber \\
  \text{mean} &= \sum_i \lambda_i\lambda_i^T p(i) - \mu\mu^T \nonumber
\f}
Результат не возвращается напрямую, а сохраняется внутри класса; его можно
прочитать через get_mean() и get_covariance().
*/
void FeatureInitInfo::calculate_mean_and_covariance()
{
  // Матрица, в которой хранится матожидание lambda * lambda^T.
  // Это позволяет выполнить расчёт за один проход по частицам.
  VNL::Matrix<double> ExpectedSquared(PARTICLE_DIMENSION, PARTICLE_DIMENSION);
  ExpectedSquared.Fill(0.0); 

  // Обнуляем вектор среднего перед заполнением.
  mean.Fill(0.0);

  for (vector<Particle>::iterator it = particle_vector.begin();
       it != particle_vector.end(); it++)
  {
    mean += it->probability * it->lambda;
    ExpectedSquared += it->probability *
      OuterProduct(it->lambda, it->lambda);
  }
 
  covariance = ExpectedSquared - OuterProduct(mean, mean);
}

/**
Для каждой Particle печатает в <code>cout</code> значения \f$ \lambda \f$,
вероятность и накопленную вероятность.
*/
void FeatureInitInfo::print_probability_distribution()
{
  for (vector<Particle>::iterator it = particle_vector.begin();
       it != particle_vector.end(); it++) {
    cout << "Lambda " << it->lambda << " probability " 
   << it->probability << " cumulative " 
	 << it->cumulative_probability << endl;
  }
}

/**
Печатает значения \f$ \lambda \f$ у частиц и их вероятности в
<code>cerr</code> в формате двух Matlab-векторов: <code>lambda</code> и
<code>prob</code>.
*/
void FeatureInitInfo::print_probability_distribution_matlab()
{
  cerr << "lambda = [";
  for (vector<Particle>::iterator it = particle_vector.begin();
       it != particle_vector.end(); it++) {
    cerr << it->lambda << " ";
  }
  cerr << "];" << endl;

  cerr << "prob = [";
  for (vector<Particle>::iterator it = particle_vector.begin();
       it != particle_vector.end(); it++) {
    cerr << it->probability << " ";
  }
  cerr << "];" << endl;
}

/**
Берёт случайную выборку из вектора Particle согласно pdf, заданной текущими
вероятностями.
@param position_in_vector Заполняется индексом выбранной Particle.
@returns Выбранная Particle.
*/
Particle FeatureInitInfo::sample_from_cumulative_distribution(
  unsigned int *position_in_vector)
{
  double random_number = drand48();
  *position_in_vector = 0;

  for (vector<Particle>::iterator it = particle_vector.begin();
       it != particle_vector.end(); it++) {
    if (it->cumulative_probability >= random_number) {
      return *it;
    } 

    (*position_in_vector)++;
  }

  // Сюда выполнение никогда не должно дойти.
  assert(0);
}
