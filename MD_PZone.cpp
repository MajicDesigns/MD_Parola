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
 * \brief Implements MD_PZone class methods
 */

MD_PZone::MD_PZone(void) : _fsmState(END), _userChars(NULL), _MX(NULL), _fontDef(NULL)
{
}

MD_PZone::~MD_PZone(void)
{
	// release the memory for user defined characters
	charDef *p = _userChars;

	while (p!= NULL)
	{
		charDef	*pt = p;
		p = pt->next;
		delete pt;
	};
}

void MD_PZone::begin(MD_MAX72XX *p)
{
	_MX = p;	
}

void MD_PZone::setInitialConditions(void)
// set the global variables initial conditions for all display effects
{
	PRINTS("\nsetInitialConditions");

	if (_pText == NULL)
	return;

	_pCurChar = _pText;
	_limitOverflow = !calcTextLimits(_pText);
}

void MD_PZone::setInitialEffectConditions(void)
// set the initial conditions for loops in the FSM
{
	PRINTS("\nsetInitialFSMConditions");

	_startPos = _nextPos = (_textAlignment == RIGHT ? _limitRight : _limitLeft);
	_endPos = (_textAlignment == RIGHT ? _limitLeft+1 : _limitRight);
	_posOffset = (_textAlignment == RIGHT ? 1 : -1);
}

uint16_t MD_PZone::getTextWidth(char *p)
// Get the width in columns for the text string passed to the function
// This is the sum of all the characters and the space between them.
{
	uint16_t	sum = 0;

	PRINTS("\ngetTextWidth");

	while (*p != '\0')
	{
		sum += findChar(*p++, ARRAY_SIZE(_cBuf), _cBuf);
		if (*p)
		sum += _charSpacing;
	}

	PRINT(": W=", sum);

	return(sum);
}

bool MD_PZone::calcTextLimits(char *p)
// Work out left and right sides for the text to be displayed,
// depending on the text alignment. If the message will not fit
// in the current display the return false, otherwise true.
{
	bool b = true;
	uint16_t	displayWidth = ZONE_END_COL(_zoneEnd) - ZONE_START_COL(_zoneStart);
	
	_textLen = getTextWidth(p);

	PRINTS("\ncalcTextLimits");

	switch (_textAlignment)
	{
		case LEFT:
		_limitLeft = ZONE_END_COL(_zoneEnd);
		if (_textLen > displayWidth)
		{
			_limitRight = ZONE_START_COL(_zoneStart);
			b = false;
		}
		else
		{
			_limitRight = _limitLeft - _textLen;
		}
		break;

		case RIGHT:
		_limitRight = ZONE_START_COL(_zoneStart);
		if (_textLen > displayWidth)
		{
			_limitLeft = ZONE_END_COL(_zoneEnd);
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
			_limitLeft = ZONE_END_COL(_zoneEnd);
			_limitRight = ZONE_START_COL(_zoneStart);
			b= false;
		}
		else
		{
			_limitRight = ZONE_START_COL(_zoneStart) + (displayWidth - _textLen)/2;
			_limitLeft = _limitRight + _textLen;
		}
		break;
	}

	PRINT(" L:", _limitLeft);
	PRINT(" R:", _limitRight);
	PRINT(" O:", !b);

	return (b);
}

bool MD_PZone::addChar(uint8_t code, uint8_t *data)
// Add a user defined character to the replacement list
{
	charDef	*pcd;

	if (code == 0)
	return(false);

	PRINTX("\naddChar 0x", code);

	// first see if we have the code in our list
	pcd = _userChars;
	while (pcd != NULL)
	{
		if (pcd->code == code)
		{
			pcd->data = data;
			PRINTS(" found existing in list");
			return(true);
		}
		pcd = pcd->next;
	}

	// Now see if we have an empty slot in our list
	pcd = _userChars;
	while (pcd != NULL)
	{
		if (pcd->code == 0)
		{
			pcd->code = code;
			pcd->data = data;
			PRINTS(" found empty slot");
			return(true);
		}
		pcd = pcd->next;
	}

	// default is to add a new node to the front of the list
	if ((pcd = new charDef) != NULL)
	{
		pcd->code = code;
		pcd->data = data;
		pcd->next = _userChars;
		_userChars = pcd;
		PRINTS(" added new node");
	}
	else
	{
		PRINTS(" failed allocating new node");
	}

	return(pcd != NULL);
}

