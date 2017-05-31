// Use the Parola library to scroll text on the display
//
// Demonstrates the use of the scrolling function to display text received
// from the serial interface and shared with a menu to change the parateres 
// for the scrolling display.
//
// User can enter text on the serial monitor and this will display as a
// scrolling message on the display.
// Speed, scroll direction, brightness and invert are controlled from the menu.
//
// Interface for menu control can be either 3 momentary on (tact) switches or
// a rotary encoder with integrated switch.
//
// Keyswitch library can be found at https://github.com/MajicDesigns/MD_KeySwitch
// Rotary Encoder library can be found at https://github.com/MajicDesigns/MD_REncoder
// Menu library can be found at https://github.com/MajicDesigns/MD_Menu
//
// NOTE: MD_MAX72xx library must be installed and configured for the LED
// matrix type being used. Refer documentation included in the MD_MAX72xx
// library or see this link:
// https://majicdesigns.github.io/MD_MAX72XX/page_hardware.html
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <MD_Menu.h>

// set to 1 if depending on the menu control interface
#define MENU_SWITCH   0
#define MENU_RENCODER 1

// Turn on debug statements to the serial output
#define DEBUG 1

#if DEBUG
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
#define MAX_DEVICES 8
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// HARDWARE SPI
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
//MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Scrolling parameters
uint8_t speed = 25;    // default frame delay value
uint8_t bright = 7;    // default brightness
textEffect_t effect = PA_SCROLL_LEFT;
textPosition_t align = PA_LEFT;
uint16_t pause = 2000; // in milliseconds
bool invert = false;

// Global message buffers shared by Serial and Scrolling functions
#define	BUF_SIZE	75
char curMessage[BUF_SIZE];
char newMessage[BUF_SIZE];
bool newMessageAvailable = false;

// Function prototypes for Navigation/Display
bool display(MD_Menu::userDisplayAction_t, char*);
MD_Menu::userNavAction_t navigation(uint16_t &incDelta);
void *mnuValueRqst(MD_Menu::mnuId_t id, uint8_t idx, bool bGet);

// Menu definition
const PROGMEM MD_Menu::mnuHeader_t mnuHdr[] =
{
  { 10, "Menu", 10, 15, 0 },
};

const PROGMEM MD_Menu::mnuItem_t mnuItm[] =
{
  // Starting (Root) menu
  { 10, "Speed",  MD_Menu::MNU_INPUT, 10 },
  { 11, "Pause",  MD_Menu::MNU_INPUT, 11 },
  { 12, "Scroll", MD_Menu::MNU_INPUT, 12 },
  { 13, "Align",  MD_Menu::MNU_INPUT, 13 },
  { 14, "Bright", MD_Menu::MNU_INPUT, 14 },
  { 15, "Invert", MD_Menu::MNU_INPUT, 15 },
};

const PROGMEM char listAlign[] = "L|C|R";
const PROGMEM char listScroll[] = "L|R";

const PROGMEM MD_Menu::mnuInput_t mnuInp[] =
{
  { 10, 0, "Spd",  MD_Menu::INP_INT8,  mnuValueRqst, 3, 0, 255, 10, nullptr },
  { 11, 0, "Pse",  MD_Menu::INP_INT16, mnuValueRqst, 4, 0, 2000, 10, nullptr },
  { 12, 0, "Scrl", MD_Menu::INP_LIST,  mnuValueRqst, 1, 0, 0, 0, listScroll },
  { 13, 0, "Algn", MD_Menu::INP_LIST,  mnuValueRqst, 1, 0, 0, 0, listAlign },
  { 14, 0, "Bri",  MD_Menu::INP_INT8,  mnuValueRqst, 2, 0, 15, 10, nullptr },
  { 15, 0, "Inv",  MD_Menu::INP_BOOL,  mnuValueRqst, 1, 0, 0, 0, nullptr },
};

// Menu global object
MD_Menu M(navigation, display,// user navigation and display
  mnuHdr, ARRAY_SIZE(mnuHdr), // menu header data
  mnuItm, ARRAY_SIZE(mnuItm), // menu item data
  mnuInp, ARRAY_SIZE(mnuInp));// menu input data

// Menu timeout data and functions
const uint16_t MENU_TIMEOUT = 5000;  // in milliseconds
uint32_t timeStart;

void timerStart(void)
{
  timeStart = millis();
}

void timerCheck(void)
{
  if (millis() - timeStart >= MENU_TIMEOUT)
    M.reset();
}

// Menu system callback functions
void *mnuValueRqst(MD_Menu::mnuId_t id, uint8_t idx, bool bGet)
// Value request callback for variables
{
  static uint8_t listValue;

  switch (id)
  {
  case 10:  // Speed
    if (bGet) return((void *)&speed);
    else P.setSpeed(speed);
    break;

  case 11:  // Pause
    if (bGet) return((void *)&pause);
    else P.setPause(pause);
    break;

  case 12:  // Scroll
    if (bGet)
    {
      listValue = (effect == PA_SCROLL_LEFT ? 0 : 1);
      return((void *)&listValue);
    }
    else
    {
      effect = (listValue == 0 ? PA_SCROLL_LEFT : PA_SCROLL_RIGHT);
      P.setTextEffect(effect, effect);
    }
    break;

  case 13:  // Align
    if (bGet)
    {
      switch (align)
      {
      case PA_LEFT: listValue = 0; break;
      case PA_CENTER: listValue = 1; break;
      case PA_RIGHT: listValue = 2; break;
      }
      return((void *)&listValue);
    }
    else
    {
      switch (listValue)
      {
      case 0: align = PA_LEFT;  break;
      case 1: align = PA_CENTER; break;
      case 2: align = PA_RIGHT; break;
      }
      P.setTextAlignment(align);
    }
    break;

  case 14: // Bright
    if (bGet) return((void *)&bright);
    else P.setIntensity(bright);
    break;

  case 15: // Invert
    if (bGet) return((void *)&invert);
    else P.setInvert(invert);
    break;
  }

  return(nullptr);
}

