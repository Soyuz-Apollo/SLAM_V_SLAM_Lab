/*
===============================================================================

	FILE:  calcengine.cpp
	
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

#include <cmath>
#include <cstdio>

namespace std {} // For compilers that don't put C libraries in namespace std

using namespace std;

#include "glowDebug.h"

GLOW_NAMESPACE_USING

#include "calcengine.h"


/*
===============================================================================
	Methods of CalcEngine
===============================================================================
*/

CalcEngine::CalcEngine()
{
	GLOW_DEBUGSCOPE("CalcEngine::CalcEngine");
	
	for (int i=0; i<STACKSIZE; ++i)
	{
		_stack[i] = 0;
	}
	_state = postEntry;
	_format = decimalFormat;
	_angleUnits = degrees;
	_mantissaNegative = _exponentNegative = false;
}


void CalcEngine::Button(
	int button)
{
	GLOW_DEBUGSCOPE("CalcEngine::Button");
	
	double tempval = 0;
	
	switch (button)
	{
		case enterButton:
			_Enter();
			break;
		
		case pointButton:
			if (_state == preEntry)
			{
				_Enter();
			}
			if (_state == postEntry)
			{
				_Clear();
			}
			if (_state != enterFrac && _state != enterExpon)
			{
				_state = enterFrac;
			}
			break;
		
		case exponentButton:
			if (_state == preEntry)
			{
				_Enter();
			}
			if (_state == postEntry)
			{
				_Clear();
			}
			if (_state != enterExpon)
			{
				_state = enterExpon;
			}
			break;
		
		case negateButton:
			if (_state == preEntry || _state == postEntry)
			{
				_UnaryOp(-_stack[0]);
			}
			else if (_state == enterExpon)
			{
				_exponentNegative = !_exponentNegative;
			}
			else
			{
				_mantissaNegative = !_mantissaNegative;
			}
			break;
		
		case deleteButton:
			if (_state == postEntry)
			{
				_Clear();
			}
			else if (_state == enterExpon)
			{
				if (_exponent.size() > 0)
				{
					_exponent.erase(_exponent.size()-1, 1);
				}
				else
				{
					_exponentNegative = false;
					if (_fracMantissa.size() > 0)
					{
						_state = enterFrac;
					}
					else
					{
						_state = enterInt;
					}
				}
			}
			else if (_state == enterFrac)
			{
				if (_fracMantissa.size() > 0)
				{
					_fracMantissa.erase(_fracMantissa.size()-1, 1);
				}
				else
				{
					_state = enterInt;
				}
			}
			else if (_state == enterInt)
			{
				if (_intMantissa.size() > 0)
				{
					_intMantissa.erase(_intMantissa.size()-1, 1);
				}
			}
			break;
		
		case plusButton:
			_BinaryOp(_stack[1]+_stack[0]);
			break;
		
		case minusButton:
			_BinaryOp(_stack[1]-_stack[0]);
			break;
		
		case multButton:
			_BinaryOp(_stack[1]*_stack[0]);
			break;
		
		case divButton:
			_BinaryOp(_stack[1]/_stack[0]);
			break;
		
		case recipButton:
			_UnaryOp(Numeric(1)/_stack[0]);
			break;
		
		case sqrtButton:
			_UnaryOp(sqrt(_stack[0]));
			break;
		
		case sqrButton:
			_UnaryOp(_stack[0]*_stack[0]);
			break;
		
		case expButton:
			_UnaryOp(exp(_stack[0]));
			break;
		
		case logButton:
			_UnaryOp(log(_stack[0]));
			break;
		
		case powButton:
			_BinaryOp(pow(_stack[1], _stack[0]));
			break;
		
		case rootButton:
			_BinaryOp(pow(_stack[1], 1.0/_stack[0]));
			break;
		
		case sinButton:
			_UnaryOp(sin(_ToRadians(_stack[0])));
			break;
		
		case cosButton:
			_UnaryOp(cos(_ToRadians(_stack[0])));
			break;
		
		case tanButton:
			_UnaryOp(tan(_ToRadians(_stack[0])));
			break;
		
		case arcsinButton:
			_UnaryOp(_FromRadians(asin(_stack[0])));
			break;
		
		case arccosButton:
			_UnaryOp(_FromRadians(acos(_stack[0])));
			break;
		
		case arctanButton:
			_UnaryOp(_FromRadians(atan(_stack[0])));
			break;
		
		case clearButton:
			_Clear();
			break;
		
		case swapButton:
			tempval = _stack[0];
			_stack[0] = _stack[1];
			_stack[1] = tempval;
			_state = preEntry;
			break;
		
		case consumeButton:
			for (int i=1; i<STACKSIZE; ++i)
			{
				_stack[i-1] = _stack[i];
			}
			_stack[STACKSIZE-1] = 0;
			_state = preEntry;
			break;
		
		default:
			GLOW_ASSERT(button >= 0 && button <= 9);
			if (_state == preEntry)
			{
				_Enter();
			}
			if (_state == postEntry)
			{
				_Clear();
			}
			if (_state == enterInt)
			{
				if (!_intMantissa.empty() || button != 0)
				{
					_intMantissa += char('0'+button);
				}
			}
			else if (_state == enterFrac)
			{
				_fracMantissa += char('0'+button);
			}
			else if (_state == enterExpon)
			{
				if ((!_exponent.empty() || button != 0) &&
					_exponent.size() < 3 &&
					(_exponent.size() < 2 || _exponent[0] < '3'))
				{
					_exponent += char('0'+button);
				}
			}
	}
	
	if (_state != preEntry && _state != postEntry)
	{
		string temp;
		_BuildString(temp);
		_stack[0] = atof(temp.c_str());
	}
}


