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
 * \brief Implements vertical scroll effect
 */

void MD_Parola::effectVScroll(textEffect_t effectSelect, bool bIn)
// Scroll the display horizontally up of down, depending on the selected effect
{
	static uint8_t	bitOffset = 0;

	if (bIn)	// incoming
	{
		switch (_fsmState)
		{
		case INITIALISE:
			PRINT_STATE("I VSCROLL");
			bitOffset = 0;
			_D.control(MD_MAX72XX::WRAPAROUND, MD_MAX72XX::OFF);
			_fsmState = PUT_CHAR;
			// fall through to next state

		case GET_FIRST_CHAR:
		case GET_NEXT_CHAR:
		case PUT_CHAR:
		case PAUSE:
			PRINT_STATE("I VSCROLL");

			displayClear();
			commonPrint();

			for (uint8_t i=bitOffset; i<7; i++)
				// scroll the whole display so that the message appears to be animated
				// Note: Directions are reversed because we start with the message in the 
				// middle position thru commonprint() and to see it animated move DOWN we 
				// need to scroll it UP, and vice versa.
				_D.transform(effectSelect == SCROLL_UP ? MD_MAX72XX::TSD : MD_MAX72XX::TSU);

			// check if we have finished
			if (bitOffset == 7) _fsmState = PAUSE;

			bitOffset++;
			break;

		default:
			PRINT_STATE("I VSCROLL");
			_fsmState = PAUSE;
		}
	}
	else	// exiting
	{
		switch (_fsmState)
		{
		case PAUSE:
		case INITIALISE:
			PRINT_STATE("O VSCROLL");
			bitOffset = 0;
			_fsmState = PUT_CHAR;
			// fall through to next state

		case GET_FIRST_CHAR:
		case GET_NEXT_CHAR:
		case PUT_CHAR:
			PRINT_STATE("O VSCROLL");

			_D.transform(effectSelect == SCROLL_UP ? MD_MAX72XX::TSU : MD_MAX72XX::TSD);

			// check if we have finished
			if (bitOffset == 7) _fsmState = END;

			bitOffset++;
			break;

		default:
			PRINT_STATE("O VSCROLL");
			_fsmState = END;
			break;
		}
	}
}
