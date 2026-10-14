/** 
 * @file     transform.h
 * @author   JGH Knight
 * @date     02/08/2002
 *
 *              (c) Copyright University of Oxford
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/**
 * @class   VW::Transform
 * @ingroup GeometricObjects
 * @author  JGH Knight
 * @date    02/08/2002
 * @brief   The base class from which all transforms are made.
 *
 * The abstract class Transform is here merely to introduce some consistency between
 * types of transforms and the sorts of information/methods they must all provide,
 * rather than because they are strictly necessary.  This is because we are using
 * compile-time polymorphism (templates) for our compute functions, rather than
 * run-time polymorphism (virtual functions).  If you derive a new transform type
 * from this base class you can be sure it will be possible to instantiate versions
 * of the various compute classes from it.
 */


#ifndef _transform_h_
#define _transform_h_

namespace VW {


class Transform {
 public:
  static const unsigned int MIN_SET_SIZE = 4;
  static const unsigned int NUM_VIEWS = 2;
  static const char *const NAME;

  template <class F> unsigned int GetMinFeatures (const F&) 
    {
      assert (false);
      return 0;
    }
  unsigned int GetNumViews () 
    {
      assert (false);
      return 0;
    };
};
};

#endif // #ifndef _transform_h_
