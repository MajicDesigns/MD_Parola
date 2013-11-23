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
 * \brief Implements slice effect
 */

void MD_Parola::effectSlice(bool bIn)
{
	static int16_t	nextColumn, animColumn;

	if (bIn)
	{
		switch(_fsmState)
		{
		case INITIALISE:
		case GET_FIRST_CHAR:
			PRINT_STATE("I SLICE");

			if ((_charCols = getFirstChar()) == 0)
			{
				_fsmState = END;
				break;
			}
			displayClear();
			_countCols = 0;
			animColumn = 0;
			nextColumn = _limitLeft;

			_fsmState = PUT_CHAR;
			break;

		case GET_NEXT_CHAR:	// Load the next character from the font table
			PRINT_STATE("I SLICE");
			// have we reached the end of the characters string?
			if ((_charCols = getNextChar()) == 0)
			{
				_fsmState = PAUSE;
				break;
			}
			_countCols = 0;
			_fsmState = PUT_CHAR;
			// !! fall through to next state to start displaying

		case PUT_CHAR:	// display the next part of the character
			PRINT_STATE("I SLICE");
			FSMPRINT(" - Next ", nextColumn);
			FSMPRINT(", anim ", animColumn);

			if (_cBuf[_countCols] == 0)
			{
				animColumn = nextColumn;	// pretend we just animated it!
			}
			else
			{
				// clear the column and animate the next one
				if (animColumn != nextColumn) _D.setColumn(animColumn, EMPTY_BAR);
				animColumn++;
				_D.setColumn(animColumn, DATA_BAR(_cBuf[_countCols]));
			}

			// set up for the next time
			if (animColumn == nextColumn) 
			{
				animColumn = 0;
				_countCols++;
				nextColumn--;
			}
			if (_countCols == _charCols) _fsmState = GET_NEXT_CHAR;
			break;

		default:
			_fsmState = PAUSE;
	  }
	}
	else	// exiting
	{
		switch(_fsmState)
		{
		case PAUSE:
			PRINT_STATE("O SLICE");
			animColumn = nextColumn = _limitLeft;
			_fsmState = PUT_CHAR;
			// fall through

		case GET_FIRST_CHAR:
		case GET_NEXT_CHAR:
		case PUT_CHAR:
			PRINT_STATE("O SLICE");
			FSMPRINT(" - Next ", nextColumn);
			FSMPRINT(", anim ", animColumn);

			if (_D.getColumn(animColumn) == EMPTY_BAR)
			{
				animColumn = _D.getColumnCount();	// pretend we just animated it!
			}
			else
			{
				// Move the column over
				_D.setColumn(animColumn+1, _D.getColumn(animColumn));
				_D.setColumn(animColumn, EMPTY_BAR);
				animColumn++;
			}

			// set up for the next time
			if (animColumn == _D.getColumnCount()) animColumn = nextColumn--;

			if (nextColumn < _limitRight) _fsmState = END;	//reached the end
			break;

		default:
			_fsmState = END;
		}
	}
}
