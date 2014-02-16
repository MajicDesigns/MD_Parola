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
/**
 * \file
 * \brief Implements scan effect
 */

void MD_Parola::effectHScan(bool bIn)
// Scan the message over with a new one
// Print up the whole message and then remove the parts we 
// don't need in order to do the animation.
{
	static int16_t	nextPos = 0;
	static int8_t	posOffset = -1;
	static uint16_t	startPos = 0;
	static uint16_t	posLimit = 0;

	if (bIn)	// incoming
	{
		switch (_fsmState)
		{
		case INITIALISE:
			PRINT_STATE("I SCANH");
			posOffset = (_textAlignment == RIGHT ? 1 : -1);
			startPos = nextPos = (_textAlignment == RIGHT ? _limitRight : _limitLeft);
			posLimit = (_textAlignment == RIGHT ? _limitLeft+1 : _limitRight);
			_fsmState = PUT_CHAR;
			// fall through to next state

		case GET_FIRST_CHAR:
		case GET_NEXT_CHAR:
		case PUT_CHAR:
		case PAUSE:
			PRINT_STATE("I SCANH");
			if (_fsmState == PAUSE)
				_fsmState = PUT_CHAR;

			commonPrint();
			// check if we have finished
			if (nextPos == posLimit)
			{
				_fsmState = PAUSE;
				break;
			}

			// blank out the part of the display we don't need
			FSMPRINT("Keep ", nextPos);
			for (uint8_t i=startPos; i != posLimit; i += posOffset)
			{
				if (i != nextPos)
					_D.setColumn(i, EMPTY_BAR);
			}

			nextPos += posOffset;	// for the next time around
			break;

		default:
			PRINT_STATE("I SCANH");
			_fsmState = PAUSE;
		}
	}
	else	// exiting
	{
		switch (_fsmState)
		{
		case PAUSE:
		case INITIALISE:
			PRINT_STATE("O SCANH");
			startPos = nextPos = (_textAlignment == RIGHT ? _limitRight : _limitLeft);
			posLimit = (_textAlignment == RIGHT ? _limitLeft+1 : _limitRight);
			posOffset = (_textAlignment == RIGHT ? 1 : -1);
			_fsmState = PUT_CHAR;
			// fall through to next state

		case GET_FIRST_CHAR:
		case GET_NEXT_CHAR:
		case PUT_CHAR:
			PRINT_STATE("O SCANH");
			commonPrint();

			// blank out the part of the display we don't need
			FSMPRINT(" Keep ", nextPos);
			for (uint8_t i=startPos; i != posLimit; i += posOffset)
			{
				if (i != nextPos)
					_D.setColumn(i, EMPTY_BAR);
			}

			// check if we have finished
			if (nextPos == posLimit) _fsmState = END;

			nextPos += posOffset;	// for the next time around
			break;

		default:
			PRINT_STATE("O SCANH");
			_fsmState = END;
			break;
		}
	}
}

void MD_Parola::effectVScan(bool bIn)
// Scan the message over with a new one
// Print up the whole message and then remove the parts we 
// don't need in order to do the animation.
{
	static int16_t	nextPos = 0;
	static int8_t	posOffset = -1;
	static uint16_t	startPos = 0;
	static uint16_t	posLimit = 0;
	uint8_t	maskCol = 0;

	if (bIn)	// incoming
	{
		switch (_fsmState)
		{
		case INITIALISE:
			PRINT_STATE("I SCANV");
			posOffset = (_textAlignment == RIGHT ? 1 : -1);
			startPos = (_textAlignment == RIGHT ? _limitRight : _limitLeft);
			posLimit = (_textAlignment == RIGHT ? _limitLeft+1 : _limitRight);
			_fsmState = PUT_CHAR;
			nextPos = 1;		// this is the bit number
			// fall through to next state

		case GET_FIRST_CHAR:
		case GET_NEXT_CHAR:
		case PUT_CHAR:
		case PAUSE:
			PRINT_STATE("I SCANV");
			if (_fsmState == PAUSE)
				_fsmState = PUT_CHAR;

			commonPrint();
			// check if we have finished
			if (nextPos == 8)		// bits numbered 0 to 7	
			{
				_fsmState = PAUSE;
				break;
			}

			// blank out the part of the display we don't need
			FSMPRINT("Keep bit ", nextPos);
			maskCol = (1 << nextPos);
			for (uint8_t i=startPos; i != posLimit; i += posOffset)
				_D.setColumn(i, _D.getColumn(i) & maskCol);

			nextPos++;	// for the next time around
			break;

		default:
			PRINT_STATE("I SCANV");
			_fsmState = PAUSE;
		}
	}
	else	// exiting
	{
		switch (_fsmState)
		{
		case PAUSE:
		case INITIALISE:
			PRINT_STATE("O SCANV");
			startPos = (_textAlignment == RIGHT ? _limitRight : _limitLeft);
			posLimit = (_textAlignment == RIGHT ? _limitLeft+1 : _limitRight);
			posOffset = (_textAlignment == RIGHT ? 1 : -1);
			nextPos = 7;	// the bit number
			_fsmState = PUT_CHAR;
			// fall through to next state

		case GET_FIRST_CHAR:
		case GET_NEXT_CHAR:
		case PUT_CHAR:
			PRINT_STATE("O SCANV");
			commonPrint();

			// blank out the part of the display we don't need
			FSMPRINT(" Keep bit ", nextPos);
			maskCol = (1 << nextPos);
			for (uint8_t i=startPos; i != posLimit; i += posOffset)
				_D.setColumn(i, _D.getColumn(i) & maskCol);

			// check if we have finished
			if (nextPos == 0) _fsmState = END;

			nextPos--;	// for the next time around
			break;

		default:
			PRINT_STATE("O SCANV");
			_fsmState = END;
			break;
		}
	}
}