/** 
 * @file   glutloop.cpp
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

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <VW/Utils/timer.h>


#include "glutwindow.h"
#include "glutloop.h"

//#include <sys/time.h>


using namespace std;

#ifdef VERBOSE
#   undef VERBOSE
#endif
#define VERBOSE false

// For range-checked array access, set to true
#define SAFE false


// Rather nasty global variable and functions: don't know if I can 
// get around this? These are callback functions that Glut calls.
// I didn't seem to be able to use class member functions, so these
// global functions call the relavant class member functions.
std::vector<VW::GLUTWindow*> windows;

bool GLUT_INITIALISED=false;
bool USE_IDLER=false;


void GLUTAddWindow(VW::GLUTWindow *w)
{
  int win_id = w->GetID();

  if (VERBOSE) 
  cerr << "GLUTAddWindow - add window "<<win_id<<" (currently have " << windows.size() << ")" << endl;

  while(win_id > int(windows.size()))
    windows.push_back(0);
  windows.push_back(w);
}


int GetWindowID() {
  int i=glutGetWindow();
  if (SAFE) {
  if (VERBOSE) cerr << "Window = " << i << endl;
  if ((i >= 0) && (i < int(windows.size()))) {
    if (windows[i]==0) {
      cerr << "GetWindowID: windows["<<i<<"] is a null pointer!" << endl;
    exit(0);
    }
  } else {
    cerr << "GLUTMotionFunc: Asked for window "<<i<<", but valid range is 0<=i<"<< windows.size()<< "!" << endl;
    exit(0);
  }
  }
  return i;
}

void GLUTDrawFunc()
{
  if (VERBOSE) cerr << "GLUTDrawFunc" << endl;
  // Always redraw all displays, as GLUT appears to only call
  // redraw once (not once per window, as you might expect).
  std::vector<VW::GLUTWindow*>::iterator w;
  for (w=windows.begin();
       w!=windows.end();
       w++) {
    if (*w != 0)
      (*w)->Draw();
  }
}

void GLUTResizeFunc(int newwidth, int newheight)
{
  if (VERBOSE) cerr << "GLUTResizeFunc: new size " << newwidth << ",  " << newheight << endl;
  windows[GetWindowID()]->Resize(newwidth, newheight);
}

void GLUTMotionFunc(int newx, int newy)
{
	static VW::Timer timer;

  if (timer.GetAsMilliseconds() > 40) { // Not faster than 25 FPS
  if (VERBOSE) cerr << "GLUTMotionFunc: Motion to " << newx << ",  " << newy << endl;

  windows[GetWindowID()]->Motion(newx, newy);

  timer.Start();
  }
}

// Only for GUI windows for now
void GLUTPassiveMotionFunc(int newx, int newy)
{
	static VW::Timer timer;

  if (timer.GetAsMilliseconds() > 40) { // Not faster than 25 FPS
    if (VERBOSE) cerr << "GLUTPassiveMotionFunc: Motion to " << newx << ",  " << newy << endl;
    windows[GetWindowID()]->PassiveMotion(newx, newy);
    
    timer.Start();
  }
}

void GLUTMouseFunc(int button, int state, int x, int y)
{
  if (VERBOSE) cerr << "GLUTMouseFunc: Button " << button << ", state " << state 
		    << ", position " << x << ", " << y << endl;
    windows[GetWindowID()]->Mouse(button, state, x, y);

}
            
void GLUTIdleEvent() 
{
  if (VERBOSE) cerr << "GLUTIdleEvent" << endl;
}                                                          

void GLUTRegisterCallbacks()
{
  if (VERBOSE) cerr << "GLUTRegisterCallbacks" << endl;
  // Register the callback functions to do the drawing, resize,
  // mouse interactions
  glutDisplayFunc(&GLUTDrawFunc);
  glutReshapeFunc(&GLUTResizeFunc);
  glutMouseFunc(&GLUTMouseFunc);
  glutMotionFunc(&GLUTMotionFunc);
  glutPassiveMotionFunc(&GLUTPassiveMotionFunc);

  // Idler costs a lot of CPU, so don't activate it unless requested
  if (USE_IDLER) glutIdleFunc(&GLUTIdleEvent);

}

void VW::GLUTInit(int argc, char** argv, bool use_idler)
{
  if (VERBOSE) cerr << "GLUTInit" << endl;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

  GLUT_INITIALISED = true;
  USE_IDLER = use_idler;
  return;
}

void VW::GLUTLoop()
{
  if (VERBOSE) cerr << "GLUTLoop - Go!" << endl;
  glutMainLoop();
}

