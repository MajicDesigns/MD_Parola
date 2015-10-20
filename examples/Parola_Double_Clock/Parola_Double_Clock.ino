// Program to demonstrate the MD_Parola library
//
// Display the time in a double height display with a fixed width font.
// - Time is shown in a user defined seven segment font
// - Optional use of DS1307 module for time
//   - DS1307 library (MD_DS1307) found at http://arduinocode.codeplex.com
//

// Use the DS1307 clock module
#define	USE_DS1307	0

// Header file includes
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#if USE_LIBRARY_SPI
#include <SPI.h>
#endif
#include "Font_Data.h"

#if USE_GENERIC_HW || USE_PAROLA_HW
#define INVERT_UPPER_ZONE
#endif

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may 
// need to be adapted
#define MAX_ZONES 2
#define ZONE_SIZE 7
#define	MAX_DEVICES	(MAX_ZONES * ZONE_SIZE)

#define ZONE_UPPER  1
#define ZONE_LOWER  0

#define	CLK_PIN		13
#define	DATA_PIN	11
#define	CS_PIN		10

#if	USE_DS1307
#include <MD_DS1307.h>
#include <Wire.h>
#endif

// Hardware SPI connection
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define	SPEED_TIME	75
#define	PAUSE_TIME	0

#define	MAX_MESG	6

// Turn on debug statements to the serial output
#define  DEBUG  0

// Global variables
char	szTimeL[MAX_MESG];		// mm:ss\0
char  szTimeH[MAX_MESG];

void getTime(char *psz, bool f = true)
// Code for reading clock time
// Simulated clock runs 1 minute every seond
{
#if	USE_DS1307
  RTC.readTime();
  sprintf(psz, "%02d%c%02d", RTC.h, (f ? ':' : ' '), RTC.m);
#else
  uint16_t	h, m, s;
	
  m = millis()/1000;
  h = (m/60) % 24;
  m %= 60;
  sprintf(psz, "%02d%c%02d", h, (f ? ':' : ' '), m);
#endif
}

void createHString(char *pH, char *pL)
{
  for (; *pL != '\0'; pL++)
    *pH++ = *pL | 0x80;   // offset character

  *pH = '\0'; // terminate the string
}

void setup(void)
{
  // initialise the LED display
  P.begin(MAX_ZONES);

  // Set up zones for 2 halves of the display
  P.setZone(ZONE_LOWER, 0, ZONE_SIZE - 1);
  P.setZone(ZONE_UPPER, ZONE_SIZE, MAX_DEVICES - 1);
  P.setFont(numeric7SegDouble);

  P.setCharSpacing(P.getCharSpacing() * 2); // double height --> double spacing

#ifdef INVERT_UPPER_ZONE
  P.setZoneEffect(ZONE_UPPER, true, FLIP_UD);
  P.setZoneEffect(ZONE_UPPER, true, FLIP_LR);

  P.displayZoneText(ZONE_LOWER, szTimeL, RIGHT, SPEED_TIME, PAUSE_TIME, PRINT, NO_EFFECT);
  P.displayZoneText(ZONE_UPPER, szTimeH, LEFT, SPEED_TIME, PAUSE_TIME, PRINT, NO_EFFECT);
#else
  P.displayZoneText(ZONE_LOWER, szTimeL, CENTER, SPEED_TIME, PAUSE_TIME, PRINT, NO_EFFECT);
  P.displayZoneText(ZONE_UPPER, szTimeH, CENTER, SPEED_TIME, PAUSE_TIME, PRINT, NO_EFFECT);
#endif

#if USE_DS1307
  RTC.control(DS1307_CLOCK_HALT, DS1307_OFF);
  RTC.control(DS1307_12H, DS1307_OFF);
#endif
}

void loop(void)
{
  static uint32_t	lastTime = 0;		// millis() memory
  static bool	flasher = false;	// seconds passing flasher

  // Adjust the time string if we have to. It will be adjusted
  // every second at least for the flashing colon separator.
  if (millis() - lastTime >= 1000)
  {
    lastTime = millis();
    getTime(szTimeL, flasher);
    createHString(szTimeH, szTimeL);
    flasher = !flasher;

    P.displayReset(ZONE_LOWER);
    P.displayReset(ZONE_UPPER);
  }

  // synchronise the start and run the display to completion
  P.synchZoneStart();
  while (!P.getZoneStatus(ZONE_LOWER) || !P.getZoneStatus(ZONE_UPPER))
    P.displayAnimate();
}
