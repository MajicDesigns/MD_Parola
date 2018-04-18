/*
MD_Parola - Library for modular scrolling text and Effects

See header file for comments

Copyright (C) 2018 Marco Colli. All rights reserved.

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
 * \brief Implements various sprite effects
 */

#if ENA_SPRITE

void MD_PZone::setupSprite(bool bIn, uint8_t id)
{
  const uint8_t F_ROCKET = 2;
  const uint8_t W_ROCKET = 11;
  static const uint8_t PROGMEM rocket[F_ROCKET * W_ROCKET] =  // ROCKET
  {
    0x18, 0x24, 0x42, 0x81, 0x99, 0x18, 0x99, 0x18, 0xa5, 0x5a, 0x81,
    0x18, 0x24, 0x42, 0x81, 0x18, 0x99, 0x18, 0x99, 0x24, 0x42, 0x99,
  };

  const uint8_t F_FBALL = 2;
  const uint8_t W_FBALL = 11;
  static const uint8_t PROGMEM fireball[F_FBALL * W_FBALL] =  // FIREBALL
  {
    0x7e, 0xab, 0x54, 0x28, 0x52, 0x24, 0x40, 0x18, 0x04, 0x10, 0x08,
    0x7e, 0xd5, 0x2a, 0x14, 0x24, 0x0a, 0x30, 0x04, 0x28, 0x08, 0x10,
  };

  switch (id)
  {
  case PA_ROCKET:    
    if (bIn) { _spriteInData = rocket;  _spriteInWidth = W_ROCKET;  _spriteInFrames = F_ROCKET;  }
    else     { _spriteOutData = rocket; _spriteOutWidth = W_ROCKET; _spriteOutFrames = F_ROCKET; }
    break;

  case PA_FIREBALL:  
    if (bIn) { _spriteInData = fireball;  _spriteInWidth = W_FBALL; _spriteInFrames = F_FBALL;   }
    else     { _spriteOutData = fireball; _spriteOutWidth = W_FBALL; _spriteOutFrames = F_FBALL; }
    break;

  case PA_SPRITE:
    // nothing to do - assume the user has already set this up using setSpriteData()
    break;
  }
}

void MD_PZone::effectSprite(bool bIn, uint8_t id)
// Animated Pacman sprite leads or eats up the message.
// Print up the whole message and then remove the parts we
// don't need in order to do the animation.
{
  if (bIn)  // incoming - sprite moves left to right in the zone
  {
    switch (_fsmState)
    {
    case INITIALISE:
      PRINT_STATE("I SPRITE");
      setInitialEffectConditions();
      if (_startPos < _endPos)
      {
        int16_t t = _startPos;
        _startPos = _endPos;
        _endPos = t;
      }
      setupSprite(bIn, id);
      if (_spriteInData == nullptr)
      {
        _fsmState = END;
        break;
      }
      _posOffset = 0;   // current animation frame for the sprite
      _nextPos = ZONE_END_COL(_zoneEnd) + 1;
      _fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
    case PAUSE:
      PRINT_STATE("I SPRITE");

      commonPrint();

      // move reference column and draw new graphic
      _nextPos--;
      for (uint8_t i = 0; i < _spriteInWidth; i++)
      {
        if ((_nextPos + i) <= ZONE_END_COL(_zoneEnd) && (_nextPos + i) >= ZONE_START_COL(_zoneStart))
          _MX->setColumn(_nextPos + i, DATA_BAR(pgm_read_byte(_spriteInData + (_posOffset * _spriteInWidth) + i)));
      }

      // blank out the part of the display we don't need
      // this is the part to the right of the sprite
      for (int16_t i = _nextPos - 1; i >= _endPos; i--)
        _MX->setColumn(i, EMPTY_BAR);

      // advance the animation frame
      _posOffset++;
      if (_posOffset >= _spriteInFrames)
        _posOffset = 0;

      // check if we have finished
      if (_nextPos == ZONE_START_COL(_zoneStart) - _spriteInWidth - 1)
        _fsmState = PAUSE;
      break;

    default:
      PRINT_STATE("I SPRITE");
      _fsmState = PAUSE;
    }
  }
  else  // exiting - sprite moves left to right in the zone
  {
    switch (_fsmState)
    {
    case PAUSE:
    case INITIALISE:
      PRINT_STATE("O SPRITE");
      setInitialEffectConditions();
      if (_startPos < _endPos)
      {
        int16_t t = _startPos;
        _startPos = _endPos;
        _endPos = t;
      }
      setupSprite(bIn, id);
      if (_spriteOutData == nullptr)
      {
        _fsmState = END;
        break;
      }
      _nextPos = ZONE_START_COL(_zoneStart) - 1;
      _posOffset = 0;
      _fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      PRINT_STATE("O SPRITE");
      commonPrint();

      // move reference column and draw new graphic
      _nextPos++;
      for (uint8_t i = 0; i < _spriteOutWidth; i++)
      {
        if ((_nextPos - i) <= ZONE_END_COL(_zoneEnd) && (_nextPos - i) >= ZONE_START_COL(_zoneStart))
          _MX->setColumn(_nextPos - i, DATA_BAR(pgm_read_byte(_spriteOutData + (_posOffset * _spriteOutWidth) + i)));
      }

      // blank out the part of the display we don't need
      // this is the part to the right of the sprite
      for (int16_t i = _nextPos - _spriteOutWidth; i >= _endPos; i--)
        _MX->setColumn(i, EMPTY_BAR);

      // advance the animation frame
      _posOffset++;
      if (_posOffset >= _spriteOutFrames)
        _posOffset = 0;

      // check if we have finished
      if (_nextPos == ZONE_END_COL(_zoneEnd) + _spriteOutWidth + 1)
        _fsmState = END;
      break;

    default:
      PRINT_STATE("O SPRITE");
      _fsmState = END;
      break;
    }
  }
}

#endif