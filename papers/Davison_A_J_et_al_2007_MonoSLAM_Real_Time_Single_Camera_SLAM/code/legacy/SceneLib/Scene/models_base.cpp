/*  SceneLib: software for real-time SLAM

    models_base.cpp
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
#include "settings.h"
#include "exceptions.h"

using namespace std;

/***************************Функции модели движения****************************/

/** 
Конструктор базового класса. Его нужно вызывать в конструкторе любого
производного класса.
@param position_state_size Размерность позиционного состояния
@param state_size Полный размер состояния
@param state_size Размер управляющего состояния
@param m_m_d_t Строка, представляющая размерность модели движения, например
<code>THREED</code>
@param m_m_t Уникальная строка, представляющая конкретный тип модели движения
**/
Motion_Model::Motion_Model(const unsigned int position_state_size, 
			   const unsigned int state_size, 
			   const unsigned int control_size, 
			   const char *m_m_d_t, const char *m_m_t)
  : POSITION_STATE_SIZE(position_state_size),
    STATE_SIZE(state_size),
    CONTROL_SIZE(control_size),
    motion_model_dimensionality_type(m_m_d_t),
    motion_model_type(m_m_t)
{
  fvRES.Resize(STATE_SIZE);
  dfv_by_dxvRES.Resize(STATE_SIZE, STATE_SIZE);
  QxRES.Resize(STATE_SIZE, STATE_SIZE);
  xpRES.Resize(POSITION_STATE_SIZE);
  dxp_by_dxvRES.Resize(POSITION_STATE_SIZE, STATE_SIZE);
  fv_noisyRES.Resize(STATE_SIZE);
  xvredefRES.Resize(STATE_SIZE);
  dxvredef_by_dxvRES.Resize(STATE_SIZE, STATE_SIZE);
  dxvredef_by_dxpdefRES.Resize(STATE_SIZE, POSITION_STATE_SIZE);
  xvnormRES.Resize(STATE_SIZE);
  dxvnorm_by_dxvRES.Resize(STATE_SIZE, STATE_SIZE);
  zeroedxvRES.Resize(STATE_SIZE);
  dzeroedxv_by_dxvRES.Resize(STATE_SIZE, STATE_SIZE);
  xpredefRES.Resize(POSITION_STATE_SIZE);
  dxpredef_by_dxpRES.Resize(POSITION_STATE_SIZE, POSITION_STATE_SIZE);
  dxpredef_by_dxpdefRES.Resize(POSITION_STATE_SIZE, 
			       POSITION_STATE_SIZE);
}

/** Явный деструктор. Сейчас ничего не делает. TODO: почему это здесь, а не в
обычном деструкторе?
*/
void Motion_Model::Motion_Model_destructor()
{
}

// Нормализует вектор состояния, если используется избыточное представление.
// Пустая версия по умолчанию.
void Motion_Model::
func_xvnorm_and_dxvnorm_by_dxv(const VNL::Vector<double> &xv)
{
  xvnormRES.Update(xv);
  dxvnorm_by_dxvRES.SetIdentity();
}

// Обнуляет оси в текущем состоянии: переопределяет систему координат так, что
// текущее оценённое позиционное состояние становится новым началом координат.
void Motion_Model::
func_zeroedxv_and_dzeroedxv_by_dxv(const VNL::Vector<double> &xv)
{
  func_xp(xv);

  func_xvredef_and_dxvredef_by_dxv_and_dxvredef_by_dxpdef(xv, xpRES);

  zeroedxvRES.Update(xvredefRES);

  // Здесь есть тонкость: нужно найти dzeroedxv_by_dxv, а у нас есть два
  // якобиана dxvredef_by_dxv и dxvredef_by_dxpdef. При этом важно помнить,
  // что xv и xpdef не независимы.
  func_dxp_by_dxv(xv);

  VNL::Matrix<double> Temp_SS1 = dxvredef_by_dxpdefRES * dxp_by_dxvRES;
  dzeroedxv_by_dxvRES = Temp_SS1 + dxvredef_by_dxvRES;  
}

const double INCREMENT_SIZE = 0.00000001;
bool Motion_Model::
jacobians_test(const VNL::Vector<double> &, const VNL::Vector<double> &, 
               double )
{
  return true;
}


