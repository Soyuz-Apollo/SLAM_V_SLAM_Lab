/** 
 * @file   glutwindow.cpp
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

#include <iostream>
#include <GL/glut.h>
#include "glutwindow.h"

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false

using namespace std;

int VW::GLUTWindow::_CreateWindow(unsigned int width,
				  unsigned int height,
				  const std::string& title)
{
    // Create and open a new window onscreen
    if (VERBOSE) cout << "VW::GLUTWindow: New window!" << endl;
    
    if (VERBOSE) cout << "   Init" << endl;
    glutInitWindowSize(width, height);
    
    // Open a window 
    _my_glut_id = glutCreateWindow( title.c_str() );
    
    // Let the world know I exist
    if (VERBOSE) cout << "   Add to world" << endl;
    extern void GLUTAddWindow(VW::GLUTWindow *);
    GLUTAddWindow(this);
    
    if (VERBOSE) cout << "   Register callbacks" << endl;
    extern void GLUTRegisterCallbacks();
    GLUTRegisterCallbacks();
    if (VERBOSE) cout << "VW::GLUTWindow: done" << endl;
    
  return _my_glut_id;
}


int VW::GLUTWindow::_CreateSubwindow(int parent_window_id,
				     unsigned int x,
				     unsigned int y,
				     unsigned int width,
				     unsigned int height)
{
  
    // We want a subwindow
  cout << "Init subwindow on parent " << parent_window_id << endl;
    _my_glut_id = glutCreateSubWindow(parent_window_id, 
				      x,y,width,height);


    cout << "Add to world" << endl;
    extern void GLUTAddWindow(VW::GLUTWindow *);
    GLUTAddWindow(this);

    cout << "Register callbacks" << endl;
    extern void GLUTRegisterCallbacks();
    GLUTRegisterCallbacks();

    cout << "done" << endl;

    return _my_glut_id;
}
