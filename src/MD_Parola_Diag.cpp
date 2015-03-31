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
 * \brief Implements diagonal scroll effect
 */

void MD_PZone::effectDiag(bool bUp, bool bLeft, bool bIn)
// Scroll the display diagonally up or down, left or right, depending on the selected effect
{
	if (bIn)	// incoming
	{
		switch (_fsmState)
		{
		case INITIALISE:
			PRINT_STATE("I DIAG");
			_nextPos = 0;		// the position in the animation
			_MX->control(_zoneStart, _zoneEnd, MD_MAX72XX::WRAPAROUND, MD_MAX72XX::OFF);
			_fsmState = PUT_CHAR;
			// fall through to next state

		case GET_FIRST_CHAR:
		case GET_NEXT_CHAR:
		case PUT_CHAR:
		case PAUSE:
			PRINT_STATE("I DIAG");

			zoneClear();
			commonPrint();

			for (uint8_t i = _nextPos; i < 7; i++)
			{
				// scroll the whole display so that the message appears to be animated
				// Note: Directions are reversed because we start with the message in the 
				// middle position thru commonPrint() and to see it animated move DOWN we 
				// need to scroll it UP, and vice versa.
				_MX->transform(_zoneStart, _zoneEnd, bUp ? MD_MAX72XX::TSD : MD_MAX72XX::TSU);
				_MX->transform(_zoneStart, _zoneEnd, bLeft ? MD_MAX72XX::TSR : MD_MAX72XX::TSL);
			}

			// check if we have finished
			if (_nextPos == 7) _fsmState = PAUSE;

			_nextPos++;
			break;

		default:
			PRINT_STATE("I DIAG");
			_fsmState = PAUSE;
		}
	}
	else	// exiting
	{
		switch (_fsmState)
		{
		case PAUSE:
		case INITIALISE:
			PRINT_STATE("O DIAG");
			_nextPos = 0;
			_fsmState = PUT_CHAR;
			// fall through to next state

		case GET_FIRST_CHAR:
		case GET_NEXT_CHAR:
		case PUT_CHAR:
			PRINT_STATE("O DIAG");

			_MX->transform(_zoneStart, _zoneEnd, bUp ? MD_MAX72XX::TSU : MD_MAX72XX::TSD);
			_MX->transform(_zoneStart, _zoneEnd, bLeft ? MD_MAX72XX::TSL : MD_MAX72XX::TSR);

			// check if we have finished
			if (_nextPos == 7) _fsmState = END;

			_nextPos++;
			break;

		default:
			PRINT_STATE("O DIAG");
			_fsmState = END;
			break;
		}
	}
}
