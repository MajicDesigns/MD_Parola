// Demonstrates one double height display using Parola using a single font file
// definition created with the MD_MAX72xx font builder.
//
// Each font file has the lower part of a character as ASCII codes 0-127 and the
// upper part of the character in ASCII code 128-255. Adding 128 to each lower
// character creates the correct index for the upper character.
// The upper and lower portions are managed as 2 zones 'stacked' on top of each other
// so that the module numbers are in the sequence shown below:
//
// * Modules (like FC-16) that can fit over each other with no gap
//  n n-1 n-2 ... n/2+1   <- this direction top row
//  n/2 ... 3  2  1  0    <- this direction bottom row
//
// * Modules (like Generic and Parola) that cannot fit over each other with no gap
//  n/2+1 ... n-2 n-1 n   -> this direction top row
//   n/2  ...  2   1  0   <- this direction bottom row
//
// Sending the original string to the lower zone and the modified (+128) string to the
// upper zone creates the complete message on the display.
//
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "Font_Data.h"

// Turn debugging on and off
#define DEBUG 0

#if DEBUG
#define PRINTS(s)    { Serial.print(F(s)); }
#define PRINT(s, v)  { Serial.print(F(s)); Serial.print(v); }
#define PRINTX(s, v) { Serial.print(F(s)); Serial.print(F("0x")); Serial.print(v, HEX); }
#else
#define PRINTS(s)
#define PRINT(s, v)
#define PRINTX(s, v)
#endif

// User interface pin and switch definitions
#define SPEED_IN  A5  // control the speed with an external pot

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers may not work with your hardware and may need changing
#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define NUM_ZONES 2
#define ZONE_SIZE 6
#define MAX_DEVICES (NUM_ZONES * ZONE_SIZE)

bool invertUpperZone = false;

#define PAUSE_TIME      1000
#define SPEED_DEADBAND  5 // in analog units

#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
//MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define ZONE_LOWER 0
#define ZONE_UPPER 1

// Alignment of the two zones
#ifdef INVERT_UPPER_ZONE
// if inverted, alignment should be opposite and
// CENTER may not work well depending on message
#define ALIGN_LOWER PA_LEFT
#define ALIGN_UPPER PA_RIGHT
#else
// if not inverted, should always be the same
#define ALIGN_LOWER PA_CENTER
#define ALIGN_UPPER ALIGN_LOWER
#endif

const char *msgL[] =
{
  "Abc",
  "123",
  "xyz"
};
char* msgH; // allocated memory in setup()

struct catalogItem_t
{
  bool fPause;
  textEffect_t zFX[NUM_ZONES];
};

const catalogItem_t *catalog;

const PROGMEM catalogItem_t catalogInvert[] =
{
  { true, { PA_PRINT, PA_PRINT } },
  { false, { PA_SCROLL_LEFT, PA_SCROLL_RIGHT } },
  { false, { PA_SCROLL_RIGHT, PA_SCROLL_LEFT } },
  { true, { PA_SCROLL_UP, PA_SCROLL_UP } },
  { true, { PA_SCROLL_DOWN, PA_SCROLL_DOWN } },
#if ENA_MISC
//  { true, { PA_SLICE, PA_SLICE } }, // looks wrong because of reversal
  { true, { PA_FADE, PA_FADE } },
  { true, { PA_MESH, PA_MESH } },
  { true, { PA_BLINDS, PA_BLINDS } },
  { true, { PA_DISSOLVE, PA_DISSOLVE } },
#endif
#if ENA_WIPE
  { true, { PA_WIPE, PA_WIPE } },
  { true, { PA_WIPE_CURSOR, PA_WIPE_CURSOR } },
#endif
#if ENA_OPNCLS
  { true, { PA_OPENING, PA_OPENING } },
  { true, { PA_OPENING_CURSOR, PA_OPENING_CURSOR } },
  { true, { PA_CLOSING, PA_CLOSING } },
  { true, { PA_CLOSING_CURSOR, PA_CLOSING_CURSOR } },
#endif
#if ENA_SCR_DIA
  { true, { PA_SCROLL_UP_LEFT, PA_SCROLL_UP_LEFT } },
  { true, { PA_SCROLL_UP_RIGHT, PA_SCROLL_UP_RIGHT } },
  { true, { PA_SCROLL_DOWN_LEFT, PA_SCROLL_DOWN_LEFT } },
  { true, { PA_SCROLL_DOWN_RIGHT, PA_SCROLL_DOWN_RIGHT } },
#endif
#if ENA_SCAN
  { true, { PA_SCAN_HORIZ, PA_SCAN_HORIZ } },
  { true, { PA_SCAN_VERT, PA_SCAN_VERT } },
#endif
#if ENA_GROW
  { true, { PA_GROW_UP, PA_GROW_UP } },
  { true, { PA_GROW_DOWN, PA_GROW_DOWN } },
#endif
};

