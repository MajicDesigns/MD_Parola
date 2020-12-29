// Demonstrates using Parola to display text and graphics in different
// zones simultaneously.
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

// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
//MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_MAX72XX *pM;

// Display parameters
const uint8_t SCROLL_SPEED = 50;    // text speed
const textEffect_t SCROLL_EFFECT = PA_SCROLL_LEFT;
const textPosition_t SCROLL_ALIGN = PA_CENTER;
const uint16_t SCROLL_PAUSE = 0;    // in milliseconds

const uint32_t BALL_MOVE_DELAY = 40;   // in milliseconds

const uint8_t TEXT_ZONE = 0;    // just change this to adjust display
// Define other dependent zone constants
const uint8_t GRAPHIC_ZONE = (TEXT_ZONE == 0 ? 1 : 0);
const uint8_t ZONE_BOUNDARY = MAX_DEVICES/2;
const uint8_t GRA_LOWER = (GRAPHIC_ZONE == 1 ? ZONE_BOUNDARY : 0);
const uint8_t GRA_UPPER = (GRAPHIC_ZONE == 1 ? MAX_DEVICES - 1 : ZONE_BOUNDARY - 1);
const uint8_t TXT_LOWER = (TEXT_ZONE == 1 ? ZONE_BOUNDARY : 0);
const uint8_t TXT_UPPER = (TEXT_ZONE == 1 ? MAX_DEVICES - 1 : ZONE_BOUNDARY - 1);


char message[] = { "Bouncing Ball" };

void bounceBall(void)
{
  static bool bInitialized = false;
  static uint16_t ballX, ballY;
  static int8_t deltaX = 1, deltaY = 1;
  static uint32_t prevTime = 0;
  static uint16_t lower, upper;

  if (!bInitialized)    // one time initialization of zone parameters
  {
    P.getDisplayExtent(GRAPHIC_ZONE, lower, upper);

    ballX = lower;
    ballY = 0;
    
    bInitialized = true;
  }
  
  // Move the ball if its time to do so
  if (millis() - prevTime <= BALL_MOVE_DELAY)
    return;

  prevTime = millis();

  // block display updates for graphic zone modules
  for (uint8_t m = GRA_LOWER; m <= GRA_UPPER; m++)
    pM->control(m, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  // work it all out here
  pM->setPoint(ballY, ballX, false);     // take the old ball out

  // new ball positions
  ballX += deltaX;
  ballY += deltaY;

  // check for edge collisions
  if (ballX == upper || ballX == lower)   // end bounce
    deltaX *= -1;

  if (ballY == 0 || ballY == ROW_SIZE - 1)  // top/bottom bounce
    deltaY *= -1;  // top bounce

  pM->setPoint(ballY, ballX, true);   // put the new ball in

  // finally re-enable the blocked modules
  for (uint8_t m = GRA_LOWER; m <= GRA_UPPER; m++)
    pM->control(m, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void setup(void)
{
  P.begin(2);
  P.setZone(TEXT_ZONE, TXT_LOWER, TXT_UPPER);
  P.setZone(GRAPHIC_ZONE, GRA_LOWER, GRA_UPPER);
  P.displayZoneText(TEXT_ZONE, message, SCROLL_ALIGN, SCROLL_SPEED, SCROLL_PAUSE, SCROLL_EFFECT, SCROLL_EFFECT);

  pM = P.getGraphicObject();
}

void loop(void)
{
    if (P.displayAnimate())
      P.displayReset();
    bounceBall();
}
