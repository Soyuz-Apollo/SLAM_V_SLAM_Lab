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
	Constants
===============================================================================
*/

// Drag status
const int NO_DRAG = 0;
const int ZOOM_IN_DRAG = 1;
const int ZOOM_OUT_DRAG = 2;

// Identifiers for menu items. These constants are reported to the
// menu receiver so it can identify which item was selected
// (new for lesson 3)
const int RESETZOOM_ITEM = 0;
const int QUIT_ITEM = 1;
const int RED_ITEM = 10;
const int GREEN_ITEM = 11;
const int BLUE_ITEM = 12;
const int MULTI_ITEM = 13;


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
	Glow::mouseEvents | Glow::dragEvents | Glow::menuEvents)
{
	// Get data
	data->SetThreshhold(1500);
	_data = data;
	_image = 0;
	_dragType = NO_DRAG;
	_r = 255;
	_g = 0;
	_b = 0;
	_imageValid = false;
	
	// The following is new for lesson 3...
	
	// Create new menu
	GlowMenu* menu = new GlowMenu;
	
	// Add entries to the menu. Give each item an identifying constant
	menu->AddEntry("Reset zoom", RESETZOOM_ITEM);
	menu->AddEntry("Quit", QUIT_ITEM);
	
	// Create a submenu
	GlowMenu* colorMenu = new GlowMenu;
	colorMenu->AddEntry("Red", RED_ITEM);
	colorMenu->AddEntry("Green", GREEN_ITEM);
	colorMenu->AddEntry("Blue", BLUE_ITEM);
	colorMenu->AddEntry("Multi", MULTI_ITEM);
	
	// Attach this submenu to the first menu
	menu->AddSubmenu("Color", colorMenu);
	
	// Mark the first item in the submenu
	colorMenu->SetItemMark(0, "=> ");
	
	// Bind the menu to the right mouse button on our window
	SetMenu(Glow::rightButton, menu);
	
	// Set both menus to notify our receiver when it gets a hit
	menu->Notifier().Bind(this);
	colorMenu->Notifier().Bind(this);
}


// Destructor

MandelWind::~MandelWind()
{
	delete _data;
	delete _image;
}


// Menu receiver method. This method is called when the MandelWind
// receives notification that a menu item was selected.
// (New with lesson 3)

void MandelWind::OnMessage(
	const GlowMenuMessage& message)
{
	// Respond based on the identifier given in the message
	switch (message.code)
	{
		case RESETZOOM_ITEM:
			// Reset the zoom to the starting point
			ResetZoom();
			break;
		
		case QUIT_ITEM:
			// Exit the program
			exit(0);
			break;
		
		case RED_ITEM:
			// Set the background color to red
			SetColor(255, 0, 0);
			// Mark the red item, and unmark the other two items
			message.menu->UnmarkAllItems();
			message.menu->SetItemMark(0, "=> ");
			break;
		
		case GREEN_ITEM:
			SetColor(0, 255, 0);
			message.menu->UnmarkAllItems();
			message.menu->SetItemMark(1, "=> ");
			break;
		
		case BLUE_ITEM:
			SetColor(0, 0, 255);
			message.menu->UnmarkAllItems();
			message.menu->SetItemMark(2, "=> ");
			break;
		
		case MULTI_ITEM:
			SetColor(0, 0, 0);
			message.menu->UnmarkAllItems();
			message.menu->SetItemMark(3, "=> ");
			break;
	}
}


// Idle receiver method
// (New with lesson 3)

void MandelWind::OnMessage(
	const GlowIdleMessage& message)
{
	// Post a refresh event. This causes the window to be redrawn, which
	// will, in turn, cause the next few lines to be computed.
	Refresh();
}


// Reset zoom factor
// (New with lesson 3)

void MandelWind::ResetZoom()
{
	_data->SetCenter(-0.5, 0);
	_data->SetPixelWidth(0.015);
	Refresh();
}


// Set color
// (New with lesson 3)

void MandelWind::SetColor(
	unsigned char r,
	unsigned char g,
	unsigned char b)
{
	_r = r;
	_g = g;
	_b = b;
	_imageValid = false;
	Refresh();
}


// Redraw method

