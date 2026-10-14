/** 
 * @file   slider.h
 * @author BJ Tordoff
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
#ifndef GLUTE_SLIDER_H
#define GLUTE_SLIDER_H

#include <string>
#include <VW/events.h>
#include "widget.h"


namespace Glute2 {
  

  class Slider : public Widget {
  public:
    Slider(const std::string& label,
	   double min_val,
	   double max_val,
	   double default_val,
	   const std::string& numberformat=std::string("%3.1f"));
    ~Slider();

    virtual void Draw();
    virtual void SetSize(unsigned int width, unsigned int height);

    // Events to attach your functions to
    /** The value has been changed. The argument is the new slider value (double), 
     * and the return type of your function should be void. 
     */
    VWEvents::Event1<void,double> ValueChangedEvent;


  protected:
    void _UpdateValString();
    void _FindMaxValStringLength();
   // Overload some widget funcs
    virtual void _MousePressed(int x, int y, int button);    
    virtual void _MouseReleased(int startx, int starty, 
				int endx, int endy, int button);
    virtual void _Drag(int startx, int starty,
		       int endx, int endy, int button);

    bool _down;
    double _curr_val,_min_val,_max_val;
    std::string _label,_format,_units,_valstring;
    // Button dimensions
    unsigned int _label_w, _slider_w, _val_w;
  };
  

} // namespace Glute2


#endif // GLUTE_SLIDER_H
