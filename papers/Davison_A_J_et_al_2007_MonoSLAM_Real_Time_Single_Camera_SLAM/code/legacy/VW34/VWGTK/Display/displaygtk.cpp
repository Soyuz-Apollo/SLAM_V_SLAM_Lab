/** 
 * @file   displaygtk.cpp
 * @author BJ Tordoff
 * @date   02/08/2002
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



#include "displaygtk.h"
#include <unistd.h>

#define WINDOW_BORDER 5

using namespace std;
using namespace VW;


DisplayGTK::DisplayGTK(const unsigned int width, 
			   const unsigned int height, 
			   const string title) : 
  Window(GTK_WINDOW_TOPLEVEL)
 {

  cerr << "Hurray, a new display!" << endl;
  _my_title=title;

  Init(width,height,title);

  // Put it on-screen
  show_all();
  _visible=true;
};


DisplayGTK::DisplayGTK(const string title) : 
  Window(GTK_WINDOW_TOPLEVEL) {

  _my_title=title;

  cerr << "Hurray, a new display!" << endl;
  _initialised = false;
  _visible = false;
}

void DisplayGTK::Init(const unsigned int width, 
			const unsigned int height, 
			const string title) {

  cerr << "Initialising display." << endl;
  if (title.length()>0) set_title (title.c_str());
  set_border_width(WINDOW_BORDER);
  set_policy(1,1,0);
  set_resize_mode(GTK_RESIZE_IMMEDIATE);
  
  cerr << "   Create widget (title='"<<title<<"')." << endl;
  drawingarea = new DisplayGTKWidget(width,height,title);
  // Add to window and show window
  cerr << "   Add to form." << endl;
  add(*drawingarea);

  // Attach child events/signals to this one's
  drawingarea->ClickEvent.Attach(this,&DisplayGTK::_HandleClick);
  drawingarea->DragEvent.Attach(this,&DisplayGTK::_HandleDrag);
  drawingarea->DragFinishedEvent.Attach(this,&DisplayGTK::_HandleDragFinished);

  _initialised=true;
}


void DisplayGTK::SetSize(const unsigned int new_width,
			 const unsigned int new_height) {
  if (!_initialised) {
    // Not yet initialized, so just initialize to the new size
    Init(new_width,new_height,_my_title);
  } else {
    // To get the window to resize, must hide and show it.
    // nasty nasty nasty
    if(_visible) hide();
    drawingarea -> SetSize(new_width,new_height);
    set_default_size(new_width,new_height);
  }
  show_all();
  _visible = true;
}
