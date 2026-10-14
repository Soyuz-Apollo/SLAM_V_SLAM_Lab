/*  SceneLib: software for real-time SLAM

    models_base.h
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

// Модели движения платформы и измерения признаков.

/********************************Модели движения********************************/
/* Здесь используется иерархия наследования:
   Motion_Model должен охватывать все варианты.

   ThreeD_Motion_Model относится к 3D-движению.
   TwoD_Motion_Model относится к 2D-движению на плоскости.
   OneD_Motion_Model относится к 1D-движению вдоль линии.
*/

#ifndef _scene_models_base_h_
#define _scene_models_base_h_

#include <VNL/vector.h>
#include <VNL/vectorfixed.h>
#include <VNL/matrix.h>
#include <VNL/matrixfixed.h>
#include <VW/geomobjects.h>

#include "newbits.h"
#include "settings.h"

class Settings;

/**
Класс, описывающий движение робота или сенсорной платформы.
@ingroup gScene
*/
class Motion_Model
{
 public:
  Motion_Model(const unsigned int position_state_size, 
               const unsigned int state_size,
               const unsigned int control_size,
               const char *m_m_d_t,
               const char *m_m_t);

  virtual ~Motion_Model() {}

  /** Считывает параметры этой модели движения из настроек. Если эта функция
  переопределяется, внутри новой реализации нужно вызвать read_parameters()
  базового класса.*/
  virtual void read_parameters(const Settings& ) {}
    
  /** Для абстрактного класса обычно делают виртуальный деструктор, но здесь
  есть явная функция для очистки матриц, определённых в базовом классе. */
  void Motion_Model_destructor();    

  /** Минимальное число параметров для представления позиции в этой размерности.
  */
  const unsigned int POSITION_STATE_SIZE; 
  /** Число параметров, которое реально используется в векторе состояния. Это
  даёт большую общность. */
  const unsigned int STATE_SIZE;
  /** Число параметров в управляющем векторе. */
  const unsigned int CONTROL_SIZE;
  /** Строковый идентификатор размерности модели движения. */
  const char *motion_model_dimensionality_type; 
  /** Строковый идентификатор конкретной модели движения. */
  const char *motion_model_type;

  /****Следующие виртуальные функции должны быть заданы в наследнике****/

  /** 
  Вычисляет новое состояние \f$ f_v \f$ как функцию старого состояния
  \f$ x_v \f$, управляющего вектора \f$ u \f$ и временного интервала
  \f$ \Delta t \f$. Также вычисляет якобиан
  \f$ \partial f_v / \partial x_v \f$.
  Эта функция применяет только уравнения процесса, без шума.
  **/
  virtual void func_fv_and_dfv_by_dxv(const VNL::Vector<double> &xv,
                                      const VNL::Vector<double> &u,
                                      const double delta_t) = 0;
  /** 
  Формирует ковариационную матрицу \f$ Q \f$ шума процесса, связанного с
  \f$ f_v \f$. Для этого строится ковариация шума \f$ P_{nn}\f$ и якобиан
  \f$ \frac{\partial f_v}{n} \f$, после чего вычисляется
  \f$ Q = \frac{\partial f_v}{n} P_{nn}
  \frac{\partial f_v}{n}^T \f$.
  **/
  virtual void func_Q(const VNL::Vector<double> &xv, 
                      const VNL::Vector<double> &u,
                      const double delta_t) = 0;

  /** 
  Извлекает позицию и ориентацию из вектора состояния.
  **/
  virtual void func_xp(const VNL::Vector<double> &xv) = 0;

  /** 
  Вычисляет якобиан для func_xp. В этом случае это просто единичная матрица.
  **/
  virtual void func_dxp_by_dxv(const VNL::Vector<double> &xv) = 0;

  /** 
  Зашумлённая версия уравнения процесса func_fv_and_dfv_by_dxv(), используемая
  в симуляции. Обычно она просто добавляет к \f$ x_v \f$ гауссов шум и затем
  пропускает результат через func_fv_and_dfv_by_dxv().
  **/
  virtual void func_fv_noisy(const VNL::Vector<double> &xv_true, 
                             const VNL::Vector<double> &u_true,
                             const double delta_t) = 0;

  /** 
  Переопределяет \f$ x_v \f$ и его якобиан для новой системы координат.
  **/
  virtual void func_xvredef_and_dxvredef_by_dxv_and_dxvredef_by_dxpdef  
    (const VNL::Vector<double> &xv, const VNL::Vector<double> &xpdef) = 0;

  /******Эти виртуальные функции необязательны и не являются чисто виртуальными*****/
  /**********У них есть пустые реализации по умолчанию. Определяйте при необходимости.*******/

  /**  
  Нормализует вектор состояния, если он использует избыточное представление.
  Например, состояние часто содержит кватернион, который нужно нормализовать,
  чтобы его длина оставалась равной 1.
  **/
  virtual void func_xvnorm_and_dxvnorm_by_dxv(const VNL::Vector<double> &xv);

  /******Эта функция определена в models_base.cc и должна переопределяться*******/
  /***********************только в особых случаях**************************/

  // Как "обнулить" оси в текущей позиции робота.
  virtual void func_zeroedxv_and_dzeroedxv_by_dxv(
    const VNL::Vector<double> &xv);

