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

#define STATIC_ZONES 0

MD_Parola::MD_Parola(MD_MAX72XX::moduleType_t mod, uint8_t dataPin, uint8_t clkPin, uint8_t csPin, uint8_t numDevices):
_D(mod, dataPin, clkPin, csPin, numDevices), _numModules(numDevices)
{
}

MD_Parola::MD_Parola(MD_MAX72XX::moduleType_t mod, uint8_t csPin, uint8_t numDevices):
_D(mod, csPin, numDevices), _numModules(numDevices)
{
}

void MD_Parola::begin(uint8_t numZones)
{
  _D.begin();

  // Check boundaries for the number of zones
  if (numZones == 0) numZones = 1;
#if STATIC_ZONES
  if (numZones > MAX_ZONES) numZones = MAX_ZONES;  // static zones
#endif
  _numZones = numZones;

#if !STATIC_ZONES
  // Create the zone objects array
  _Z = new MD_PZone[_numZones];   // for dynamic zones
#endif

  for (uint8_t i = 0; i < _numZones; i++)
    _Z[i].begin(&_D);

  // for one zone automatically make it all modules, user will override if not intended
  if (_numZones == 1)
    setZone(0, 0, _numModules - 1);

  // initialise zone-independent options
  setSpeed(10);
  setPause(10 * getSpeed());
  setCharSpacing(1);
  setScrollSpacing(0);
  setTextAlignment(PA_LEFT);
  setTextEffect(PA_PRINT, PA_NO_EFFECT);
  setTextBuffer(nullptr);
  setInvert(false);
  setIntensity(MAX_INTENSITY / 2);

  // Now set the default viewing parameters for this library
  _D.setFont(nullptr);
}

MD_Parola::~MD_Parola(void)
{
#if !STATIC_ZONES
  // release the zone array (dynamically allocated)
  delete [] _Z;
#endif
}

bool MD_Parola::setZone(uint8_t z, uint8_t moduleStart, uint8_t moduleEnd)
{
  if ((moduleStart <= moduleEnd) && (moduleEnd < _numModules) && (z < _numZones))
  {
    _Z[z].setZone(moduleStart, moduleEnd);
    return(true);
  }

  return(false);
}

void MD_Parola::displayZoneText(uint8_t z, const char *pText, textPosition_t align, uint16_t speed, uint16_t pause, textEffect_t effectIn, textEffect_t effectOut)
{
  setTextBuffer(z, pText);
  setTextAlignment(z, align);
  setSpeed(z, speed);
  setPause(z, pause);
  setTextEffect(z, effectIn, effectOut);

  displayReset(z);
}

bool MD_Parola::displayAnimate(void)
{
  bool b = false;

  // suspend the display while we animate a frame
  _D.update(MD_MAX72XX::OFF);

  for (uint8_t i = 0; i < _numZones; i++)
    b |= _Z[i].zoneAnimate();

  // re-enable and update the display
  _D.update(MD_MAX72XX::ON);

  return(b);
}

size_t MD_Parola::write(const char *str)
{
  displayText((char *)str, getTextAlignment(), 0, 0, PA_PRINT, PA_NO_EFFECT);
  while (displayAnimate())
    /* do nothing */;

  return(strlen(str));
}

size_t MD_Parola::write(const uint8_t *buffer, size_t size)
{
  char *psz = (char *)malloc(sizeof(char) * (size + 1));

  if (psz == nullptr) return(0);

  memcpy(psz, buffer, size);
  psz[size] = '\0';
  write(psz);
  free(psz);

  return(size);
}
