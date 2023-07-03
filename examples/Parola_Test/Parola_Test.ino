// Program to exercise the MD_Parola library
//
// Demonstrates most of the functions of the Parola library.
// All animations can be run and tested under user control.
//
// Speed for the display is controlled by a pot on SPEED_IN analog input.
// Digital switches used for control of Justification, Effect progression,
// Pause between animations, LED intensity, Display flip, and invert mode.
// UI switches are normally HIGH.
//
// UISwitch library can be found at https://github.com/MajicDesigns/MD_UISwitch
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <MD_UISwitch.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 11
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
//MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Turn on debug statements to the serial output
#define  DEBUG_ENABLE  1

#if  DEBUG_ENABLE
#define DEBUG(s, x) { Serial.print(F(s)); Serial.print(x); }
#define DEBUGS(x) Serial.print(F(x))
#define DEBUGX(x) Serial.println(x, HEX)
#else
#define DEBUG(s, x)
#define DEBUGS(x)
#define DEBUGX(x)
#endif

// User interface pin and switch definitions
const uint8_t SPEED_IN = A5;      // control the speed with an external pot
const uint8_t PAUSE_SET = 4;      // toggle pause time
const uint8_t FLIP_SET = 5;       // toggle flip status
const uint8_t JUSTIFY_SET = 6;    // change the justification
const uint8_t INTENSITY_SET = 7;  // change the intensity of the display
const uint8_t EFFECT_SET = 8;     // change the effect
const uint8_t INVERSE_SET = 9;    // set/reset the display to inverse

uint8_t uiPins[] = { PAUSE_SET, FLIP_SET, JUSTIFY_SET, INTENSITY_SET, EFFECT_SET, INVERSE_SET };

const uint16_t PAUSE_TIME = 1000; // in milliseconds
const uint8_t SPEED_DEADBAND = 5; // in analog units

// Sprite Definition
const uint8_t F_ROCKET = 2;
const uint8_t W_ROCKET = 11;
static const uint8_t PROGMEM rocket[F_ROCKET * W_ROCKET] =  // rocket
{
  0x18, 0x24, 0x42, 0x81, 0x99, 0x18, 0x99, 0x18, 0xa5, 0x5a, 0x81,
  0x18, 0x24, 0x42, 0x81, 0x18, 0x99, 0x18, 0x99, 0x24, 0x42, 0x99,
};

// Global variables
uint8_t	curString = 0;
const char *msg[] =
{
//  "Parola for Arduino LED Matrix Display", 
  "Parola for",
  "Arduino",
  "LED Matrix",
  "Display"
};
#define NEXT_STRING ((curString + 1) % ARRAY_SIZE(msg))

MD_UISwitch_Digital uiSwitches(uiPins, ARRAY_SIZE(uiPins));

void doUI(void)
{
  // set the speed if it has changed - Analog read
  {
    int16_t	speed = map(analogRead(SPEED_IN), 0, 1023, 0, 100);

    if ((speed >= ((int16_t)P.getSpeed() + SPEED_DEADBAND)) ||
        (speed <= ((int16_t)P.getSpeed() - SPEED_DEADBAND)))
    {
      P.setSpeed(speed);
      DEBUG("\nChanged speed to ", P.getSpeed());
    }
  }

  // now process the switch digital inputs
  if (uiSwitches.read() == MD_UISwitch::KEY_PRESS) // a switch was pressed!
  {
    switch (uiSwitches.getKey())
    {
      case JUSTIFY_SET: // TEXT ALIGNMENT - nothing on initialise
      {
        static uint8_t  curMode = 1;
        textPosition_t  textAlign[] =
        {
          PA_CENTER,
          PA_LEFT,
          PA_RIGHT
        };

        DEBUG("\nChanging alignment to ", curMode);
        P.setTextAlignment(textAlign[curMode]);
        P.displayClear();
        P.displayReset();
        curMode = (curMode + 1) % ARRAY_SIZE(textAlign);
      }
      break;

      case EFFECT_SET:  // EFFECT CHANGE
      {
        static uint8_t  curFX = 1;

        textEffect_t effect[] =
        {
          PA_PRINT, PA_SCROLL_UP, PA_SCROLL_DOWN, PA_SCROLL_LEFT, PA_SCROLL_RIGHT,
#if ENA_MISC
          PA_SLICE, PA_FADE, PA_MESH, PA_BLINDS, PA_DISSOLVE, PA_RANDOM, 
#endif
#if ENA_SPRITE
          PA_SPRITE,
#endif
#if ENA_WIPE
          PA_WIPE, PA_WIPE_CURSOR,
#endif
#if ENA_OPNCLS
          PA_OPENING, PA_OPENING_CURSOR, PA_CLOSING, PA_CLOSING_CURSOR,
#endif
#if ENA_SCR_DIA
          PA_SCROLL_UP_LEFT, PA_SCROLL_UP_RIGHT, PA_SCROLL_DOWN_LEFT, PA_SCROLL_DOWN_RIGHT,
#endif
#if ENA_SCAN
          PA_SCAN_HORIZ, PA_SCAN_HORIZX, PA_SCAN_VERT, PA_SCAN_VERTX,
#endif
#if ENA_GROW
          PA_GROW_UP, PA_GROW_DOWN,
#endif
        };

        DEBUG("\nChanging effect to ", curFX);
        P.setTextEffect(effect[curFX], effect[curFX]);
        P.displayClear();
        P.displayReset();
        curFX = (curFX + 1) % ARRAY_SIZE(effect);
      }
      break;

      case PAUSE_SET: // PAUSE DELAY
      {
        DEBUGS("\nChanging pause");
        if ((P.getPause() <= P.getSpeed()))
          P.setPause(PAUSE_TIME);
        else
          P.setPause(0);
      }
      break;

      case INTENSITY_SET: // INTENSITY
      {
        static uint8_t	intensity = MAX_INTENSITY/2;

        if (intensity == 0)
        {
          P.displayShutdown(true);
          DEBUG("\nDisplay shutdown ", intensity);
        }
        else
        {
          P.setIntensity(intensity);
          P.displayShutdown(false);
          DEBUG("\nChanged intensity to ", P.getIntensity());
        }

        intensity = (intensity + 1) % (MAX_INTENSITY + 1);
      }
      break;

      case INVERSE_SET:  // INVERSE
      {
        P.setInvert(!P.getInvert());
      }
      break;

      case FLIP_SET: // FLIP
      {
        P.setZoneEffect(0, !P.getZoneEffect(0, PA_FLIP_LR), PA_FLIP_LR);
        P.setZoneEffect(0, !P.getZoneEffect(0, PA_FLIP_UD), PA_FLIP_UD);
      }
      break;
    }
  }
}

void setup(void)
{
#if DEBUG_ENABLE
  Serial.begin(57600);
  DEBUGS("[Parola Test]");
#endif

  // user interface switches
  uiSwitches.begin();

  // Parola object
  if (!P.begin()) DEBUGS("\nMD_Parola library initialization failed");
#if ENA_SPRITE
  P.setSpriteData(rocket, W_ROCKET, F_ROCKET, rocket, W_ROCKET, F_ROCKET);
#endif
  P.displayText(msg[curString], PA_CENTER, P.getSpeed(), PAUSE_TIME, PA_PRINT, PA_PRINT);
  curString = NEXT_STRING;
}

void loop(void)
{
  doUI();

  if (P.displayAnimate())
  {
    P.setTextBuffer(msg[curString]);
    P.displayReset();
    curString = NEXT_STRING;
  }
}


