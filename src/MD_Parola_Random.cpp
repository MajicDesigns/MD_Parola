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
 * \brief Implements random effect
 */

void MD_PZone::effectRandom(bool bIn)
// Text enters as random dots
{
  static uint8_t pix[COL_SIZE];  // pixel data (one byte per column) to mask the display
  uint8_t c, r;   // the row and column coordinates being considered

  if (bIn)  // incoming
  {
    switch (_fsmState)
    {
    case INITIALISE:
      PRINT_STATE("I RAND");
      for (uint8_t i = 0; i < COL_SIZE; i++)
        pix[i] = 0;

      _fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      PRINT_STATE("I RAND");

      // set up a new display
      commonPrint();

      // Work out and set the next random pixel in the column mask.
      // Use a loop counter to make sure we just don't loop forever.
      _nextPos = 0;
      do
      {
        r = random(COL_SIZE);
        c = random(ROW_SIZE);
        _nextPos++;
      } while (pix[c] & (1 << r) || _nextPos >= 1000);

      pix[c] |= (1 << r); // set the r,c location in the mask

      // now mask each column by the pixel mask - this repeats every COL_SIZE columns, but the 
      // characters don't occupy every pixel so the effect looks 'random'.
      _nextPos = 0;
      for (uint8_t c = ZONE_START_COL(_zoneStart); c <= ZONE_END_COL(_zoneEnd); c++)
      {
        uint8_t col = _MX->getColumn(c);

        col &= pix[_nextPos++];
        _MX->setColumn(c, col);
        if (_nextPos == COL_SIZE) _nextPos = 0;
      }

      // check if we have finished. This is when all the columns are 0xFF (all pixels on)
      {
        bool bEnd = true;
        for (uint8_t i = 0; bEnd && i < COL_SIZE; i++)
          bEnd = (pix[i] == 0xff);
        if (bEnd) _fsmState = PAUSE;
      }
      break;

    default:
      PRINT_STATE("I RAND");
      _fsmState = PAUSE;
    }
  }
  else  // exiting
  {
    switch (_fsmState)
    {
    case PAUSE:
    case INITIALISE:
      PRINT_STATE("O RAND");
      for (uint8_t i = 0; i < COL_SIZE; i++)
        pix[i] = 0;

      _fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      PRINT_STATE("O RAND");

      // set up a new display
      commonPrint();

      // work out and set the next random pixel in the column mask
      do
      {
        r = random(COL_SIZE);
        c = random(ROW_SIZE);
      } while (pix[c] & (1 << r));

      pix[c] |= (1 << r); // set the r,c location in the mask

      // now mask each column by the pixel mask - this repeats every COL_SIZE columns, but the 
      // characters don't occupy every pixel so the effect looks 'random'.
      _nextPos = 0;
      for (uint8_t c = ZONE_START_COL(_zoneStart); c <= ZONE_END_COL(_zoneEnd); c++)
      {
        uint8_t col = _MX->getColumn(c);

        col &= ~pix[_nextPos++];
        _MX->setColumn(c, col);
        if (_nextPos == COL_SIZE) _nextPos = 0;
      }

      // check if we have finished. This is when all the columns are 0xFF (all pixels on)
      {
        bool bEnd = true;
        for (uint8_t i = 0; bEnd && i < COL_SIZE; i++)
          bEnd = (pix[i] == 0xff);
        if (bEnd) _fsmState = END;
      }
      break;

    default:
      PRINT_STATE("O RAND");
      _fsmState = END;
      break;
    }
  }
}
