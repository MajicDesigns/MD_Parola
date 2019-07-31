// Program to exercise the MD_Parola library
//
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#include "Parola_Fonts_data.h"

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

#define PAUSE_TIME  3000

// Turn on debug statements to the serial output
#define  DEBUG  0

#if  DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTX(s, x) { Serial.print(F(s)); Serial.print(x, HEX); }
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(s, x)
#endif

// Global variables
typedef struct
{
  char  name[10];
  MD_MAX72XX::fontType_t *pFont;
  textEffect_t effect;
  const char * pMsg;
} message_t;

const message_t M[] =
{
  { "Roman",    nullptr,      PA_SCROLL_LEFT,  "Arduino" },
  { "Japanese", fontKatakana, PA_SCROLL_LEFT,  "\x0b1\x0b0\x0c2\x0b2\x0c9" },
  { "Arabic",   fontArabic,   PA_SCROLL_RIGHT, "\x0a9\x0a7\x0ab\x0a9\x090\x0a5\x088" },		// ا ر د و ي ن و
  { "Greek",    fontGreek,    PA_SCROLL_LEFT,  "\x080\x0a8\x09b\x0b2\x0a0\x0a4\x0a6" }
};

uint8_t curM = 0;   // current message definition to use

void setup(void)
{
  Serial.begin(57600);
  PRINTS("\n[Parola Demo]");

  P.begin();
  P.setFont(M[curM].pFont);
  P.displayText(M[curM].pMsg, PA_CENTER, P.getSpeed(), PAUSE_TIME, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
}

void loop(void)
{
  if (P.displayAnimate())
  {
    curM = (curM + 1) % ARRAY_SIZE(M);

    PRINT("\nChanging font to ", M[curM].name);
    PRINTS(", msg data ");
    for (uint8_t i = 0; i<strlen(M[curM].pMsg); i++)
    {
      PRINTX(" 0x", (uint8_t)M[curM].pMsg[i]);
    }

    P.setFont(M[curM].pFont);
    P.setTextBuffer(M[curM].pMsg);
    P.setTextEffect(M[curM].effect, M[curM].effect);

    P.displayReset();
  }
}