/**
Считывает начальный вектор состояния и ковариацию из потока.
Так как состояние \f$ x_v \f$ и ковариация \f$ P_{xx} \f$ не хранятся в этом
классе, они передаются по ссылке и заполняются внутри функции.
**/
void Motion_Model::read_initial_state(istream& stream, 
                                      VNL::Vector<double>& initial_xv,
                                      VNL::Matrix<double>& initial_Pxx) const
{
  // Убеждаемся, что вектор и матрица имеют правильные размеры.
  initial_xv.Resize(STATE_SIZE);
  initial_Pxx.Resize(STATE_SIZE, STATE_SIZE);

  initial_xv.Fill(0.0);
  initial_Pxx.Fill(0.0);

  for(unsigned int r = 0; r < STATE_SIZE; r++)
    stream >> initial_xv(r);

  for(unsigned int r = 0; r < STATE_SIZE; r++)
    for(unsigned int c = 0; c < STATE_SIZE; c++)
      stream >> initial_Pxx(r, c);
}

/**
Считывает начальный вектор состояния и ковариацию из класса настроек.
Так как состояние \f$ x_v \f$ и ковариация \f$ P_{xx} \f$ не хранятся в этом
классе, они передаются по ссылке и заполняются внутри функции.
**/
void Motion_Model::read_initial_state(Settings& settings, 
                                      VNL::Vector<double>& initial_xv,
                                      VNL::Matrix<double>& initial_Pxx) const
{
  // Проверяем, что модель движения совпадает с ожидаемой.
  if(settings.get_entry("InitialState", "MotionModel") != motion_model_type)
  {
    ostringstream error;
    error << "Attempted to read an initial state with a motion model of type \""
          << motion_model_type
          << "\" where the initialisation data in the [InitialState] section"
          " reports the type \""
          << settings.get_entry("InitialState", "MotionModel") << "\".";
    throw Scene::InitialisationError(error.str());
  }

  // Убеждаемся, что вектор и матрица имеют правильные размеры.
  initial_xv.Resize(STATE_SIZE);
  initial_Pxx.Resize(STATE_SIZE, STATE_SIZE);

  initial_xv.Fill(0.0);
  initial_Pxx.Fill(0.0);

  istringstream xv_stream(settings.get_entry("InitialState", "xv"));
  initial_xv.ReadASCII(xv_stream);

  istringstream Pxx_stream(settings.get_entry("InitialState", "Pxx"));
  initial_Pxx.ReadASCII(Pxx_stream);
}

/**
Конструктор.
@param state_size Полное число параметров в состоянии модели
@param control_size Число параметров в управляющем состоянии
@param m_m_t Уникальная строка, представляющая конкретный тип модели движения
**/
ThreeD_Motion_Model::
ThreeD_Motion_Model(const unsigned int state_size, 
                    const unsigned int control_size,
                    const char *m_m_t)
  : Motion_Model(7, state_size, control_size, "THREED", m_m_t) 
{
}

/**
Деструктор.
Ничего не делает.
*/
ThreeD_Motion_Model::~ThreeD_Motion_Model()
{
}

