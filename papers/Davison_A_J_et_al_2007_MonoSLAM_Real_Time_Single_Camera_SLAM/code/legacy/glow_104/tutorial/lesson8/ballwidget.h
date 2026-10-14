/*
===============================================================================

	FILE:  ballwidget.h
	
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


#ifndef BALLWIDGET__H
#define BALLWIDGET__H


/*
===============================================================================
	Headers and declarations
===============================================================================
*/

#include "glowPanelWidget.h"
#include "glowVectorAlgebra.h"
#include "glowViewTransform.h"

GLOW_NAMESPACE_USING


class BallMessage;
typedef TReceiver<const BallMessage&> BallReceiver;


/*
===============================================================================
	CLASS BallWidget
	
	Arcball widget
===============================================================================
*/

// To create a widget, we inherit from GlowWidget
class BallWidget :
	public GlowWidget
{
	//-------------------------------------------------------------------------
	//	Public interface
	//-------------------------------------------------------------------------
	
	public:
	
		// The constructor. We could also create a more complex constructor
		// using a parameter block, but we'll keep this simple for now
		BallWidget(
			GlowWidget* parent,
			int x = 0,
			int y = 0,
			int width = 100);
		
		// Reset the transform
		inline void Reset();
		
		// Access (but don't allow changes to) the transform data
		inline const Quatf& GetRotation() const;
		
		// Access the list of notifiers
		inline TSender<const BallMessage&>& Notifier();
	
	
	//-------------------------------------------------------------------------
	//	Overrideable implementation
	//-------------------------------------------------------------------------
	
	protected:
	
		// Override this method to specify how the widget will autopack.
		// This is used by systems that automatically arrange widgets,
		// such as the QuickPalette API. You should usually provide an
		// OnAutoPack method, since the default method doesn't do much.
		virtual AutoPackError OnAutoPack(
			int hSize,
			int vSize,
			AutoPackOptions hOption,
			AutoPackOptions vOption,
			int& leftMargin,
			int& rightMargin,
			int& topMargin,
			int& bottomMargin);
		
		// We're providing two methods for a subclass to override to
		// handle events differently.
		virtual void OnDragged(
			Glow::MouseButton button,
			Glow::Modifiers modifiers);
		virtual void OnReleased(
			Glow::MouseButton button,
			Glow::Modifiers modifiers);
	
	
	//-------------------------------------------------------------------------
	//	Private implementation
	//-------------------------------------------------------------------------
	
	private:
	
		// The manipulator object for the ball
		GlowViewManipulator* _manip;
		
		// A sender for widget events
		TSender<const BallMessage&> _sender;
		
		// The mouse button and modifiers that were pressed
		Glow::MouseButton _button;
		Glow::Modifiers _modifiers;
	
	protected:
	
		// We override this method to draw the widget
		virtual void OnWidgetPaint();
		
		// We override these methods to handle widget events. For this
		// widget, we're only going to handle mouse events.
		virtual void OnWidgetMouseDown(
			Glow::MouseButton button,
			int x,
			int y,
			Glow::Modifiers modifiers);
		virtual void OnWidgetMouseUp(
			Glow::MouseButton button,
			int x,
			int y,
			Glow::Modifiers modifiers);
		virtual void OnWidgetMouseDrag(
			int x,
			int y);
};


/*
===============================================================================
	CLASS BallMessage
	
	Message sent by ball widget
===============================================================================
*/

class BallMessage
{
	public:
	
		BallWidget* widget;
		Quatf rotation;
		bool released;
		Glow::MouseButton mouseButton;
		Glow::Modifiers modifiers;
};


/*
===============================================================================
	Inline methods
===============================================================================
*/

// Reset the transform
inline void BallWidget::Reset()
{
	_manip->SetIdentity();
}


// Access (but don't allow changes to) the transform data
inline const Quatf& BallWidget::GetRotation() const
{
	return _manip->GetRotation();
}


// Access the list of notifiers
inline TSender<const BallMessage&>& BallWidget::Notifier()
{
	return _sender;
}


/*
===============================================================================
*/


#endif