bool display(MD_Menu::userDisplayAction_t action, char *msg)
// Simple output to a one line LED Matrix display managed 
// by the Parola library.
{
  switch (action)
  {
  case MD_Menu::DISP_CLEAR:
    P.displayClear();
    break;

  case MD_Menu::DISP_L0:
    // P.print(msg);    // only one zone, no line 0
    break;

  case MD_Menu::DISP_L1:
    P.print(msg);
    break;
  }

  return(true);
}

#if MENU_SWITCH
// Implemented as momentary on 3 switches 
// one switch each for INC, DEC
// one switch for SEL (click) or ESC (long press)

#include <MD_KeySwitch.h>

const uint8_t INC_PIN = 2;
const uint8_t DEC_PIN = 3;
const uint8_t CTL_PIN = 4;

MD_KeySwitch swInc(INC_PIN);
MD_KeySwitch swDec(DEC_PIN);
MD_KeySwitch swCtl(CTL_PIN);

void setupNav(void)
{
  swInc.begin();
  swDec.begin();
  swCtl.begin();
  swCtl.enableRepeat(false);
}

MD_Menu::userNavAction_t navigation(uint16_t &incDelta)
{
  MD_Menu::userNavAction_t nav = MD_Menu::NAV_NULL;

  if (swInc.read() == MD_KeySwitch::KS_PRESS) nav = MD_Menu::NAV_INC;
  else if (swDec.read() == MD_KeySwitch::KS_PRESS) nav = MD_Menu::NAV_DEC;

  switch (swCtl.read())
  {
  case MD_KeySwitch::KS_PRESS: nav = MD_Menu::NAV_SEL; break;;
  case MD_KeySwitch::KS_LONGPRESS: nav = MD_Menu::NAV_ESC; break;
  }

  if (nav != MD_Menu::NAV_NULL) timerStart();

  return(nav);
}
#endif // USE_UI_CONTROL

#if MENU_RENCODER
// Implemented as a rotary encoder with integrated push switch.
// Rotate encoder clockwise for INC, anti-clockwise for DEC
// Switch for SEL (press) or ESC (long press)
//
// If a value is being edited, the rotary encoder will also apply 
// a factor for how fast the RE is being turned to increase the rate 
// of change of values. This will be applied by the menu library to 
// numeric input only.

#include <MD_REncoder.h>
#include <MD_KeySwitch.h>

extern MD_Menu M;

const uint8_t RE_A_PIN = 2;
const uint8_t RE_B_PIN = 3;
const uint8_t CTL_PIN = 4;

MD_REncoder  RE(RE_A_PIN, RE_B_PIN);
MD_KeySwitch swCtl(CTL_PIN);

void setupNav(void)
{
  RE.begin();
  swCtl.begin();
  swCtl.enableRepeat(false);
}

MD_Menu::userNavAction_t navigation(uint16_t &incDelta)
{
  uint8_t re = RE.read();

  if (re != DIR_NONE)
  {
    if (M.isInEdit()) incDelta = 1 << abs(RE.speed() / 10);
    timerStart();
    return(re == DIR_CCW ? MD_Menu::NAV_DEC : MD_Menu::NAV_INC);
  }

  switch (swCtl.read())
  {
  case MD_KeySwitch::KS_PRESS:     timerStart(); return(MD_Menu::NAV_SEL);
  case MD_KeySwitch::KS_LONGPRESS: timerStart(); return(MD_Menu::NAV_ESC);
  }

  return(MD_Menu::NAV_NULL);
}
#endif

void readSerial(void)
{
  static char *cp = newMessage;

  while (Serial.available())
  {
    *cp = (char)Serial.read();
    if ((*cp == '\n') || (cp - newMessage >= BUF_SIZE-2)) // end of message character or full buffer
    {
      *cp = '\0'; // end the string
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
  Serial.print("\n[Parola Scrolling Display]\nType a message for the scrolling display\nEnd message line with a newline");

  setupNav();
  M.begin();

  P.begin();
  P.displayClear();
  P.displaySuspend(false);

  P.displayText(curMessage, align, speed, pause, effect, effect);
  P.setInvert(invert);
  P.setIntensity(bright);

  strcpy(curMessage, "Hello! Enter new message?");
  newMessage[0] = '\0';
}

void loop()
{
  // receive new serial characters
  readSerial();

  if (M.isInMenu())
  {
    M.runMenu();        // keep running the menu
    timerCheck();       // timeout check/reset
    if (!M.isInMenu())  // was running, now no more, so reset the display
    {
      P.displayClear();
      P.displayText(curMessage, align, speed, pause, effect, effect);
    }
  }
  else
  {
    uint16_t delta;

    if (navigation(delta) == MD_Menu::NAV_SEL)
      M.runMenu(true);  // start the menu display
    else
    {
      // animate and swap message if not displaying menu
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
  }
}

