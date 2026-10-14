/** 
 * @file   widget.h
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
#ifndef GLUTE2_WIDGET_H
#define GLUTE2_WIDGET_H

#include <iostream>

namespace Glute2 {

  class Window;
  class Container;

  
  class Widget {
    friend class Window;
    friend class Container;
  public:
    Widget()
      : _x(0), _y(0), _w(0), _h(0), _focus(false),
	_parent_window(0),
	_wants_to_grow_x(false), _wants_to_grow_y(false),
	_minsize_x(0), _minsize_y(0) { }
    Widget(int x, int y, int w, int h)
      : _x(x), _y(y), _w(w), _h(h), _focus(false),
 	_parent_window(0),
	_wants_to_grow_x(false), _wants_to_grow_y(false),
	_minsize_x(0), _minsize_y(0) { }
    
    virtual ~Widget() { }
        virtual void Draw() = 0;

    /* This one should use the demanded size to calculate the width and height
     * of both the filled area, and the sensitive area (focussing area) of the
     * widget. 
     */
    virtual void SetSize(unsigned int width, unsigned int height) = 0;
    void SetPosition(unsigned int x, unsigned int y);

    unsigned int GetMinimumWidth() const;
    unsigned int GetMinimumHeight() const;
    bool WantsToGrowX() const;
    bool WantsToGrowY() const;
    virtual void SetParentWindow(Glute2::Window* parent_window);
    bool IsYInverted() const;
  protected:
    // Funcs to be overloaded

    //void _SetWindow(Glute2::Window* win);
    /** Is this coordinate inside this widget's sensitive area. */
    virtual bool _IsIn(int x, int y);

    // Some funcs for internal use
    void _SetFocus(bool state);
    
    bool _NeedsRedraw() const;


    virtual void _MousePressed(int x, int y, int button) { }
    virtual void _MouseReleased(int startx, int starty, 
				int endx, int endy, int button) { }
    
    virtual void _PassiveMotion(int x, int y) { }
    virtual void _Drag(int startx, int starty,
			  int endx, int endy, int button) { }
    
    virtual void _KeyPress(int x, int y, unsigned char key) { }


    int _x, _y,_w, _h; // dimensions of whole widget
    int _sx, _sy,_sw, _sh; // dimensions of sensitive area
    bool _focus,_needs_redraw;

    Glute2::Window* _parent_window;

    // Size params
    bool _wants_to_grow_x, _wants_to_grow_y;
    unsigned int _minsize_x, _minsize_y;
  };


  inline void Widget::_SetFocus(bool state)
    {
      _focus = state;
      _needs_redraw = true;
    }
  

  inline unsigned int Widget::GetMinimumWidth() const
  {
      return _minsize_x;
  }

  inline unsigned int Widget::GetMinimumHeight() const
  {
      return _minsize_y;
  }
  inline void Widget::SetPosition(unsigned int x, unsigned int y) 
  {
    _x = x;
    _y = y;
  };

  inline bool Widget::WantsToGrowX() const
  {
    return _wants_to_grow_x;
  }

  inline bool Widget::WantsToGrowY() const
  {
    return _wants_to_grow_y;
  }

  inline bool Widget::_NeedsRedraw() const
  {
    return _needs_redraw;
  }

} // namespace Glute2


#endif // GLUTE2_WIDGET_H
