/** 
 * @file   pscanvas.cpp
 * @author ID Reid
 * @date   01/09/2004
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
/**
 * @class   VW::PSCanvas
 * @ingroup Displays
 * @author  ID Reid
 * @date    01/09/2004
 * @brief   A canvas for drawing raw postscript.
 *
 */


#include <iostream>
#include <string>
#include <vector>
#include "pscanvas.h"

using namespace std;
using namespace VW;

// PSCanvas::PSCanvas(const std::string filename) 
//   : _width(0), _height(0), _filename(filename) 
// {
//   _initialised = false;
//   if (_filename.length()!=0) {
//     _fp.open(filename.data());
//     if (!_fp.fail()) {
//       _initialised = true;
//     }
//   }
// }      




PSCanvas::~PSCanvas()
{
  if (_initialised) Close();
}

void PSCanvas::Open(const unsigned int width,
		    const unsigned int height, 
		    const std::string filename)
{
  _filename = filename;
  _initialised = false;
  if (_filename.length()!=0) {
    _fp.open(filename.data());
    if (!_fp.fail()) {
      _initialised = true;
      SetSize(width,height);
    }
  }
}

void PSCanvas::SetSize(const unsigned int w, const unsigned int h) 
{
  if (w==0 || h==0) 
    return;

  _width=w;
  _height=h;

  if (!_initialised) {
    cerr << "Warning: PSCanvas not initialised\n";
    return;
  }
  _fp << "%!PS-Adobe-3.0 EPSF-3.0\n";
  _fp << "%%Title: " << _filename << endl;
  _fp << "%%Creator: VW (3.4) PSCanvas" << endl;
  _fp << "%%BoundingBox: 0 0 " << _width << " " << _height << endl;
  _fp << "%%LanguageLevel: 2" << endl;
  _fp << "%%Pages: 1" << endl;
  _fp << "%%DocumentData: Clean7Bit" << endl;
  _fp << endl;

  // Now some useful definitions
  _fp << "/line { newpath moveto lineto stroke } def\n";
  _fp << endl;
  return;
}


void PSCanvas::SetPenColour(const unsigned char red, const unsigned char green, 
			    const unsigned char blue, const unsigned char alpha)
{
  _red = red/255.0; 
  _blue = blue/255.0; 
  _green = green/255.0; 
  _alpha = alpha/255.0;
  if (_fp.is_open()) {
    _fp << _red << " " << _green << " " << _blue << " setrgbcolor\n"; 
  }
  return;
}

void PSCanvas::SetLineThickness(double thickness)
{
  if (_fp.is_open()) {
    _fp << thickness << " setlinewidth\n"; 
  }
  return;  
}

void PSCanvas::SetPixel(const int x, const int y)
{
  cerr << "Warning: PSCanvas doesn't support SetPixel\n";
  return;
}


void PSCanvas::SetPixel(const int x, const int y, const unsigned char red, 
			const unsigned char green, const unsigned char blue, 
			const unsigned char alpha)
{
  cerr << "Warning: PSCanvas doesn't support SetPixel\n";
  return;
}


void PSCanvas::DrawPoint(const float x, const float y) 
{
  cerr << "Warning: PSCanvas doesn't support DrawPoint\n";
  return;
}  


void PSCanvas::DrawLine(const float x0, const float y0, 
			const float x1, const float y1)
{
  if (!_initialised) {
    cerr << "Warning: PSCanvas not initialised\n";
    return;
  }

  _fp << x0 << " " << _height - y0 << " "  << x1 << " " << _height - y1  << " " 
      << " line\n";
  return;
}


void PSCanvas::Flush(void)
{
  if (!_initialised) {
    cerr << "Warning: PSCanvas not initialised\n";
    return;
  }

  _fp.flush();
  return;
}


void PSCanvas::Close(void)
{
  if (!_fp.is_open()) {
    cerr << "Warning: PSCanvas not initialised\n";
    return;
  }

  _fp << "%%EOF\n";

  _fp.flush();
  _fp.close();

  _initialised = false;
}
