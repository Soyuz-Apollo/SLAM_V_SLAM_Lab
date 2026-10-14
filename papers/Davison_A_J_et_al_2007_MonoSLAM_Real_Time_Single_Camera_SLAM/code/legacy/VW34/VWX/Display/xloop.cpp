/** 
 * @file   xloop.cpp
 * @author ID Reid
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


#include "displayx.h"
#include "xloop.h"

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE true

using namespace std;
using namespace VW;


// global X stuff
Display *globalXdisplay = NULL;
std::vector<DisplayX *> xdisplaylist;

void XAddDisplay(VW::DisplayX *d)
{
  if (VERBOSE) cerr << "XAddDisplay - now have " << xdisplaylist.size()+1 << endl;
  xdisplaylist.push_back(d);
  return;
}

Display* GetXDisplay()
{
  return globalXdisplay;
}

static int DisplayXEventHandler(XEvent* event, DisplayX* parent) {
  static int last_x=0;
  static int last_y=0;
  static int last_button=0;
  static bool dragging=false;

  //cout << event->type << "("<<Expose<<","<<ButtonPress<<","<<ButtonRelease<<","<<MotionNotify<<")"<<endl;
  switch (event->type) {
  case Expose:
    cout << "Expose" << endl;
    break;
  case ButtonPress:
    {
      last_x=int(event->xbutton.x);
      last_y=int(event->xbutton.y);
      last_button=int(event->xbutton.button);
      dragging=false;
      //parent->ClickEvent.emit(last_x,last_y,last_button);
    }
    break;
  case ButtonRelease:
    {
      if (dragging) {
	int x=int(event->xmotion.x);
	int y=int(event->xmotion.y);
	parent->DragFinishedEvent(last_x,last_y,x,y,last_button);
      } else {
	parent->ClickEvent(last_x,last_y,last_button);
      }
      dragging=false;
    }
    break;
  case MotionNotify:
    {
      int x=int(event->xmotion.x);
      int y=int(event->xmotion.y);
      // Drags always start with a click, so can use last click to work out
      // the start position and button
      if (last_button>0)
	parent->DragEvent(last_x,last_y,x,y,last_button);
      dragging=true;
    }
    break;
  default:
    cout << "Unbound event" << endl;
  }
  return(1);
}

void VW::GetAndDispatchXEvent()
{
  XEvent ev;
  vector<DisplayX*>::iterator i;

  XPeekEvent(globalXdisplay, &ev);
  for (i=xdisplaylist.begin(); i!=xdisplaylist.end(); i++) {
    if ((*i)->window == ev.xany.window) {
      XNextEvent(globalXdisplay, &ev);
      DisplayXEventHandler(&ev, (*i));
      break;
    }
  }
  //if (i==displaylist.end()) {
  //  cerr << "Event received from unrecognised window\n";
  //}
}

void VW::XInit(int argc, char** argv)
{
  if (globalXdisplay==NULL) {
    globalXdisplay = XOpenDisplay(NULL);
  }
}

void VW::XLoop()
{
  if (VERBOSE) cerr << "XLoop - Go!" << endl;
  while(true) {
    VW::GetAndDispatchXEvent();
  }

}