void ThreeD_Motion_Model::
    func_xpredef_and_dxpredef_by_dxp_and_dxpredef_by_dxpdef  
    (const VNL::Vector<double> &xp, const VNL::Vector<double> &xpdef)
{
  // Разделяем позиционное состояние на декартову часть и кватернион.

  // r0, q0: позиция в исходной системе координат.
  func_r(xp);
  func_q(xp);
  VW::Vector3D r0 = rRES;
  VW::Quaternion q0 = qRES;

  // rn, qn: определение новой системы координат.
  func_r(xpdef);
  func_q(xpdef);
  VW::Vector3D rn = rRES;
  VW::Quaternion qn = qRES;

  VW::Quaternion qnbar = qn.Conjugate();

  // Вычисляем новую декартову часть xp.
  VW::Vector3D Temp31a = r0 - rn; // Temp31a равен r0 - rn.
  VW::RotationMatrix Temp33a = qnbar.RotationMatrix();
  VW::Vector3D Temp31b = Temp33a * Temp31a;

  // Копируем в xpredefRES.
  xpredefRES(0) = Temp31b.GetX();
  xpredefRES(1) = Temp31b.GetY();
  xpredefRES(2) = Temp31b.GetZ();

  // Вычисляем новую кватернионную часть xp.
  VW::Quaternion Tempqa = qnbar * q0;

  // Копируем в xpredefRES.
  VNL::VectorFixed<4,double> vecTempqa = Tempqa.GetRXYZ();
  xpredefRES.Update(vecTempqa, 3);

  // Формируем якобиан dxpredef_by_dxpRES.
  dxpredef_by_dxpRES.Fill(0.0);
  dxpredef_by_dxpRES.Update(Temp33a, 0, 0);

  VNL::MatrixFixed<4,4,double> Temp44a = dq3_by_dq1(qnbar);
  dxpredef_by_dxpRES.Update(Temp44a, 3, 3);

  // Формируем якобиан dxpredef_by_dxpdefRES.
  dxpredef_by_dxpdefRES.Fill(0.0);
  Temp33a *= -1.0;
  dxpredef_by_dxpdefRES.Update(Temp33a, 0, 0);

  Temp44a = dq3_by_dq2(q0);
  
  VNL::MatrixFixed<4,4,double> Temp44b = dqbar_by_dq();
  VNL::MatrixFixed<4,4,double> Temp44c = Temp44a * Temp44b;
  dxpredef_by_dxpdefRES.Update(Temp44c, 3, 3);

  // Верхний правый угол этого якобиана сложный, потому что нужно
  // дифференцировать матрицу поворота.
  // Используется функция, которая делает это в bits.cc.
  // Собираем этот угол в Temp34a.
  VNL::MatrixFixed<3,4,double> Temp34a =
    dRq_times_a_by_dq(qnbar, Temp31a.GetVNL3());

  // Пока у нас есть drN_by_dqnbar; нужен вариант по dqn.
  VNL::MatrixFixed<3,4,double> Temp34b = Temp34a * Temp44b;

  // Наконец копируем в матрицу результата.
  dxpredef_by_dxpdefRES.Update(Temp34b, 0, 3);

  // cout << "dxpredef_by_dxpdefRES" << dxpredef_by_dxpdefRES;
}

void ThreeD_Motion_Model::func_xpose_and_Rpose(const VNL::Vector<double> &xv)
{
  func_xp(xv);

  // Преобразуем вектор xp (x, y, z, q0, q1, q2, q3) в позу.
  xposeRES = xpRES.Extract(3, 0);

  VNL::VectorFixed<4,double> qvec = xpRES.Extract(4, 3);
  VW::Quaternion q;
  q.SetRXYZ(qvec);

  RposeRES = q.RotationMatrix();
}

bool ThreeD_Motion_Model::bounding_box_test(const VNL::Vector<double> &xp1, 
					    const VNL::Vector<double> &xp2, 
					    const VNL::Vector<double> &xptest)
{
  // 3D-позиционное состояние xp (x, y, z, q0, q1, q2, q3).
  // Проверяем только x, y, z.
  return (xptest(0) >= xp1(0) &&
          xptest(0) <  xp2(0) &&
          xptest(1) >= xp1(1) &&
          xptest(1) <  xp2(1) &&
          xptest(2) >= xp1(2) &&
          xptest(2) <  xp2(2));
}

/** 
Извлекает декартову трансляционную часть позиционного вектора состояния.
**/
void ThreeD_Motion_Model::func_r(const VNL::Vector<double> &xp)
{
  rRES.Set(xp(0), xp(1), xp(2));
}

void ThreeD_Motion_Model::func_dr_by_dxp(const VNL::Vector<double> &)
{
  dr_by_dxpRES.Fill(0.0);

  dr_by_dxpRES.Put(0, 0, 1.0);
  dr_by_dxpRES.Put(1, 1, 1.0);
  dr_by_dxpRES.Put(2, 2, 1.0);
}