bool MD_PZone::delChar(uint8_t code)
// Delete a user defined character from the replacement list
{
	charDef	*pcd = _userChars;

	if (code == 0)
	return(false);

	// Scan down the linked list
	while (pcd != NULL)
	{
		if (pcd->code == code)
		{
			pcd->code = 0;
			pcd->data = NULL;
			break;
		}
		pcd = pcd->next;
	}

	return(pcd != NULL);
}

uint8_t MD_PZone::findChar(uint8_t code, uint8_t size, uint8_t *cBuf)
// Find a character either in user defined list or from foint table
{
	charDef	*pcd = _userChars;
	uint8_t	len;

	PRINTX("\nfindUserChar 0x", code);
	while (pcd != NULL)
	{
		PRINTX(" ", pcd->code);
		if (pcd->code == code)	// found it
		{
			PRINTS(" found character");
			len = min(size, pcd->data[0]);
			memcpy(cBuf, &pcd->data[1], len);
			return(len);
		}
		pcd = pcd->next;
	}

	PRINTS(" no user char");
	_MX->setFont(_fontDef);		// change to the font for this zone
	len = _MX->getChar(code, size, cBuf);

	return(len);
}

uint8_t MD_PZone::makeChar(char c)
// Load a character bitmap and add in trailing char spacing blanks
{
	uint8_t	len;

	PRINTX("\nmakeChar 0x", c);

	// look for the character
	len = findChar((uint8_t)c, ARRAY_SIZE(_cBuf), _cBuf);

	// Add in the inter char spacing
	for (uint8_t i = 0; i<_charSpacing; i++)
	{
		if (len < ARRAY_SIZE(_cBuf))
		_cBuf[len++] = 0;
	}

	PRINT(", len=", len);

	return(len);
}

uint8_t MD_PZone::reverseBuf(uint8_t *p, uint8_t size)
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

void MD_PZone::moveTextPointer(void)
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

uint8_t MD_PZone::getFirstChar(void)
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

uint8_t MD_PZone::getNextChar(void)
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

bool MD_PZone::zoneAnimate(void)
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
				zoneClear();
				_moveIn = true;
			// fall through to process the effect, first call will be with INITIALISE

			default: // All state except END are handled by the special effect functions
			switch (_moveIn ? _effectIn : _effectOut)
			{
				case PRINT:				effectPrint(_moveIn);			break;
				case SLICE:				effectSlice(_moveIn);			break;
				case WIPE:				effectWipe(false, _moveIn);		break;
				case WIPE_CURSOR:		effectWipe(true, _moveIn);		break;
				case OPENING:			effectOpen(false, _moveIn);		break;
				case OPENING_CURSOR:	effectOpen(true, _moveIn);		break;
				case CLOSING:			effectClose(false, _moveIn);	break;
				case CLOSING_CURSOR:	effectClose(true, _moveIn);		break;
				case BLINDS:			effectBlinds(_moveIn);			break;
				case DISSOLVE:			effectDissolve(_moveIn);		break;
				case SCAN_HORIZ:		effectHScan(_moveIn);			break;
				case SCAN_VERT:			effectVScan(_moveIn);			break;
				case GROW_UP:			effectGrow(true, _moveIn);		break;
				case GROW_DOWN:			effectGrow(false, _moveIn);		break;
				case SCROLL_UP:			effectVScroll(true, _moveIn);	break;
				case SCROLL_DOWN:		effectVScroll(false, _moveIn);	break;
				case SCROLL_LEFT:		effectHScroll(true, _moveIn);	break;
				case SCROLL_RIGHT:		effectHScroll(false, _moveIn);	break;
				case SCROLL_UP_LEFT:	effectDiag(true, true, _moveIn);	break;
				case SCROLL_UP_RIGHT:	effectDiag(true, false, _moveIn);	break;
				case SCROLL_DOWN_LEFT:	effectDiag(false, true, _moveIn);	break;
				case SCROLL_DOWN_RIGHT:	effectDiag(false, false, _moveIn);	break;
				default:
				_fsmState = END;
			}

			// one way toggle for input to output, reset on initialize
			_moveIn = _moveIn && !(_fsmState == PAUSE);
			break;
		}
	}

	TIME_PROFILE("\nAnimation time ");
	TIME_PROFILE(": Cycle time ");

	return(_fsmState == END);
}