  /******Эти функции определены ниже в моделях конкретной размерности*******/
  /****поэтому их не нужно отдельно задавать для каждой модели****/

  // Как изменить позиционное состояние, если система координат переопределена
  // так, что xpdef в позиционных координатах становится новым началом.
  virtual void func_xpredef_and_dxpredef_by_dxp_and_dxpredef_by_dxpdef  
    (const VNL::Vector<double> &xp, const VNL::Vector<double> &xpdef) = 0;

  // Параметры позы: 3D-позиция и матрица поворота для графики и т.д.
  // Определяются для каждой размерности.
  virtual void func_xpose_and_Rpose(const VNL::Vector<double> &xv) = 0;

  // Проверяет, лежит ли позиционное состояние внутри ограничивающего бокса.
  virtual bool bounding_box_test(const VNL::Vector<double> &xp1, 
                                 const VNL::Vector<double> &xp2,
                                 const VNL::Vector<double> &xptest) = 0;

  /*************Эта функция определена в models_base.cc и*********************/
  /**********************не должна переопределяться************************/
  // Только для тестирования:
  // проверяет, выглядят ли якобианы осмысленно.
  bool jacobians_test(const VNL::Vector<double> &xv, 
                      const VNL::Vector<double> &u,
                      const double delta_t);

 protected:
  // Здесь хранятся результаты после вызовов функций.
  VNL::Vector<double> fvRES, xpRES, fv_noisyRES, xvredefRES, xvnormRES, 
    zeroedxvRES, xpredefRES, xposeRES;
  VNL::Matrix<double> dfv_by_dxvRES, QxRES, dxp_by_dxvRES, dxvredef_by_dxvRES, 
    dxvredef_by_dxpdefRES, dxvnorm_by_dxvRES, dzeroedxv_by_dxvRES, 
    dxpredef_by_dxpRES, dxpredef_by_dxpdefRES, RposeRES;

 public:
  // Геттеры.
  const VNL::Vector<double> &get_fvRES() const {return fvRES;} 
  const VNL::Vector<double> &get_xpRES() const {return xpRES;} 
  const VNL::Vector<double> &get_fv_noisyRES() const {return fv_noisyRES;} 
  const VNL::Vector<double> &get_xvredefRES() const {return xvredefRES;} 
  const VNL::Vector<double> &get_xvnormRES() const {return xvnormRES;} 
  const VNL::Vector<double> &get_zeroedxvRES() const {return zeroedxvRES;} 
  const VNL::Vector<double> &get_xpredefRES() const {return xpredefRES;} 
  const VNL::Vector<double> &get_xposeRES() const {return xposeRES;} 

  const VNL::Matrix<double> &get_dfv_by_dxvRES() const {return dfv_by_dxvRES;} 
  const VNL::Matrix<double> &get_QxRES() const {return QxRES;} 
  const VNL::Matrix<double> &get_dxp_by_dxvRES() const {return dxp_by_dxvRES;} 
  const VNL::Matrix<double> &get_dxvredef_by_dxvRES() const 
    {return dxvredef_by_dxvRES;} 
  const VNL::Matrix<double> &get_dxvredef_by_dxpdefRES() const 
    {return dxvredef_by_dxpdefRES;} 
  const VNL::Matrix<double> &get_dxvnorm_by_dxvRES() const 
    {return dxvnorm_by_dxvRES;} 
  const VNL::Matrix<double> &get_dzeroedxv_by_dxvRES() const 
    {return dzeroedxv_by_dxvRES;} 
  const VNL::Matrix<double> &get_dxpredef_by_dxpRES() const 
    {return dxpredef_by_dxpRES;} 
  const VNL::Matrix<double> &get_dxpredef_by_dxpdefRES() const 
    {return dxpredef_by_dxpdefRES;} 
  const VNL::Matrix<double> &get_RposeRES() const {return RposeRES;} 

  // Инициализация.
  void read_initial_state(std::istream& stream, 
                          VNL::Vector<double>& initial_xv,
                          VNL::Matrix<double>& initial_Pxx) const;
  void read_initial_state(Settings& settings,
                          VNL::Vector<double>& initial_xv,
                          VNL::Matrix<double>& initial_Pxx) const;
};

/** Общая 3D-модель движения.
Позиционное состояние в стандартном представлении:
\f$ (\begin{matrix}x & y & z & q_0 & q_1 & q_2 & q_3 \end{matrix})^T \f$
@ingroup gScene
**/
class ThreeD_Motion_Model : public Motion_Model
{
 public:
  ThreeD_Motion_Model(const unsigned int state_size, 
                      const unsigned int control_size,
                      const char *m_m_t);
  ~ThreeD_Motion_Model();

  // Общие функции модели движения.
  void func_xpredef_and_dxpredef_by_dxp_and_dxpredef_by_dxpdef  
    (const VNL::Vector<double> &xp, const VNL::Vector<double> &xpdef);
  void func_xpose_and_Rpose(const VNL::Vector<double> &xv);
  bool bounding_box_test(const VNL::Vector<double> &xp1, 
                         const VNL::Vector<double> &xp2,
                         const VNL::Vector<double> &xptest);

  // Функции, специфичные для 3D-случая.

  // Извлекает декартову часть позиционного вектора состояния.
  void func_r(const VNL::Vector<double> &xp);
  // И соответствующий якобиан.
  void func_dr_by_dxp(const VNL::Vector<double> &xp);

