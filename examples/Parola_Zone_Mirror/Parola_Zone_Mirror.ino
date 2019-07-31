// Program to demonstrate the MD_Parola library
//
// Iterate through all combinations of entry and exit effects
// in 2 zones - one in normal mode and the second in inverted
// mirrored mode.
//
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 11
#define NUM_ZONES 2

#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define SPEED_TIME  25
#define PAUSE_TIME  1000

// Turn on debug statements to the serial output
#define  DEBUG  0

#if  DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTX(x) Serial.println(x, HEX)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

// Global data
typedef struct
{
  textEffect_t effect; // text effect to display
  const char   *psz;   // text string nul terminated
  uint16_t     speed;  // speed multiplier of library default
  uint16_t     pause;  // pause multiplier for library default
} sCatalog;

sCatalog catalog[] =
{
  { PA_PRINT, "PRINT", 1, 1 },
//  { PA_SLICE, "SLICE", 1, 1 },
  { PA_MESH, "MESH", 20, 1 },
  { PA_WIPE, "WIPE", 3, 1 },
  { PA_WIPE_CURSOR, "WPE_C", 4, 1 },
  { PA_RANDOM, "RAND", 3, 1 },
  { PA_OPENING, "OPEN", 3, 1 },
  { PA_OPENING_CURSOR, "OPN_C", 4, 1 },
  { PA_CLOSING, "CLOSE", 3, 1 },
  { PA_CLOSING_CURSOR, "CLS_C", 4, 1 },
  { PA_BLINDS, "BLIND", 7, 1 },
  { PA_DISSOLVE, "DSLVE", 7, 1 },
  { PA_SCROLL_UP, "SC_U", 5, 1 },
  { PA_SCROLL_DOWN, "SC_D", 5, 1 },
//  { PA_SCROLL_LEFT, "SC_L",  5, 1 },
//  { PA_SCROLL_RIGHT, "SC_R",  5, 1 },
  { PA_SCROLL_UP_LEFT, "SC_UL", 7, 1 },
  { PA_SCROLL_UP_RIGHT, "SC_UR", 7, 1 },
  { PA_SCROLL_DOWN_LEFT, "SC_DL", 7, 1 },
  { PA_SCROLL_DOWN_RIGHT, "SC_DR", 7, 1 },
  { PA_SCAN_HORIZ, "SCNH", 4, 1 },
  { PA_SCAN_HORIZX, "SCNHX", 4, 1 },
  { PA_SCAN_VERT, "SCNV", 3, 1 },
  { PA_SCAN_VERTX, "SCNVX", 3, 1 },
  { PA_GROW_UP, "GRW_U", 7, 1 },
  { PA_GROW_DOWN, "GRW_D", 7, 1 },
};

void setup(void)
{
#if DEBUG
  Serial.begin(57600);
  PRINTS("[Parola Zone Mirror]");
#endif

  P.begin(NUM_ZONES);
  P.setInvert(false);

  for (uint8_t i=0; i<ARRAY_SIZE(catalog); i++)
  {
    catalog[i].speed *= P.getSpeed();     // use the library defaults as multiplier
    catalog[i].pause *= 500;
  }

  P.setZone(0, 0, (MAX_DEVICES/2) - 1);
  P.setZone(1, MAX_DEVICES/2, MAX_DEVICES-1);

  P.setZoneEffect(1, true, PA_FLIP_UD);
  P.setZoneEffect(1, true, PA_FLIP_LR);
}

void loop(void)
{
  static uint8_t  nCurIdx = 0;

  // animates and returns true when an animation is completed. These are synchronised, so assume they are all done.
  if (P.displayAnimate())
  {
    for (uint8_t z=0; z<NUM_ZONES; z++)
      P.displayZoneText(z, catalog[nCurIdx].psz, PA_CENTER, catalog[nCurIdx].speed, catalog[nCurIdx].pause, catalog[nCurIdx].effect, catalog[nCurIdx].effect);

    nCurIdx = (nCurIdx + 1) % ARRAY_SIZE(catalog);
  }
}
