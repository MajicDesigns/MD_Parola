// Demonstrates one way to create a double height display using Parola.
//
// Largely based on code shared by arduino.cc forum user Arek00, 26 Sep 2015.
// Video of running display can be seen at https://www.youtube.com/watch?v=7nPCIMVUo5g
//
// Key to the double height code is the definition of 2 fonts and 2 zones 'stacked' 
// on top of each other so that the modules numbers are in sequence like
//
//  n n-1 n-2 ... n/2+1   <- this direction top row
//  n/2 ... 3  2  1  0    <- this direction bottom row
//
// Each font displays letters for either top or bottom half of the message. Sending the 
// same string to both zones creates the complete message
// on the display.
// 
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "Font_Data.h"

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may 
// need to be adapted
#define MAX_ZONES 2
#define ZONE_SIZE 4
#define	MAX_DEVICES	(MAX_ZONES * ZONE_SIZE)

#define ZONE_UPPER  1
#define ZONE_LOWER  0

#define	CLK_PIN		13
#define	DATA_PIN	11
#define	CS_PIN		10

// HARDWARE SPI
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
//MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
char *msg[] = 
{
  "Create double height displays using 2 custom fonts and 2 zones",
  "Zone 0 for lower half",
  "Zone 1 for upper half",
  "BIG",
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
  "abcdefghijklmnopqrstuvwxyz",
  "0123456789",
  "`!@#$%^&*()_+-={};:'\"<>?,./|\\{}",
  "Download the Parola at github.com/MajicDesigns",
  "Watch the video on YouTube"
};

void setup(void)
{
  // initialise the LED display
  P.begin(MAX_ZONES);

  // Set up zones for 2 halves of the display
  // Each zone gets a different font, making up the top
  // and bottom half of each letter
  P.setZone(ZONE_LOWER, 0, ZONE_SIZE - 1);
  P.setFont(ZONE_LOWER, BigFontLower);

  P.setZone(ZONE_UPPER, ZONE_SIZE, MAX_DEVICES-1);
  P.setFont(ZONE_UPPER, BigFontUpper);
  P.setCharSpacing(P.getCharSpacing() * 2); // double height --> double spacing
}

void loop(void)
{
  static uint8_t cycle = 0;
  static bool doAnimate = false;

  if (doAnimate)
  {
    P.displayAnimate();
    doAnimate &= (!P.getZoneStatus(ZONE_LOWER) && !P.getZoneStatus(ZONE_UPPER));
  }
  else
  {
    switch (cycle)
    {
    case 0:
    default:
      P.setFont(ZONE_LOWER, BigFontLower);
      P.setFont(ZONE_UPPER, BigFontUpper);
      P.displayZoneText(ZONE_LOWER, msg[cycle], PA_CENTER, 30, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      P.displayZoneText(ZONE_UPPER, msg[cycle], PA_CENTER, 30, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      break;

    case 1:
      P.setFont(ZONE_LOWER, NULL);
      P.setFont(ZONE_UPPER, BigFontUpper);
      P.displayZoneText(ZONE_LOWER, msg[cycle], PA_CENTER, 30, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      P.displayZoneText(ZONE_UPPER, msg[4], PA_CENTER, 30, 0, PA_PRINT, PA_NO_EFFECT);
      break;

    case 2:
      P.setFont(ZONE_LOWER, BigFontLower);
      P.setFont(ZONE_UPPER, NULL);
      P.displayZoneText(ZONE_LOWER, msg[4], PA_CENTER, 30, 0, PA_PRINT, PA_NO_EFFECT);
      P.displayZoneText(ZONE_UPPER, msg[cycle], PA_CENTER, 30, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      break;

    case 3:
      P.setFont(ZONE_LOWER, BigFontLower);
      P.setFont(ZONE_UPPER, BigFontUpper);
      P.displayZoneText(ZONE_LOWER, msg[cycle], PA_CENTER, 30, 2000, PA_PRINT, PA_SCROLL_UP);
      P.displayZoneText(ZONE_UPPER, msg[cycle], PA_CENTER, 30, 2000, PA_PRINT, PA_SCROLL_DOWN);
      break;
    }

    // prepare for next pass
    cycle = (cycle + 1) % ARRAY_SIZE(msg);

    // synchronise the start
    P.synchZoneStart();
    doAnimate = true;
  }
}