  // Извлекает кватернионную часть позиционного вектора состояния.
  void func_q(const VNL::Vector<double> &xp);
  // И соответствующий якобиан.
  void func_dq_by_dxp(const VNL::Vector<double> &xp);

  const VNL::MatrixFixed<3,7,double> &get_dr_by_dxpRES() const
    {return dr_by_dxpRES;}
  const VNL::MatrixFixed<4,7,double> &get_dq_by_dxpRES() const
    {return dq_by_dxpRES;}

 protected:
  // Матрицы результатов.
  VW::Vector3D rRES;
  VNL::MatrixFixed<3,7,double> dr_by_dxpRES;
  VW::Quaternion qRES;
  VNL::MatrixFixed<4,7,double> dq_by_dxpRES;

 public:
  // Геттеры.
  const VW::Vector3D &get_rRES() const {return rRES;} 
  const VW::Quaternion &get_qRES() const {return qRES;}
};


/** 2D-модель движения на плоскости.
Позиционное состояние в минимальном представлении:
\f$(\begin{matrix} z & x & \phi \end{matrix})\f$. По историческим причинам
"вперёд" соответствует оси z, а "влево" — оси x. Угол \f$ \phi \f$ задаёт
ориентацию против часовой стрелки относительно оси z и ограничен диапазоном
\f$-\pi < \phi \leq \pi \f$.
@ingroup gScene
**/
class TwoD_Motion_Model : public Motion_Model
{
 public:
  TwoD_Motion_Model(const unsigned int state_size, 
                    const unsigned int control_size,
                    const char *m_m_t)
    : Motion_Model(3, state_size, control_size, "TWOD", m_m_t) 
    {}

  void func_xpredef_and_dxpredef_by_dxp_and_dxpredef_by_dxpdef  
    (const VNL::Vector<double> &xp, const VNL::Vector<double> &xpdef);
  void func_xpose_and_Rpose(const VNL::Vector<double> &xv);
  bool bounding_box_test(const VNL::Vector<double> &xp1, 
                         const VNL::Vector<double> &xp2,
                         const VNL::Vector<double> &xptest);
};


/** 1D-модель.
Позиционное состояние в минимальном представлении: \f$( z )\f$. В 1D оно
описывает смещение вдоль прямой линии.
@ingroup gScene
**/
class OneD_Motion_Model : public Motion_Model
{
public:
  OneD_Motion_Model(const unsigned int state_size, 
                    const unsigned int control_size,
                    const char *m_m_t)
    : Motion_Model(1, state_size, control_size, "ONED", m_m_t) 
    {}

  void func_xpredef_and_dxpredef_by_dxp_and_dxpredef_by_dxpdef  
    (const VNL::Vector<double> &xp, const VNL::Vector<double> &xpdef);
  void func_xpose_and_Rpose(const VNL::Vector<double> &xv);
  bool bounding_box_test(const VNL::Vector<double> &xp1, 
                         const VNL::Vector<double> &xp2,
                         const VNL::Vector<double> &xptest);
};

/**
Класс для создания экземпляра модели движения по строке типа.
Это абстрактный базовый класс: авторы конкретных моделей движения должны
наследоваться от него и реализовывать создание своих моделей.
TODO: кто удаляет созданные объекты?
@ingroup gScene
**/
class Motion_Model_Creator
{
public:
  /// Конструктор. Ничего не делает.
  Motion_Model_Creator() {} 
  /// Деструктор. Ничего не делает.
  virtual ~Motion_Model_Creator() {} 

  /// Возвращает экземпляр запрошенной модели движения, обычно через оператор
  /// <code>new</code>.
  virtual Motion_Model* create_model(const std::string& type) = 0;
};


/*************************Модели измерения признаков**************************/

/** 
Базовый класс для классов, описывающих поведение измерений признаков. В этом
базовом классе лежит только общая функциональность: графическое представление,
преобразования и т.п. Производные классы
Partially_Initialised_Feature_Measurement_Model and
Fully_Initialised_Feature_Measurement_Model, а также классы, унаследованные от
них, задают основное поведение.

Сам этот класс не хранит постоянного состояния: он только предоставляет функции
для работы с состоянием и измерениями, которые хранятся снаружи. Основные
векторы состояния и ковариации:
<table border=0>
<tr><td>\f$ y_i \f$</td><td>Состояние признака, например 3D-вектор
положения.</td><td>
<tr><td>\f$ P_{y_iy_i} \f$</td><td>Ковариация признака
\f$y_i\f$.</td><td>
<tr><td>\f$ h_i \f$</td><td>Измерение состояния признака, например
положение признака на изображении.</td><td>
<tr><td>\f$ R_i \f$</td><td>Ковариация измерения \f$ h_i \f$, например
неопределённость из-за разрешения изображения.</td><td>
<tr><td>\f$ x_p \f$</td><td>Позиционное состояние робота, например его
3D-положение и ориентация.</td><td>
<tr><td>\f$ S_i \f$</td><td>Ковариация инновации: объединение
неопределённости состояния робота, состояния признака и измерения, например
общая неопределённость положения признака на изображении.</td><td>
</table>
@ingroup Scene
**/
class Feature_Measurement_Model
{
 public:
  /** Конструктор.
    @param measurement_size Число параметров, представляющих измерение признака.
    @param feature_state_size Число параметров для представления состояния
    признака.
    @param graphics_state_size Число параметров для графической абстракции
    признака.
    @param m_m Модель движения робота. Нужна, чтобы понимать и использовать
    позиционное состояние робота \f$x_p\f$ при измерении признаков.
    @param feature_type Имя этой модели измерения признака, задаётся
    производным классом.
    @param feature_graphics_type Имя типа признака для отрисовки, задаётся
    производным классом.
    @param fully_initialised_flag Относится ли эта модель к полностью
    инициализированным признакам? Если <code>false</code>, то к частично
    инициализированным. Задаётся производным классом.
  **/
  Feature_Measurement_Model(const unsigned int measurement_size,
                            const unsigned int feature_state_size,
                            const unsigned int graphics_state_size,
                            Motion_Model *m_m,
                            const char *feature_type,
                            const char *feature_graphics_type,
                            const bool fully_initialised_flag);

