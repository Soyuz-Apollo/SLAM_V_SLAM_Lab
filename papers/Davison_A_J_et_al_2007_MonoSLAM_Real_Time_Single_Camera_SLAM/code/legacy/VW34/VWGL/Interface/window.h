/** 
 * @file   window.h
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

#ifndef GLUTE2_WINDOW_H
#define GLUTE2_WINDOW_H

// System deps
#include <string>

// Unfortunate but necessary external dependency
#include <VWGL/Display/glutloop.h>
#include <VWGL/Display/glutwindow.h>

// Local deps
#include "colour.h"
#include "widget.h"

namespace Glute2 {


  /*
   * class Window
   *
   * Window is a class which behaves as a glut window,
   * it receives events which may be handled by derived classes.
   *
   */
  
  class Window : protected VW::GLUTWindow {
  public:
    
    // Top-level window constructor
    Window(int w, int h, const std::string& title);
    
    // Destructor
    virtual ~Window();
    
    void Add(Widget* child);


    // Position
    void Position(int x, int y);
    
    // Fullscreen
    void FullScreen();
    
    // Show
    void Show();
    
    // Hide
    void Hide();
    
    // Iconify
    void Iconify();
    
    
    // Push (Bring window up front)
    void Push();
    
    // Pop (Take window to back)
    void Pop();
    
    // Set background clear colour
    void SetClearColour(float r, float g, float b, float a);
    
    // Cursor
    void SetCursor(int cursor);
    
    // Mark for display
    void Redisplay();
    
    // Make current for GL calls
    void MakeCurrent();
    
    // Swap Buffers
    void SwapBuffers();
    // flush is same as swapbuffers!
    void Flush();

    // The openGL ID for this window
    int GetWindowID() const;

    unsigned int GetWidth() const;
    unsigned int GetHeight() const;

    
  protected:
     // Resize the window
    void Resize(int w, int h);
     // Ask window to redraw its contents
    void Draw();
    // Stuff for the event loop - don't use!
    void Mouse(int button, int state, int x, int y);   
    void Motion(int x, int y); 
    void PassiveMotion(int x, int y);      
    void Keyboard(unsigned char key, int x, int y);

   
   
    // Overidable event handling
    
    virtual void Entry(int state);
    
    virtual void Special(int key, int x, int y);
    
    //    virtual void Reshape(int w, int h);
    
    
    // Window data
    int _x, _y;
    int _width, _height;
    int _window_id;
    bool _focused;
    Widget* _childwidget;
    bool _havechild;
  // For dragging
   bool _dragging;
  int _last_x, _last_y;
  int _current_button;
 
    // other data
    Colour _clear_colour;
  };

    inline unsigned int 
    Window::GetWidth() const
    {
      return _width;
    }

    inline unsigned int 
    Window::GetHeight() const
    {
      return _height;
    }

} // namespace Glute2







#endif // GLUTE2_WINDOW_H
