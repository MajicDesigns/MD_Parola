// Program to demonstrate the MD_Parola library
//
// Demonstrate use of effects on different zones to create an animated sign
//
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//

#include <MD_Parola.h>
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

uint8_t frameDelay = 25;  // default frame delay value

// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define SPEED_TIME  25
#define SPLAT_PAUSE_TIME  50
#define	TEXT_PAUSE_TIME   1000

// Global variables
uint8_t curText;
const char *pc[] = { "\x00f", "Evacuate" };

void setup(void)
{

  P.begin(3);

  // Set zone boundaries
  P.setZone(0, 0, 0);
  P.setZone(1, 1, MAX_DEVICES - 2);
  P.setZone(2, MAX_DEVICES - 1, MAX_DEVICES - 1);

  // All zones
  P.setInvert(false);
  P.setIntensity(4);

  // Specific zones
  P.displayZoneText(0, pc[0], PA_CENTER, SPEED_TIME, SPLAT_PAUSE_TIME, PA_PRINT);
  P.displayZoneText(1, pc[1], PA_CENTER, SPEED_TIME, TEXT_PAUSE_TIME, PA_PRINT);
  P.displayZoneText(2, pc[0], PA_CENTER, SPEED_TIME, SPLAT_PAUSE_TIME, PA_PRINT);
}

void loop(void)
{
  static uint8_t  inten = 0;
  static int8_t   offset = 1;

  if (P.displayAnimate()) // animates and returns true when an animation is completed
  {
    // Splats
    if (P.getZoneStatus(0)) // in sync with zone 2, so do both
    {
      inten += offset;
      if (inten == 15 || inten == 0)
        offset = -offset;
      P.setIntensity(0, inten);
      P.setIntensity(2, 15-inten);
      P.displayReset(0);
      P.displayReset(2);
    }

    // Message
    if (P.getZoneStatus(1))
    {
      P.setInvert(1, !P.getInvert(1));
      P.displayReset(1);
    }
  }
}
