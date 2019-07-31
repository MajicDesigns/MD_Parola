// Program to demonstrate the MD_Parola library
//
// Demonstrated how UTF-8 multi-byte characters can be mapped to Extended ASCII
// characters that can be displayed with a suitable font file.
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

// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

const uint16_t PAUSE_TIME = 2000;

// Turn on debug statements to the serial output
#define  DEBUG  0

#if  DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTX(s, x)  { Serial.print(F(s)); Serial.print(x, HEX); }
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(s, x)
#endif

// Global variables
char	pc[][20] =
{
  "abcABC",
  "äöüßÄÖÜ",
  "50€/kg³",
  "Español",
  "30m/s²",
  "Français",
  "20µs/°C",
};

uint8_t utf8Ascii(uint8_t ascii)
// Convert a single Character from UTF8 to Extended ASCII according to ISO 8859-1,
// also called ISO Latin-1. Codes 128-159 contain the Microsoft Windows Latin-1
// extended characters:
// - codes 0..127 are identical in ASCII and UTF-8
// - codes 160..191 in ISO-8859-1 and Windows-1252 are two-byte characters in UTF-8
//                 + 0xC2 then second byte identical to the extended ASCII code.
// - codes 192..255 in ISO-8859-1 and Windows-1252 are two-byte characters in UTF-8
//                 + 0xC3 then second byte differs only in the first two bits to extended ASCII code.
// - codes 128..159 in Windows-1252 are different, but usually only the €-symbol will be needed from this range.
//                 + The euro symbol is 0x80 in Windows-1252, 0xa4 in ISO-8859-15, and 0xe2 0x82 0xac in UTF-8.
//
// Modified from original code at http://playground.arduino.cc/Main/Utf8ascii
// Extended ASCII encoding should match the characters at http://www.ascii-code.com/
//
// Return "0" if a byte has to be ignored.
{
  static uint8_t cPrev;
  uint8_t c = '\0';

  if (ascii < 0x7f)   // Standard ASCII-set 0..0x7F, no conversion
  {
    cPrev = '\0';
    c = ascii;
  }
  else
  {
    switch (cPrev)  // Conversion depending on preceding UTF8-character
    {
    case 0xC2: c = ascii;  break;
    case 0xC3: c = ascii | 0xC0;  break;
    case 0x82: if (ascii==0xAC) c = 0x80; // Euro symbol special case
    }
    cPrev = ascii;   // save last char
  }

  PRINTX("\nConverted 0x", ascii);
  PRINTX(" to 0x", c);

  return(c);
}

void utf8Ascii(char* s)
// In place conversion UTF-8 string to Extended ASCII
// The extended ASCII string is always shorter.
{
  uint8_t c;
  char *cp = s;

  PRINT("\nConverting: ", s);

  while (*s != '\0')
  {
    c = utf8Ascii(*s++);
    if (c != '\0')
      *cp++ = c;
  }
  *cp = '\0';   // terminate the new string
}

void setup(void)
{
#if DEBUG
  Serial.begin(57600);
#endif
  PRINTS("\n[Parola UTF-8 display Test]");

  // Do one time in-place conversion of the strings to be displayed
  for (uint8_t i=0; i<ARRAY_SIZE(pc); i++)
    utf8Ascii(pc[i]);

  // Initialise the Parola library
  P.begin();
  P.setInvert(false);
  P.setPause(PAUSE_TIME);
  P.setFont(ExtASCII);
}

void loop(void)
{
  static uint32_t timeLast = PAUSE_TIME;
  static uint16_t  idx = 0;
  static bool showMessage = true;

  if (millis() - timeLast <= PAUSE_TIME)
    return;

  timeLast = millis();

  if (showMessage)
  {
    // show the strings with the UTF-8 characters
    PRINT("\nS:", idx);
    PRINT(" - ", pc[idx]);
    P.print(pc[idx]);

    // Set the display for the next string.
    idx++;
    if (idx == ARRAY_SIZE(pc))
    {
      showMessage = false;
      idx = 128;
    }
  }
  else
  {
    // show the list of characters in order
    char szMsg[20];

    sprintf(szMsg, "%3d %c", idx, idx);
    PRINT("\nC:", idx);
    PRINT(" - ", szMsg);
    P.print(szMsg);

    // set up for next character
    idx++;
    if (idx == 256)
    {
      idx = 0;
      showMessage = true;
    }
  }
}
