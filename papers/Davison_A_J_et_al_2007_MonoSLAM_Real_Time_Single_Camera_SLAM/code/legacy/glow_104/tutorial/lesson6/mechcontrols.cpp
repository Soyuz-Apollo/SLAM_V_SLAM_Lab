/*
===============================================================================

	FILE:  mechcontrols.cpp
	
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


/*
===============================================================================
	Headers
===============================================================================
*/

#include "mechcontrols.h"


/*
===============================================================================
	External things defined in glowmech.cpp
	that we need access to
===============================================================================
*/

extern int shoulder1, shoulder2, shoulder3, shoulder4, lat1, lat2,
  elbow1, elbow2, pivot, tilt, ankle1, ankle2, heel1,
  heel2, hip11, hip12, hip21, hip22, solid_part;
extern void FireCannon(void);
extern void animation(void);


/*
===============================================================================
	Methods of MandelWind
===============================================================================
*/

// Constructor

MechControls::MechControls(
	int mainWindowID)
{
	GLOW_DEBUGSCOPE("MechControls::MechControls");
	
	_mainWindowID = mainWindowID;
	
	// Create a control panel window
	_controlWindow = new GlowQuickPaletteWindow("Controls");
	
	// Add controls
	// First, we'll put a little blurb at the top of the window
	_controlWindow->AddLabel(
		"Glowmech (lesson 6)\n"
		"Original glutmech code by Simon Parkinson-Bates\n"
		"Glow controls added by Daniel Azuma");
	
	// The rest of the window appears in three panels. The panels are arranged
	// horizontally, but widgets are arranged vertically within each panel
	GlowQuickPanelWidget* hpanel = _controlWindow->AddArrangingPanel(
		GlowQuickPanelWidget::horizontal);
	
	// First we have a few general controls
	GlowQuickPanelWidget* panel = hpanel->AddPanel(
		GlowQuickPanelWidget::loweredStyle, "Main",
		GlowQuickPanelWidget::vertical);
	
	// Rotate hip slider
	_rotateHipSlider = panel->AddSlider(-180, 180, 0,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Rotation:\n%.0f", this);
	// Tilt torso slider
	_tiltTorsoSlider = panel->AddSlider(-90, 90, 10,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Tilt:\n%.0f", this);
	// A separator
	panel->AddSeparator();
	// Toggle animation checkbox
	_animationCheckbox = panel->AddCheckBox(
		"Animation", GlowCheckBoxWidget::off, this);
	// Toggle wireframe mode checkbox
	_wireframeCheckbox = panel->AddCheckBox(
		"Wireframe", GlowCheckBoxWidget::on, this);
	// Fire guns button
	_fireButton = panel->AddPushButton("Fire cannon", this);
	// Quit button
	_quitButton = panel->AddPushButton("Quit", this);
	
	// Now make a bunch of sliders for each side of the robot. First the right side.
	panel = hpanel->AddPanel(
		GlowQuickPanelWidget::loweredStyle, "Right",
		GlowQuickPanelWidget::vertical);
	
	// Shoulder back/forward slider
	_rShoulderFwdSlider = panel->AddSlider(-180, 180, 0,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Shoulder fwd:\n%.0f", this);
	// Shoulder in/out slider
	_rShoulderOutSlider = panel->AddSlider(0, 180, 20,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Shoulder out:\n%.0f", this);
	// Elbow back/forward slider
	_rElbowFwdSlider = panel->AddSlider(-90, 90, 0,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Elbow fwd:\n%.0f", this);
	// Elbow in/out slider
	_rElbowOutSlider = panel->AddSlider(-180, 180, 0,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Arm out:\n%.0f", this);
	// Hip back/forward slider
	_rHipFwdSlider = panel->AddSlider(-90, 90, 0,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Hip fwd:\n%.0f", this);
	// Hip in/out slider
	_rHipOutSlider = panel->AddSlider(-30, 90, 10,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Hip out:\n%.0f", this);
	// Knee slider
	_rKneeSlider = panel->AddSlider(-90, 90, 0,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Knee:\n%.0f", this);
	// Ankle slider
	_rAnkleSlider = panel->AddSlider(-90, 90, 0,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Ankle:\n%.0f", this);

	// Now the left side
	panel = hpanel->AddPanel(
		GlowQuickPanelWidget::loweredStyle, "Left",
		GlowQuickPanelWidget::vertical);
	
	// Shoulder back/forward slider
	_lShoulderFwdSlider = panel->AddSlider(-180, 180, 0,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Shoulder fwd:\n%.0f", this);
	// Shoulder in/out slider
	_lShoulderOutSlider = panel->AddSlider(0, 180, 20,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Shoulder out:\n%.0f", this);
	// Elbow back/forward slider
	_lElbowFwdSlider = panel->AddSlider(-90, 90, 0,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Elbow fwd:\n%.0f", this);
	// Elbow in/out slider
	_lElbowOutSlider = panel->AddSlider(-180, 180, 0,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Arm out:\n%.0f", this);
	// Hip back/forward slider
	_lHipFwdSlider = panel->AddSlider(-90, 90, 0,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Hip fwd:\n%.0f", this);
	// Hip in/out slider
	_lHipOutSlider = panel->AddSlider(-30, 90, 10,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Hip out:\n%.0f", this);
	// Knee slider
	_lKneeSlider = panel->AddSlider(-90, 90, 0,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Knee:\n%.0f", this);
	// Ankle slider
	_lAnkleSlider = panel->AddSlider(-90, 90, 0,
		GlowSliderWidget::defaultOptions, 3, "%.0f", "Ankle:\n%.0f", this);
	
	// Arrange controls and show the control panel window
	_controlWindow->Pack();
}


// Destructor

MechControls::~MechControls()
{
	delete _controlWindow;
}


// Receive pushbutton events
void MechControls::OnMessage(
	const GlowPushButtonMessage& message)
{
	GLOW_DEBUGSCOPE("MechControls::OnMessage(pushbutton)");
	
	// Was it the quit button?
	if (message.widget == _quitButton)
	{
		exit(0);
	}
	// Fire cannons button?
	else if (message.widget == _fireButton)
	{
		FireCannon();
		Glow::RefreshGlutWindow(_mainWindowID);
	}
}


// Receive checkbox events
void MechControls::OnMessage(
	const GlowCheckBoxMessage& message)
{
	GLOW_DEBUGSCOPE("MechControls::OnMessage(checkbox)");
	
	// Was it the toggle animation checkbox?
	if (message.widget == _animationCheckbox)
	{
		if (message.state == GlowCheckBoxWidget::on)
		{
			// We need to use this special method instead of ::glutIdleFunc()
			Glow::SetIdleFunc(animation);
		}
		else
		{
			// We need to use this special method instead of ::glutIdleFunc()
			Glow::SetIdleFunc(NULL);
		}
	}
	// Was it the toggle wireframe checkbox?
	else if (message.widget == _wireframeCheckbox)
	{
		solid_part = 1-solid_part;
		Glow::RefreshGlutWindow(_mainWindowID);
	}
}


// Receive slider events
void MechControls::OnMessage(
	const GlowSliderMessage& message)
{
	GLOW_DEBUGSCOPE("MechControls::OnMessage(slider)");
	
	// Check central sliders
	
	// Hip rotate
	if (message.widget == _rotateHipSlider)
	{
		pivot = int(message.value);
	}
	else if (message.widget == _tiltTorsoSlider)
	{
		tilt = int(message.value);
	}
	
	// Check left side sliders
	
	// Shoulder forward/back
	else if (message.widget == _lShoulderFwdSlider)
	{
		shoulder2 = int(message.value);
	}
	// Shoulder in/out
	else if (message.widget == _lShoulderOutSlider)
	{
		lat2 = int(message.value);
	}
	// Elbow forward/back
	else if (message.widget == _lElbowFwdSlider)
	{
		elbow2 = int(message.value);
	}
	// Elbow in/out
	else if (message.widget == _lElbowOutSlider)
	{
		shoulder4 = int(message.value);
	}
	// Hip forward/back
	else if (message.widget == _lHipFwdSlider)
	{
		hip21 = int(message.value);
	}
	// Hip in/out
	else if (message.widget == _lHipOutSlider)
	{
		hip22 = int(message.value);
	}
	// Knee
	else if (message.widget == _lKneeSlider)
	{
		heel2 = int(message.value);
	}
	// Ankle
	else if (message.widget == _lAnkleSlider)
	{
		ankle2 = int(message.value);
	}
	
	// Check right side sliders
	
	// Shoulder forward/back
	else if (message.widget == _rShoulderFwdSlider)
	{
		shoulder1 = int(message.value);
	}
	// Shoulder in/out
	else if (message.widget == _rShoulderOutSlider)
	{
		lat1 = int(message.value);
	}
	// Elbow forward/back
	else if (message.widget == _rElbowFwdSlider)
	{
		elbow1 = int(message.value);
	}
	// Elbow in/out
	else if (message.widget == _rElbowOutSlider)
	{
		shoulder3 = int(message.value);
	}
	// Hip forward/back
	else if (message.widget == _rHipFwdSlider)
	{
		hip11 = int(message.value);
	}
	// Hip in/out
	else if (message.widget == _rHipOutSlider)
	{
		hip12 = int(message.value);
	}
	// Knee
	else if (message.widget == _rKneeSlider)
	{
		heel1 = int(message.value);
	}
	// Ankle
	else if (message.widget == _rAnkleSlider)
	{
		ankle1 = int(message.value);
	}
	
	// Redraw scene
	Glow::RefreshGlutWindow(_mainWindowID);
}

