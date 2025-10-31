// Demonstrates using Parola to display textand graphics in the same zone
// at different times(coordinated).
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

const uint32_t GRAPHIC_TIME = 5000;     // in milliseconds
const uint32_t BALL_MOVE_DELAY = 40;   // in milliseconds

char message[] = { "Bouncing Ball" };


void bounceBall(void)
{
  static uint16_t ballX = 0, ballY = 0;
  static int8_t deltaX = 1, deltaY = 1;
  static uint32_t prevTime = 0;
  uint16_t lower, upper;

  // Move the ball if its time to do so
  if (millis() - prevTime <= BALL_MOVE_DELAY)
    return;

  prevTime = millis();

  // block display updates until the calcs are complete
  pM->control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  // work it all out here
  pM->setPoint(ballY, ballX, false);     // take the old ball out

  // new ball positions
  ballX += deltaX;
  ballY += deltaY;

  // check for edge collisions
  P.getDisplayExtent(lower, upper);

  if (ballX == upper || ballX == lower)   // end bounce
    deltaX *= -1;

  if (ballY == 0 || ballY == ROW_SIZE - 1)  // top/bottom bounce
    deltaY *= -1;  // top bounce

  pM->setPoint(ballY, ballX, true);   // put the new ball in

  // finally re-enable the display
  pM->control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void setup(void)
{
  P.begin();
  P.displayText(message, SCROLL_ALIGN, SCROLL_SPEED, SCROLL_PAUSE, SCROLL_EFFECT, SCROLL_EFFECT);

  pM = P.getGraphicObject();
}

void loop(void)
{
  static enum { INIT_MESSAGE, SHOW_MESSAGE, INIT_GRAPHIC, SHOW_GRAPHIC } state = INIT_MESSAGE;
  static uint32_t startTime;

  switch (state)
  {
  case INIT_MESSAGE:      // initialize for the message display
    P.displayClear();
    P.displayReset();
    state = SHOW_MESSAGE;
    break;

  case SHOW_MESSAGE:      // display message until animation is ended
    if (P.displayAnimate())
      state = INIT_GRAPHIC;
    break;

  case INIT_GRAPHIC:      // initialise the display for graphic
    pM->clear();
    startTime = millis();
    state = SHOW_GRAPHIC;
    break;

  case SHOW_GRAPHIC:      // show the graphic for GRAPHIC_TIME
    bounceBall();
    if (millis() - startTime >= GRAPHIC_TIME)
      state = INIT_MESSAGE;
    break;

  default:
    state = INIT_MESSAGE;
    break;
  }
}
