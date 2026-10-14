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

// New with lesson 3:
// Implement GlowMenuReceiver interface to receive menu events
// Implement GlowIdleReceiver interface to receive idle events

class MandelWind :
	public GlowWindow,
	public GlowMenuReceiver,
	public GlowIdleReceiver
{
	public:
	
		MandelWind(
			MandelData* data);
		virtual ~MandelWind();
		
		// New with lesson 3
		void ResetZoom();
		void SetColor(
			unsigned char r,
			unsigned char g,
			unsigned char b);
	
	protected:
	
		// Refresh event
		virtual void OnEndPaint();
		
		// Basic window and mouse events
		virtual void OnReshape(
			int width,
			int height);
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
	
		// Implement this method to receive menu events (lesson 3)
		virtual void OnMessage(
			const GlowMenuMessage& message);
		
		// Implement this method to receive idle events (lesson 3)
		virtual void OnMessage(
			const GlowIdleMessage& message);
	
	private:
	
		void _ComputeZoomFactor(
			int x,
			int y);
	
	private:
	
		// Image data
		MandelData* _data;
		unsigned char* _image;
		
		// Zooming code
		double _halfdiagonal;
		int _dragType;
		int _xdown;
		int _ydown;
		double _factor;
		
		// New with lesson 3
		unsigned char _r;
		unsigned char _g;
		unsigned char _b;
		bool _imageValid;
};


#endif
