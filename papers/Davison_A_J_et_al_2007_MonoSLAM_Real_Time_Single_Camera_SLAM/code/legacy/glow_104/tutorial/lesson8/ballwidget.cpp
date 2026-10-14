/*
===============================================================================

	FILE:  ballwidget.cpp
	
	PROJECT:
	
		GLOW widgets for glutmech demo
	
	CONTENTS:
	
		Ball widget
	
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
	Headers and declarations
===============================================================================
*/

#include <algorithm>

using namespace std;

#include "glowViewTransform.h"

GLOW_NAMESPACE_USING

#include "ballwidget.h"


/*
===============================================================================
	Component that draws a wire sphere
===============================================================================
*/

class DrawWireSphere :
	public GlowComponent
{
	public:
	
		DrawWireSphere(
			GlowComponent* parent) :
		GlowComponent(parent) {}
	
	protected:
	
		virtual void OnEndPaint();
};


void DrawWireSphere::OnEndPaint()
{
	::glEnable(GL_CULL_FACE);
	::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	::glColor3ub(0, 255, 255);
	::glutSolidSphere(1.0, 12, 6);
	::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	::glDisable(GL_CULL_FACE);
}


/*
===============================================================================
	Methods of BallWidget
===============================================================================
*/

BallWidget::BallWidget(
	GlowWidget* parent,
	int x,
	int y,
	int width)
{
	GLOW_DEBUGSCOPE("BallWidget::BallWidget");
	
	// Initialize widget
	GlowWidgetParams params;
	params.x = x;
	params.y = y;
	params.width = params.height = width;
	GlowWidget::Init(parent->Root(), parent, params);
	
	// Make view manipulator and drawing component
	_manip = new GlowViewManipulator(this, GlowViewManipulatorParams::defaults);
	new DrawWireSphere(_manip);
	
	// Register to receive mouse events
	RegisterMouseEvents();
}


// Override this method to draw the widget
void BallWidget::OnWidgetPaint()
{
	// Here we just clear the widget's rectangle to black.
	// Since OnWidgetPaint() is called during GlowComponent::OnBeginPaint(),
	// this clears the rectangle to black, before the trackball is drawn.
	::glColor3ub(0, 0, 0);
	::glBegin(GL_QUADS);
	::glVertex2f(-1.0f, -1.0f);
	::glVertex2f(1.0f, -1.0f);
	::glVertex2f(1.0f, 1.0f);
	::glVertex2f(-1.0f, 1.0f);
	::glEnd();
}


// Handle mouse down in widget events
void BallWidget::OnWidgetMouseDown(
	Glow::MouseButton button,
	int x,
	int y,
	Glow::Modifiers modifiers)
{
	GLOW_DEBUGSCOPE("BallWidget::OnWidgetMouseDown");
	
	if (!_manip->IsDragging())
	{
		GLfloat xn;
		GLfloat yn;
		NormalizeCoordinates(x, y, xn, yn);
		_manip->BeginRotationDrag(xn, yn);
		_button = button;
		_modifiers = modifiers;
	}
}


// Handle mouse up in widget events
void BallWidget::OnWidgetMouseUp(
	Glow::MouseButton button,
	int x,
	int y,
	Glow::Modifiers modifiers)
{
	GLOW_DEBUGSCOPE("BallWidget::OnWidgetMouseUp");
	
	if (_manip->IsDragging())
	{
		GLfloat xn;
		GLfloat yn;
		NormalizeCoordinates(x, y, xn, yn);
		_manip->EndDrag(xn, yn);
		OnReleased(_button, _modifiers);
	}
}


// Handle mouse drag in widget events
void BallWidget::OnWidgetMouseDrag(
	int x,
	int y)
{
	GLOW_DEBUGSCOPE("BallWidget::OnWidgetMouseDrag");
	
	if (_manip->IsDragging())
	{
		GLfloat xn;
		GLfloat yn;
		NormalizeCoordinates(x, y, xn, yn);
		_manip->InDrag(xn, yn);
		OnDragged(_button, _modifiers);
	}
}


