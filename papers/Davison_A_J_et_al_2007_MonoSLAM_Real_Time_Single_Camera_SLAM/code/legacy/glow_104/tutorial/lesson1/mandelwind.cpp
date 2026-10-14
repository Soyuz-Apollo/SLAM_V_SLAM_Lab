/*
===============================================================================

	FILE:  mandelwind.cpp
	
	PROJECT:
	
		GLOW-based mandelbrot viewer
	
	CONTENTS:
	
		Viewer window
	
	PROGRAMMERS:
	
		Daniel Azuma <dazuma@kagi.com>
	
	COPYRIGHT:
	
		Copyright (C) 2000  Daniel Azuma  (dazuma@kagi.com)
		
		This library is free software; you can redistribute it and/or
		modify it under the terms of the GNU Lesser General Public
		License as published by the Free Software Foundation; either
		version 2.1 of the License, or (at your option) any later version.
		
		This library is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
		GNU Lesser General Public License for more details.
		
		You should have received a copy of the GNU Lesser General Public
		License along with this library; if not, write to
			Free Software Foundation, Inc.
			59 Temple Place, Suite 330
			Boston, MA 02111-1307 USA
	
	VERSION:
	
		The GLOW Toolkit tutorial -- version 1.0.3  (10 September 2000)
	
	CHANGES:
	
		Original release (DA)

===============================================================================
*/


/*
===============================================================================
	Headers
===============================================================================
*/

#include "mandelwind.h"
#include "mandeldata.h"


/*
===============================================================================
	Constructor for MandelWind
===============================================================================
*/

MandelWind::MandelWind(
	MandelData* data) :
// We first construct the window itself.
GlowWindow("Mandelglow", GlowWindow::autoPosition, GlowWindow::autoPosition,
	data->Width(), data->Height(), Glow::rgbBuffer | Glow::doubleBuffer,
	Glow::noEvents)
{
	// Get data
	_data = data;
	_data->Recalc();
	
	// Make image
	_image = new unsigned char[_data->Width()*_data->Height()*4];
	const int* rawimage = _data->Data();
	int mx = _data->Width()*_data->Height();
	for (int i=0; i<mx; ++i)
	{
		if (rawimage[i] == 0)
		{
			// Black color
			_image[i*4] = 0;
			_image[i*4+1] = 0;
			_image[i*4+2] = 0;
			_image[i*4+3] = 0;
		}
		else
		{
			// Red color
			_image[i*4] = 255;
			_image[i*4+1] = 0;
			_image[i*4+2] = 0;
			_image[i*4+3] = 0;
		}
	}
}


/*
===============================================================================
	Destructor for MandelWind
===============================================================================
*/

MandelWind::~MandelWind()
{
	delete _data;
	delete _image;
}


/*
===============================================================================
	Redraw method. Called when the window needs to be redrawn
===============================================================================
*/

void MandelWind::OnEndPaint()
{
	// Put OpenGL calls here.
	::glRasterPos2f(-1.0f, -1.0f);
	::glDrawPixels(_data->Width(), _data->Height(), GL_RGBA, GL_UNSIGNED_BYTE, _image);
}