  /// Деструктор.
  virtual ~Feature_Measurement_Model();

  /** Считывает параметры этой модели измерения признака из настроек.*/
  virtual void read_parameters(const Settings& ) {}
  
  virtual void read_initial_state(
    const Settings::Section& settings,
    VNL::Vector<>& yi,
    VNL::Vector<>& xp_orig);
    
    // Константы: задаются в списке инициализации конкретных моделей.
  /// Число параметров, представляющих измерение признака.
  const unsigned int MEASUREMENT_SIZE; 

  /// Число параметров для представления состояния признака.
  const unsigned int FEATURE_STATE_SIZE;

  /// Число параметров для представления графической абстракции признака.
  /// Часто совпадает с FEATURE_STATE_SIZE, но не обязано совпадать.
  const unsigned int GRAPHICS_STATE_SIZE;
  
  /// Какая модель движения связана с этими измерениями признаков?
  Motion_Model* get_motion_model() const
    {return _motion_model;}
  
  /// Имя этой модели измерения признака, задаётся производным классом.
  const char *feature_type;
  /// Имя типа признака для отрисовки, задаётся производным классом.
  const char *feature_graphics_type;
  /// Относится ли эта модель к полностью инициализированным признакам?
  /// Если <code>false</code>, то к частично инициализированным.
  const bool fully_initialised_flag;
  /// Относится ли эта модель к полностью инициализированным признакам?
  /// Если <code>false</code>, то к частично инициализированным.
  bool fully_initialised() const {return fully_initialised_flag;}
  
  /** Берёт состояние и ковариацию признака (\f$ y_i \f$ и
     \f$ P_{{y_i}{y_i}}\f$) и вычисляет его графическое состояние
     \f$ y_i^{graphics} \f$ и ковариацию
     \f$ P_{{y_i}{{y_i^{graphics}}}} \f$. Иногда реальное представление
     состояния содержит дополнительные параметры и т.п.
     @param yi Состояние признака \f$ y_i \f$
     @param Pyiyi Ковариация признака \f$ P_{y_iy_i} \f$
  **/
  virtual void func_yigraphics_and_Pyiyigraphics(
          const VNL::Vector<double> &yi, const VNL::Matrix<double> &Pyiyi) = 0;

  /** Вычисляет графическое состояние признака относительно системы координат,
      обнулённой в текущей позиции робота. Это нужно для отрисовки вида,
      центрированного на роботе, с правильными относительными
      неопределённостями. Если графическое состояние совпадает с состоянием
      признака, функция делает то же, что func_zeroedyi() ниже, но иногда эти
      состояния различаются.
      @param yi Состояние признака \f$ y_i \f$
      @param xv Состояние робота \f$ x_v \f$
      @param Pxx Ковариация состояния робота \f$ P_{xx} \f$
      @param Pxyi Ковариация \f$ P_{xy_i} \f$ между состоянием робота
        \f$ x_v \f$ и состоянием признака \f$ y_i \f$.
      @param Pyiyi Ковариация признака \f$ P_{y_iy_i} \f$
  **/
  virtual void func_zeroedyigraphics_and_Pzeroedyigraphics(
    const VNL::Vector<double> &yi, const VNL::Vector<double> &xv,
    const VNL::Matrix<double> &Pxx,
    const VNL::Matrix<double> &Pxyi,
    const VNL::Matrix<double> &Pyiyi) = 0;

  /** Вычисляет \f$ \vct{y}_i^{zeroed} \f$: состояние признака в системе
      координат, где начало находится в позиционном состоянии
      \f$ \vct{x}_p \f$. Также вычисляет соответствующие якобианы \f$
      \partfrac{\vct{y}_i^{zeroed}}{\vct{x}_p} \f$ и \f$
      \partfrac{\vct{y}_i^{zeroed}}{\vct{y}_i} \f$. Эта функция используется
      как первый шаг при предсказании значений измерений.
      @param yi Состояние признака \f$ \vct{y}_i \f$
      @param xp Позиционное состояние \f$ \vct{x}_p \f$
  **/
  virtual void func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi(
    const VNL::Vector<double> &yi, const VNL::Vector<double> &xp) = 0;

  /** Вычисляет ковариацию \f$ R_i \f$ шума измерения для измерения
  с состоянием \f$ h_i \f$.
  @param hi Измерение признака \f$ h_i \f$
  */
  virtual void func_Ri(const VNL::Vector<double> &hi) = 0;

