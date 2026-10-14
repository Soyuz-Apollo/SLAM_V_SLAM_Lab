/** @file pixptr.h 
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
 * @class   VW::PixPtr
 * @ingroup Image
 * @author  ID Reid
 * @date    02/08/02
 * @brief   A pixel pointer implementation.
 *
 * The real purpose is to allow interleaved images to appear
 * the same as ordinary ones by overloading the pointer
 * arithmetic to skip over extra bytes.
 *
 */


#ifndef _pixptr_h_
#define _pixptr_h_


namespace VW {

template <class Type, int Skip>
class PixPtr {
 public:
  Type *p;

  // constructor
  PixPtr(Type *pp=NULL) : p(pp) {};

  // accessors
  Type & operator*() { return *p; }
  Type & operator[](int i) { return *(p+(Skip*i)); }

  // pointer arithmetic
  PixPtr operator++(int) { PixPtr res(*this); p+=Skip; return res; }
  PixPtr & operator++() { p+=Skip; return *this; }
  PixPtr & operator+=(int i) { p+=(Skip*i); return *this; }
  PixPtr & operator-=(int i) { p-=(Skip*i); return *this; }
  PixPtr & operator--(int) { p-=Skip; return *this; }
  PixPtr & operator--() { p-=Skip; return *this; }
  PixPtr operator+(int i) { PixPtr res(p+(Skip*i)); return res; }
  PixPtr operator-(int i) { PixPtr res(p-(Skip*i)); return res; }
};

}; // end namespace VW
#endif /* _pixptr_h_ */

