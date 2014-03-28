// Program to demonstrate the MD_Parola library
//
// For every string defined by pc[] iterate through all combinations 
// of entry and exit effects in each zone independently.
//
// Animation speed can be controlled using a pot on pin SPEED_IN

#include <MD_Parola.h>
#include <MD_MAX72xx.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may 
// need to be adapted
#define	MAX_DEVICES	9
#define	MAX_ZONES	3

#define	CLK_PIN		13
#define	DATA_PIN	11
#define	CS_PIN		10

// set to 1 if we are implementing the user interface pot
#define	USE_UI_CONTROL	0

#if USE_UI_CONTROL
#define	SPEED_IN	A5
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
char	*pc[MAX_ZONES] = 
{ 
  "GHI",
  "DEF",
  "ABC",
};

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

uint8_t	inFX[MAX_ZONES] = { 0, ARRAY_SIZE(effect)/3, 2*ARRAY_SIZE(effect)/3 };
uint8_t	outFX[MAX_ZONES] = { 0, ARRAY_SIZE(effect)/3, 2*ARRAY_SIZE(effect)/3 }; 

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
      PRINT("\nChanged speed to ", P.getSpeed());
    }
  }
}
#endif // USE_UI_CONTROL

void setup(void)
{
#if DEBUG
  Serial.begin(57600);
  PRINTS("[Parola Zone Display Demo]");
#endif

#if USE_UI_CONTROL
  pinMode(SPEED_IN, INPUT);
  doUI();
#endif // USE_UI_CONTROL

  P.begin(MAX_ZONES);
  P.setInvert(false);
  
  P.setZone(0, 0, 2);
  P.setZone(1, 3, 5);
  P.setZone(2, 6, 8);
  
  for (uint8_t i=0; i<MAX_ZONES; i++)
    P.displayZoneText(i, pc[i], CENTER, SPEED_TIME, PAUSE_TIME, effect[inFX[i]], effect[outFX[i]]);
}

void loop(void)
{
#if USE_UI_CONTROL
  doUI();
#endif // USE_UI_CONTROL

  if (P.displayAnimate()) // animates and returns true when an animation is completed
  {
    for (uint8_t i=0; i<MAX_ZONES; i++)
    {
      if (P.getZoneStatus(i))
      {
        outFX[i] = (++outFX[i]) % ARRAY_SIZE(effect);
        if (outFX[i] == 0)
        inFX[i] = (++inFX[i]) % ARRAY_SIZE(effect);
        
        P.setTextEffect(i, effect[inFX[i]], effect[outFX[i]]);

        // Tell Parola we have a new animation
        P.displayReset(i);
      }
    }
  }
}
