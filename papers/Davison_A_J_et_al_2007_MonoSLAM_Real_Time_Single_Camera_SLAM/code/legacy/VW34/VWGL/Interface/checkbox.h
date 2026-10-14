/** 
 * @file   checkbox.h
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
#ifndef GLUTE_CHECKBOX_H
#define GLUTE_CHECKBOX_H

#include <string>
#include <VW/events.h>
#include "widget.h"


namespace Glute2 {
  

  class CheckBox : public Widget {
  public:
    CheckBox(const std::string& label);
    ~CheckBox();

    virtual void Draw();
    virtual void SetSize(unsigned int width, unsigned int height);

    // Event to attach your function to
    /** Called whenever the state is toggled and passes the button state */
    VWEvents::Event1<void,bool> ToggledEvent;

    // The user can also query the button state at any time
    bool GetDownState() const {return _down;}

    // And set the state manually
    void SetDownState(bool newstate) {_down = newstate;}

  protected:
    // Overload some widget funcs
    virtual void _MousePressed(int x, int y, int button);    
    virtual void _MouseReleased(int startx, int starty, 
				int endx, int endy, int button);

    bool _down;
    std::string _text;
    // Button dimensions
    unsigned int _text_w, _but_w, _but_h;
  };
  

} // namespace Glute2


#endif // GLUTE_CHECKBOX_H
