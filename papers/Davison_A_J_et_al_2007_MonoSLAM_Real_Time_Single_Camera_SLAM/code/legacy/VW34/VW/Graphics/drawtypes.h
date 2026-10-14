/** @file drawtypes.h
 *  @author BJ Tordoff
 *  @date   09/08/03
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


#ifndef _drawtypes_h_
#define _drawtypes_h_


namespace VW {


namespace DrawFunctions {

  /** Small struct that stores the colour to be used for drawing.
   * @ingroup CoreGraphics
   */
  struct Colour {
    unsigned char _r, _g, _b, _a;
    
    Colour (){};
    Colour (unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0) {
      _r = r; _g = g; _b = b; _a = a;
    }
    
    bool operator== (const Colour& col) const {
      return (col._r == _r && col._g == _g && col._b == _b);
    }
  };


  /** Enum of shapes for drawing point features.
   * @ingroup CoreGraphics
   */
  enum CornerShape {CROSS, PLUS, SQUARE, BAD_SHAPE};


}; // end namespace DrawFunctions

} // end namespace VW

#endif
