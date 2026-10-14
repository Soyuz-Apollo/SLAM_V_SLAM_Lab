/** 
 * @file   hbox.h
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

#ifndef GLUTE2_HBOX_H
#define GLUTE2_HBOX_H

#include "container.h"


namespace Glute2 {


  /*
   * class HBox
   *
   * A container that stacks members horizontally.
   *
   */  
  
  class HBox : public Container {
  public:

    void AddToLeft(Widget* w);
    void AddToRight(Widget* w);

  protected:
    virtual void _RecalcSizes();

  };

}; // end namespace Glute


#endif
