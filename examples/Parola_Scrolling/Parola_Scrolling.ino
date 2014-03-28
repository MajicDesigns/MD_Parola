// Use the Parola library to scroll text on the display
//
// Demonstrates the use of the scrolling function to display text received 
// from the serial interface
//
// User can enter text on the serial monitor and this will display as a
// scrolling message on the display.
// Speed for the display is controlled by a pot on SPEED_IN analog in.
// Scrolling direction is controlled by a switch on DIRECTION_SET digital in.
// Invert ON/OFF is set by a switch on INVERT_SET digital in.
//
// Keyswitch library can be found at http://arduinocode.codeplex.com
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>

// set to 1 if we are implementing the user interface pot, switch, etc
#define	USE_UI_CONTROL	0

#if USE_UI_CONTROL
#include <MD_KeySwitch.h>
#endif

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


#define	PAUSE_TIME		1000
#define	SPEED_DEADBAND	5

// Scrolling parameters
#if USE_UI_CONTROL
#define	SPEED_IN		A5
#define	DIRECTION_SET	8	// change the effect
#define	INVERT_SET		9	// change the invert

#endif // USE_UI_CONTROL

uint8_t	frameDelay = 25;	// default frame delay value
textEffect_t	scrollEffect = SCROLL_LEFT;

// Global message buffers shared by Serial and Scrolling functions
#define	BUF_SIZE	75
char curMessage[BUF_SIZE];
char newMessage[BUF_SIZE];
bool newMessageAvailable = false;

#if USE_UI_CONTROL

MD_KeySwitch uiDirection(DIRECTION_SET);
MD_KeySwitch uiInvert(INVERT_SET);

void doUI(void)
{
  // set the speed if it has changed
  {
    int16_t	speed = map(analogRead(SPEED_IN), 0, 1023, 10, 150);

    if ((speed >= ((int16_t)P.getSpeed() + SPEED_DEADBAND)) || 
      (speed <= ((int16_t)P.getSpeed() - SPEED_DEADBAND)))
    {
      P.setSpeed(speed);
      P.setPause(speed);
      frameDelay = speed;
      PRINT("\nChanged speed to ", P.getSpeed());
    }
  }

  if (uiDirection.read())	// SCROLL DIRECTION
  {
    PRINTS("\nChanging scroll direction");
    scrollEffect = (scrollEffect == SCROLL_LEFT ? SCROLL_RIGHT : SCROLL_LEFT);
    P.setTextEffect(scrollEffect, scrollEffect);
    P.displayReset();
  }

  if (uiInvert.read())	// INVERT MODE
  {
    PRINTS("\nChanging invert mode");
    P.setInvert(!P.getInvert());
  }
}
#endif // USE_UI_CONTROL

void readSerial(void)
{
  static uint8_t	putIndex = 0;

  while (Serial.available())
  {
    newMessage[putIndex] = (char)Serial.read();
    if ((newMessage[putIndex] == '\n') || (putIndex >= BUF_SIZE-2))	// end of message character or full buffer
    {
      // put in a message separator and end the string
      newMessage[putIndex] = '\0';
      // restart the index for next filling spree and flag we have a message waiting
      putIndex = 0;
      newMessageAvailable = true;
    }
      else
      // Just save the next char in next location
      newMessage[putIndex++];
  }
}

void setup()
{
  Serial.begin(57600);
  PRINTS("[Parola Test]");

#if USE_UI_CONTROL
  uiDirection.begin();
  uiInvert.begin();
  pinMode(SPEED_IN, INPUT);

  doUI();
#endif // USE_UI_CONTROL

  P.begin();
  P.displayClear();
  P.displaySuspend(false);

  P.displayScroll(curMessage, LEFT, scrollEffect, frameDelay);

  strcpy(curMessage, "Hello! Enter new message?");
  newMessage[0] = '\0';

  Serial.begin(57600);
  Serial.print("\n[Parola Scrolling Display]\nType a message for the scrolling display\nEnd message line with a newline");
}

void loop() 
{
#if USE_UI_CONTROL
	doUI();
#endif // USE_UI_CONTROL

  readSerial();
  if (P.displayAnimate()) 
  {
    if (newMessageAvailable)
    {
      strcpy(curMessage, newMessage);
      newMessageAvailable = false;
    }
    P.displayReset();
  }
}

