// Program to exercise the MD_Parola library

#include <MD_Parola.h>
#include <MD_MAX72xx.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may 
// need to be adapted
#define	MAX_DEVICES	8

#define	CLK_PIN		13
#define	DATA_PIN	11
#define	CS_PIN		10

// HARDWARE SPI
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// GENERIC SPI
//MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define	PAUSE_TIME		1000

// Turn on debug statements to the serial output
#define  DEBUG  0

#if  DEBUG
#define	PRINT(s, x)	{ Serial.print(F(s)); Serial.print(x); }
#define	PRINTS(x)	Serial.print(F(x))
#define	PRINTX(x)	Serial.println(x, HEX)
#else
#define	PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

uint8_t degC[] = { 6, 3, 3, 56, 68, 68, 68 };	// Deg C
uint8_t degF[] = { 6, 3, 3, 124, 20, 20, 4 };	// Deg F
uint8_t	waveSine[] = { 8, 1, 14, 112, 128, 128, 112, 14, 1 };	// Sine wave
uint8_t waveSqar[] = { 8, 1, 1, 255, 128, 128, 128, 255, 1 };	// Square wave
uint8_t waveTrng[] = { 10, 2, 4, 8, 16, 32, 64, 32, 16, 8, 4 };	// Triangle wave

// Global variables
typedef struct
{
	uint8_t	spacing;	// character spacing
	char *	msg;		// message to display
} msgDef_t;

msgDef_t	M[] = 
{
  { 1, "User char" },
  { 0, "~~~~" }, 
  { 1, "24$" },
  { 0, "++++" }, 
  { 1, "40&" },
  { 0, "^^^^" } 
};
#define	MAX_STRINGS  (sizeof(M)/sizeof(M[0]))

	
void setup(void)
{
  Serial.begin(57600);
  PRINTS("\n[Parola User Char Demo]");

  P.begin();

  P.addChar('$', degC);
  P.addChar('&', degF);
  P.addChar('~', waveSine);
  P.addChar('+', waveSqar);
  P.addChar('^', waveTrng);
  
  P.setCharSpacing(M[0].spacing);
  P.displayText(M[0].msg, CENTER, P.getSpeed(), PAUSE_TIME, SCROLL_LEFT, SCROLL_LEFT);
}

void loop(void)
{
  static uint8_t	n = 1;

  if (P.displayAnimate()) 
  {
    P.setTextBuffer(M[n].msg);
    P.setCharSpacing(M[n].spacing);
    P.displayReset();
    n = (n + 1) % MAX_STRINGS;
  }
}

