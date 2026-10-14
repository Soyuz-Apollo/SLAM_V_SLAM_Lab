/*  SceneLib: software for real-time SLAM

    identifier.h
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


#ifndef _scene_identifier_h_
#define _scene_identifier_h_

/**
Базовый класс для идентификаторов признаков. Идентификатор признака — это
объект в реальном мире, который однозначно задаёт признак, например патч
изображения. Уникальные идентификаторы признаков должны наследоваться от
этого класса. Наличие базового класса означает, что производные классы должны
уметь удалять себя при удалении признаков. При сравнении двух идентификаторов
проверяются только указатели, поэтому каждый идентификатор остаётся уникальным.
@ingroup gScene
**/
class ExtraData
{
public:
  /** Конструктор. Конструктор базового класса ничего не делает. */
  ExtraData() {} 
  /** Деструктор ничего не делает. TODO: он должен быть виртуальным,
  но тогда tracking не работает!*/
  virtual ~ExtraData() {}
};

/**
Для удобства понимания в остальном коде указатель на ExtraData называется
Identifier. Identifier — это указатель на объект в реальном мире, который
однозначно идентифицирует признак.
@ingroup Scene
*/
typedef ExtraData* Identifier;

#endif