/** 
Извлекает вращательную, кватернионную часть позиционного вектора состояния.
**/
void ThreeD_Motion_Model::func_q(const VNL::Vector<double> &xp)
{
  VNL::VectorFixed<4,double> qvec = xp.Extract(4, 3);
  qRES.SetRXYZ(qvec);
}

void ThreeD_Motion_Model::func_dq_by_dxp(const VNL::Vector<double> &)
{
  dq_by_dxpRES.Fill(0.0);
  
  dq_by_dxpRES.Put(0, 3, 1.0);
  dq_by_dxpRES.Put(1, 4, 1.0);
  dq_by_dxpRES.Put(2, 5, 1.0);
  dq_by_dxpRES.Put(3, 6, 1.0);
}

void TwoD_Motion_Model::
func_xpredef_and_dxpredef_by_dxp_and_dxpredef_by_dxpdef
(const VNL::Vector<double> &xp, const VNL::Vector<double> &xpdef)
{
  double zdef = xpdef(0);
  double xdef = xpdef(1);
  double phidef = xpdef(2);
  double cphidef = cos(phidef);
  double sphidef = sin(phidef);
  double z0 = xp(0);
  double x0 = xp(1);
  double phi0 = xp(2);

  double a[3] = {sphidef * (x0 - xdef) + cphidef * (z0 - zdef),
		 cphidef * (x0 - xdef) - sphidef * (z0 - zdef),
		 phi0 - phidef};
  xpredefRES.CopyIn(a);
  and_pi_range(xpredefRES(2));

  double b[9] = {cphidef, sphidef, 0.0,
		-sphidef, cphidef, 0.0,
		 0.0, 0.0, 1.0};
  dxpredef_by_dxpRES.CopyIn(b);

  double c[9] = 
    {-cphidef, -sphidef, (x0 - xdef) * cphidef - (z0 - zdef) * sphidef,
     sphidef, -cphidef, -(x0 - xdef) * sphidef - (z0 - zdef) * cphidef,
     0.0, 0.0, -1.0};
  dxpredef_by_dxpdefRES.CopyIn(c);

  
}

void TwoD_Motion_Model::func_xpose_and_Rpose(const VNL::Vector<double> &xv)
{
  func_xp(xv);

  // Преобразуем вектор xp (z, x, phi) в позу.
  xposeRES(0) = xpRES(1); 
  xposeRES(1) = 0.0; 
  xposeRES(2) = xpRES(0);

  double a[9] = {cos(xpRES(2)), 0.0, sin(xpRES(2)),
                 0.0, 1.0, 0.0,
                 -sin(xpRES(2)), 0.0, cos(xpRES(2))};
                                        /* Rpose — это MLR */
  RposeRES.CopyIn(a);
}


bool TwoD_Motion_Model::bounding_box_test(const VNL::Vector<double> &xp1, 
                                          const VNL::Vector<double> &xp2,
                                          const VNL::Vector<double> &xptest)
{
  //  cout << "xp1" << xp1 << "xp2" << xp2 << "xptest" << xptest;

  // 2D-позиционное состояние xp (z, x, phi).
  // Проверяем только z и x.
  return (xptest(0) >= xp1(0) &&
          xptest(0) <  xp2(0) &&
          xptest(1) >= xp1(1) &&
          xptest(1) <  xp2(1));
}

void OneD_Motion_Model::func_xpredef_and_dxpredef_by_dxp_and_dxpredef_by_dxpdef
    (const VNL::Vector<double> &xp, const VNL::Vector<double> &xpdef)
{
  double zdef = xpdef(0);
  double z0 = xp(0);

  xpredefRES(0) = z0 - zdef;

  dxpredef_by_dxpRES(0, 0) = 1.0;

  dxpredef_by_dxpdefRES(0, 0) = -1.0;

  
}

void OneD_Motion_Model::func_xpose_and_Rpose(const VNL::Vector<double> &xv)
{
  func_xp(xv);

  // Преобразуем вектор xp (z) в позу.
  xposeRES(0) = 0.0; 
  xposeRES(1) = 0.0; 
  xposeRES(2) = xpRES(0);

  RposeRES.SetIdentity();
                                        /* Rpose — это MLR */
  
}

