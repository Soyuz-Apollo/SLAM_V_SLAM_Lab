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

#include <iostream>
#include <cmath>

using namespace std;

#include "mandelwind.h"
#include "mandeldata.h"


/*
===============================================================================
	Constants (the current state of user dragging)
===============================================================================
*/

const int NO_DRAG = 0;
const int ZOOM_IN_DRAG = 1;
const int ZOOM_OUT_DRAG = 2;


/*
===============================================================================
	Methods of MandelWind
===============================================================================
*/

// Constructor

MandelWind::MandelWind(
	MandelData* data) :
GlowWindow("Mandelglow", GlowWindow::autoPosition, GlowWindow::autoPosition,
	data->Width(), data->Height(), Glow::rgbBuffer | Glow::doubleBuffer,
	Glow::mouseEvents | Glow::dragEvents)
{
	// Get data
	_data = data;
	_image = 0;
	_dragType = NO_DRAG;
	
	// We've moved the image recomputation into OnEndPaint()
	// because it will need to be recomputed again whenever the user
	// resizes the window or zooms in or out.
}


// Destructor

MandelWind::~MandelWind()
{
	delete _data;
	delete _image;
}


// Redraw method

void MandelWind::OnEndPaint()
{
	// Recompute if necessary
	if (!_data->IsDataValid())
	{
		_data->Recalc();
		
		// Make image
		delete[] _image;
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
	
	// Draw image
	::glDisable(GL_LIGHTING);
	::glDisable(GL_DEPTH_TEST);
	::glRasterPos2f(-1.0f, -1.0f);
	::glDrawPixels(_data->Width(), _data->Height(), GL_RGBA, GL_UNSIGNED_BYTE, _image);
	
	// Draw drag rectangle (lesson 2)
	if ((_dragType == ZOOM_IN_DRAG || _dragType == ZOOM_OUT_DRAG) &&
		_factor != 0)
	{
		GLfloat xcenter, ycenter;
		NormalizeCoordinates(_xdown, _ydown, xcenter, ycenter);
		::glColor3f(1.0f, 1.0f, 1.0f);
		::glBegin(GL_LINE_LOOP);
		::glVertex2f(xcenter-_factor, ycenter-_factor);
		::glVertex2f(xcenter+_factor, ycenter-_factor);
		::glVertex2f(xcenter+_factor, ycenter+_factor);
		::glVertex2f(xcenter-_factor, ycenter+_factor);
		::glEnd();
	}
}


// Respond to reshape events
// (New with lesson 2)

void MandelWind::OnReshape(
	int width,
	int height)
{
	// Update the viewport to specify the entire window
	::glViewport(0, 0, width, height);
	_halfdiagonal = sqrt(double(width*width+height*height))*0.5;
	
	// Update the mandel data
	_data->SetSize(width, height);
	
	// We don't need to explicitly refresh here, because a refresh
	// event will be generated anyway whenever the window is resized.
}


// Respond to mouse down events
// (New with lesson 2)

void MandelWind::OnMouseDown(
	Glow::MouseButton button,
	int x,
	int y,
	Glow::Modifiers modifiers)
{
	// Ignore mousedowns if we're already dragging
	if (_dragType == NO_DRAG)
	{
		// Zoom with left mouse button
		if (button == Glow::leftButton)
		{
			// Zoom out if shift key is down
			// Set up variables and move into zoom-out state
			if (modifiers & Glow::shiftModifier)
			{
				_xdown = Width()/2;
				_ydown = Height()/2;
				_dragType = ZOOM_OUT_DRAG;
			}
			else
			// Zoom in if shift key isn't down
			// Set up variables and move into zoom-in state
			{
				_xdown = x;
				_ydown = y;
				_dragType = ZOOM_IN_DRAG;
			}
			_ComputeZoomFactor(x, y);
			// Explicitly refresh so the zoom rectangle can be drawn
			Refresh();
		}
	}
}


// Respond to mouse up events
// (New with lesson 2)

void MandelWind::OnMouseUp(
	Glow::MouseButton button,
	int x,
	int y,
	Glow::Modifiers modifiers)
{
	if (_dragType == ZOOM_IN_DRAG)
	{
		// For zooming "in"
		_ComputeZoomFactor(x, y);
		if (_factor != 0)
		{
			// Alter the data object for new position and scale
			_data->MoveCenter(
				(_xdown-Width()/2)*_data->GetPixelWidth(),
				(Height()/2-_ydown)*_data->GetPixelWidth());
			_data->ScalePixelWidth(_factor);
		}
		// Now in "no-dragging" state
		_dragType = NO_DRAG;
		// Explicitly refresh so the image can be updated
		Refresh();
	}
	else if (_dragType == ZOOM_OUT_DRAG)
	{
		// For zooming "out"
		_ComputeZoomFactor(x, y);
		if (_factor != 0)
		{
			// Alter the data object for new scale
			_data->ScalePixelWidth(1.0/_factor);
		}
		// Now in "no-dragging" state
		_dragType = NO_DRAG;
		// Explicitly refresh so the image can be updated
		Refresh();
	}
}


// Respond to mouse drag events
// (New with lesson 2)

void MandelWind::OnMouseDrag(
	int x,
	int y)
{
	if (_dragType == ZOOM_IN_DRAG || _dragType == ZOOM_OUT_DRAG)
	{
		// Determine new zoom factor from mouse position
		_ComputeZoomFactor(x, y);
		// Explicitly refresh so the zoom rectangle can be redrawn
		// in its new position
		Refresh();
	}
}


// Compute current zooming factor
// (New with lesson 2)

void MandelWind::_ComputeZoomFactor(
	int x,
	int y)
{
	// If the mouse is outside the window, zoom is being cancelled.
	if (x<0 || x>=Width() || y<0 || y>=Height())
	{
		_factor = 0;
	}
	else
	{
		// Figure out zoom factor
		_factor = sqrt(double((_xdown-x)*(_xdown-x)+
			(_ydown-y)*(_ydown-y)))/_halfdiagonal;
	}
}
