// Demonstrates using double height to create a scoreboard using Parola.
//
// Each score has 2 zones, an upper and a lower zone to display the top and bottom 
// sections of the double height score. The numeric font is a fixed font with
// just the numbers required to display scores.
//
// The upper and lower portions are 'stacked' on top of each other
// so that the module numbers are in the sequence shown below:
//
// * Modules (like FC-16) that can fit over each other with no gap
//  n n-1 n-2 ... n/2+1   <- this direction top row
//  n/2 ... 3  2  1  0    <- this direction bottom row
//
// * Modules (like Generic and Parola) that cannot fit over each other with no gap
//  n/2+1 ... n-2 n-1 n   -> this direction top row
//  n/2 ... 3  2  1  0    <- this direction bottom row
//
// Each font displays letters for either top or bottom half of the message. Sending the
// same string to both zones creates the complete message
// on the display.
//
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
// MD_UISwitch library can be found at https://github.com/MajicDesigns/MD_UISwitch
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <MD_UISwitch.h>
#include <SPI.h>
#include "Font_Data.h"

#define DEBUG 0

#if DEBUG
#define PRINT(s, v)   { Serial.print(F(s)); Serial.print(v); }      ///< Print a string followed by a value (decimal)
#define PRINTX(s, v)  { Serial.print(F(s)); Serial.print(v, HEX); } ///< Print a string followed by a value (hex)
#define PRINTS(s)     { Serial.print(F(s)); }                       ///< Print a string
#else
#define PRINT(s, v)   ///< Print a string followed by a value (decimal)
#define PRINTX(s, v)  ///< Print a string followed by a value (hex)
#define PRINTS(s)     ///< Print a string
#endif


// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
const MD_MAX72XX::moduleType_t HARDWARE_TYPE = MD_MAX72XX::FC16_HW;
const uint8_t NUM_ZONES = 4;
const uint8_t ZONE_SIZE = 4;
const uint8_t MAX_DEVICES = (NUM_ZONES * ZONE_SIZE);

const uint8_t CLK_PIN = 13;
const uint8_t DATA_PIN = 11;
const uint8_t CS_PIN = 10;

// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
//MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Other global variables
bool invertUpperZone = false; // Hardware type adaptation

const uint8_t NUM_SCORES = 2;   // Number of scores displayed
const uint8_t NUM_DIGITS = 3;   // Maximum number of decimal digits in the score
uint16_t maxScore;              // Maximum score displayed, calculated from number of digits
uint16_t score[NUM_SCORES] = { 0 };   // Running score registers
char mesg[NUM_SCORES][NUM_DIGITS+1];  // Persistent message buffer with the score to be displayed

const uint8_t SCORE_BASE_ZONE[NUM_SCORES] = { 0, 2 };    // Scores base zone number, add in UPPER and LOWER
const uint8_t ZONE_UPPER = 1;
const uint8_t ZONE_LOWER = 0;

// Hardware and objects for score inc/dec input switches
// if NUM_SCORES is changed the number of pins in the array will need to be adjusted
uint8_t pinInc[NUM_SCORES] = { 9, 7 };   // increment score pins
uint8_t pinDec[NUM_SCORES] = { 8, 6 };   // decrement score pins

MD_UISwitch_Digital scoreInc(pinInc, NUM_SCORES);
MD_UISwitch_Digital scoreDec(pinDec, NUM_SCORES);

