/** 
 * @file   vbox.cpp
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

#include <iostream>
using namespace std;
#include "vbox.h"

#define BORDER 0

void Glute2::VBox::AddToTop(Widget* w)
{
  this->_PushFront(w);
  _RecalcSizes();
}
void Glute2::VBox::AddToBottom(Widget* w)
{
  this->_PushBack(w);
  _RecalcSizes();
}
void Glute2::VBox::_RecalcSizes()
{
  const unsigned int n = _widgets.size();
  if (n > 0) {
    
    
    // First work out the minimums
    _minsize_x=2*BORDER;
    _minsize_y=2*BORDER;
    unsigned int num_resize_y=0;
    for (unsigned int i=0;i< n;i++) {

      if (_widgets[i]->GetMinimumWidth()>_minsize_x) {
	_minsize_x = _widgets[i]->GetMinimumWidth() + 2*BORDER;
      }
      _minsize_y += _widgets[i]->GetMinimumHeight();

      if (_widgets[i]->WantsToGrowX()) _wants_to_grow_x=true;
      if (_widgets[i]->WantsToGrowY()) num_resize_y++;
    }
    if (num_resize_y>0) {
      //cout << "VBox: "<<num_resize_y<<" elements want to grow" << endl;
      _wants_to_grow_y = true;
      double growth_factor = double(_h - _minsize_y) / double(num_resize_y);
      //cout << "VBox: growth factor is " << growth_factor << endl;
      unsigned int pos = BORDER;
      for (unsigned int i=0;i< n;i++) {
	  _widgets[n-i-1]->SetPosition(_x+BORDER,_y+pos);

	unsigned int minh = _widgets[n-i-1]->GetMinimumHeight();

	if (_widgets[n-i-1]->WantsToGrowY()) {
	  int new_height = int(minh + growth_factor);
	  if (new_height<0) new_height=minh;
	  _widgets[n-i-1]->SetSize(_w-(2*BORDER),new_height);
	  pos += new_height;
	} else {
	  _widgets[n-i-1]->SetSize(_w-(2*BORDER),minh);
	  pos += minh;
	}
      }
   } else {
      // None want to resize, so they all have to
      _wants_to_grow_y = false;
      int wy = (_h-(2*BORDER))/n;
      for (unsigned int i=0;i< n;i++) {
	_widgets[n-i-1]->SetPosition(_x+BORDER,_y+BORDER+i*wy);
	_widgets[n-i-1]->SetSize(_w-(2*BORDER),wy);
      }
     
    }

  }
  _needs_redraw = true;
}
