// Use the Parola library to scroll text on the display
//
// Demonstrates the use of the scrolling function to display text received
// from the serial interface and how to adjust the spacing between the end
// of one message at the start of the following one.
//
// User can enter text on the serial monitor and this will display as a
// scrolling message on the display.
// Spacing for the display is controlled by a pot on SPACE_IN analog in.
// Scrolling direction is controlled by a switch on DIRECTION_SET digital in.
// Invert ON/OFF is set by a switch on INVERT_SET digital in.
//
// UISwitch library can be found at https://github.com/MajicDesigns/MD_UISwitch
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <MD_UISwitch.h>

// Turn on debug statements to the serial output
#define  DEBUG  1

#if  DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTX(x) Serial.println(x, HEX)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

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

#define PAUSE_TIME    0
#define FRAME_TIME    50
#define SPACE_DEADBAND  2

// Scrolling parameters
#define SPACE_IN      A5
#define DIRECTION_SET 8 // change the effect
#define INVERT_SET    9 // change the invert

textEffect_t  scrollEffect = PA_SCROLL_LEFT;

// Global message buffers shared by Serial and Scrolling functions
#define BUF_SIZE  75
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "Hi! Enter new message?" };
bool newMessageAvailable = true;

MD_UISwitch_Digital uiDirection(DIRECTION_SET);
MD_UISwitch_Digital uiInvert(INVERT_SET);

void doUI(void)
{
  // SPACING
  {
    uint16_t  space = map(analogRead(SPACE_IN), 0, 1023, 0, (MAX_DEVICES+1)*COL_SIZE);

    if (space != P.getScrollSpacing())
    {
      P.setScrollSpacing(space);
      PRINT("\nChanged spacing to ", P.getScrollSpacing());
    }
  }

  if (uiDirection.read() == MD_UISwitch::KEY_PRESS) // SCROLL DIRECTION
  {
    PRINTS("\nChanging scroll direction");
    scrollEffect = (scrollEffect == PA_SCROLL_LEFT ? PA_SCROLL_RIGHT : PA_SCROLL_LEFT);
    P.setTextEffect(scrollEffect, scrollEffect);
    P.displayReset();
  }

  if (uiInvert.read() == MD_UISwitch::KEY_PRESS)  // INVERT MODE
  {
    PRINTS("\nChanging invert mode");
    P.setInvert(!P.getInvert());
  }
}

void readSerial(void)
{
  static char *cp = newMessage;

  while (Serial.available())
  {
    *cp = (char)Serial.read();
    if ((*cp == '\n') || (cp - newMessage >= BUF_SIZE-2)) // end of message character or full buffer
    {
      *cp = '\0';      // end the string
      // restart the index for next filling spree and flag we have a message waiting
      cp = newMessage;
      newMessageAvailable = true;
    }
    else  // move char pointer to next position
      cp++;
  }
}

void setup()
{
  Serial.begin(57600);
  Serial.print("\n[Parola Scrolling Spacing]\nType a message for the scrolling display\nEnd message line with a newline");

  uiDirection.begin();
  uiInvert.begin();
  pinMode(SPACE_IN, INPUT);

  doUI();

  P.begin();
  P.displayText(curMessage, PA_CENTER, FRAME_TIME, PAUSE_TIME, scrollEffect, scrollEffect);
}

void loop()
{
  doUI();

  if (P.displayAnimate())
  {
    if (newMessageAvailable)
    {
      strcpy(curMessage, newMessage);
      newMessageAvailable = false;
    }
    P.displayReset();
  }
  readSerial();
}
