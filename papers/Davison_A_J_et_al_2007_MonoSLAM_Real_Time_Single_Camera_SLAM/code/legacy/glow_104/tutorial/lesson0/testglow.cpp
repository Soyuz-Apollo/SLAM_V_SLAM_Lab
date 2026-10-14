/*
===============================================================================

	FILE:  testglow.cpp
	
	PROJECT:
	
		Testing a GLOW installation and build
	
	CONTENTS:
	
		Main program
	
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

using namespace std;

// These are the GLOW headers used by this program
#include "glow.h"
#include "glowLabelWidget.h"
#include "glowPushButtonWidget.h"
#include "glowViewTransform.h"

GLOW_NAMESPACE_USING


/*
===============================================================================
	MyCubeViewer class
	This is a component that draws a simple cube
===============================================================================
*/

class MyCubeViewer :
	public GlowComponent
{
	public:
	
		inline MyCubeViewer(
			GlowComponent* parent);
		
		virtual void OnEndPaint();
};


// Constructor
// Just passes the parent component to GlowComponent's constructor

inline MyCubeViewer::MyCubeViewer(
	GlowComponent* parent) :
GlowComponent(parent)
{
}


// Draw the cube

void MyCubeViewer::OnEndPaint()
{
	::glColor3f(1.0, 0.0, 1.0);
	
	::glBegin(GL_QUADS);
	
	// front
	::glVertex3d(1.0, 1.0, 1.0);
	::glVertex3d(-1.0, 1.0, 1.0);
	::glVertex3d(-1.0, -1.0, 1.0);
	::glVertex3d(1.0, -1.0, 1.0);
	
	// back
	::glVertex3d(1.0, 1.0, -1.0);
	::glVertex3d(1.0, -1.0, -1.0);
	::glVertex3d(-1.0, -1.0, -1.0);
	::glVertex3d(-1.0, 1.0, -1.0);
	
	// right
	::glVertex3d(1.0, 1.0, 1.0);
	::glVertex3d(1.0, -1.0, 1.0);
	::glVertex3d(1.0, -1.0, -1.0);
	::glVertex3d(1.0, 1.0, -1.0);
	
	// left
	::glVertex3d(-1.0, 1.0, 1.0);
	::glVertex3d(-1.0, 1.0, -1.0);
	::glVertex3d(-1.0, -1.0, -1.0);
	::glVertex3d(-1.0, -1.0, 1.0);
	
	// top
	::glVertex3d(1.0, 1.0, 1.0);
	::glVertex3d(1.0, 1.0, -1.0);
	::glVertex3d(-1.0, 1.0, -1.0);
	::glVertex3d(-1.0, 1.0, 1.0);
	
	// bottom
	::glVertex3d(1.0, -1.0, 1.0);
	::glVertex3d(-1.0, -1.0, 1.0);
	::glVertex3d(-1.0, -1.0, -1.0);
	::glVertex3d(1.0, -1.0, -1.0);
	
	::glEnd();
}


/*
===============================================================================
	MySubwindow class
	This is a subwindow that contains MyCubeViewer embedded in a view
	manipulator so it can be interactively rotated around
===============================================================================
*/

class MySubwindow :
	public GlowSubwindow
{
	public:
	
		inline MySubwindow(
			GlowWindow* parent);
		
		virtual bool OnBeginPaint();
		
		virtual void OnMouseDown(
			Glow::MouseButton button,
			int x,
			int y,
			Glow::Modifiers modifiers);
		virtual void OnMouseDrag(
			int x,
			int y);
		virtual void OnMouseUp(
			Glow::MouseButton button,
			int x,
			int y,
			Glow::Modifiers modifiers);
	
	private:
	
		GlowViewManipulator* _manip;
};


// Constructor
// Sets up the subwindow, and sets up the component hierarchy under it
// MySubwindow contains a GlowViewManipulator, which contains a
// MyCubeViewer.

