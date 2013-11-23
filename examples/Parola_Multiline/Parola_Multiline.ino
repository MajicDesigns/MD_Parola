// Program to demonstrate the MD_Parola library
//
// 3 Parola module string coordinated displays as 3 separate line.
// Line 1 is the top line, rest are below this line.
//
// Animation speed can be controlled using a pot on pin SPEED_IN

#include <MD_Parola.h>
#include <MD_MAX72xx.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may 
// need to be adapted
#define	MAX_LINES	3									// number of separate lines
const uint8_t MAX_DEVICES[MAX_LINES] = {3 , 3 , 3 };	// number of modules per display line
const uint8_t CS_PIN[MAX_LINES] = { 10 , 9, 8 };		// select pins for each line
#define	CLK_PIN		13
#define	DATA_PIN	11

// set to 1 if we are implementing the user interface pot
#define	USE_UI_CONTROL	0

#if USE_UI_CONTROL
#define	SPEED_IN	A0
#endif // USE_UI_CONTROL

#define	PAUSE_TIME		1000
#define	SPEED_DEADBAND	5
uint8_t	frameDelay = 25;	// default frame delay value

// Hardware SPI connection
MD_Parola P[MAX_LINES] = 
{ 
	MD_Parola(CS_PIN[0], MAX_DEVICES[0]), 
	MD_Parola(CS_PIN[1], MAX_DEVICES[1]),
	MD_Parola(CS_PIN[2], MAX_DEVICES[2]),
};
// Generic output pins
/*
MD_Parola P[MAX_LINES] = 
{
	MD_Parola(DATA_PIN, CLK_PIN, CS_PIN[0], MAX_DEVICES[0]), 
	MD_Parola(DATA_PIN, CLK_PIN, CS_PIN[1], MAX_DEVICES[1]), 
	MD_Parola(DATA_PIN, CLK_PIN, CS_PIN[2], MAX_DEVICES[2]), 
};
*/

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
char	*pc[] = 
{ 
  "L1",
  "L2",
  "L3",
};

uint8_t  inFX, outFX;
MD_Parola::textEffect_t	effect[] =
{
  MD_Parola::PRINT,
  MD_Parola::SCROLL_LEFT,
  MD_Parola::WIPE,
  MD_Parola::SCROLL_UP,
  MD_Parola::OPENING_CURSOR,
  MD_Parola::BLINDS,
  MD_Parola::CLOSING,
  MD_Parola::WIPE_CURSOR,
  MD_Parola::DISSOLVE,
  MD_Parola::OPENING,
  MD_Parola::CLOSING_CURSOR,
  MD_Parola::SCROLL_RIGHT,
  MD_Parola::SCROLL_DOWN,
  MD_Parola::SLICE,
};

#if USE_UI_CONTROL
void doUI(void)
{
  // set the speed if it has changed
  {
    int16_t	speed = map(analogRead(SPEED_IN), 0, 1023, 0, 250);

//    if ((speed >= ((int16_t)P.getSpeed() + SPEED_DEADBAND)) || 
//      (speed <= ((int16_t)P.getSpeed() - SPEED_DEADBAND)))
    if (speed != (int16_t)P[0].getSpeed()) 
    {
	  for (uint8_t i=0; i<MAX_LINES; i++)
	  {
		  P[i].setSpeed(speed);
		  P[i].setPause(speed);
	  }
	  frameDelay = speed;
      PRINT("\nChanged speed to ", P[0].getSpeed());
    }
  }
}
#endif // USE_UI_CONTROL

void setup(void)
{
  Serial.begin(57600);
  PRINTS("[Parola Multiline Demo]");

#if USE_UI_CONTROL
  pinMode(SPEED_IN, INPUT);
  doUI();
#endif // USE_UI_CONTROL

  for (uint8_t i=0; i<MAX_LINES; i++)
  {
	  P[i].begin();
	  P[i].setInvert(false);
	  P[i].displayText(pc[i], MD_Parola::CENTER, SPEED_TIME, PAUSE_TIME, effect[inFX], effect[outFX]);
  }
}

void loop(void)
{
	bool	allDone = true;

#if USE_UI_CONTROL
	doUI();
#endif // USE_UI_CONTROL
  
  for (uint8_t i=0; i<MAX_LINES; i++)
  {
	allDone &= P[i].displayAnimate();
  }

  if (allDone)
  {
	PRINTS("\nAll strings complete");
	outFX = (++outFX) % ARRAY_SIZE(effect);
	if (outFX == 0)
		inFX = (++inFX) % ARRAY_SIZE(effect);
        
	for (uint8_t i=0; i<MAX_LINES; i++)
	{
		P[i].setTextBuffer(pc[i]);
		// Set a new animation effecta and tell Parola
		P[i].setTextEffect(effect[inFX], effect[outFX]);
		P[i].displayReset();
	}
  }
}

