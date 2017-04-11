// Program to demonstrate the MD_Parola library
//
// Display messages in the zones. Wait for each zone to display before continuing
//
// Animation speed can be controlled using a pot on pin SPEED_IN
//
// NOTE: MD_MAX72xx library must be installed and configured for the LED
// matrix type being used. Refer documentation included in the MD_MAX72xx
// library or see this link:
// https://majicdesigns.github.io/MD_MAX72XX/page_hardware.html
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define MAX_DEVICES 8
#define MAX_ZONES   4
#define ZONE_SIZE (MAX_DEVICES/MAX_ZONES)   // integer multiple works best

#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// Hardware SPI connection
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define SPEED_TIME  25
#define PAUSE_TIME  1000

// Turn on debug statements to the serial output
#define DEBUG  0

#if  DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTX(x) Serial.println(x, HEX)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

// Global variables
uint8_t  curText;
char	*pc[] =
{
  "M1",
  "M2",
  "M3",
  "M4",
  "M5",
};


uint8_t curFX = 0;
textEffect_t  effect[] =
{
  PA_PRINT,
  PA_SCAN_HORIZ,
  PA_SCROLL_LEFT,
  PA_WIPE,
  PA_RANDOM,
  PA_SCROLL_UP_LEFT,
  PA_SCROLL_UP,
  PA_FADE,
  PA_OPENING_CURSOR,
  PA_GROW_UP,
  PA_SCROLL_UP_RIGHT,
  PA_BLINDS,
  PA_MESH,
  PA_CLOSING,
  PA_GROW_DOWN,
  PA_SCAN_VERT,
  PA_SCROLL_DOWN_LEFT,
  PA_WIPE_CURSOR,
  PA_DISSOLVE,
  PA_OPENING,
  PA_CLOSING_CURSOR,
  PA_SCROLL_DOWN_RIGHT,
  PA_SCROLL_RIGHT,
  PA_SLICE,
  PA_SCROLL_DOWN,
};

void setup(void)
{
#if DEBUG
  Serial.begin(57600);
  PRINTS("[Parola Zone Mesg Demo]");
#endif

  P.begin(MAX_ZONES);
  P.setInvert(false);

  for (uint8_t i=0; i<MAX_ZONES; i++)
  {
    P.setZone(i, ZONE_SIZE*i, (ZONE_SIZE*(i+1))-1);
    PRINT("\nZ", i);
    PRINT(" from ", ZONE_SIZE*i);
    PRINT(" to ", (ZONE_SIZE*(i+1))-1);
  }
}

void loop(void)
{
  static uint8_t  curZone = 0;
  uint8_t inFX = ++curFX % ARRAY_SIZE(effect);
  uint8_t outFX = ++curFX % ARRAY_SIZE(effect);

  PRINT("\nNew Z", curZone);
  PRINT(": ", pc[curText]);
  PRINT(" @ ", millis());
  P.displayZoneText(curZone, pc[curText], PA_CENTER, SPEED_TIME, PAUSE_TIME, effect[inFX], effect[outFX]);

  // Check for individual zone completion. Note that we check the status of the zone rather than use the
  // return status of the displayAnimate() method as the other unused zones are completed, but we
  // still need to call it to run the animations.
  while (!P.getZoneStatus(curZone))
    P.displayAnimate();

  // increment for next time
  curText = ++curText % ARRAY_SIZE(pc);
  curZone = ++curZone % MAX_ZONES;
}
