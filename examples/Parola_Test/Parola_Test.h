// typedefs to circumvent Arduino IDE complaints

typedef struct
{
	uint8_t		swPin;		// the pin number
	bool		swActive;	// the last status
	uint32_t	swActTime;	// the millis() time it was last activated
} switchDef_t;