  /** Проверяет видимость признака по позиции робота \f$ x_p \f$, состоянию
      признака \f$ y_i \f$, исходной позиции робота при первом наблюдении
      признака \f$ x_p^{orig}\f$ и текущему предсказанному измерению
      \f$ h_i \f$. Используется при активном выборе измерений, чтобы решить,
      стоит ли пытаться измерить этот признак из позиции робота
      \f$ x_p \f$. Критерий должен задаваться по относительным положениям:
      например, успешное сопоставление можно ожидать только в ограниченном
      диапазоне движения от позиции, где признак впервые увидели и сохранили
      шаблон.
      @param xp Позиционное состояние робота \f$ x_p \f$
      @param yi Состояние признака \f$ y_i \f$
      @param xp_orig Позиционное состояние робота при первом наблюдении
        признака, \f$ x_p^{orig} \f$
      @param hi Текущее предсказанное измерение \f$ h_i \f$
      @returns ноль при успехе, любое другое значение при неудаче.
      **/
  virtual unsigned int visibility_test(const VNL::Vector<double> &xp, 
                                       const VNL::Vector<double> &yi,
                                       const VNL::Vector<double> &xp_orig,
                                       const VNL::Vector<double> &hi) = 0;

  /** Вычисляет оценку полезности признака на основе ковариации инновации:
      насколько полезно измерить именно этот признак? Обычно признаки с большой
      ковариацией инновации полезнее, потому что имеет смысл измерять там, где
      результат неопределён, а не там, где его можно точно предсказать.
      @param Si Ковариация инновации \f$ S_i \f$
  **/
  virtual double selection_score(const VNL::Matrix<double> &Si) = 0;

  // Ковариация инновации: функция не виртуальная, потому что она общая,
  // хотя при необходимости её можно переопределить.
  virtual void func_Si(const VNL::Matrix<double> &Pxx, 
                       const VNL::Matrix<double> &Pxyi,
                       const VNL::Matrix<double> &Pyiyi,
                       const VNL::Matrix<double> &dhi_by_dxv,
                       const VNL::Matrix<double> &dhi_by_dyi,
                       const VNL::Matrix<double> &Ri);

 protected:
  // Здесь хранятся результаты после вызовов функций.
  VNL::Vector<double> yigraphicsRES, zeroedyigraphicsRES, zeroedyiRES;
  VNL::Matrix<double> PyiyigraphicsRES, PzeroedyigraphicsRES, 
    RiRES, SiRES, dzeroedyi_by_dxpRES, dzeroedyi_by_dyiRES;
  Motion_Model* _motion_model;

 public:
  // Геттеры.
  /// Возвращает графическое состояние признака \f$ y_i^{graphics} \f$,
  /// вычисленное в func_yigraphics_and_Pyiyigraphics().
  const VNL::Vector<double> &get_yigraphicsRES() const {return yigraphicsRES;} 
  /// Возвращает графическое состояние признака, как если бы робот находился
  /// в другой системе координат: \f$ y_i^{zeroedgraphics} \f$, вычисленное в
  /// func_zeroedyigraphics_and_Pzeroedyigraphics().
  const VNL::Vector<double> &get_zeroedyigraphicsRES() const 
    {return zeroedyigraphicsRES;} 
  /// Возвращает состояние признака, как если бы робот находился в другой
  /// системе координат: \f$ y_i^{zeroed} \f$, вычисленное в
  /// func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi().
  const VNL::Vector<double> &get_zeroedyiRES() const {return zeroedyiRES;} 

  /// Возвращает ковариацию графического состояния признака
  /// \f$ P_{{y_i}{{y_i^{graphics}}}} \f$, вычисленную в
  /// func_yigraphics_and_Pyiyigraphics().
  const VNL::Matrix<double> &get_PyiyigraphicsRES() const 
    {return PyiyigraphicsRES;} 
  /// Возвращает ковариацию графического состояния признака в другой системе
  /// координат: \f$ P_{{y_i}{{y_i^{zeroedgraphics}}}} \f$, вычисленную в
  /// func_zeroedyigraphics_and_Pzeroedyigraphics().
  const VNL::Matrix<double> &get_PzeroedyigraphicsRES() const 
    {return PzeroedyigraphicsRES;} 
  /// Возвращает ковариацию измерения признака \f$ R_i \f$, вычисленную ранее,
  /// например через func_Ri().
  const VNL::Matrix<double> &get_RiRES() const {return RiRES;} 
  /** Возвращает ковариацию инновации, то есть общую неопределённость на
  изображении для этого признака, \f$ S_i \f$, вычисленную func_Si().*/
  const VNL::Matrix<double> &get_SiRES() const {return SiRES;} 
  /// Возвращает якобиан
  /// \f$ \frac{\partial y_i^{zeroed}}{\partial x_p} \f$, вычисленный через
  /// func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi().
  const VNL::Matrix<double> &get_dzeroedyi_by_dxpRES() const 
    {return dzeroedyi_by_dxpRES;} 
  /// Возвращает якобиан
  /// \f$ \frac{\partial y_i^{zeroed}}{\partial y_i} \f$, вычисленный через
  /// func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi().
  const VNL::Matrix<double> &get_dzeroedyi_by_dyiRES() const 
    {return dzeroedyi_by_dyiRES;} 
};

