// Program to exercise the MD_Parola library

#include <MD_Parola.h>
#include <MD_MAX72xx.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may 
// need to be adapted
#define	MAX_DEVICES	4
#define	CLK_PIN		13
#define	DATA_PIN	11
#define	CS_PIN		10

// HARDWARE SPI
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// GENERIC SPI
//MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define	PAUSE_TIME		1000
#define	SPEED_DEADBAND	5

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

// Global variables
char	*pc[] = 
{ 
  "Hello!", 
//  "Parola for",
//  "Arduino Uno", 
//  "LED Matrix",
//  "Display" 
};
#define	MAX_STRINGS  (sizeof(pc)/sizeof(pc[0]))

// User interface pin definitions
#define	SPEED_IN	A0	// control the speed with an external pot
#define JUSTIFY_SET	7	// change the justification
#define	EFFECT_SET	8	// change the effect
#define	MODE_SET	9	// change the mode
#define	INTENSITY_SET	6	// change the intensity of the display

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

  // TEXT ALIGNMENT
  {
    static uint8_t	curMode = 0;
    static bool		bLastHigh = true;

    MD_Parola::textPosition_t	align = P.getTextAlignment();
    MD_Parola::textPosition_t	mode[] = 
    { 
      MD_Parola::LEFT, 
      MD_Parola::CENTER, 
      MD_Parola::RIGHT
    };

    bool  b = (digitalRead(JUSTIFY_SET) == HIGH);

    if (!bLastHigh && b)
    {
      PRINT("\nChanging alignment to ", curMode);
      P.setTextAlignment(mode[curMode++]);
      P.displayReset();
      curMode %= ARRAY_SIZE(mode);
    }
    bLastHigh = b;
  }

  // EFFECT CHANGE
  {
    static uint8_t  curFX = 0;
    static bool     bLastHigh = true;

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
    };

    bool	b = (digitalRead(EFFECT_SET) == HIGH);

    if (!bLastHigh && b)
    {
      PRINT("\nChanging effect to ", curFX);
      P.setTextEffect(effect[curFX], effect[curFX]);
      curFX++;
      P.displayReset();
      curFX %= ARRAY_SIZE(effect);
    }
    bLastHigh = b;
  }

  // PAUSE DELAY
  {
    static bool		bLastHigh = true;

    bool	b = (digitalRead(MODE_SET) == HIGH);

    if (!bLastHigh && b)
    {
      PRINTS("\nChanging pause");
      if (P.getPause() <= P.getSpeed())
        P.setPause(PAUSE_TIME);
      else
        P.setPause(0);
    }
    bLastHigh = b;
  }

  // INTENSITY
  {
    static bool		bLastHigh = true;
    static uint8_t	intensity = 7;

    bool	b = (digitalRead(INTENSITY_SET) == HIGH);

    if (!bLastHigh && b)
    {
      intensity = ++intensity % 16;
      P.setIntensity(intensity);
      PRINT("\nChanged intensity to ", intensity);
    }
    bLastHigh = b;
  }
}

void setup(void)
{
  Serial.begin(57600);
  PRINTS("[Parola Test]");

  pinMode(SPEED_IN, INPUT);
  pinMode(JUSTIFY_SET, INPUT);
  pinMode(EFFECT_SET, INPUT);
  pinMode(MODE_SET, INPUT);

  P.begin();
  P.displayText(pc[0], MD_Parola::CENTER, P.getSpeed(), PAUSE_TIME, MD_Parola::PRINT, MD_Parola::PRINT);
}

void loop(void)
{
  static uint8_t	n = 1;

  doUI();

  if (P.displayAnimate()) 
  {
    P.setTextBuffer(pc[n++]);
    P.displayReset();
    n %= MAX_STRINGS;
  }
}


