/** 
 * @file   frame.h
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

#ifndef GLUTE2_FRAME_H
#define GLUTE2_FRAME_H

#include "container.h"


namespace Glute2 {


  /*
   * class Frame
   *
   * A class that draws a frame (optionally with a label) and can contain a
   * single widget.
   *
   */  
  
  class Frame : public Container {
  public:
    Frame(const std::string label=std::string(""));

    void Add(Widget* w);
    virtual void Draw();

  protected:
    virtual void _RecalcSizes();
    std::string _label;
  };

}; // end namespace Glute


#endif