/**
Класс для признака, который уже полностью инициализирован и поэтому доступен
для измерений. Этот класс не хранит постоянного состояния: он только даёт
вспомогательные функции для преобразования между разными векторами состояния.

Используемые векторы состояния и ковариации:
<table border=0>
<tr><td>\f$ y_i \f$</td><td>Состояние признака, например 3D-вектор
положения.</td><td>
<tr><td>\f$ h_i \f$</td><td>Состояние измерения признака, например проекция
положения признака на изображение.</td><td>
<tr><td>\f$ z_i \f$</td><td>Реальное измерение состояния признака.</td><td>
<tr><td>\f$ \nu_i \f$</td><td>Инновация: разница между ожидаемым измерением
\f$ h_i \f$ и реальным измерением \f$ z_i \f$.</td><td>
<tr><td>\f$ x_p \f$</td><td>Позиционное состояние робота, например его
3D-положение и ориентация.</td><td>
</table>
@ingroup gScene
**/
class Fully_Initialised_Feature_Measurement_Model 
: public Feature_Measurement_Model {

 public:
  Fully_Initialised_Feature_Measurement_Model(
    const unsigned int measurement_size,
    const unsigned int feature_state_size,
    const unsigned int graphics_state_size,
    Motion_Model *motion_model,
    const char *feature_type,
    const char *feature_graphics_type);

  /// Деструктор.
  virtual ~Fully_Initialised_Feature_Measurement_Model();

  /** Основная функция измерения. Вычисляет измерение \f$ h_i \f$ из состояния
  признака \f$ y_i \f$ и позиционного состояния робота \f$ x_p \f$, а также
  вычисляет якобианы \f$ \frac{\partial h_i}{\partial x_p} \f$ и
  \f$ \frac{\partial h_i}{\partial y_i} \f$.
  @param yi Состояние признака \f$ y_i \f$
  @param xp Позиционное состояние робота \f$ xp \f$
  */
  virtual void func_hi_and_dhi_by_dxp_and_dhi_by_dyi(
    const VNL::Vector<double> &yi,
    const VNL::Vector<double> &xp) = 0;

  /** Вычисляет инновацию \f$ \nu_i \f$: разницу между предсказанным измерением
      \f$ h_i \f$ и реальным измерением \f$ z_i \f$. Обычно это просто
      вычитание \f$ \nu_i = z_i - h_i \f$, но функция оставлена виртуальной на
      случай других ситуаций, например если измерение — угол, который нужно
      нормализовать в диапазон \f$ -\pi \rightarrow \pi \f$.
      @param hi Ожидаемое измерение признака \f$ h_i \f$
      @param zi Реальное измерение признака \f$ z_i \f$
  */
  virtual void func_nui(const VNL::Vector<double> &hi, 
    const VNL::Vector<double> &zi) = 0;

  /** Зашумлённое измерение для симуляции: создаёт измерение с добавленным
  случайным шумом.
  @param yi_true Истинное состояние признака \f$ yi \f$
  @param xp_true Истинное позиционное состояние робота \f$ x_p \f$
  */
  virtual void func_hi_noisy(const VNL::Vector<double> &yi_true, 
    const VNL::Vector<double> &xp_true) = 0;

 protected:
  VNL::Vector<double> hiRES, nuiRES, hi_noisyRES;
  VNL::Matrix<double> dhi_by_dxpRES, dhi_by_dyiRES;

 public:
  // Геттеры.
  /// Возвращает ожидаемое измерение признака \f$ h_i \f$, вычисленное через
  /// func_hi_and_dhi_by_dxp_and_dhi_by_dyi().
  const VNL::Vector<double> &get_hiRES() const {return hiRES;} 
  /// Возвращает инновацию \f$ \nu_i \f$, вычисленную через func_nui().
  const VNL::Vector<double> &get_nuiRES() const {return nuiRES;} 
  /// Возвращает зашумлённое измерение \f$ h_i^noisy \f$, вычисленное через
  /// func_hi_noisy().
  const VNL::Vector<double> &get_hi_noisyRES() const {return hi_noisyRES;} 

  /// Возвращает якобиан \f$ \frac{\partial h_i}{\partial x_p} \f$,
  /// вычисленный через func_hi_and_dhi_by_dxp_and_dhi_by_dyi().
  const VNL::Matrix<double> &get_dhi_by_dxpRES() const {return dhi_by_dxpRES;} 
  /// Возвращает \f$ \frac{\partial h_i}{\partial y_i} \f$, вычисленный через
  /// func_hi_and_dhi_by_dxp_and_dhi_by_dyi().
  const VNL::Matrix<double> &get_dhi_by_dyiRES() const {return dhi_by_dyiRES;} 
};



