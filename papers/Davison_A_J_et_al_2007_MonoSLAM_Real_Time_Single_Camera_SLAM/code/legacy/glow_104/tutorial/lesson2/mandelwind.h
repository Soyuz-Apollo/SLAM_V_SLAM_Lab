/*
===============================================================================

	FILE:  mandelwind.h
	
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

#ifndef MANDELWIND__H
#define MANDELWIND__H


/*
===============================================================================
	Headers
===============================================================================
*/

#include "glow.h"

GLOW_NAMESPACE_USING


/*
===============================================================================
	Window class
===============================================================================
*/

class MandelData;

class MandelWind :
	public GlowWindow
{
	public:
	
		MandelWind(
			MandelData* data);
		virtual ~MandelWind();
	
	protected:
	
		virtual void OnEndPaint();
		
		// New with lesson 2:
		// This method is called when the window is resized
		virtual void OnReshape(
			int width,
			int height);
		// These methods are called when mouse events happen in this window.
		virtual void OnMouseDown(
			Glow::MouseButton button,
			int x,
			int y,
			Glow::Modifiers modifiers);
		virtual void OnMouseUp(
			Glow::MouseButton button,
			int x,
			int y,
			Glow::Modifiers modifiers);
		virtual void OnMouseDrag(
			int x,
			int y);
	
	private:
	
		void _ComputeZoomFactor(
			int x,
			int y);
	
	private:
	
		MandelData* _data;
		unsigned char* _image;
		
		// New with lesson 2:
		double _halfdiagonal;
		int _dragType;
		int _xdown;
		int _ydown;
		double _factor;
};


#endif
