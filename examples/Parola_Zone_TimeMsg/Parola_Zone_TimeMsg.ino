// Program to demonstrate the MD_Parola library
//
// Display the time in one zone and other information scrolling through in
// another zone. 
// - Time is shown in a user defined fixed width font 
// - Scrolling text uses the default font
// - Temperature display uses user defined characters
// - Optional use of DS1307 module for time and DHT11 sensor for temp and humidity
//   - DS1307 library (MD_DS1307) found at http://arduinocode.codeplex.com
//   - DHT11 library (DHT11_lib) found at http://arduino.cc/playground/Main/DHT11Lib
//

// Use the DHT11 temp and humidity sensor
#define	USE_DHT11	1

// Use the DS1307 clock module
#define	USE_DS1307	1

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may 
// need to be adapted
#define	MAX_DEVICES	10

#define	CLK_PIN		13
#define	DATA_PIN	11
#define	CS_PIN		10

// Header file includes
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include "Font_Data.h"

#if	USE_DHT11
#include <dht11.h>

#define	DHT11_PIN	2

dht11	DHT11;
#endif

#if	USE_DS1307
#include <MD_DS1307.h>
#include <Wire.h>
#endif

// Hardware SPI connection
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// Generic output pins
// MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define	SPEED_TIME	75
#define	PAUSE_TIME	0

// Turn on debug statements to the serial output
#define  DEBUG  0

// Global variables
char	szTime[9];		// mm:ss\0
char	szMesg[20] = "";

#if USE_DHT11
uint8_t degC[] = { 6, 3, 3, 56, 68, 68, 68 };	// Deg C
uint8_t degF[] = { 6, 3, 3, 124, 20, 20, 4 };	// Deg F
#endif

char *mon2str(uint8_t mon)
{
  static char *str[] = 
  {
	"Jan", "Feb", "Mar", "Apr", 
	"May", "Jun", "Jul", "Aug", 
	"Sep", "Oct", "Nov", "Dec"
  };
  
  return(str[mon-1]);
}

char *dow2str(uint8_t code)
{
  static char *str[] = 
  {
	"Sunday", "Monday", "Tuesday", 
	"Wednesday", "Thursday", "Friday", 
	"Saturday"
  };
  
  return(str[code-1]);
}

void getTime(char *psz, bool f = true)
// Code for reading clock time
{
#if	USE_DS1307
	RTC.ReadTime();
	sprintf(psz, "%02d%c%02d", RTC.h, (f ? ':' : ' '), RTC.m);
#else
	uint16_t	h, m, s;
	
	s = millis()/1000;
	m = s/60;
	h = m/60;
	m %= 60;
	s %= 60;
	sprintf(psz, "%02d%c%02d", h, (f ? ':' : ' '), m);
#endif
}

void getDate(char *psz)
// Code for reading clock date
{
#if	USE_DS1307
	RTC.ReadTime();
	sprintf(psz, "%d %s %04d", RTC.dd, mon2str(RTC.mm), RTC.yyyy);
#else
	strcpy(szMesg, "29 Feb 2016");
#endif
}

void setup(void)
{
  P.begin(2);
  P.setInvert(false);
  
  P.setZone(0, 0, MAX_DEVICES-5);
  P.setZone(1, MAX_DEVICES-4, MAX_DEVICES-1);
  P.setFont(1, numeric7Seg);

  P.displayZoneText(1, szTime, CENTER, SPEED_TIME, PAUSE_TIME, PRINT, NO_EFFECT);
  P.displayZoneText(0, szMesg, CENTER, SPEED_TIME, 0, SCROLL_LEFT, SCROLL_LEFT);
  
#if USE_DHT11
  P.addChar('$', degC);
  P.addChar('&', degF);
#endif

#if USE_DS1307
  RTC.Control(DS1307_CLOCK_HALT, DS1307_OFF);
  RTC.Control(DS1307_12H, DS1307_OFF);
#endif

  getTime(szTime);
}

void loop(void)
{
	static uint32_t	lastTime = 0;		// millis() memory
	static uint8_t	display = 0;		// current display mode
	static bool		flasher = false;	// seconds passing flasher
  
	P.displayAnimate();
  
	if (P.getZoneStatus(0))
	{
		switch (display)
		{
		case 0:	// Temperature deg C
			display++;
#if USE_DHT11
			if (DHT11.read(DHT11_PIN) == 0)
			{
				dtostrf(DHT11.temperature, 3, 1, szMesg);
				strcat(szMesg, "$"); 
			}
#else
			strcpy(szMesg, "22.0$");
#endif
			break;
			
		case 1:	// Temperature deg F
			display++;
#if USE_DHT11
			if (DHT11.read(DHT11_PIN) == 0)
			{
				dtostrf((1.8 * DHT11.temperature)+32, 3, 1, szMesg);
				strcat(szMesg, "&"); 
			}
#else
			strcpy(szMesg, "71.6&");
#endif
			break;

		case 2:	// Relative Humidity
			display++;
#if	USE_DHT11
			if (DHT11.read(DHT11_PIN) == 0)
			{
				dtostrf(DHT11.humidity, 3, 0, szMesg);
				strcat(szMesg, "% RH"); 
			}
#else
			strcpy("36 % RH");
#endif
			break;

		case 3:	// day of week
			display++;
#if	USE_DS1307
			//sprintf(szMesg, "%02d", RTC.dow);
			strcpy(szMesg, dow2str(RTC.dow));			
#else
			strcpy(szMesg, dow2str(4));
#endif
			break;

		default:	// Calendar
			display = 0;
			getDate(szMesg);
			break;
		}
		
		P.displayReset(0); 
	}
  
  // Finally, adjust the time string if we have to
  if (millis() - lastTime >= 1000)
  {
	  lastTime = millis();
	  getTime(szTime, flasher);
	  flasher = !flasher;
	  
	  P.displayReset(1);
  }
}