/**
Класс для признака, который не может быть полностью инициализирован по одному
измерению и поэтому требует дополнительных измерений. Обычно у частично
инициализированного признака есть известные параметры с гауссовой
неопределённостью и свободные параметры, представленные другим способом,
например частицами. Этот класс не хранит постоянного состояния: он только
предоставляет вспомогательные функции для преобразования между разными
векторами состояния.

Используемые векторы состояния и ковариации:
<table border=0>
<tr><td>\f$ y_{pi} \f$</td><td>Частичное состояние признака, например
параметры 3D-линии.</td><td>
<tr><td>\f$ h_{pi} \f$</td><td>Состояние измерения частично
инициализированного признака, например положение признака на изображении.</td><td>
<tr><td>\f$ h_i \f$</td><td>Начальное измерение признака, используемое для
инициализации частично инициализированного признака.</td><td>
<tr><td>\f$ R_i \f$</td><td>Ковариация измерения \f$ h_i \f$.</td><td>
<tr><td>\f$ \lambda \f$</td><td>Вектор значений свободных параметров.</td><td>
<tr><td>\f$ y_{fi} \f$</td><td>Состояние полностью инициализированного признака,
которое можно построить по \f$ y_{pi} \f$ и \f$ \lambda \f$.</td><td>
<tr><td>\f$ x_p \f$</td><td>Позиционное состояние робота, например его
3D-положение и ориентация.</td><td>
</table>
@ingroup gScene
**/
class Partially_Initialised_Feature_Measurement_Model 
: public Feature_Measurement_Model {

 public:
  Partially_Initialised_Feature_Measurement_Model(
    const unsigned int measurement_size,
    const unsigned int feature_state_size,
    const unsigned int graphics_state_size,
    Motion_Model *motion_model,
    const char *feature_type,
    const char *feature_graphics_type,
    const unsigned int free_parameter_size,
    Feature_Measurement_Model*
    more_initialised_feature_measurement_model);
  /// Деструктор.
  virtual ~Partially_Initialised_Feature_Measurement_Model();

  virtual void read_parameters(const Settings& settings);
  
  /** Число параметров, которые должны оставаться вне основного гауссова
      представления и описываться другим способом, например частицами. */
  const unsigned int FREE_PARAMETER_SIZE;

  /// В какую модель измерения признака это превратится после полной
  /// инициализации?
  Feature_Measurement_Model *const more_initialised_feature_measurement_model;

  /** Инициализирует новый частичный признак \f$ y_{pi} \f$ как функцию
      измерения \f$ h_i \f$ и позиционного состояния робота \f$ x_p \f$.
      Также вычисляет якобианы \f$ \frac{\partial y_{pi}}{\partial x_p} \f$,
      \f$ \frac{\partial y_{pi}}{\partial y_i} \f$ и неопределённость
      измерения \f$ Ri \f$.
  */
  virtual void func_ypi_and_dypi_by_dxp_and_dypi_by_dhi_and_Ri(
    const VNL::Vector<double> &hi,
    const VNL::Vector<double> &xp) = 0;

  /** Вычисляет измерение \f$ h_{pi} \f$ из частичного состояния признака
      \f$ y_i \f$, позиционного состояния робота \f$ x_p \f$ и набора значений
      \f$ \lambda \f$ для свободных параметров признака. Также вычисляет
      якобианы \f$ \frac{\partial h_{pi}}{\partial x_p} \f$ и
      \f$ \frac{\partial h_{pi}}{\partial y_i} \f$. */
  virtual void func_hpi_and_dhpi_by_dxp_and_dhpi_by_dyi(
    const VNL::Vector<double> &yi,
    const VNL::Vector<double> &xp,
    const VNL::Vector<double> &lambda) = 0;


  /** Преобразует состояние частично инициализированного признака
      \f$ y_{pi} \f$ в состояние полностью инициализированного признака
      \f$ y_{fi} \f$, используя значения свободных параметров \f$ \lambda \f$.
      Также вычисляет якобианы \f$ \frac{\partial yh_{fi}}{\partial y_{pi}} \f$
      и \f$ \frac{\partial y_{fi}}{\partial \lambda} \f$. */
  virtual void func_yfi_and_dyfi_by_dypi_and_dyfi_by_dlambda(
    const VNL::Vector<double> &ypi,
    const VNL::Vector<double> &lambda) = 0;

 protected:
  // Здесь хранятся результаты после вызовов функций.
  VNL::Vector<double> ypiRES, hpiRES, yfiRES;
  VNL::Matrix<double> dypi_by_dxpRES, dypi_by_dhiRES, dhpi_by_dxpRES, 
    dhpi_by_dyiRES, dyfi_by_dypiRES, dyfi_by_dlambdaRES;

 public:
  // Геттеры.
  /// Возвращает значение \f$ y_{pi} \f$, вычисленное ранее.
  const VNL::Vector<double> &get_ypiRES() const {return ypiRES;} 
  /// Возвращает значение \f$ h_{pi} \f$, вычисленное ранее.
  const VNL::Vector<double> &get_hpiRES() const {return hpiRES;} 
  /// Возвращает значение \f$ y_{fi} \f$, вычисленное ранее.
  const VNL::Vector<double> &get_yfiRES() const {return yfiRES;} 

  /// Возвращает \f$ \frac{\partial y_{pi}}{\partial x_p} \f$, вычисленное
  /// ранее.
  const VNL::Matrix<double> &get_dypi_by_dxpRES() const 
    {return dypi_by_dxpRES;} 
  /// Возвращает \f$ \frac{\partial y_{pi}}{\partial h_i} \f$, вычисленное
  /// ранее.
  const VNL::Matrix<double> &get_dypi_by_dhiRES() const 
    {return dypi_by_dhiRES;} 
  /// Возвращает \f$ \frac{\partial h_{pi}}{\partial x_p} \f$, вычисленное
  /// ранее.
  const VNL::Matrix<double> &get_dhpi_by_dxpRES() const 
    {return dhpi_by_dxpRES;} 
  /// Возвращает \f$ \frac{\partial h_{pi}}{\partial y_i} \f$, вычисленное
  /// ранее.
  const VNL::Matrix<double> &get_dhpi_by_dyiRES() const
    {return dhpi_by_dyiRES;} 
  /// Возвращает \f$ \frac{\partial h_{fi}}{\partial y_{pi}} \f$, вычисленное
  /// ранее.
  const VNL::Matrix<double> &get_dyfi_by_dypiRES() const
    {return dyfi_by_dypiRES;} 
  /// Возвращает \f$ \frac{\partial h_{fi}}{\partial \lambda} \f$, вычисленное
  /// ранее.
  const VNL::Matrix<double> &get_dyfi_by_dlambdaRES() const 
    {return dyfi_by_dlambdaRES;} 
};

