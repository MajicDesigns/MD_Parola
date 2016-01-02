// Program to show full catalog of the MD_Parola animations
//

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
  { PRINT,              "PRINT", 1, 1 },
  { SLICE,              "SLICE", 1, 1 },
  { MESH,               "MESH",  20, 1 },
  { FADE, 				"FADE",  20, 1 },
  { WIPE,               "WIPE",  5, 1 },
  { WIPE_CURSOR,        "WPE_C", 4, 1 },
  { OPENING,            "OPEN",  3, 1 },
  { OPENING_CURSOR,     "OPN_C", 4, 1 },
  { CLOSING,            "CLOSE", 3, 1 },
  { CLOSING_CURSOR,     "CLS_C", 4, 1 },
  { BLINDS,             "BLIND", 7, 1 },
  { DISSOLVE,           "DSLVE", 7, 1 },
  { SCROLL_UP,          "SC_U",  5, 1 },
  { SCROLL_DOWN,        "SC_D",  5, 1 },
  { SCROLL_LEFT,        "SC_L",  5, 1 },
  { SCROLL_RIGHT,       "SC_R",  5, 1 },
  { SCROLL_UP_LEFT,     "SC_UL", 7, 1 },
  { SCROLL_UP_RIGHT,    "SC_UR", 7, 1 },
  { SCROLL_DOWN_LEFT,   "SC_DL", 7, 1 },
  { SCROLL_DOWN_RIGHT,  "SC_DR", 7, 1 },
  { SCAN_HORIZ,         "SCANH", 4, 1 },
  { SCAN_VERT,          "SCANV", 3, 1 },
  { GROW_UP,            "GRW_U", 7, 1 },
  { GROW_DOWN,          "GRW_D", 7, 1 },
};


void setup(void)
{
  P.begin();
  P.setInvert(false);
  
  for (uint8_t i=0; i<ARRAY_SIZE(catalog); i++)
  {
    catalog[i].speed *= P.getSpeed();
    catalog[i].pause *= 500;
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
