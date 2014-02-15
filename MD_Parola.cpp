/*
MD_Parola - Library for modular scrolling text and Effects
  
See header file for comments
  
Copyright (C) 2013 Marco Colli. All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <MD_Parola.h>
#include <MD_Parola_lib.h>
#include <MD_MAX72xx.h>
/**
 * \file
 * \brief Implements core MD_Parola class methods
 */

MD_Parola::MD_Parola(uint8_t dataPin, uint8_t clkPin, uint8_t csPin, uint8_t numDevices):
_D(dataPin, clkPin, csPin, numDevices)
{
}

MD_Parola::MD_Parola(uint8_t csPin, uint8_t numDevices): _D(csPin, numDevices)
{
}

void MD_Parola::begin(void)
{
  _D.begin();
  // Set up the MAX72XX library
  // The MAX72XX is in power-saving mode on startup,
  // we have to do a wakeup call, set the brightness, enable updates and clear the display
  _D.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY/2);
  _D.control(MD_MAX72XX::SHUTDOWN, MD_MAX72XX::OFF);
  _D.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  _fsmState = END;

  // initialise options in this library using methods supplied
  setSpeed(10);
  setPause(10*getSpeed());
  setCharSpacing(1);
  setTextAlignment(LEFT);
  setTextEffect(PRINT, NO_EFFECT);
  setTextBuffer(NULL);
  setInvert(false);
  displaySuspend(false);
  displayClear();

  // Now set the default viewing parameters for this library
  _D.setFont(NULL);
}

MD_Parola::~MD_Parola(void)
{
}

void MD_Parola::displayScroll(char *pText, textPosition_t align, textEffect_t effect, uint16_t speed)
{
	setTextAlignment(align);
	setSpeed(speed);
	setPause(0);
	setTextEffect(effect, effect);
	setTextBuffer(pText);

	displayReset();
}

void MD_Parola::displayText(char *pText, textPosition_t align, uint16_t speed, uint16_t pause, textEffect_t effectIn, textEffect_t effectOut)
{
	setTextAlignment(align);
	setSpeed(speed);
	setPause(pause);
	setTextEffect(effectIn, effectOut);
	setTextBuffer(pText);

	displayReset();
}

bool MD_Parola::displayAnimate(void)
{
	if (_fsmState == END) 
		return(true);

	// work through things that stop us running this at all
	if (((_fsmState == PAUSE) && (millis() - _lastRunTime < _pauseTime)) ||
		(millis() - _lastRunTime < _tickTime) ||
		(_suspend))
		return(false);

	// save the time now, before we run the animation, so that the animation is part of the 
	// delay between animations giving more accurate frame timing.
	_lastRunTime = millis();	

	// suspend the display while we animate a frame
	_D.update(MD_MAX72XX::OFF);

	// any text to display?
	if (_pText != NULL)
	{
		switch (_fsmState)
		{
		case END:		// do nothing in this state
			PRINT_STATE("ANIMATE");
		break;

		case INITIALISE:
			PRINT_STATE("ANIMATE");

			setInitialConditions();
			displayClear();
			_moveIn = true;
			// fall through to process the effect, first call will be with INITIALISE

		default: // All state except END are handled by the special effect functions
			switch (_moveIn ? _effectIn : _effectOut)
			{
				case PRINT:				effectPrint(_moveIn);		break;
				case SLICE:				effectSlice(_moveIn);		break;
				case WIPE:				effectWipe(false, _moveIn);	break;
				case WIPE_CURSOR:		effectWipe(true, _moveIn);	break;
				case OPENING:			effectOpen(false, _moveIn);	break;
				case OPENING_CURSOR:	effectOpen(true, _moveIn);	break;
				case CLOSING:			effectClose(false, _moveIn);	break;
				case CLOSING_CURSOR:	effectClose(true, _moveIn);	break;
				case BLINDS:			effectBlinds(_moveIn);		break;
				case DISSOLVE:			effectDissolve(_moveIn);	break;
				case SCROLL_UP:
				case SCROLL_DOWN:		effectVScroll((_moveIn ? _effectIn : _effectOut), _moveIn);	break;
				case SCROLL_LEFT:
				case SCROLL_RIGHT:		effectHScroll((_moveIn ? _effectIn : _effectOut), _moveIn);	break;
				default:
					_fsmState = END;
			}

			// one way toggle for input to output, reset on Initialise
			_moveIn = _moveIn && !(_fsmState == PAUSE);
		break;
		}
	}

	TIME_PROFILE("\nAnimation time ");

	// re-enable and update the display
	_D.update(MD_MAX72XX::ON);

	TIME_PROFILE(": Cycle time ");

	return(_fsmState == END);
}

void MD_Parola::displayReset(void)
{
	_suspend = false;
	_fsmState = INITIALISE;
}

