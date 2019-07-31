// Use the Parola library to scroll text on the display
//
// Demonstrates the use of the scrolling function to display text received
// from the serial interface and shared with a menu to change the parameters 
// for the scrolling display and save to EEPROM.
//
// User can enter text on the serial monitor and this will display as a
// scrolling message on the display.
//
// Speed, scroll direction, brightness and invert are controlled from the menu.
//
// Interface for menu control can be either 3 momentary on (tact) switches or
// a rotary encoder with integrated switch.
//
// UISwitch library can be found at https://github.com/MajicDesigns/MD_UISwitch
// Rotary Encoder library can be found at https://github.com/MajicDesigns/MD_REncoder
// Menu library can be found at https://github.com/MajicDesigns/MD_Menu
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//

#include <EEPROM.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <MD_Menu.h>

// set to 1 if depending on the menu control interface
#define MENU_SWITCH   0   // tact switches
#define MENU_RENCODER 1   // rotary encoder

const uint16_t MENU_TIMEOUT = 5000;  // in milliseconds
const uint16_t EEPROM_ADDRESS = 0;   // config data address

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
#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 11

#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
//MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Scrolling parameters configuration block
struct cfgParameter_t
{
  uint8_t signature[2];   // signature for EEPROM

  uint8_t speed;        // animation frame delay
  uint16_t pause = 2000;// animation pause in milliseconds
  uint8_t bright;       // display intensity
  textEffect_t effect;  // text animation effect
  textPosition_t align; // text alignment at pause
  bool invert = false;  // inverted display
} Config;

// Global message buffers shared by Serial and Scrolling functions
#define	BUF_SIZE	75
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "Hello! Enter new message?" };
bool newMessageAvailable = true;

// Function prototypes for Navigation/Display
bool display(MD_Menu::userDisplayAction_t, char*);
MD_Menu::userNavAction_t navigation(uint16_t &incDelta);
void *mnuValueRqst(MD_Menu::mnuId_t id, bool bGet);

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
  { 10, "Spd",  MD_Menu::INP_INT,  mnuValueRqst, 3, 0, 0,  255, 0, 10, nullptr },
  { 11, "Pse",  MD_Menu::INP_INT,  mnuValueRqst, 4, 0, 0, 2000, 0, 10, nullptr },
  { 12, "Scrl", MD_Menu::INP_LIST, mnuValueRqst, 1, 0, 0,    0, 0,  0, listScroll },
  { 13, "Algn", MD_Menu::INP_LIST, mnuValueRqst, 1, 0, 0,    0, 0,  0, listAlign },
  { 14, "Bri",  MD_Menu::INP_INT,  mnuValueRqst, 2, 0, 0,   15, 0, 10, nullptr },
  { 15, "Inv",  MD_Menu::INP_BOOL, mnuValueRqst, 1, 0, 0,    0, 0,  0, nullptr },
};

// Menu global object
MD_Menu M(navigation, display,// user navigation and display
  mnuHdr, ARRAY_SIZE(mnuHdr), // menu header data
  mnuItm, ARRAY_SIZE(mnuItm), // menu item data
  mnuInp, ARRAY_SIZE(mnuInp));// menu input data

// Configuration Load/Save to/from EEPROM
void paramSave(void)
{
  PRINTS("\nSaving Parameters");
  EEPROM.put(EEPROM_ADDRESS, Config);
}

void paramLoad(void)
{
  uint8_t sig[2] = { 0xa5, 0x5a };

  PRINTS("\nLoading Parameters");

  EEPROM.get(EEPROM_ADDRESS, Config);

  if (Config.signature[0] != sig[0] || 
      Config.signature[1] != sig[1])
  {
    PRINTS("\n\nSetting Default Parameters");
    Config.signature[0] = sig[0];
    Config.signature[1] = sig[1];
    Config.speed = 25;
    Config.bright = 7;
    Config.effect = PA_SCROLL_LEFT;
    Config.align = PA_LEFT;
    Config.pause = 2000;
    Config.invert = false;

    paramSave();
  }
}

