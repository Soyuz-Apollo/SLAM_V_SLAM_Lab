/** 
 * @file   container.h
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


#ifndef GLUTE2_CONTAINER_H
#define GLUTE2_CONTAINER_H

#include <deque>
#include "widget.h"


namespace Glute2 {


  /*
   * class Container
   *
   * Container is a base class for all widgets that can contain other widgets.
   *
   */  
  
  class Container : public Widget {
  public:
    Container();
    virtual void Draw();
    virtual void SetSize(unsigned int width, unsigned int height);

  protected:
    // Overload some widget funcs
    virtual void _MousePressed(int x, int y, int button);    
    virtual void _MouseReleased(int startx, int starty, 
				int endx, int endy, int button);
    virtual void _Drag(int startx, int starty,
		       int endx, int endy, int button);
     virtual void _PassiveMotion(int x, int y);
   
    virtual void _KeyPress(int x, int y, unsigned char key) {};

    /** Recalculate sizes of child widgets. This depends on the layout used, so
     * must be overloaded by the inheriting class.
     */
    virtual void _RecalcSizes() = 0;

    // Overload the default widget version so that we can pass the ID to the descendants
    virtual void SetParentWindow(Glute2::Window* parent_window);

    // Internal funcs
    void _PushBack(Widget* w);
    void _PushFront(Widget* w);


    // Data
    int _focused;
    int _pressed;
    int _press_x;
    int _press_y;
    std::deque<Widget*> _widgets;
  };


} // namespace Glute2


#endif // GLUTE2_CONTAINER_H
