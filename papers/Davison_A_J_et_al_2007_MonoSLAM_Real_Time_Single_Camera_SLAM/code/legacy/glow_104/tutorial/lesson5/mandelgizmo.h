/*
===============================================================================

	FILE:  mandelgizmo.h
	
	PROJECT:
	
		GLOW-based mandelbrot viewer
	
	CONTENTS:
	
		Gizmo window
	
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

#ifndef MANDELGIZMO__H
#define MANDELGIZMO__H


/*
===============================================================================
	Headers
===============================================================================
*/

#include "glow.h"
#include "glowQuickPalette.h"

GLOW_NAMESPACE_USING


/*
===============================================================================
	Gizmo toplevel window class
===============================================================================
*/

class MandelGizmoSubwindow;

class MandelGizmoWindow :
	public GlowWindow
{
	public:
	
		MandelGizmoWindow(
			const unsigned char* image,
			int width,
			int height);
	
	protected:
	
		virtual void OnReshape(
			int width,
			int height);
	
	private:
	
		GlowQuickPaletteSubwindow* _widgetsPanel;
		MandelGizmoSubwindow* _gizmoPanel;
};


#endif
