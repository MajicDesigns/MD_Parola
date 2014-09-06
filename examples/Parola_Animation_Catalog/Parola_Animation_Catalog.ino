// Program to show full catalog of the MD_Parola animations
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may 
// need to be adapted
#define	MAX_DEVICES	4
#define	CLK_PIN		13
#define	DATA_PIN	11
#define	CS_PIN		10

// Hardware SPI connection
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// Generic output pins
// MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES)

#define ARRAY_SIZE(x)  (sizeof(x)/sizeof(x[0]))

// Global data
typedef struct 
{
  textEffect_t  effect;   // text effect to display
  char *        psz;      // text string nul terminated
  uint16_t      speed;    // speed multiplier of library default
  uint16_t      pause;    // pause multiplier for library default
} sCatalog;

sCatalog  catalog[] = 
{
	{ PRINT,              "PRINT",  1, 1 },
	{ SLICE,              "SLICE",  5, 1 },
	{ WIPE,               "WIPE",  15, 1 },
	{ WIPE_CURSOR,        "WPE_C", 20, 1 },
	{ OPENING,            "OPEN",  20, 1 },
	{ OPENING_CURSOR,     "OPN_C", 25, 1 },
	{ CLOSING,            "CLOSE", 20, 1 },
	{ CLOSING_CURSOR,     "CLS_C", 25, 1 },
	{ BLINDS,             "BLIND", 40, 1 },
	{ DISSOLVE,           "DSLVE", 60, 1 },
	{ SCROLL_UP,          "SC_U",  25, 1 },
	{ SCROLL_DOWN,        "SC_D",  25, 1 },
	{ SCROLL_LEFT,        "SC_L",  25, 1 },
	{ SCROLL_RIGHT,       "SC_R",  25, 1 },
	{ SCROLL_UP_LEFT,     "SC_UL", 25, 1 },
	{ SCROLL_UP_RIGHT,    "SC_UR", 25, 1 },
	{ SCROLL_DOWN_LEFT,   "SC_DL", 25, 1 },
	{ SCROLL_DOWN_RIGHT,  "SC_DR", 25, 1 },
	{ SCAN_HORIZ,         "SCANH", 20, 1 },
	{ SCAN_VERT,          "SCANV", 20, 1 },
	{ GROW_UP,            "GRW_U", 30, 1 },
	{ GROW_DOWN,          "GRW_D", 30, 1 },
};


void setup(void)
{
  P.begin();
  P.setInvert(false);
  
  for (uint8_t i=0; i<ARRAY_SIZE(catalog); i++)
  {
    catalog[i].pause *= P.getSpeed();
    catalog[i].pause *= P.getPause();
  }
}

void loop(void)
{
  for (uint8_t j=0; j<3; j++)
  {
    textPosition_t  just;
    
    switch (j)
    {
    case 0: just = LEFT;    break;
    case 1: just = CENTER;  break;
    case 2: just = RIGHT;   break;
    }    
    
    for (uint8_t i=0; i<ARRAY_SIZE(catalog); i++)
    {
      P.displayText(catalog[i].psz, just, catalog[i].speed, catalog[i].pause, catalog[i].effect, catalog[i].effect);

      while (!P.displayAnimate()) 
        ; // animates and returns true when an animation is completed
      
      delay(catalog[i].pause);
    }
  }  
}
