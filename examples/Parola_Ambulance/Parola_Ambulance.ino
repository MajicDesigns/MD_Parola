// Demonstrates reversed text (as if on front of an ambulance)
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
#define MAX_DEVICES 8

#define PAUSE_TIME    1000
#define SCROLL_SPEED  50

#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
//MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

const char *msg[] =
{
  "Ambulance",
  "Emergency",
};

void setup(void)
{
  // initialise the LED display
  P.begin();
  P.setZoneEffect(0, true, PA_FLIP_LR);
}

void loop(void)
{
  static uint8_t cycle = 0;

  if (P.displayAnimate())
  {
    // set up the string
    P.displayText(msg[cycle], PA_CENTER, SCROLL_SPEED, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);

    // prepare for next pass
    cycle = (cycle + 1) % ARRAY_SIZE(msg);
  }
}