// Menu system callback functions
void *mnuValueRqst(MD_Menu::mnuId_t id, bool bGet)
// Value request callback for variables
{
  static MD_Menu::value_t v;

  switch (id)
  {
  case 10:  // Speed
    if (bGet)
      v.value = Config.speed;
    else
    {
      Config.speed = v.value;
      P.setSpeed(Config.speed);
      PRINT("\nSet speed: ", Config.speed);
    }
    break;

  case 11:  // Pause
    if (bGet)
      v.value = Config.pause;
    else
    {
      Config.pause = v.value;
      P.setPause(Config.pause);
      PRINT("\nSet pause: ", Config.pause);
    }
    break;

  case 12:  // Scroll
    if (bGet)
      v.value = (Config.effect == PA_SCROLL_LEFT ? 0 : 1);
    else
    {
      Config.effect = (v.value == 0 ? PA_SCROLL_LEFT : PA_SCROLL_RIGHT);
      P.setTextEffect(Config.effect, Config.effect);
      PRINT("\nSet scroll effect: ", Config.effect);
    }
    break;

  case 13:  // Align
    if (bGet)
    {
      switch (Config.align)
      {
      case PA_LEFT:   v.value = 0; break;
      case PA_CENTER: v.value = 1; break;
      case PA_RIGHT:  v.value = 2; break;
      }
    }
    else
    {
      switch (v.value)
      {
      case 0: Config.align = PA_LEFT;   break;
      case 1: Config.align = PA_CENTER; break;
      case 2: Config.align = PA_RIGHT;  break;
      }
      P.setTextAlignment(Config.align);
      PRINT("\nSet align: ", Config.align);
    }
    break;

  case 14: // Bright
    if (bGet)
      v.value = Config.bright;
    else
    {
      Config.bright = v.value;
      P.setIntensity(Config.bright);
      PRINT("\nSet intensity: ", Config.bright);
    }
    break;

  case 15: // Invert
    if (bGet)
      v.value = Config.invert;
    else
    {
      Config.invert = v.value;
      P.setInvert(Config.invert);
      PRINT("\nSet invert: ", Config.invert);
    }
    break;
  }

  // if things were requested, return the buffer
  if (bGet)
    return(&v);
  else  // save the parameters
    paramSave();

  return(nullptr);
}

bool display(MD_Menu::userDisplayAction_t action, char *msg)
// Simple output to a one line LED Matrix display managed 
// by the Parola library.
{
  switch (action)
  {
  case MD_Menu::DISP_INIT:
    // nothing to do
    break;

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

#include <MD_UISwitch.h>

const uint8_t INC_PIN = 9;
const uint8_t DEC_PIN = 8;
const uint8_t CTL_PIN = 6;

uint8_t pins[] = { INC_PIN, DEC_PIN, CTL_PIN };

MD_UISwitch_Digital swNav(pins, ARRAY_SIZE(pins));

void setupNav(void)
{
  swNav.begin();
  swNav.enableRepeat(false);
}

MD_Menu::userNavAction_t navigation(uint16_t &incDelta)
{
  MD_Menu::userNavAction_t nav = MD_Menu::NAV_NULL;

  switch (swNav.read())
  {
    case MD_UISwitch::KEY_PRESS:
    {
      switch (swNav.getKey())
      {
      case INC_PIN: nav = MD_Menu::NAV_INC; break;
      case DEC_PIN: nav = MD_Menu::NAV_DEC; break;
      case ESC_PIN: nav = MD_Menu::NAV_SEL; break;
      }
    }
    break;

    case MD_UISwitch::KEY_LONGPRESS:
    {
      if (swNav.getKey() == 2)
        nav = MD_Menu::NAV_ESC;
    }
    break;
  }

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
#include <MD_UISwitch.h>

extern MD_Menu M;

const uint8_t RE_A_PIN = 2;
const uint8_t RE_B_PIN = 3;
const uint8_t CTL_PIN = 4;

MD_REncoder  RE(RE_A_PIN, RE_B_PIN);
MD_UISwitch_Digital swCtl(CTL_PIN);

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
    if (M.isInEdit()) incDelta = 1 << abs(RE.speed() >> 3);
    return(re == DIR_CCW ? MD_Menu::NAV_DEC : MD_Menu::NAV_INC);
  }

  switch (swCtl.read())
  {
  case MD_UISwitch::KEY_PRESS:     return(MD_Menu::NAV_SEL);
  case MD_UISwitch::KEY_LONGPRESS: return(MD_Menu::NAV_ESC);
  }

  return(MD_Menu::NAV_NULL);
}
#endif

// Serial input for new message
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
  Serial.print("\n[Parola Scrolling Display]\nPress SEL button for configuration menu");
  Serial.print("\nType a message for the scrolling display\nEnd message line with a newline");

  paramLoad();

  setupNav();

  M.begin();
  M.setAutoStart(true);
  M.setTimeout(MENU_TIMEOUT);

  P.begin();
  P.setInvert(Config.invert);
  P.setIntensity(Config.bright);
}

void loop()
{
  static bool wasInMenu = true;   // ensure we initialise the display first
  
  if (wasInMenu && !M.isInMenu())   // was running, but not any more
  {
    // Reset the display to show message
    PRINTS("\nMenu Stopped, running message");
    P.displayClear();
    P.displayText(curMessage, Config.align, Config.speed, Config.pause, Config.effect, Config.effect);
    wasInMenu = false;
  }
  
  wasInMenu = M.isInMenu(); // save current status
  M.runMenu();              // run or autostart the menu
  
  if (!M.isInMenu())         // not running the menu? do something else
  {
    // animate display and check swap message if ended
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

  readSerial();   // receive new serial characters
}

