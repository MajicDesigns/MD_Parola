// Program to demonstrate the MD_Parola library
//
// For every string defined by pc[] iterate through all combinations 
// of entry and exit effects.
//
// Animation speed can be controlled using a pot on pin SPEED_IN

#include <MD_Parola.h>
#include <MD_MAX72xx.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may 
// need to be adapted
#define	MAX_DEVICES	8
#define	CLK_PIN		13
#define	DATA_PIN	11
#define	CS_PIN		10

// set to 1 if we are implementing the user interface pot
#define	USE_UI_CONTROL	1

#if USE_UI_CONTROL
#define	SPEED_IN	A5
uint8_t	frameDelay = 25;	// default frame delay value
#endif // USE_UI_CONTROL

// Hardware SPI connection
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// Generic output pins
// MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define	SPEED_TIME	25
#define	PAUSE_TIME	1000

// Turn on debug statements to the serial output
#define  DEBUG  0

#if  DEBUG
#define	PRINT(s, x)	{ Serial.print(F(s)); Serial.print(x); }
#define	PRINTS(x)	Serial.print(F(x))
#define	PRINTX(x)	Serial.println(x, HEX)
#else
#define	PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

// Global variables
uint8_t  curText;
char	*pc[] = 
{ 
  "Parola for",
  "Arduino",
};

uint8_t  inFX, outFX;
textEffect_t	effect[] =
{
	PRINT,
	SCAN_HORIZ,
	SCROLL_LEFT,
	WIPE,
	SCROLL_UP_LEFT,
	SCROLL_UP,
	OPENING_CURSOR,
	GROW_UP,
	SCROLL_UP_RIGHT,
	BLINDS,
	CLOSING,
	GROW_DOWN,
	SCAN_VERT,
	SCROLL_DOWN_LEFT,
	WIPE_CURSOR,
	DISSOLVE,
	OPENING,
	CLOSING_CURSOR,
	SCROLL_DOWN_RIGHT,
	SCROLL_RIGHT,
	SLICE,
	SCROLL_DOWN,
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
  P.displayText(pc[curText], CENTER, SPEED_TIME, PAUSE_TIME, effect[inFX], effect[outFX]);
}

void loop(void)
{
#if USE_UI_CONTROL
	doUI();
#endif // USE_UI_CONTROL

  if (P.displayAnimate()) // animates and returns true when an animation is completed
  {
    // Set the display for the next string.
    curText = (++curText) % ARRAY_SIZE(pc);
    P.setTextBuffer(pc[curText]);

    // When we have gone back to the first string, set a new exit effect
    // and when we have done all those set a new entry effect.
    if (curText == 0)
    {
      outFX = (++outFX) % ARRAY_SIZE(effect);
      if (outFX == 0)
      {
        inFX = (++inFX) % ARRAY_SIZE(effect);
        if (inFX == 0)
          P.setInvert(!P.getInvert());
      }
        
      P.setTextEffect(effect[inFX], effect[outFX]);
    }

    // Tell Parola we have a new animation
    P.displayReset();
  }
}