void MandelWind::OnEndPaint()
{
	// Recompute if necessary
	if (!_data->IsDataValid())
	{
		// Lesson 3: changed from Recalc()
		for (int i=0; i<10; ++i) _data->RecalcOneLine();
		
		// Make image
		delete[] _image;
		_image = new unsigned char[_data->Width()*_data->Height()*4];
		_imageValid = false;
		
		// Lesson 3 (incremental updating)
		Glow::RegisterIdle(this);
	}
	else
	{
		// Lesson 3 (incremental updating)
		Glow::UnregisterIdle(this);
	}
	
	if (!_imageValid)
	{
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
			else if (_r != 0 || _g != 0 || _b != 0)
			{
				// Outside color (non-multi)
				_image[i*4] = _r;
				_image[i*4+1] = _g;
				_image[i*4+2] = _b;
				_image[i*4+3] = 0;
			}
			// outside color (multi...)
			else if (rawimage[i]<=256)
			{
				_image[i*4] = 256-rawimage[i];
				_image[i*4+1] = rawimage[i]-1;
				_image[i*4+2] = 0;
				_image[i*4+3] = 0;
			}
			else if (rawimage[i]<=512)
			{
				_image[i*4] = 0;
				_image[i*4+1] = 512-rawimage[i];
				_image[i*4+2] = rawimage[i]-257;
				_image[i*4+3] = 0;
			}
			else if (rawimage[i]<=768)
			{
				_image[i*4] = rawimage[i]-513;
				_image[i*4+1] = 0;
				_image[i*4+2] = 768-rawimage[i];
				_image[i*4+3] = 0;
			}
			else if (rawimage[i]<=1024)
			{
				_image[i*4] = 1024-rawimage[i];
				_image[i*4+1] = rawimage[i]-769;
				_image[i*4+2] = 0;
				_image[i*4+3] = 0;
			}
			else if (rawimage[i]<=1280)
			{
				_image[i*4] = 0;
				_image[i*4+1] = 1280-rawimage[i];
				_image[i*4+2] = rawimage[i]-1025;
				_image[i*4+3] = 0;
			}
			else if (rawimage[i]<=1536)
			{
				_image[i*4] = rawimage[i]-1281;
				_image[i*4+1] = 0;
				_image[i*4+2] = 1536-rawimage[i];
				_image[i*4+3] = 0;
			}
			else
			{
				// Default color (red)
				_image[i*4] = 255;
				_image[i*4+1] = 0;
				_image[i*4+2] = 0;
				_image[i*4+3] = 0;
			}
		}
		_imageValid = true;
	}
	
	// Draw image
	::glDisable(GL_LIGHTING);
	::glDisable(GL_DEPTH_TEST);
	::glRasterPos2f(-1.0f, -1.0f);
	::glDrawPixels(_data->Width(), _data->Height(), GL_RGBA, GL_UNSIGNED_BYTE, _image);
	
	// Draw drag rectangle
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

void MandelWind::OnReshape(
	int width,
	int height)
{
	// Update the viewport to specify the entire window
	::glViewport(0, 0, width, height);
	_halfdiagonal = sqrt(double(width*width+height*height))*0.5;
	
	// Update the mandel data
	_data->SetSize(width, height);
}


// Respond to mouse down events

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
			if (modifiers & Glow::shiftModifier)
			{
				_xdown = Width()/2;
				_ydown = Height()/2;
				_dragType = ZOOM_OUT_DRAG;
			}
			else
			// Zoom in if shift key isn't down
			{
				_xdown = x;
				_ydown = y;
				_dragType = ZOOM_IN_DRAG;
			}
			_ComputeZoomFactor(x, y);
			Refresh();
		}
	}
}


// Respond to mouse up events

void MandelWind::OnMouseUp(
	Glow::MouseButton button,
	int x,
	int y,
	Glow::Modifiers modifiers)
{
	if (_dragType == ZOOM_IN_DRAG)
	{
		// for zooming "in"
		_ComputeZoomFactor(x, y);
		if (_factor != 0)
		{
			_data->MoveCenter(
				(_xdown-Width()/2)*_data->GetPixelWidth(),
				(Height()/2-_ydown)*_data->GetPixelWidth());
			_data->ScalePixelWidth(_factor);
		}
		_dragType = NO_DRAG;
		Refresh();
	}
	else if (_dragType == ZOOM_OUT_DRAG)
	{
		// for zooming "out"
		_ComputeZoomFactor(x, y);
		if (_factor != 0)
		{
			_data->ScalePixelWidth(1.0/_factor);
		}
		_dragType = NO_DRAG;
		Refresh();
	}
}


// Respond to mouse drag events

void MandelWind::OnMouseDrag(
	int x,
	int y)
{
	if (_dragType == ZOOM_IN_DRAG || _dragType == ZOOM_OUT_DRAG)
	{
		_ComputeZoomFactor(x, y);
		Refresh();
	}
}


// Compute current zooming factor

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