inline MySubwindow::MySubwindow(
	GlowWindow* parent) :
GlowSubwindow(parent, 0, 0, 400, 400,
	Glow::rgbBuffer | Glow::depthBuffer | Glow::doubleBuffer,
	Glow::mouseEvents | Glow::dragEvents)
{
	_manip = new GlowViewManipulator(this,
		GlowViewManipulatorParams::defaults);
	_manip->SetSpinnable(true);
	new MyCubeViewer(_manip);
}


// Begin painting: clear frame buffer and set up lighting

bool MySubwindow::OnBeginPaint()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.5, 0.5, -0.5, 0.5, 1.0, 12.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0, 0.0, -5.0);
	
	return true;
}


// Handle mouse down events by redirecting to the view manipulator

void MySubwindow::OnMouseDown(
	Glow::MouseButton button,
	int x,
	int y,
	Glow::Modifiers modifiers)
{
	if (!_manip->IsDragging())
	{
		float xn, yn;
		NormalizeCoordinates(x, y, xn, yn);
		_manip->BeginRotationDrag(xn, yn);
	}
}


// Handle mouse drag events by redirecting to the view manipulator

void MySubwindow::OnMouseDrag(
	int x,
	int y)
{
	if (_manip->IsDragging())
	{
		float xn, yn;
		NormalizeCoordinates(x, y, xn, yn);
		_manip->InDrag(xn, yn);
	}
}


// Handle mouse up events by redirecting to the view manipulator

void MySubwindow::OnMouseUp(
	Glow::MouseButton button,
	int x,
	int y,
	Glow::Modifiers modifiers)
{
	if (_manip->IsDragging())
	{
		float xn, yn;
		NormalizeCoordinates(x, y, xn, yn);
		_manip->EndDrag(xn, yn);
	}
}


/*
===============================================================================
	MyQuitPushButtonReceiver class
	This responds to button presses by quitting the program
===============================================================================
*/

class MyQuitPushButtonReceiver :
	public GlowPushButtonReceiver
{
	public:
	
		virtual void OnMessage(
			const GlowPushButtonMessage& message);
};


// Receive a button press message

void MyQuitPushButtonReceiver::OnMessage(
	const GlowPushButtonMessage& message)
{
	exit(0);
}


/*
===============================================================================
	SetupDemo
===============================================================================
*/

static void SetupDemo()
{
	// Create the toplevel window
	GlowFixedSizeWindow* wind = new GlowFixedSizeWindow(
		"Test Glow", GlowWindow::autoPosition, GlowWindow::autoPosition,
		400, 470, Glow::rgbBuffer, Glow::noEvents);
	
	// Add the panel with the rotatable cube
	new MySubwindow(wind);
	
	// Create a widget panel
	GlowWidgetSubwindow* palette = new GlowWidgetSubwindow(wind,
		0, 400, 400, 70);
	
	// Make a little label
	GlowLabelParams lparams;
	lparams.x = 10;
	lparams.y = 10;
	lparams.width = 300;
	lparams.text = "Welcome to GLOW! Rotate the cube above!";
	new GlowLabelWidget(palette, lparams);
	
	// A button that quits the program
	GlowPushButtonParams pbparams;
	pbparams.x = 10;
	pbparams.y = 35;
	pbparams.width = 100;
	pbparams.text = "Quit";
	pbparams.receiver = new MyQuitPushButtonReceiver;
	new GlowPushButtonWidget(palette, pbparams);
}


/*
===============================================================================
	Entry point
===============================================================================
*/

int main(
	int argc,
	char **argv)
{
	GLOW_DEBUGSIGNAL(SIGSEGV);
	GLOW_DEBUGSCOPE("main");
	
	cout << "Testing GLOW" << endl;
	
	Glow::Init(argc, argv);
	
	SetupDemo();
	
	Glow::MainLoop();
	
	return 0;
}

