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
#include <fstream>

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
// Removed because we aren't using the old menus anymore
/*
const int RESETZOOM_ITEM = 0;
const int QUIT_ITEM = 1;
const int RED_ITEM = 10;
const int GREEN_ITEM = 11;
const int BLUE_ITEM = 12;
const int MULTI_ITEM = 13;
*/


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
	data->SetThreshhold(1000);
	_data = data;
	_image = 0;
	_dragType = NO_DRAG;
	_r = 255;
	_g = 0;
	_b = 0;
	_imageValid = false;
	_updateInBackground = true;
	_multiCycleRate = 1.0f;
	_multiCycleOffset = 0.0f;
	
	// We removed the menu material because we aren't using menus anymore
	
/*	// Create new menu
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
	colorMenu->Notifier().Bind(this); */
	
	// The following is new with lesson 4:
	
	// Create a control panel window to go along with this window
	_controlWindow = new GlowQuickPaletteWindow("Controls");
	
	// Add controls
	// First, we'll put a little blurb at the top of the window
	_controlWindow->AddLabel(
		"Mandelglow (lesson 4)\nby Daniel Azuma");
	
	// The next set of controls will be within a panel called "calculation"
	GlowQuickPanelWidget* panel = _controlWindow->AddPanel(
		GlowQuickPanelWidget::etchedStyle, "Calculation");
	
	// Checkbox for background calculation.
	// Put it within the calculation panel.
	// Initial value is on (checked), and bind notifier to this MandelWind.
	panel->AddCheckBox("Background calcuation", GlowCheckBoxWidget::on, this);
	
	// Create a slider for the number of iterations
	// Min iterations = 100, max = 2000, initial value = 1000.
	// Specify the syntax for the value labels and the main label.
	// Bind notifier to this MandelWind.
	_iterationSlider = panel->AddSlider(100, 2000, 1000,
		GlowSliderWidget::defaultOptions, 2, "%.0f", "Iterations:\n%.0f", this);
	
	// Create another panel called "view"
	panel = _controlWindow->AddPanel(
		GlowQuickPanelWidget::etchedStyle, "View");
	
	// Color scheme popup menu put it within the view panel
	GlowLabeledPopupMenuWidget* popup = panel->AddPopupMenu(
		"Color scheme:", this);
	popup->AddItem("Red");
	popup->AddItem("Green");
	popup->AddItem("Blue");
	popup->AddItem("Multi");
	
	// Create a nested panel.
	_multiColorControlsPanel = panel->AddPanel(
		GlowQuickPanelWidget::etchedStyle, "Multicolor settings");
	
	// Slider giving the rate of cycling for "multi" coloring style
	_multiCycleRateSlider = _multiColorControlsPanel->AddSlider(0.2, 20.0, 1.0,
		GlowSliderWidget::logarithmic, 2, "%.2f", "Cycle rate:\n%.2f", this);
	_multiCycleOffsetSlider = _multiColorControlsPanel->AddSlider(0.0, 3.0, 0.0,
		GlowSliderWidget::defaultOptions, 2, "%.2f", "Cycle offset:\n%.2f", this);
	
	// The panel starts off inactive because the initial color scheme is
	// plain red (not multicolor). Note that this also deactivates both
	// sliders that are within the panel
	_multiColorControlsPanel->Deactivate();
	
	// Reset view button.
	_resetButton = panel->AddPushButton("Reset View", this);
	
	// Quit button and save button. Use an arranging panel to cause
	// the buttons to be laid out horizontally
	panel = _controlWindow->AddArrangingPanel(
		GlowQuickPanelWidget::horizontal);
	_quitButton = panel->AddPushButton("Quit", this);
	_saveButton = panel->AddPushButton("Save Image", this);
	
	// Arrange controls and show the control panel window
	_controlWindow->Pack();
}


// Destructor

MandelWind::~MandelWind()
{
	delete _data;
	delete _image;
	
	// We don't need to delete all the individual widgets because the
	// window contains them and will delete them automatically. Just
	// delete the window.
	delete _controlWindow;
}


// Menu receiver method. This method is called when the MandelWind
// receives notification that a menu item was selected.
// Not used anymore as of lesson 4

/*
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
*/


// Idle receiver method

void MandelWind::OnMessage(
	const GlowIdleMessage& message)
{
	// Post a refresh event. This causes the window to be redrawn, which
	// will, in turn, cause the next few lines to be computed.
	Refresh();
}


// Reset zoom factor

void MandelWind::ResetZoom()
{
	_data->SetCenter(-0.5, 0);
	_data->SetPixelWidth(0.015);
	Refresh();
}


