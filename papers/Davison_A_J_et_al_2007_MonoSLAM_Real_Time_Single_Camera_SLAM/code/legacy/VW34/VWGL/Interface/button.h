/** 
 * @file   button.h
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

#ifndef GLUTE_BUTTON_H
#define GLUTE_BUTTON_H

#include <string>
#include <VW/events.h>
#include "widget.h"


namespace Glute2 {
  

  class Button : public Widget {
  public:
    Button(const std::string& label);
    ~Button();

    virtual void Draw();
    virtual void SetSize(unsigned int width, unsigned int height);

    // Events to attach your functions to
    /** The button has been pressed. */
    VWEvents::Event0<void> PressedEvent;
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


#endif // GLUTE_BUTTON_H
