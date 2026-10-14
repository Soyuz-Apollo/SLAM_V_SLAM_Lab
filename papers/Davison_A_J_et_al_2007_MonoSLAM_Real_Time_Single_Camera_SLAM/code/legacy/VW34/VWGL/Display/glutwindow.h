/** 
 * @file   glutwindow.h
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

#ifndef _vw_glutwindow_h_
#define _vw_glutwindow_h_

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include <string>


namespace VW {

  class GLUTWindow {
  public:
    virtual ~GLUTWindow() {}

    bool IsMe(int i) const;
    void SetID(int id);
    int GetID() const;


    // Funcs that *must* be overloaded
    virtual void Mouse(int button, int state, int x, int y) = 0;
    virtual void Draw() = 0;
    virtual void Resize(int w, int h) = 0;
    virtual void Motion(int x, int y) = 0;
    virtual void PassiveMotion(int x, int y) = 0;      
    virtual void Keyboard(unsigned char key, int x, int y) = 0;

  protected:
    // Call this from your derrived class to actually put
    // the window onscreen. Returns the GLUT id for the new
    // window. 
    int _CreateWindow(unsigned int width,
		      unsigned int height,
		      const std::string& title);
    int _CreateSubwindow(int parent_window_id,
			 unsigned int x,
			 unsigned int y,
			 unsigned int width,
			 unsigned int height);
    void _MakeCurrent() const;

  private:
    int _my_glut_id;
  };

}; // End namespace VW


inline bool VW::GLUTWindow::IsMe(int i) const
{
  return (i==_my_glut_id);
}

inline void VW::GLUTWindow::SetID(int id)
{
  _my_glut_id = id;
}
inline int VW::GLUTWindow::GetID() const
{
  return _my_glut_id;
}
inline void VW::GLUTWindow::_MakeCurrent() const
{
  glutSetWindow(_my_glut_id);
}
  
#endif
