/*
===============================================================================

	FILE:  mandeldata.cpp
	
	PROJECT:
	
		GLOW-based mandelbrot viewer
	
	CONTENTS:
	
		Data and computation engine
	
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

#include "mandeldata.h"


/*
===============================================================================
	Methods of MandelData
===============================================================================
*/

MandelData::MandelData()
{
	_data = new int[201*201];
	_width = 201;
	_height = 201;
	_xcenter = -0.5;
	_ycenter = 0;
	_pixelwidth = 0.015;
	_valid = false;
	_linesLeft = _height;
	_threshhold = 1023;
}


MandelData::~MandelData()
{
	delete[] _data;
}


void MandelData::SetSize(
	int width,
	int height)
{
	_width = width;
	_height = height;
	delete[] _data;
	_data = new int[width*height];
	_valid = false;
	_linesLeft = _height;
}


void MandelData::Recalc()
{
	while (!_valid)
	{
		RecalcOneLine();
	}
}


void MandelData::RecalcOneLine()
{
	if (_valid) return;
	if (_linesLeft == _height)
	{
		_cury = _ycenter-Numeric(_height-1)*0.5*_pixelwidth;
	}
	
	int* data = _data+(_height-_linesLeft)*_width;
	Numeric x = _xcenter-Numeric(_width-1)*0.5*_pixelwidth;
	for (int xindex=0; xindex<_width; ++xindex)
	{
		Numeric a=0;
		Numeric b=0;
		int index=1;
		while (index<_threshhold && a*a+b*b<4)
		{
			Numeric anew = a*a-b*b+x;
			b = a*b*2+_cury;
			a = anew;
			++index;
		}
		*data = (index == _threshhold) ? 0 : index;
		data++;
		x += _pixelwidth;
	}
	_cury += _pixelwidth;
	
	--_linesLeft;
	if (_linesLeft == 0)
	{
		_valid = true;
	}
}


void MandelData::InvalidateData()
{
	_valid = false;
	_linesLeft = _height;
	for (int i=0; i<_width*_height; ++i)
	{
		_data[i] = 0;
	}
}