bool OneD_Motion_Model::bounding_box_test(const VNL::Vector<double> &xp1, 
                                          const VNL::Vector<double> &xp2,
                                          const VNL::Vector<double> &xptest)
{
  return(xptest(0) >= xp1(0) &&
         xptest(0) <  xp2(0));
}

/**********************Функции моделей измерения признаков********************/

// Модель измерения признака: базовый класс.
Feature_Measurement_Model::Feature_Measurement_Model(
                                 const unsigned int measurement_size, 
				 const unsigned int feature_state_size,
		       	         const unsigned int graphics_state_size,
			         Motion_Model *m_m, 
				 const char *f_t, const char *f_g_t,
                                 bool f_i_f)
  : MEASUREMENT_SIZE(measurement_size), 
    FEATURE_STATE_SIZE(feature_state_size),
    GRAPHICS_STATE_SIZE(graphics_state_size),
    feature_type(f_t),
    feature_graphics_type(f_g_t),
    fully_initialised_flag(f_i_f),
    _motion_model(m_m)
{
  // Выделяем матрицы для хранения результатов.
  yigraphicsRES.Resize(GRAPHICS_STATE_SIZE);
  PyiyigraphicsRES.Resize(GRAPHICS_STATE_SIZE, GRAPHICS_STATE_SIZE);
  zeroedyigraphicsRES.Resize(GRAPHICS_STATE_SIZE);
  PzeroedyigraphicsRES.Resize(GRAPHICS_STATE_SIZE, GRAPHICS_STATE_SIZE);
  RiRES.Resize(MEASUREMENT_SIZE, MEASUREMENT_SIZE);
  SiRES.Resize(MEASUREMENT_SIZE, MEASUREMENT_SIZE);
  zeroedyiRES.Resize(FEATURE_STATE_SIZE);
  dzeroedyi_by_dxpRES.Resize(FEATURE_STATE_SIZE, 
			     get_motion_model()->POSITION_STATE_SIZE);
  dzeroedyi_by_dyiRES.Resize(FEATURE_STATE_SIZE, FEATURE_STATE_SIZE);
}

Feature_Measurement_Model::~Feature_Measurement_Model()
{

}

/** Вычисляет ковариацию инновации. Это общая неопределённость измерения
признака, объединяющая неопределённость состояния робота, состояния признака и
самого измерения. Вычисление общее для всех моделей измерения признаков, но при
необходимости его можно переопределить, например ради эффективности. Ковариация
инновации:
\f[
S_i = \frac{\partial h_i}{\partial x_v} P_{xx}
        \frac{\partial h_i}{\partial x_v}^T
  + \frac{\partial h_i}{\partial x_v} P_{xy_i}
        \frac{\partial h_i}{\partial y_i}^T
  + \frac{\partial h_i}{\partial y_i} P_{y_i x}
        \frac{\partial h_i}{\partial x_v}^T
  + \frac{\partial h_i}{\partial y_i} P_{y_i y_i}
        \frac{\partial h_i}{\partial y_i}^T
  + R_i
\f]
где \f$ R_i \f$ — ковариация шума измерений. Обычно предполагается, что она
диагональная, а величина определяется разрешением изображения.
@param Pxyi Ковариация \f$ P_{xy_i} \f$ между состоянием робота
  \f$ x_v \f$ и состоянием признака \f$ y_i \f$.
@param Pyiyi Ковариация признака \f$ P_{y_iy_i} \f$
@param dhi_by_dxv Якобиан \f$ \frac{\partial h_i}{\partial x_v} \f$ между
измерением признака \f$ h_i \f$ и состоянием робота \f$ x_v \f$
@param dhi_by_dyi Якобиан \f$ \frac{\partial h_i}{\partial y_i} \f$ между
измерением признака \f$ h_i \f$ и состоянием признака \f$ y_i \f$
@param Ri Ковариация инновации \f$ R_i \f$
**/
void Feature_Measurement_Model::func_Si(const VNL::Matrix<double> &Pxx, 
                                        const VNL::Matrix<double> &Pxyi,
                                        const VNL::Matrix<double> &Pyiyi,
                                        const VNL::Matrix<double> &dhi_by_dxv,
                                        const VNL::Matrix<double> &dhi_by_dyi,
                                        const VNL::Matrix<double> &Ri)
{
  // Обнуляем SiRES и добавляем отдельные слагаемые.
  SiRES.Fill(0.0);

  SiRES += dhi_by_dxv * Pxx * dhi_by_dxv.Transpose();
  
  VNL::Matrix<double> Temp_MM1 = dhi_by_dxv * Pxyi * dhi_by_dyi.Transpose();
  SiRES += Temp_MM1;

  SiRES += Temp_MM1.Transpose();

  SiRES += dhi_by_dyi * Pyiyi * dhi_by_dyi.Transpose();

  SiRES += Ri;

  
}

