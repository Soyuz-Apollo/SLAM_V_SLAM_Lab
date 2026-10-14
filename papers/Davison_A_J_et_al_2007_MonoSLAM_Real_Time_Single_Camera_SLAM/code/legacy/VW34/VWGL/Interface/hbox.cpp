/** 
 * @file   hbox.cpp
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
#include "hbox.h"

#define BORDER 0

void Glute2::HBox::AddToLeft(Widget* w)
{
  this->_PushFront(w);
  _RecalcSizes();
}
void Glute2::HBox::AddToRight(Widget* w)
{
  this->_PushBack(w);
  _RecalcSizes();
}
void Glute2::HBox::_RecalcSizes()
{
  const unsigned int n = _widgets.size();
  if (n > 0) {
    
    
    // First work out the minimums
    _minsize_x=2*BORDER;
    _minsize_y=2*BORDER;
    unsigned int num_resize_x=0;
    for (unsigned int i=0;i< n;i++) {

      if (_widgets[i]->GetMinimumHeight()>_minsize_y) {
	_minsize_y = _widgets[i]->GetMinimumHeight() + 2*BORDER;
      }
      _minsize_x += _widgets[i]->GetMinimumWidth();

      if (_widgets[i]->WantsToGrowY()) _wants_to_grow_y=true;
      if (_widgets[i]->WantsToGrowX()) num_resize_x++;
    }
    cout << "HBox: Current size = ("<<_w<<"x"<<_h<<") but minimum is ("<<_minsize_x<<"x"<<_minsize_y<<")"<<endl;
    if (num_resize_x>0) {
      cout << "HBox: "<<num_resize_x<<" elements want to grow" << endl;
      _wants_to_grow_x = true;
      double growth_factor = (double(_w) - double(_minsize_x)) / double(num_resize_x);
      cout << "HBox: growth factor is " << growth_factor << endl;
      unsigned int pos = BORDER;
      for (unsigned int i=0;i< n;i++) {
	  _widgets[i]->SetPosition(_x+pos,_y+BORDER);
	unsigned int minw = _widgets[i]->GetMinimumWidth();

	if (_widgets[i]->WantsToGrowX()) {
	  int new_width = int(minw + growth_factor);
	  if (new_width<0) new_width=minw;
	  _widgets[i]->SetSize(new_width, _h-(2*BORDER));
	pos += new_width;
	} else {
	  _widgets[i]->SetSize(minw, _h-(2*BORDER));
	pos += minw;
	}
      }
   } else {
      // None want to resize, so they all have to
      int wx = (_w-(2*BORDER))/n;
      for (unsigned int i=0;i< n;i++) {
	_widgets[i]->SetPosition(_x+BORDER+i*wx,_y+BORDER);
	_widgets[i]->SetSize(wx,_h-(2*BORDER));
      }
     
    }

  }
  _needs_redraw = true;
}
