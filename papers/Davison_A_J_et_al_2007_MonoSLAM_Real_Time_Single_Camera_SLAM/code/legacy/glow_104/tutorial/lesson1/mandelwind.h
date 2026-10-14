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

// Notice the class inherits from GlowWindow. All toplevel window objects
// must inherit from this class.
class MandelWind :
	public GlowWindow
{
	public:
	
		MandelWind(
			MandelData* data);
		virtual ~MandelWind();
	
	protected:
	
		// This method is called when the window needs to be redrawn.
		virtual void OnEndPaint();
	
	private:
	
		MandelData* _data;
		unsigned char* _image;
};


#endif
