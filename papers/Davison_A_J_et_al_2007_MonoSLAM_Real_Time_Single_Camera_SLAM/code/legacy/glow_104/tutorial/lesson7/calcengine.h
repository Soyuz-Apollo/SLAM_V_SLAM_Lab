/*
===============================================================================

	FILE:  calcengine.h
	
	PROJECT:
	
		GLOW-based RPN calculator
	
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

#ifndef CALCENGINE__H
#define CALCENGINE__H


/*

This module can be treated as a "black box" for the purposes of this lesson.
It implements all the machinery for making a simple RPN calculator. It just
lacks a user interface.

*/


/*
===============================================================================
	Headers
===============================================================================
*/

#include <string>

using namespace std;


/*
===============================================================================
	Parameters
===============================================================================
*/

static const int STACKSIZE = 4;
typedef double Numeric;


/*
===============================================================================
	Data structure
===============================================================================
*/

class CalcEngine
{
	public:
	
		enum Button
		{
			enterButton = 100,
			pointButton,
			exponentButton,
			negateButton,
			deleteButton,
			plusButton,
			minusButton,
			multButton,
			divButton,
			recipButton,
			sqrButton,
			sqrtButton,
			expButton,
			logButton,
			powButton,
			rootButton,
			sinButton,
			cosButton,
			tanButton,
			arcsinButton,
			arccosButton,
			arctanButton,
			swapButton,
			consumeButton,
			clearButton
		};
		
		enum Format
		{
			decimalFormat,
			scientificFormat
		};
		
		enum AngleUnits
		{
			degrees,
			radians
		};
	
	public:
	
		// Create a calculator
		CalcEngine();
		
		// Simulate a button press. Pass 0 through 9 or one of the Button
		// constants given above.
		void Button(
			int button);
		
		// Get a string describing what should be displayed for the stack
		string Display(
			int level = 0) const;
		
		// Get the value in a position on the stack
		inline Numeric Value(
			int level = 0) const;
		
		// The format (normal/scientific)
		inline Format GetFormat() const;
		inline void SetFormat(
			Format fmt);
		
		// Radians or degrees
		inline AngleUnits GetAngleUnits() const;
		inline void SetAngleUnits(
			AngleUnits units);
	
	private:
	
		enum State
		{
			preEntry,
			enterInt,
			enterFrac,
			enterExpon,
			postEntry
		};
	
	private:
	
		void _Enter();
		void _Clear();
		void _BinaryOp(
			Numeric result);
		void _UnaryOp(
			Numeric result);
		void _BuildString(
			string& str) const;
		inline Numeric _ToRadians(
			Numeric val);
		inline Numeric _FromRadians(
			Numeric val);
	
	private:
	
		Numeric _stack[STACKSIZE];
		State _state;
		Format _format;
		AngleUnits _angleUnits;
		string _intMantissa;
		string _fracMantissa;
		string _exponent;
		bool _mantissaNegative;
		bool _exponentNegative;
};


/*
===============================================================================
	Inline methods
===============================================================================
*/

inline double CalcEngine::Value(
	int level) const
{
	GLOW_ASSERT(level >= 0 && level < STACKSIZE);
	return _stack[level];
}


inline CalcEngine::Format CalcEngine::GetFormat() const
{
	return _format;
}


inline void CalcEngine::SetFormat(
	Format fmt)
{
	_format = fmt;
}


inline CalcEngine::AngleUnits CalcEngine::GetAngleUnits() const
{
	return _angleUnits;
}


inline void CalcEngine::SetAngleUnits(
	AngleUnits angleUnits)
{
	_angleUnits = angleUnits;
}


inline Numeric CalcEngine::_ToRadians(
	Numeric val)
{
	return (_angleUnits == radians) ? val :
		val * Numeric(3.14159265358979323846264338328)/Numeric(180);
}


inline Numeric CalcEngine::_FromRadians(
	Numeric val)
{
	return (_angleUnits == radians) ? val :
		val * Numeric(180)/Numeric(3.14159265358979323846264338328);
}


#endif
