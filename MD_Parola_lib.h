
#ifndef MD_PAROLALIB_H
#define	MD_PAROLALIB_H
/**
 * \file
 * \brief Contains internal library definitions
 */

#define DEBUG_PAROLA		0		///< Set to 1 to enable General debug output
#define	DEBUG_PAROLA_FSM	0		///< Set to 1 to enable Finite State Machine debug output
#define	TIME_PROFILING		0		///< Set to 1 to enable Time Profile debug output

#if  DEBUG_PAROLA
#define	PRINT(s, v)		{ Serial.print(F(s)); Serial.print(v); }		///< (GENERAL) Print a string followed by a value (decimal)
#define	PRINTX(s, v)	{ Serial.print(F(s)); Serial.print(v, HEX); }	///< (GENERAL) Print a string followed by a value (hex)
#define	PRINTS(s)		Serial.print(F(s))								///< (GENERAL) Print a string
#else
#define	PRINT(s, v)		///< (GENERAL) Print a string followed by a value (decimal)
#define	PRINTX(s, v)	///< (GENERAL) Print a string followed by a value (hex)
#define	PRINTS(s)		///< (GENERAL) Print a string
#endif

#if  DEBUG_PAROLA_FSM
#define	FSMPRINT(s, v)	{ Serial.print(F(s)); Serial.print(v); }		///< (FSM) Print a string followed by a value (decimal)
#define	FSMPRINTX(s, v)	{ Serial.print(F(s)); Serial.print(v, HEX); }	///< (FSM) Print a string followed by a value (hex)
#define	FSMPRINTS(s)	Serial.print(F(s))								///< (FSM) Print a string
#define	PRINT_STATE(f)	{ Serial.print(F("\n")); Serial.print(F(f)); Serial.print(F(" fsm ")); Serial.print(_fsmState); } ///< (FSM) Print the current FSM state information
#else
#define	FSMPRINT(s, v)	///< (FSM) Print a string followed by a value (decimal)
#define	FSMPRINTX(s, v)	///< (FSM) Print a string followed by a value (hex)
#define	FSMPRINTS(s)	///< (FSM) Print a string
#define	PRINT_STATE(f)	///< (FSM) Print the current FSM state information
#endif

#if  TIME_PROFILING
#define	TIME_PROFILE(s)		{ Serial.print(F(s)); Serial.print(millis()-_lastRunTime); }	///< (TIME) Print current execution time for the effect
#else
#define	TIME_PROFILE(s)	///< (TIME) Print current execution time for the effect
#endif

// General macros and defines
#define	LIGHT_BAR	(_inverted ? 0 : 0xFF)	///< Turn display column to all LEDs on
#define	EMPTY_BAR	(_inverted ? 0xFF : 0)	///< Turn display column to all LEDs off
#define	DATA_BAR(d)	(_inverted ? ~d : d)	///< Turn display column to specified data

// Zone column calculations
#define	ZONE_START_COL(m)	(m*COL_SIZE)		///< The first column of the first zone module
#define	ZONE_END_COL(m)		(((m+1)*COL_SIZE)-1)///< The last column of the last zone module

#endif