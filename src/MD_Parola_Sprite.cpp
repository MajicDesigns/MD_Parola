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


uint8_t *MD_PZone::setupSprite(uint8_t id, uint8_t &dataWidth, uint8_t &numFrames)
{
  const uint8_t F_PMAN1 = 4;
  const uint8_t W_PMAN1 = 8;
  static const uint8_t PROGMEM pacman1[F_PMAN1 * W_PMAN1] =  // gobbling pacman animation
  {
    0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c,
    0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c,
    0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c,
    0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  };

  const uint8_t F_PMAN2 = 4;
  const uint8_t W_PMAN2 = 18;
  static const uint8_t PROGMEM pacman2[F_PMAN2 * W_PMAN2] =  // ghost pursued by a pacman
  {
    0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
    0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
    0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
    0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  };

  const uint8_t F_ROCKET = 2;
  const uint8_t W_ROCKET = 11;
  static const uint8_t PROGMEM rocket[F_ROCKET * W_ROCKET] =  // Rocket
  {
    0x18, 0x24, 0x42, 0x81, 0x99, 0x18, 0x99, 0x18, 0xa5, 0x5a, 0x81,
    0x18, 0x24, 0x42, 0x81, 0x18, 0x99, 0x18, 0x99, 0x24, 0x42, 0x99,
  };

  uint8_t *p = nullptr;

  switch (id)
  {
  case SPR_PACMAN:        p = pacman1; dataWidth = W_PMAN1; numFrames = F_PMAN1; break;
  case SPR_PACMAN_GHOST:  p = pacman2; dataWidth = W_PMAN2; numFrames = F_PMAN2; break;
  case SPR_ROCKET:        p = rocket;  dataWidth = W_ROCKET; numFrames = F_ROCKET; break;
  }

  return(p);
}

void MD_PZone::effectSprite(bool bIn, uint8_t id)
// Animated Pacman sprite leads or eats up the message.
// Print up the whole message and then remove the parts we
// don't need in order to do the animation.
{
  static uint8_t frame;      // current animation frame for the sprite
  static uint8_t numFrames;  // the number of frames for this sprite
  static uint8_t dataWidth;  // the data width of the current sprite
  static uint8_t *sprite;    // pointer to the sprite data table

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
      if ((sprite = setupSprite(id, dataWidth, numFrames)) == nullptr)
      {
        _fsmState = END;
        break;
      }
      frame = 0;
      _posOffset = 1;
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
      for (uint8_t i = 0; i < dataWidth; i++)
      {
        if ((_nextPos + i) <= ZONE_END_COL(_zoneEnd) && (_nextPos + i) >= ZONE_START_COL(_zoneStart))
          _MX->setColumn(_nextPos + i, DATA_BAR(pgm_read_byte(sprite + (frame * dataWidth) + i)));
      }

      // blank out the part of the display we don't need
      // this is the part to the right of the sprite
      for (int16_t i = _nextPos - 1; i >= _endPos; i--)
        _MX->setColumn(i, EMPTY_BAR);

      // advance the animation frame
      frame += _posOffset;
      if (frame == 0 || frame >= numFrames - 1)
        _posOffset = -_posOffset;

      // check if we have finished
      if (_nextPos == ZONE_START_COL(_zoneStart) - dataWidth - 1)
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
      if ((sprite = setupSprite(id, dataWidth, numFrames)) == nullptr)
      {
        _fsmState = END;
        break;
      }
      _nextPos = ZONE_START_COL(_zoneStart) - 1;
      frame = 0;
      _posOffset = 1;
      _fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      PRINT_STATE("O SPRITE");
      commonPrint();

      // move reference column and draw new graphic
      _nextPos++;
      for (uint8_t i = 0; i < dataWidth; i++)
      {
        if ((_nextPos - i) <= ZONE_END_COL(_zoneEnd) && (_nextPos - i) >= ZONE_START_COL(_zoneStart))
          _MX->setColumn(_nextPos - i, DATA_BAR(pgm_read_byte(sprite + (frame * dataWidth) + i)));
      }

      // blank out the part of the display we don't need
      // this is the part to the right of the sprite
      for (int16_t i = _nextPos - dataWidth; i >= _endPos; i--)
        _MX->setColumn(i, EMPTY_BAR);

      // advance the animation frame
      frame += _posOffset;
      if (frame == 0 || frame >= numFrames - 1)
        _posOffset = -_posOffset;

      // check if we have finished
      if (_nextPos == ZONE_END_COL(_zoneEnd) + dataWidth + 1)
        _fsmState = END;
      break;

    default:
      PRINT_STATE("O SPRITE");
      _fsmState = END;
      break;
    }
  }
}