// Set color

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
		// Lesson 4: use _updateInBackground setting
		if (_updateInBackground)
		{
			for (int i=0; i<10; ++i) _data->RecalcOneLine();
		}
		else
		{
			_data->Recalc();
		}
		
		// Make image
		delete[] _image;
		_image = new unsigned char[_data->Width()*_data->Height()*4];
		_imageValid = false;
		
		// Make sure we update again at idle time
		if (_updateInBackground)
		{
			Glow::RegisterIdle(this);
		}
	}
	else
	{
		// No longer need to update at idle time
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
			else
			{
				// Lesson 4: better code that uses multi cycle rate/offset
				float cycleLength = 256.0f/_multiCycleRate;
				float value = rawimage[i]+_multiCycleOffset*cycleLength;
				value -= floor(value/(cycleLength*3))*(cycleLength*3);
				if (value < cycleLength)
				{
					_image[i*4] = int((cycleLength-value)*255.0f/cycleLength);
					_image[i*4+1] = int(value*255.0f/cycleLength);
					_image[i*4+2] = 0;
					_image[i*4+3] = 0;
				}
				else if (value < cycleLength*2.0f)
				{
					_image[i*4] = 0;
					_image[i*4+1] = int((cycleLength*2.0f-value)*255.0f/cycleLength);
					_image[i*4+2] = int((value-cycleLength)*255.0f/cycleLength);
					_image[i*4+3] = 0;
				}
				else
				{
					_image[i*4] = int((value-cycleLength*2.0f)*255.0f/cycleLength);
					_image[i*4+1] = 0;
					_image[i*4+2] = int((cycleLength*3.0f-value)*255.0f/cycleLength);
					_image[i*4+3] = 0;
				}
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


// The following code is new in lesson 4...

// Receive pushbutton events
void MandelWind::OnMessage(
	const GlowPushButtonMessage& message)
{
	// Was it the quit button?
	if (message.widget == _quitButton)
	{
		exit(0);
	}
	// Reset button?
	else if (message.widget == _resetButton)
	{
		ResetZoom();
	}
	// Save button?
	else if (message.widget == _saveButton)
	{
		// Create window to prompt for save filename
		// Have it notify this MandelWind when the user dismisses it with
		// a button press
		new GlowTextFieldWindow("Save Image", GlowWindow::autoPosition,
			GlowWindow::autoPosition, "Enter file name:", 300, "image.ppm",
			"OK\tCancel", this);
	}
}


// Receive slider events
void MandelWind::OnMessage(
	const GlowSliderMessage& message)
{
	// Was it the multicolor cycle rate slider?
	if (message.widget == _multiCycleRateSlider)
	{
		_multiCycleRate = message.value;
		_imageValid = false;
		Refresh();
	}
	// Was it the multicolor cycle offset slider?
	else if (message.widget == _multiCycleOffsetSlider)
	{
		_multiCycleOffset = message.value;
		_imageValid = false;
		Refresh();
	}
	// Was it the number of iterations slider?
	else if (message.widget == _iterationSlider)
	{
		// Respond only on slider release (i.e. non-"live" slider)
		if (message.released)
		{
			_data->SetThreshhold(int(message.value));
			Refresh();
		}
	}
}


// Receive checkbox events
void MandelWind::OnMessage(
	const GlowCheckBoxMessage& message)
{
	// Only one checkbox was created-- assume it's the background calculation
	// checkbox
	_updateInBackground = (message.state == GlowCheckBoxWidget::on);
}


// Receive popup menu events
void MandelWind::OnMessage(
	const GlowPopupMenuMessage& message)
{
	// Only one popup menu was created-- assume it's the color scheme menu
	switch (message.item)
	{
		case 0:
			// Set the background color to red
			SetColor(255, 0, 0);
			// The popup menu will automatically mark and unmark items
			// Deactivate the sliders that control multicolor options
			_multiColorControlsPanel->Deactivate();
			break;
		
		case 1:
			// Set the background color to green
			SetColor(0, 255, 0);
			// Deactivate the sliders that control multicolor options
			_multiColorControlsPanel->Deactivate();
			break;
		
		case 2:
			// Set the background color to blue
			SetColor(0, 0, 255);
			// Deactivate the sliders that control multicolor options
			_multiColorControlsPanel->Deactivate();
			break;
		
		case 3:
			// Set the background color to multi
			SetColor(0, 0, 0);
			// Activate the sliders that control multicolor options
			_multiColorControlsPanel->Activate();
			break;
	}
}


// Receive events from the save dialog
void MandelWind::OnMessage(
	const GlowTextFieldWindowMessage& message)
{
	// Only one text field window was created-- assume it's the save dialog
	
	// Bail out if the user pressed the cancel button
	if (message.response == 1) return;
	
	// Otherwise OK button (response == 0) was pressed...
	// We're just going to save the image as simple P3 ppm. You can implement
	// a better image format if you want to
	ofstream ofs(message.text.c_str());
	if (!ofs) return;
	ofs << "P3 " << _data->Width() << " " << _data->Height() << " 255\n";
	int numpixels = _data->Width()*_data->Height();
	for (int i=0; i<numpixels; ++i)
	{
		ofs << int(_image[i*4]) << " " << int(_image[i*4+1]) << " " <<
			int(_image[i*4+2]) << endl;
	}
}

