// Program to demonstrate the MD_Parola library
//
// For every string defined by pc[] iterate through all combinations
// of entry and exit effects.
//
// Animation speed can be controlled using a pot on pin SPEED_IN
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

#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// set to 1 if we are implementing the user interface pot
#define USE_UI_CONTROL  1

#if USE_UI_CONTROL
#define SPEED_IN  A5
uint8_t frameDelay = 25;  // default frame delay value
#endif // USE_UI_CONTROL

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

// Global variables
uint8_t  curText;
const char  *pc[] =
{
  "Parola for",
  "Arduino",
};

uint8_t  inFX, outFX;
textEffect_t  effect[] =
{
  PA_PRINT,
  PA_SCAN_HORIZ,
  PA_SCROLL_LEFT,
  PA_WIPE,
  PA_SCROLL_UP_LEFT,
  PA_SCROLL_UP,
  PA_OPENING_CURSOR,
  PA_GROW_UP,
  PA_MESH,
  PA_SCROLL_UP_RIGHT,
  PA_BLINDS,
  PA_CLOSING,
  PA_RANDOM,
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

#if USE_UI_CONTROL
void doUI(void)
{
  // set the speed if it has changed
  {
    int16_t	speed = map(analogRead(SPEED_IN), 0, 1023, 0, 250);

    if (speed != (int16_t)P.getSpeed())
    {
      P.setSpeed(speed);
      P.setPause(speed);
      frameDelay = speed;
      PRINT("\nChanged speed to ", P.getSpeed());
    }
  }
}
#endif // USE_UI_CONTROL

void setup(void)
{
  Serial.begin(57600);
  PRINTS("[Parola Demo]");

#if USE_UI_CONTROL
  pinMode(SPEED_IN, INPUT);
  doUI();
#endif // USE_UI_CONTROL

  P.begin();
  P.setInvert(false);
  P.displayText(pc[curText], PA_CENTER, SPEED_TIME, PAUSE_TIME, effect[inFX], effect[outFX]);
}

void loop(void)
{
#if USE_UI_CONTROL
	doUI();
#endif // USE_UI_CONTROL

  if (P.displayAnimate()) // animates and returns true when an animation is completed
  {
    // Set the display for the next string.
    curText = (curText + 1) % ARRAY_SIZE(pc);
    P.setTextBuffer(pc[curText]);

    // When we have gone back to the first string, set a new exit effect
    // and when we have done all those set a new entry effect.
    if (curText == 0)
    {
      outFX = (outFX + 1) % ARRAY_SIZE(effect);
      if (outFX == 0)
      {
        inFX = (inFX + 1) % ARRAY_SIZE(effect);
        if (inFX == 0)
          P.setInvert(!P.getInvert());
      }

      P.setTextEffect(effect[inFX], effect[outFX]);
    }

    // Tell Parola we have a new animation
    P.displayReset();
  }
}
