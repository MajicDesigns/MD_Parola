// Program to exercise the MD_Parola library
// 
// Demonstrates most of the functions of the Parola library.
// All anoimations can be run and tested under user control.
//
// Speed for the display is controlled by a pot on SPEED_IN analog input.
// Digital switches usedfor control of Justification, Effect progression, 
// Pause between animations, LED intensity, and invert mode. UI switches 
// are normally HIGH.
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include "Parola_Test.h"

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may 
// need to be adapted
#define	MAX_DEVICES	8
#define	CLK_PIN		13
#define	DATA_PIN	11
#define	CS_PIN		10

// HARDWARE SPI
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// GENERIC SPI
//MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Turn on debug statements to the serial output
#define  DEBUG  1

#if  DEBUG
#define	PRINT(s, x)	{ Serial.print(F(s)); Serial.print(x); }
#define	PRINTS(x)	Serial.print(F(x))
#define	PRINTX(x)	Serial.println(x, HEX)
#else
#define	PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

// User interface pin and switch definitions
#define	SPEED_IN	A5	// control the speed with an external pot
#define JUSTIFY_SET	7	// change the justification
#define	EFFECT_SET	8	// change the effect
#define	PAUSE_SET	9	// toggle pause time
#define	INTENSITY_SET	6	// change the intensity of the display
#define	INVERSE_SET	5	// set/reset the display to inverse

#define	SWITCH_OFF	HIGH
#define	SWITCH_ON	LOW
#define	DEBOUNCE_TIME	20	// in milliseconds
#define	PAUSE_TIME		1000
#define	SPEED_DEADBAND	5

// Global variables
uint8_t	curString = 0;
char	*pc[] = 
{ 
  "Parola for",
  "Arduino", 
  "LED Matrix",
  "Display" 
};
#define	NEXT_STRING	((curString + 1) % ARRAY_SIZE(pc))

switchDef_t UIControl[] = 
{
	{ JUSTIFY_SET,	true,	0 }, 
	{ EFFECT_SET,	true,	0 }, 
	{ PAUSE_SET,	true,	0 }, 
	{ INTENSITY_SET, true,	0 }, 
	{ INVERSE_SET,	true,	0 },
};


bool switchCheck(switchDef_t *p)
// return true when a transition from SWITCH_OFF to SWITCH_ON states has been detected
{
	// debounce time
	if ((millis() - p->swActTime) < DEBOUNCE_TIME)
		return(false);

	// process current status
	bool b = (digitalRead(p->swPin) == SWITCH_ON);

	// OFF to ON edge detection
	if (!p->swActive && b)
	{
		p->swActive = true;
		p->swActTime = millis();
		return(true);
	}

	// ON to OFF edge detection
	if (p->swActive && !b)
		p->swActive = false;

	return(false);
}

void doUI(void)
{
  // set the speed if it has changed
  {
    int16_t	speed = map(analogRead(SPEED_IN), 0, 1023, 0, 250);

    if ((speed >= ((int16_t)P.getSpeed() + SPEED_DEADBAND)) || 
      (speed <= ((int16_t)P.getSpeed() - SPEED_DEADBAND)))
    {
      P.setSpeed(speed);
      PRINT("\nChanged speed to ", P.getSpeed());
    }
  }

  // now process the digital inputs
  for (uint8_t i=0; i<ARRAY_SIZE(UIControl); i++)
  {
    if (switchCheck(&UIControl[i]))
	{
		switch (UIControl[i].swPin)
		{
		case JUSTIFY_SET:	// TEXT ALIGNMENT
			{
				static uint8_t	curMode = 0;
				MD_Parola::textPosition_t	align = P.getTextAlignment();
				MD_Parola::textPosition_t	mode[] = 
				{ 
					MD_Parola::LEFT, 
					MD_Parola::CENTER, 
					MD_Parola::RIGHT
				};

				PRINT("\nChanging alignment to ", curMode);
				P.setTextAlignment(mode[curMode]);
				P.displayReset();
				curMode = (curMode + 1) % ARRAY_SIZE(mode);
			}
			break;

		case EFFECT_SET:	// EFFECT CHANGE
			{
				static uint8_t  curFX = 0;

				MD_Parola::textEffect_t	effect[] =
				{
					MD_Parola::PRINT,
					MD_Parola::SLICE,
					MD_Parola::WIPE,
					MD_Parola::WIPE_CURSOR,
					MD_Parola::OPENING,
					MD_Parola::OPENING_CURSOR,
					MD_Parola::CLOSING,
					MD_Parola::CLOSING_CURSOR,
					MD_Parola::BLINDS,
					MD_Parola::DISSOLVE,
					MD_Parola::SCROLL_UP,
					MD_Parola::SCROLL_DOWN,
					MD_Parola::SCROLL_LEFT,
					MD_Parola::SCROLL_RIGHT,
					MD_Parola::SCAN_HORIZ,
					MD_Parola::SCAN_VERT,
					MD_Parola::GROW_UP,
					MD_Parola::GROW_DOWN,
				};

				PRINT("\nChanging effect to ", curFX);
				P.setTextEffect(effect[curFX], effect[curFX]);
				P.displayReset();
				curFX = (curFX + 1) % ARRAY_SIZE(effect);
			}
			break;

		case PAUSE_SET:	// PAUSE DELAY
			{
				PRINTS("\nChanging pause");
				if (P.getPause() <= P.getSpeed())
					P.setPause(PAUSE_TIME);
				else
					P.setPause(0);
			}
			break;

		case INTENSITY_SET:	// INTENSITY
			{
				static uint8_t	intensity = 7;

				intensity = ++intensity % 16;
				P.setIntensity(intensity);
				PRINT("\nChanged intensity to ", intensity);
			}
			break;

		case INVERSE_SET:	// INVERSE
			PRINTS("\nToggling invert");
			P.setInvert(!P.getInvert());
			break;
		}
	}
  }
}

void setup(void)
{
  Serial.begin(57600);
  PRINTS("[Parola Test]");

  for (uint8_t i=0; i<ARRAY_SIZE(UIControl); i++)
	pinMode(UIControl[i].swPin, INPUT_PULLUP);

  P.begin();
  P.displayText(pc[curString], MD_Parola::CENTER, P.getSpeed(), PAUSE_TIME, MD_Parola::PRINT, MD_Parola::PRINT);
  curString = NEXT_STRING;
}

void loop(void)
{
  doUI();

  if (P.displayAnimate()) 
  {
    P.setTextBuffer(pc[curString]);
    P.displayReset();
	curString = NEXT_STRING;
  }
}


