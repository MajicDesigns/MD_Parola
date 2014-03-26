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
_D(dataPin, clkPin, csPin, numDevices), _numModules(numDevices)
{
}

MD_Parola::MD_Parola(uint8_t csPin, uint8_t numDevices): 
_D(csPin, numDevices), _numModules(numDevices)
{
}

void MD_Parola::begin(uint8_t numZones)
{
	_D.begin();
	// Set up the MAX72XX library
	// The MAX72XX is in power-saving mode on startup,
	// we have to do a wakeup call, set the brightness, enable updates and clear the display
	_D.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY/2);
	_D.control(MD_MAX72XX::SHUTDOWN, MD_MAX72XX::OFF);
	_D.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
	
	// Create the zone objects
	_numZones = numZones;
	_Z = new MD_PZone[_numZones];
	if (_numZones == 1)
		setZone(0, 0, _numModules-1);

	for (uint8_t i=0; i<_numZones; i++)
		_Z[i].begin(&_D);
		
	// initialise options for all zones
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
	// release the zone array
	delete [] _Z;
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

void MD_Parola::displayZoneText(uint8_t z, char *pText, textPosition_t align, uint16_t speed, uint16_t pause, textEffect_t effectIn, textEffect_t effectOut)
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

	for (uint8_t i=0; i<_numZones; i++)
		b |= _Z[i].zoneAnimate();

	// re-enable and update the display
	_D.update(MD_MAX72XX::ON);
		
	return(b);
}

