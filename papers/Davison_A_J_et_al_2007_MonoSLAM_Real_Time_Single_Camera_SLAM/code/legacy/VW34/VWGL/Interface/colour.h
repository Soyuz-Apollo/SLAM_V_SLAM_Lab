/** 
 * @file   colour.h
 * @author BJ Tordoff & KR Connor
 * @date   03/12/2003
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
#ifndef GLUTE2_COLOUR_H
#define GLUTE2_COLOUR_H

#include <GL/glut.h>


namespace Glute2 {


  class Colour {
  public:
    Colour()
      : r(0.0), g(0.0), b(0.0), a(0.0) { }

    Colour(float rr, float gg, float bb, float aa=0.0)
      : r(rr), g(gg), b(bb), a(aa) { }

    Colour(float grey)
      : r(grey), g(grey), b(grey), a(0.0) { }

    void Set(float rr=0.0, float gg=0.0, float bb=0.0, float aa=0.0)
      {
	r=rr;
	g=gg;
	b=bb;
	a=aa;
      }

    void SetGrey(float g)
      {
	r=g;
	g=g;
	b=g;
      }

    void MakeCurrent() const
      {
	glColor3f(r,g,b);
      }

    void MakeCurrentClear() const
      {
	glClearColor(r,g,b,a);
      }

    float r, g, b, a;
  };
  
  inline void SetPen(const Colour& colour) {
    colour.MakeCurrent();
  }


} // namespace Glute2


#endif // GLUTE2_COLOUR_H
