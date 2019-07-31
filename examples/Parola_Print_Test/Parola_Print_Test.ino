// Program to demonstrate the MD_Parola library
//
// Uses the Arduino Print Class extension with various output types
//
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

const uint16_t WAIT_TIME = 1000;

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

void setup(void)
{
  P.begin();
}

void loop(void)
{
  P.print("Hello");
  delay(WAIT_TIME);
  P.print(1234, DEC);
  delay(WAIT_TIME);
  P.print(1234, HEX);
  delay(WAIT_TIME);
  P.print(12.5);      // float not supported by Arduino Print class
  delay(WAIT_TIME);
  P.print(9876l);
  delay(WAIT_TIME);
  P.println("end");   // only get the /r/n characters - avoid using println
  delay(WAIT_TIME);
  P.write('A');
  delay(WAIT_TIME);
  P.write('B');
  delay(WAIT_TIME);
  P.write('C');
  delay(WAIT_TIME);
}