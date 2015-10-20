// Program to demonstrate the MD_Parola library
//
// Simplest program that does something useful - Hello World!

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#if USE_LIBRARY_SPI
#include <SPI.h>
#endif

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may 
// need to be adapted
#define	MAX_DEVICES	4
#define	CLK_PIN		13
#define	DATA_PIN	11
#define	CS_PIN		10

// Hardware SPI connection
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void setup(void)
{
  P.begin();
  P.displayText("Hello", CENTER, 0, 0, PRINT, NO_EFFECT);
}

void loop(void)
{
  P.displayAnimate();
}