/** Считывает начальное состояние этого признака из Settings. Считываются
состояние \f$ \vct{y}_i \f$ и позиция робота, из которой признак был впервые
увиден, \f$ \vct{x}_p^{orig} \f$. Они не хранятся в этом классе, поэтому
возвращаются через параметры.
@param settings Класс Settings, из которого считываются данные
@param yi Заполняется начальным состоянием признака \f$ \vct{y}_i \f$.
@param xp_orig Заполняется позицией робота, из которой признак был впервые
увиден, \f$ \vct{x}_p^{orig} \f$.
*/
void Feature_Measurement_Model::read_initial_state(
  const Settings::Section& settings,
  VNL::Vector<>& yi,
  VNL::Vector<>& xp_orig)
{
  yi.Resize(FEATURE_STATE_SIZE);
  istringstream yi_stream(settings.get_entry("yi"));
  yi.ReadASCII(yi_stream);
  
  xp_orig.Resize(get_motion_model()->POSITION_STATE_SIZE);
  istringstream xp_stream(settings.get_entry("xp_orig"));
  xp_orig.ReadASCII(xp_stream);
}


/** Конструктор.
@param measurement_size Число параметров, представляющих измерение признака.
@param feature_state_size Число параметров для представления состояния
признака.
@param graphics_state_size Число параметров для графической абстракции
признака.
@param motion_model Модель движения робота. Нужна, чтобы понимать и
использовать позиционное состояние робота \f$x_p\f$ при измерении признаков.
@param feature_type Имя этой модели измерения признака, задаётся производным
классом.
@param feature_graphics_type Имя типа признака для отрисовки, задаётся
производным классом.
**/
Fully_Initialised_Feature_Measurement_Model::
Fully_Initialised_Feature_Measurement_Model(
    const unsigned int measurement_size,
    const unsigned int feature_state_size,
    const unsigned int graphics_state_size,
    Motion_Model *motion_model,
    const char *feature_type,
    const char *feature_graphics_type)
  : Feature_Measurement_Model(measurement_size, 
                              feature_state_size,
                              graphics_state_size,
                              motion_model,
                              feature_type,
                              feature_graphics_type, true)
{
  hiRES.Resize(MEASUREMENT_SIZE);
  dhi_by_dxpRES.Resize(MEASUREMENT_SIZE, 
                       motion_model->POSITION_STATE_SIZE);
  dhi_by_dyiRES.Resize(MEASUREMENT_SIZE, FEATURE_STATE_SIZE);
  nuiRES.Resize(MEASUREMENT_SIZE);
  hi_noisyRES.Resize(MEASUREMENT_SIZE);
}

/**
Деструктор. Ничего не делает.
*/
Fully_Initialised_Feature_Measurement_Model::
~Fully_Initialised_Feature_Measurement_Model() 
{
}

