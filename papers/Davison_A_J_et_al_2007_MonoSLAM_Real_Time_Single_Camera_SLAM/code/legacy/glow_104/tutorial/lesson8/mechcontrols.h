/*
===============================================================================

	FILE:  mechcontrols.h
	
	PROJECT:
	
		GLOW widgets for glutmech demo
	
	CONTENTS:
	
		Controls window
	
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

#ifndef MECHCONTROLS__H
#define MECHCONTROLS__H


/*
===============================================================================
	Headers
===============================================================================
*/

#include "glow.h"
#include "glowQuickPalette.h"

GLOW_NAMESPACE_USING

#include "ballwidget.h"


/*
===============================================================================
	Controls class
===============================================================================
*/

// MechControls is an object that receives a number of events.
// The idle event receiver is used for background calculating.
// The TextFieldWindow event receiver is used for the save image dialog.
// The other event receivers are widgets in the control palette.

class MechControls :
	public GlowPushButtonReceiver,
	public GlowSliderReceiver,
	public GlowCheckBoxReceiver,
	public BallReceiver
{
	public:
	
		MechControls(
			int mainWindowID);
		virtual ~MechControls();
	
	protected:
	
		// Widget events
		
		// Implement this method to receive pushbutton events
		virtual void OnMessage(
			const GlowPushButtonMessage& message);
		
		// Implement this method to receive slider events
		virtual void OnMessage(
			const GlowSliderMessage& message);
		
		// Implement this method to receive check box events
		virtual void OnMessage(
			const GlowCheckBoxMessage& message);
		
		// Implement this method to receive ball events (new with lesson 8)
		virtual void OnMessage(
			const BallMessage& message);
	
	private:
	
		// Main drawing window id
		int _mainWindowID;
		
		// Pointer to the control window itself
		GlowQuickPaletteWindow* _controlWindow;
		
		// Pointers to selected widgets. We store these so that we can tell
		// what widget was hit when we receive events.
		GlowCheckBoxWidget* _animationCheckbox;
		GlowCheckBoxWidget* _wireframeCheckbox;
		GlowPushButtonWidget* _quitButton;
		GlowPushButtonWidget* _fireButton;
		GlowSliderWidget* _rotateHipSlider;
		GlowSliderWidget* _tiltTorsoSlider;
		GlowSliderWidget* _lShoulderFwdSlider;
		GlowSliderWidget* _lShoulderOutSlider;
		GlowSliderWidget* _lElbowFwdSlider;
		GlowSliderWidget* _lElbowOutSlider;
		GlowSliderWidget* _lHipFwdSlider;
		GlowSliderWidget* _lHipOutSlider;
		GlowSliderWidget* _lKneeSlider;
		GlowSliderWidget* _lAnkleSlider;
		GlowSliderWidget* _rShoulderFwdSlider;
		GlowSliderWidget* _rShoulderOutSlider;
		GlowSliderWidget* _rElbowFwdSlider;
		GlowSliderWidget* _rElbowOutSlider;
		GlowSliderWidget* _rHipFwdSlider;
		GlowSliderWidget* _rHipOutSlider;
		GlowSliderWidget* _rKneeSlider;
		GlowSliderWidget* _rAnkleSlider;
		BallWidget* _viewBall;
		BallWidget* _lightBall;
};


#endif