const PROGMEM catalogItem_t catalogNoInvert[] =
{
  { true, { PA_PRINT, PA_PRINT } },
  { false, { PA_SCROLL_LEFT, PA_SCROLL_LEFT } },
  { false, { PA_SCROLL_RIGHT, PA_SCROLL_RIGHT } },
  { true, { PA_SCROLL_UP, PA_SCROLL_DOWN } },
  { true, { PA_SCROLL_DOWN, PA_SCROLL_UP } },
#if ENA_MISC
  { true, { PA_SLICE, PA_SLICE } },
  { true, { PA_FADE, PA_FADE } },
  { true, { PA_MESH, PA_MESH } },
  { true, { PA_BLINDS, PA_BLINDS } },
  { true, { PA_DISSOLVE, PA_DISSOLVE } },
#endif
#if ENA_WIPE
  { true, { PA_WIPE, PA_WIPE } },
  { true, { PA_WIPE_CURSOR, PA_WIPE_CURSOR } },
#endif
#if ENA_OPNCLS
  { true, { PA_OPENING, PA_OPENING } },
  { true, { PA_OPENING_CURSOR, PA_OPENING_CURSOR } },
  { true, { PA_CLOSING, PA_CLOSING } },
  { true, { PA_CLOSING_CURSOR, PA_CLOSING_CURSOR } },
#endif
#if ENA_SCR_DIA
  { true, { PA_SCROLL_UP_LEFT, PA_SCROLL_DOWN_RIGHT } },
  { true, { PA_SCROLL_UP_RIGHT, PA_SCROLL_DOWN_LEFT } },
  { true, { PA_SCROLL_DOWN_LEFT, PA_SCROLL_UP_RIGHT } },
  { true, { PA_SCROLL_DOWN_RIGHT, PA_SCROLL_UP_LEFT } },
#endif
#if ENA_SCAN
  { true, { PA_SCAN_HORIZ, PA_SCAN_HORIZ } },
  { true, { PA_SCAN_VERT, PA_SCAN_VERT } },
#endif
#if ENA_GROW
  { true, { PA_GROW_UP, PA_GROW_DOWN } },
  { true, { PA_GROW_DOWN, PA_GROW_UP } },
#endif
};

void doUI(void)
{
  // set the speed if it has changed
  int16_t	speed = map(analogRead(SPEED_IN), 0, 1023, 0, 100);

  if ((speed >= ((int16_t)P.getSpeed() + SPEED_DEADBAND)) ||
    (speed <= ((int16_t)P.getSpeed() - SPEED_DEADBAND)))
  {
    P.setSpeed(speed);
    PRINT("\nChanged speed to ", P.getSpeed());
  }
}

void setup(void)
{
  uint8_t max = 0;

  invertUpperZone = (HARDWARE_TYPE == MD_MAX72XX::PAROLA_HW || HARDWARE_TYPE == MD_MAX72XX::GENERIC_HW);
  catalog = (invertUpperZone ? catalogInvert : catalogNoInvert);

#if DEBUG
  Serial.begin(57600);
#endif
  PRINTS("\n[Double_Height_Test]");
  PRINT("\nSize of catalogItem_t ", sizeof(catalogItem_t)); 

  // work out the size of buffer required
  for (uint8_t i = 0; i<ARRAY_SIZE(msgL); i++)
  {
    uint8_t l = strlen(msgL[i]);

    if (l > max) max = l;
  }

  PRINT("\nAllocating memory: ", max + 2);
  msgH = (char *)malloc(sizeof(char)*(max + 2));

  // initialise the LED display
  P.begin(NUM_ZONES);

  // Set up zones for 2 halves of the display
  P.setZone(ZONE_LOWER, 0, ZONE_SIZE - 1);
  P.setZone(ZONE_UPPER, ZONE_SIZE, MAX_DEVICES-1);
  P.setFont(BigFont);
  P.setCharSpacing(P.getCharSpacing() * 2); // double height --> double spacing
  if (invertUpperZone)
  {
    P.setZoneEffect(ZONE_UPPER, true, PA_FLIP_UD);
    P.setZoneEffect(ZONE_UPPER, true, PA_FLIP_LR);
  }
}

void createHString(char *pH, const char *pL)
{
  for (; *pL != '\0'; pL++)
    *pH++ = *pL | 0x80;   // offset character

  *pH = '\0'; // terminate the string
}

void loop(void)
{
  static uint8_t idxMsg = 0;   // message string index
  static uint16_t idxCat = 0;   // catalog item index

  doUI();
  P.displayAnimate();

  if (P.getZoneStatus(ZONE_LOWER) && P.getZoneStatus(ZONE_UPPER))
  {
    catalogItem_t ci;

    PRINT("\nC", idxCat);
    PRINT(": M", idxMsg);
    PRINT(": A ", (uint16_t)&catalog[idxCat]);
    PRINT(": ", msgL[idxMsg]);

    // copy the next catalog item into RAM
    memcpy_P(&ci, &catalog[idxCat], sizeof(catalogItem_t));

    // set up the ZONE_UPPER string
    createHString(msgH, msgL[idxMsg]);

    // renew and start the display
    P.displayClear();

    P.displayZoneText(ZONE_LOWER, msgL[idxMsg], ALIGN_LOWER,
                      P.getSpeed(), ci.fPause ? PAUSE_TIME : 0,
                      ci.zFX[ZONE_LOWER], ci.zFX[ZONE_LOWER]);
    P.displayZoneText(ZONE_UPPER, msgH, ALIGN_UPPER,
                      P.getSpeed(), ci.fPause ? PAUSE_TIME : 0,
                      ci.zFX[ZONE_UPPER], ci.zFX[ZONE_UPPER]);

    // prepare indices for next pass
    idxMsg++;
    if (idxMsg == ARRAY_SIZE(msgL))
    {
      idxMsg = 0;
      idxCat++;
      if (idxCat == ARRAY_SIZE(catalog))
        idxCat = 0;
    }

    // synchronise the start and let the display run
    P.synchZoneStart();
  }
}