void MD_Parola::setInitialConditions(void)
// set the global variables initial conditions for all display effects
{
	PRINTS("\nsetInitialConditions");

	if (_pText == NULL)
		return;

	_pCurChar = _pText;
	_limitOverflow = !calcTextLimits(_pText);
}

uint16_t MD_Parola::getTextWidth(char *p)
// Get the width in columns for the text string passed to the function
// This is the sum of all the characters and the space betwen them.
{
	uint16_t	sum = 0;

	PRINTS("\ngetTextWidth");

	while (*p != '\0')
	{
		sum += _D.getChar(*p++, ARRAY_SIZE(_cBuf), _cBuf);
		if (*p)
		  sum += _charSpacing;
	}

	PRINT(": W=", sum);

	return(sum);
}

bool MD_Parola::calcTextLimits(char *p)
// Work out left and right sides for the text to be displayed, 
// depending on the text alignment. If the message will not fit 
// in the current display the return false, otherwise true.
{
	bool b = true;
	uint16_t	displayWidth = _D.getColumnCount();
	
	_textLen = getTextWidth(p);

	PRINTS("\ncalcTextLimits");

	switch (_textAlignment)
	{
	case LEFT:
		_limitLeft = displayWidth-1;
		if (_textLen > displayWidth)
		{
			_limitRight = 0;
			b = false;
		}
		else
		{
			_limitRight = _limitLeft - _textLen;
		}
		break;

	case RIGHT:
		_limitRight = 0;
		if (_textLen > displayWidth)
		{
			_limitLeft = displayWidth-1;
			b = false;
		}
		else
		{
			_limitLeft = _limitRight + _textLen;
		}
		break;

	case CENTER:
		if (_textLen > displayWidth)
		{
			_limitLeft = displayWidth-1;
			_limitRight = 0;
			b= false;
		}
		else
		{
			_limitRight = (displayWidth - _textLen)/2;
			_limitLeft = _limitRight + _textLen;
		}
		break;
	}

	PRINT(" L:", _limitLeft);
	PRINT(" R:", _limitRight);
	PRINT(" O:", !b);

	return (b);
}

uint8_t MD_Parola::makeChar(char c)
// Load a character bitmap and add in trailing char spacing blanks
{
	uint8_t	len;

	PRINTX("\nmakeChar 0x", c);

	len = _D.getChar(c, ARRAY_SIZE(_cBuf), _cBuf);
	for (uint8_t i = 0; i<_charSpacing; i++)
	{
		if (len < ARRAY_SIZE(_cBuf))
		_cBuf[len++] = 0;
	}

	PRINT(", len=", len);

	return(len);
}

uint8_t MD_Parola::reverseBuf(uint8_t *p, uint8_t size)
// reverse the elements of the specified buffer
// useful when we are scrolling right and want to insert the columns in reverse order
{
	for (uint8_t i=0; i<size/2; i++)
	{
		uint8_t	t;

		t = p[i];
		p[i] = p[size-1-i];
		p[size-1-i] = t;
	}
}

#define	SFX(s)	((_moveIn && _effectIn == (s)) || (!_moveIn && _effectOut == (s)))	///< Effect is selected if it is the effect for the current motion

void MD_Parola::moveTextPointer(void)
// This method works when increment is done AFTER processing the character
// the _endOfText flag is set as a look ahead (ie, when the last character 
// is still valid)
// We need to move a pointer forward or back, depending on the way we are 
// travelling through the text buffer.
{
	PRINTS("\nMovePtr"); 

	if (SFX(SCROLL_RIGHT))
	{
		PRINTS(" --");
		_endOfText = (_pCurChar == _pText);
		_pCurChar--;
	}
	else 
	{
		PRINTS(" ++");
		_pCurChar++;
		_endOfText = (*_pCurChar == '\0');
	}

	PRINT(": endOfText ", _endOfText); 
}

uint8_t MD_Parola::getFirstChar(void)
// load the first char into the char buffer
// return 0 if there are no characters
{
	uint8_t len = 0;

	PRINTS("\ngetFirst");

	// initialise pointers and make sure we have a good string to process
	_pCurChar = _pText;
	if ((_pCurChar == NULL) || (*_pCurChar == '\0'))
	{
		_endOfText = true;
		return(0);
	}
	_endOfText = false;
	if (SFX(SCROLL_RIGHT))
		_pCurChar += strlen(_pText) - 1;

	// good string, get the first char into the current buffer
	len = makeChar(*_pCurChar);

	if (SFX(SCROLL_RIGHT))
		reverseBuf(_cBuf, len);
	
	moveTextPointer();

	return(len);
}

uint8_t MD_Parola::getNextChar(void)
// load the next char into the char buffer
// return 0 if there are no characters
{
	uint8_t len = 0;

	PRINTS("\ngetNext ");

	if (_endOfText)
		return(0);

	len = makeChar(*_pCurChar);

	if (SFX(SCROLL_RIGHT))
		reverseBuf(_cBuf, len);

	moveTextPointer();

	return(len);
}