void updateDisplay(uint8_t scoreIdx)
{
  char *p = &mesg[scoreIdx][NUM_DIGITS];
  uint16_t s = score[scoreIdx];  // temp score for arithmetic

  *p-- = '\0';    // terminate the string, we work backwards through the buffer

  for (uint8_t j = 0; j < NUM_DIGITS; j++)
  {
    *p-- = (s % 10) + '0';   // ascii digit for the lowest digit
    s /= 10;
  }
  
  PRINT("\nMesg[", scoreIdx);
  PRINT("]:'", mesg[scoreIdx]);
  PRINTS("'");

  P.displayZoneText(SCORE_BASE_ZONE[scoreIdx]+ZONE_LOWER, mesg[scoreIdx], PA_RIGHT, P.getSpeed(), 0, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(SCORE_BASE_ZONE[scoreIdx]+ZONE_UPPER, mesg[scoreIdx], PA_RIGHT, P.getSpeed(), 0, PA_PRINT, PA_NO_EFFECT);
  P.synchZoneStart();
}

void setup(void)
{
#if DEBUG
  Serial.begin(57600);
#endif
  PRINTS("\n[Scoreboard Example]");

  invertUpperZone = (HARDWARE_TYPE == MD_MAX72XX::GENERIC_HW || HARDWARE_TYPE == MD_MAX72XX::PAROLA_HW);

  // initialise the inc/dec switches
  scoreInc.begin();
  scoreDec.begin();
  scoreInc.enableDoublePress(false);
  scoreDec.enableDoublePress(false);
  scoreInc.enableLongPress(false);
  scoreDec.enableLongPress(false);
  //scoreInc.enableRepeat(false);
  //scoreDec.enableRepeat(false);

  // initialise the LED display
  P.begin(NUM_ZONES);

  // Set up zones for 2 halves of the display
  // Each zone gets a different font, making up the top and bottom half of each letter
  for (uint8_t i = 0; i < NUM_SCORES; i++)
  {
    uint8_t z = (i * 2 * ZONE_SIZE);

    P.setZone(SCORE_BASE_ZONE[i] + ZONE_LOWER, z, z + ZONE_SIZE - 1);
    P.setZone(SCORE_BASE_ZONE[i] + ZONE_UPPER, z + ZONE_SIZE, z + (ZONE_SIZE * 2) - 1);

    P.setFont(SCORE_BASE_ZONE[i] + ZONE_LOWER, NumFontLower);
    P.setFont(SCORE_BASE_ZONE[i] + ZONE_UPPER, NumFontUpper);

    if (invertUpperZone)
    {
      P.setZoneEffect(SCORE_BASE_ZONE[i] + ZONE_UPPER, true, PA_FLIP_UD);
      P.setZoneEffect(SCORE_BASE_ZONE[i] + ZONE_UPPER, true, PA_FLIP_LR);
    }
  }
  P.setCharSpacing(0); // spacing is built into the font definition

  // work out the maximum score that can be displayed based on number of digits
  maxScore = 0;
  for (uint8_t i = 0; i < NUM_DIGITS; i++)
    maxScore = (maxScore * 10) + 9;       // 9999...
  PRINT("\nMax score ", maxScore);

  // initialise the displayed number
  for (uint8_t i = 0; i < NUM_SCORES; i++)
    updateDisplay(i);
}

void loop(void)
{
  uint8_t pinIdx;
  int8_t scoreIdx = -1;

  // Handle the input switches to increment/decrement
  if (scoreInc.read() != MD_UISwitch::KEY_NULL)
  {
    pinIdx = scoreInc.getKey();

    for (uint8_t i = 0; i < NUM_SCORES; i++)
    {
      if (pinIdx == pinInc[i])
      {
        scoreIdx = i;
        break;
      }
    }
    PRINT("\nInc score[", scoreIdx);
    if (scoreIdx != -1)
    {
      if (score[scoreIdx] < maxScore) score[scoreIdx]++;
      PRINT("]=", score[scoreIdx]);
    }
    updateDisplay(scoreIdx);
  }
  else if (scoreDec.read() != MD_UISwitch::KEY_NULL)
  {
    pinIdx = scoreDec.getKey();

    for (uint8_t i = 0; i < NUM_SCORES; i++)
    {
      if (pinIdx == pinDec[i])
      {
        scoreIdx = i;
        break;
      }
    }
    PRINT("\nDec score[", scoreIdx);
    if (scoreIdx != -1)
    {
      if (score[scoreIdx] > 0) score[scoreIdx]--;
      PRINT("]=", score[scoreIdx]);
    }
    updateDisplay(scoreIdx);
  }

  // Always run the display animation, we don't care when it completes
  P.displayAnimate();
}