/** Конструктор.
  @param measurement_size Число параметров, представляющих измерение признака.
  @param feature_state_size Число параметров для представления состояния
  признака.
  @param graphics_state_size Число параметров для графической абстракции
  признака.
  @param motion_model Модель движения робота. Нужна, чтобы понимать и
  использовать позиционное состояние робота \f$x_p\f$ при измерении признаков.
  @param feature_type Имя этой модели измерения признака, задаётся производным
  классом.
  @param feature_graphics_type Имя типа признака для отрисовки, задаётся
  производным классом.
  @param free_parameter_size Число параметров, которые должны оставаться вне
  основного гауссова представления и описываться другим способом, например
  частицами.
  @param more_initialised_feature_measurement_model Модель измерения признака,
  в которую эта модель превратится после полной инициализации.
**/
Partially_Initialised_Feature_Measurement_Model::
Partially_Initialised_Feature_Measurement_Model(
    const unsigned int measurement_size,
    const unsigned int feature_state_size,
    const unsigned int graphics_state_size,
    Motion_Model *motion_model,
    const char *feature_type,
    const char *feature_graphics_type,
    const unsigned int free_parameter_size,
    Feature_Measurement_Model *m_i_f_m_m)
  : Feature_Measurement_Model(measurement_size, 
      feature_state_size,
      graphics_state_size,
      motion_model,
      feature_type,
      feature_graphics_type, false),
    FREE_PARAMETER_SIZE(free_parameter_size),
    more_initialised_feature_measurement_model(m_i_f_m_m)
{
  ypiRES.Resize(FEATURE_STATE_SIZE);
  dypi_by_dxpRES.Resize(FEATURE_STATE_SIZE, 
                        motion_model->POSITION_STATE_SIZE);
  dypi_by_dhiRES.Resize(FEATURE_STATE_SIZE,
                        MEASUREMENT_SIZE);
  hpiRES.Resize(MEASUREMENT_SIZE);
  dhpi_by_dxpRES.Resize(MEASUREMENT_SIZE, 
                        motion_model->POSITION_STATE_SIZE);
  dhpi_by_dyiRES.Resize(MEASUREMENT_SIZE, FEATURE_STATE_SIZE);
  yfiRES.Resize(more_initialised_feature_measurement_model->
                FEATURE_STATE_SIZE);
  dyfi_by_dypiRES.Resize(more_initialised_feature_measurement_model->
                         FEATURE_STATE_SIZE, FEATURE_STATE_SIZE);
  dyfi_by_dlambdaRES.Resize(more_initialised_feature_measurement_model->
                            FEATURE_STATE_SIZE, FREE_PARAMETER_SIZE);
}

/**
Деструктор. Ничего не делает.
*/
Partially_Initialised_Feature_Measurement_Model::
~Partially_Initialised_Feature_Measurement_Model()
{
}

/** Считывает параметры этой модели измерения признака из настроек.
Эта перегруженная версия для частично инициализированных признаков также
вызывает read_parameters() для модели полностью инициализированного признака,
на которую указывает more_initialised_feature_measurement_model. Если эта
функция переопределяется, внутри своей реализации нужно вызвать
read_parameters() базового класса.*/
void Partially_Initialised_Feature_Measurement_Model::read_parameters(
  const Settings& settings)
{
  if(more_initialised_feature_measurement_model != NULL)
      more_initialised_feature_measurement_model->read_parameters(settings);
}

Internal_Measurement_Model::Internal_Measurement_Model(
    const unsigned int measurement_size,
    Motion_Model *m_m,
    const char *i_t)
  : MEASUREMENT_SIZE(measurement_size),
    motion_model(m_m),
    internal_type(i_t)
{
  hvRES.Resize(MEASUREMENT_SIZE);
  dhv_by_dxvRES.Resize(MEASUREMENT_SIZE, motion_model->STATE_SIZE);
  RvRES.Resize(MEASUREMENT_SIZE, MEASUREMENT_SIZE);
  nuvRES.Resize(MEASUREMENT_SIZE);
  SvRES.Resize(MEASUREMENT_SIZE, MEASUREMENT_SIZE);
  hv_noisyRES.Resize(MEASUREMENT_SIZE);
}


  /** Для абстрактного класса обычно делают виртуальный деструктор, но здесь
  есть явная функция для очистки матриц, определённых в базовом классе. */
void Internal_Measurement_Model::Internal_Measurement_Model_destructor()
{
}

void Internal_Measurement_Model::func_Sv(const VNL::Matrix<double> &Pxx, 
                                         const VNL::Matrix<double> &dhv_by_dxv,
                                         const VNL::Matrix<double> &Rv)
{
  // Обнуляем SvRES и добавляем отдельные слагаемые.
  SvRES.Fill(0.0);

  SvRES += dhv_by_dxv * Pxx * dhv_by_dxv.Transpose();

  SvRES += Rv;

  
}
