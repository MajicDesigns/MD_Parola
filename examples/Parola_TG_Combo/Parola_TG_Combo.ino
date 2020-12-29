// Demonstrates the use of Parola to combine text and graphics as one combined
// display simultaneously.
//
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
// MD_MAXPanel library can be found at https://github.com/MajicDesigns/MD_MAXPanel

#include <MD_Parola.h>
#include <MD_MAXPanel.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

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

// Create the graphics library object, passing through the Parola MD_MAX72XX graphic object
// This allows us to use the graphics functions in the MD_MaxPanel library.
MD_MAXPanel MP = MD_MAXPanel(P.getGraphicObject(), MAX_DEVICES, 1);

const uint16_t PAUSE_TIME = 1000; // in milliseconds

// Global variables
uint8_t	curString = 0;
const char *msg[] =
{
  "Text &",
  "Graphics",
};

inline uint8_t nextString(void) { return((curString + 1) % ARRAY_SIZE(msg)); }

void setup(void)
{
  // Parola and MAXPanel objects
  P.begin();
  P.displayText(msg[curString], PA_CENTER, P.getSpeed(), PAUSE_TIME, PA_PRINT, PA_PRINT);
  curString = nextString();

  MP.begin();
}

void loop(void)
{
  // Do the normal animation for text
  if (P.displayAnimate())
  {
    P.setTextBuffer(msg[curString]);
    P.displayReset();
    curString = nextString();
  }

  // now check if we need to refresh the graphic part of the display
  if (P.isAnimationAdvanced())
  {
    uint16_t startDisp, endDisp;
    uint16_t startText, endText;

    P.getDisplayExtent(startDisp, endDisp);
    P.getTextExtent(startText, endText);

    // Draw rectangles in the non-text part, taking into account the change 
    // in coordinates system between Parola and MAXPanel
    MP.update(false);
    MP.drawRectangle(0, 0, MP.getXMax()-(startText+2), ROW_SIZE-1, !P.getInvert());
    MP.drawRectangle(MP.getXMax()-(endText-2), 0, MP.getXMax(), ROW_SIZE-1, !P.getInvert());
    MP.update(true);
  }
}