// Default method for handling trackball dragged events
void BallWidget::OnDragged(
	Glow::MouseButton button,
	Glow::Modifiers modifiers)
{
	GLOW_DEBUGSCOPE("BallWidget::OnDragged");
	
	// Send event message to receivers
	BallMessage message;
	message.widget = this;
	message.rotation = _manip->GetRotation();
	message.released = false;
	message.mouseButton = button;
	message.modifiers = modifiers;
	_sender.Send(message);
}


// Default method for handling trackball released events
void BallWidget::OnReleased(
	Glow::MouseButton button,
	Glow::Modifiers modifiers)
{
	GLOW_DEBUGSCOPE("BallWidget::OnReleased");
	
	// Send event message to receivers
	BallMessage message;
	message.widget = this;
	message.rotation = _manip->GetRotation();
	message.released = true;
	message.mouseButton = button;
	message.modifiers = modifiers;
	_sender.Send(message);
}


// Automatically resize widget according to constraints
GlowWidget::AutoPackError BallWidget::OnAutoPack(
	int hSize,
	int vSize,
	AutoPackOptions hOption,
	AutoPackOptions vOption,
	int& leftMargin,
	int& rightMargin,
	int& topMargin,
	int& bottomMargin)
{
	GLOW_DEBUGSCOPE("BallWidget::OnAutoPack");
	
	int hnew;
	int vnew;
	
	// noReshape is kind of like forcedSize for our purposes. We'll
	// fold the two options into one to make our life easier.
	if (hOption == noReshape)
	{
		hOption = forcedSize;
		hSize = Width();
	}
	if (vOption == noReshape)
	{
		vOption = forcedSize;
		vSize = Height();
	}
	
	// Handle forcedSize first
	// If forcing different sizes, fail because we want the widget square
	if (hOption == forcedSize && vOption == forcedSize && hSize != vSize)
	{
		return hAutoPackError;
	}
	// Set to the forced size, if it's not too small
	if (hOption == forcedSize)
	{
		if (hSize < 20)
		{
			return hAutoPackError;
		}
		hnew = hSize;
	}
	if (vOption == forcedSize)
	{
		if (vSize < 20)
		{
			return vAutoPackError;
		}
		vnew = vSize;
	}
	
	// Handle preferredSize and expandPreferredSize
	if (hOption == preferredSize || hOption == expandPreferredSize)
	{
		// Make sure the constraint isn't too small
		if (hSize != unspecifiedSize && hSize < 20)
		{
			return hAutoPackError;
		}
		// Adjust constraint if vertical size is already forced
		if (vOption == forcedSize)
		{
			// Do vertical forced size and horizontal constraint conflict?
			if (hSize != unspecifiedSize && hSize < vnew)
			{
				return hAutoPackError;
			}
			hnew = vnew;
		}
		else
		{
			// If using expandPreferredSize or constraint is smaller than 100
			// then set the size to the constraint, otherwise set it to 100.
			if (hOption == expandPreferredSize ||
				(hSize != unspecifiedSize && hSize <= 100))
			{
				hnew = hSize;
			}
			else
			{
				hnew = 100;
			}
		}
	}
	if (vOption == preferredSize || vOption == expandPreferredSize)
	{
		if (vSize != unspecifiedSize && vSize < 20)
		{
			return vAutoPackError;
		}
		if (hOption == forcedSize)
		{
			if (vSize != unspecifiedSize && vSize < hnew)
			{
				return vAutoPackError;
			}
			vnew = hnew;
		}
		else
		{
			if (vOption == expandPreferredSize ||
				(vSize != unspecifiedSize && vSize <= 100))
			{
				vnew = vSize;
			}
			else
			{
				vnew = 100;
			}
		}
	}
	
	// Handle one more case: neither vertical nor horizontal were forced,
	// but they evaluated to different sizes. To make it square, pick the
	// smaller size
	if (hnew != vnew)
	{
		hnew = vnew = min(hnew, vnew);
	}
	
	// Resize widget
	Reshape(hnew, vnew);
	
	// Done!
	return noAutoPackError;
}


/*
===============================================================================
*/