/**
Класс для создания экземпляра модели измерения признака по строке типа. Это
абстрактный базовый класс: авторы конкретных моделей измерения признаков должны
наследоваться от него и реализовывать создание своих моделей.
@ingroup gScene
**/
class Feature_Measurement_Model_Creator
{
public:
  /// Конструктор.
  /// @param m_m Модель движения, используемая при создании
  /// Feature_Measurement_Model
  Feature_Measurement_Model_Creator() {} 
  /// Деструктор. Ничего не делает.
  virtual ~Feature_Measurement_Model_Creator() {} 

  /// Возвращает экземпляр запрошенной модели измерения признака, обычно через
  /// оператор <code>new</code>. TODO: кто удаляет эти объекты?
  virtual Feature_Measurement_Model* create_model(const std::string& type,
    Motion_Model *motion_model) = 0;
};


/**************************Модель внутренних измерений*************************/

/**
Для измерений, которые относятся непосредственно к состоянию робота.
@ingroup gScene
*/
class Internal_Measurement_Model
{
 public:
  // Конструктор.
  Internal_Measurement_Model(const unsigned int measurement_size, 
                             Motion_Model *m_m,
                             const char *i_t);

  /**
  Деструктор ничего не делает. Также есть явная функция-деструктор
  Internal_Measurement_Model_destructor(), очищающая матрицы. TODO: почему это
  не находится в обычном деструкторе, который вызывался бы явно?
  */
  virtual ~Internal_Measurement_Model() {}
  
  void Internal_Measurement_Model_destructor();

  // Константы: задаются в списке инициализации конкретных моделей.
  const unsigned int MEASUREMENT_SIZE;
  Motion_Model *motion_model;
  const char *internal_type;

  /* Вычислительные функции, определяющие модель. Некоторые из них возвращают
     сразу несколько результатов: это сделано ради эффективности, потому что
     эти результаты часто нужны вместе. */

  // Функция измерения.
  virtual void func_hv_and_dhv_by_dxv(const VNL::Vector<double> &xv) = 0;

  // Шум измерения.
  virtual void func_Rv(const VNL::Vector<double> &hv) = 0;

  // Вычисление инновации.
  virtual void func_nuv(const VNL::Vector<double> &hv, 
                        const VNL::Vector<double> &zv) = 0;

  // Зашумлённое измерение для симуляции.
  virtual void func_hv_noisy(const VNL::Vector<double> &xv_true) = 0;

  // Проверка допустимости измерения.
  virtual bool feasibility_test(const VNL::Vector<double> &xv, 
                                const VNL::Vector<double> &hv) = 0;

  // Ковариация инновации: функция не виртуальная, потому что она общая.
  void func_Sv(const VNL::Matrix<double> &Pxx, 
               const VNL::Matrix<double> &dhv_by_dxv,
               const VNL::Matrix<double> &Rv);

 protected:
  // Здесь хранятся результаты после вызовов функций.
  VNL::Vector<double> hvRES, nuvRES, hv_noisyRES;
  VNL::Matrix<double> dhv_by_dxvRES, RvRES, SvRES; 

 public:
  Motion_Model *get_motion_model() {return motion_model;}

  // Геттеры.
  const VNL::Vector<double> &get_hvRES() const {return hvRES;} 
  const VNL::Vector<double> &get_nuvRES() const {return nuvRES;} 
  const VNL::Vector<double> &get_hv_noisyRES() const {return hv_noisyRES;} 

  const VNL::Matrix<double> &get_dhv_by_dxvRES() const {return dhv_by_dxvRES;} 
  const VNL::Matrix<double> &get_RvRES() const {return RvRES;} 
  const VNL::Matrix<double> &get_SvRES() const {return SvRES;} 

};

/**
Класс для создания экземпляра модели внутренних измерений по строке типа. Это
абстрактный базовый класс: авторы конкретных моделей внутренних измерений
должны наследоваться от него и реализовывать создание своих моделей.
@ingroup gScene
**/
class Internal_Measurement_Model_Creator
{
public:
  /// Конструктор.
  /// @param m_m Модель движения, используемая при создании
  /// Internal_Measurement_Model
  Internal_Measurement_Model_Creator() {} 
  /// Деструктор. Ничего не делает.
  virtual ~Internal_Measurement_Model_Creator() {} 

  /// Возвращает экземпляр запрошенной модели внутреннего измерения, обычно
  /// через оператор <code>new</code>. TODO: кто удаляет эти объекты?
  virtual Internal_Measurement_Model* create_model(const std::string& type,
    Motion_Model *motion_model) = 0;
};


#endif