void CalcEngine::_Enter()
{
	for (int i=STACKSIZE-1; i>0; --i)
	{
		_stack[i] = _stack[i-1];
	}
	_state = postEntry;
}


void CalcEngine::_Clear()
{
	_state = enterInt;
	_mantissaNegative = _exponentNegative = false;
	_intMantissa = "";
	_fracMantissa = "";
	_exponent = "";
	_stack[0] = 0;
}


void CalcEngine::_BinaryOp(
	Numeric result)
{
	_state = preEntry;
	_stack[0] = result;
	for (int i=2; i<STACKSIZE; ++i)
	{
		_stack[i-1] = _stack[i];
	}
	_stack[STACKSIZE-1] = 0;
}


void CalcEngine::_UnaryOp(
	Numeric result)
{
	_state = preEntry;
	_stack[0] = result;
}


void CalcEngine::_BuildString(
	string& str) const
{
	GLOW_DEBUGSCOPE("CalcEngine::_BuildString");
	
	if (_mantissaNegative)
	{
		str = '-';
	}
	if (_intMantissa.empty())
	{
		str += '0';
	}
	else
	{
		str.append(_intMantissa);
	}
	if (!_fracMantissa.empty() || _state == enterFrac)
	{
		str += '.';
		str.append(_fracMantissa);
	}
	if (!_exponent.empty() || _state == enterExpon)
	{
		if (_exponentNegative)
		{
			str += "e-";
		}
		else
		{
			str += "e+";
		}
		str.append(_exponent);
	}
}


string CalcEngine::Display(
	int level) const
{
	GLOW_DEBUGSCOPE("CalcEngine::Display");
	GLOW_ASSERT(level >= 0 && level < STACKSIZE);
	
	string result;
	if (_state == preEntry || _state == postEntry || level != 0)
	{
		char buf[200];
		if (_format == scientificFormat || _stack[level] >= 1e10 ||
			_stack[level] <= -1e10 || (_stack[level] < 1e-10 &&
			_stack[level] > -1e-10 && _stack[level] != 0))
		{
			sprintf(buf, "%.15e", _stack[level]);
		}
		else
		{
			sprintf(buf, "%.15f", _stack[level]);
			buf[21] = 0;
		}
		result.assign(buf);
	}
	else
	{
		_BuildString(result);
	}
	return result;
}

