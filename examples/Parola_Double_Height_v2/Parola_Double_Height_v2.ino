// Demonstrates one double height display using Parola using a single font file
// defintion created with the MD_MAX72xx font builder.
//
// Each font file has the lower part opf a character as ASCII codes 0-127 and the 
// upper part of the character in ASCII code 128-255. Adding 128 to each lower 
// character creates the correct index for the upper character.
// The upper and lower portions are managed as 2 zones 'stacked' on top of each other.
// Sending the original string to the lower zone and the modified (+128) string to the
// upper zone creates the complete message on the display.
// 
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#if USE_LIBRARY_SPI
#include <SPI.h>
#endif
#include "Font_Data.h"

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may 
// need to be adapted
#define MAX_ZONES 2
#define ZONE_SIZE 7
#define	MAX_DEVICES	(MAX_ZONES * ZONE_SIZE)

#define ZONE_UPPER  1
#define ZONE_LOWER  0

#define PAUSE_TIME  0
#define SCROLL_SPEED  50

#define	CLK_PIN		13
#define	DATA_PIN	11
#define	CS_PIN		10

// HARDWARE SPI
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
//MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
char *msgL[] = 
{
  "Managing double height displays with custom fonts and 2 zones",
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
  "abcdefghijklmnopqrstuvwxyz",
  "0123456789",
  "`!@#$%^&*()_+-={};:'<>\"?,./|\\{}",
};
char *msgH; // allocated memory in setup()

void setup(void)
{
  uint8_t max = 0;

  // work out the size of buffer required
  for (uint8_t i = 0; i<ARRAY_SIZE(msgL); i++)
    if (strlen(msgL[i]) > max) max = strlen(msgL[i]);

  msgH = (char *)malloc(sizeof(char)*(max + 2));

  // initialise the LED display
  P.begin(MAX_ZONES);

  // Set up zones for 2 halves of the display
  P.setZone(ZONE_LOWER, 0, ZONE_SIZE - 1);
  P.setZone(ZONE_UPPER, ZONE_SIZE, MAX_DEVICES-1);
  P.setFont(BigFont);

  P.setCharSpacing(P.getCharSpacing() * 2); // double height --> double spacing
}

void createHString(char *pH, char *pL)
{
  for (; *pL != '\0'; pL++)
    *pH++ = *pL | 0x80;   // offset character

  *pH = '\0'; // terminate the string
}

void loop(void)
{
  static uint8_t cycle = 0;

  // set up the string
  createHString(msgH, msgL[cycle]);
  P.displayZoneText(ZONE_LOWER, msgL[cycle], LEFT, SCROLL_SPEED, PAUSE_TIME, SCROLL_LEFT, SCROLL_LEFT);
  P.displayZoneText(ZONE_UPPER, msgH, LEFT, SCROLL_SPEED, PAUSE_TIME, SCROLL_LEFT, SCROLL_LEFT);

  // prepare for next pass
  cycle = (cycle + 1) % ARRAY_SIZE(msgL);

  // synchronise the start and run the display to completion
  P.synchZoneStart();
  while (!P.getZoneStatus(ZONE_LOWER) || !P.getZoneStatus(ZONE_UPPER))
    P.displayAnimate();
}
