/**
\mainpage Main Page
The Parola Library
------------------
The Parola library is implemented to work with the Parola hardware. It 
depends on the MD_MAX72xx library for hardware control.

This software library implements functions to simplify the implementation 
of text special effects on the Parola display.
- Left, right or center text justification in the display field.
- Text scrolling, text entering and exit effects.
- Control display parameters and animation speed.

The latest copy of the Parola Software and hardware files can be found 
at the [Parola website] (http://parola.codeview.com).

![The final product with 8 modules connected together] (Working_Display.jpg "Working System")

System Components
-----------------
- \subpage pageHardware
- \subpage pageSoftware
- \subpage pageConnection

Revision History 
----------------
March 2014 - version 2.0
- Mods to accomodate revised font handling in MD_MAX72xx library
 + Users can now provide a user defined font PROGMEM data table
 + User code can provide individual character override for equivalent font character
- Additional animations 
 + SCAN_HORIZ, SCAN_VERT
 + GROW_UP, GROW_DOWN
 + SCROLL_UP_LEFT, SCROLL_UP_RIGHT, SCROLL_DOWN_LEFT, SCROLL_DOWN_RIGHT


September 2013 - version 1.1
- Mods to accommodate changes to hardware SPI implementation in MD_MAX72xx library

June 2013 - version 1.0
- New library

Copyright
---------
Copyright (C) 2013 Marco Colli. All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

\page pageSoftware Parola Software
The Parola Library
------------------
The Parola library is implemented using the MD_MAX72xx library for hardware 
control. The library implements functions to simplify the implementation 
of text special effects on the LED matrix.
- Text left, right or center justification in the display
- Text scrolling, appearance and disappearance effects
- Control display parameters and animation speed
- Support for hardware SPI interface

### External Dependencies
- Parola uses the MD_MAX72xx library for hardware level control primitives. 
The latest copy of this library can be found 
[here] (http://arduinocode.codeview.com).

Implementing New Text Effects
-----------------------------
Each of the selected text effects is implemented as a function. This makes it easy to add new
effects:
- Choose a name for the effect and add it to the textEffect_t enumerated type.
- Clone an existing method and modify it according to the guidelines below.
- Add the function prototype for the new effect to the class definition 
(at the bottom of the MD_Parola.h file).
- Modify the displayAnimate() method in MD_Parola.cpp to invoke the new method. 

###New Text Effects
The effects functions are implemented as finite state machines that are called with the 
frequency set by the setSpeed() method. The class variable _fsmState holds the state from 
the last invocation of an effect method.

An effect method can work in one of 2 ways:
- *Additive*: where the animation frames are incrementally built up to the initial display.
With this method, the function will need to use the getFirstChar() and getNextChar() methods 
to build up the displayed text, column by column.
- *Subtractive*: where the final displayed text is placed in the buffer using the commonPrint() 
method and the elements that are not visible at that stage of the animation are removed.

Which algorithm is used depends on the type animation and what is convenient for the coder.
Examples of both are found in the supplied library text effects.

Each effect method is implemented in 2 parts. One part implements the text move IN to the display 
(method parameter bIn is true) and the other when the text is moving OUT of the display (bIn false).
Because the IN and OUT effects can be different for a display cycle, the method must not assume 
that the first part was ever called. The first phase should always end with the text in its 
display position (depending on the alignment specified) and the second phase should assume the text 
is in that position when called. Text position parameters are held in the class variables _limitLeft and  
_limitRight found in the library header file.

The first phase starts with _fsmState set to INITIALISE and ends when the state is set to PAUSE 
within the effect method. The second phase starts with a PAUSE state and ends when the state is 
set to END by the method.  Aside from the INITIALISE state (set by the displayReset() method), 
all other state changes are under the control of the effect functions. Delays between frames and 
the pause between IN and OUT are handled outside of the effect method.

Coding Tips
-----------
+ The MD_MAX72XX library sets the origin for the LED matrix at the top right of the display. This 
makes the leftmost text column a higher column number that the far right column. Sometimes this 
is not intuitive when coding and is worth remembering. Rows are numbered from top to bottom, 0-7.

+ Ensure that a new effect is tested in combination with other effects to make sure that transitions 
are smooth and the IN and OUT effects combine well. Common errors are misaligned entry compared to 
exit, with causes a small jump in the text position when the effects are combined.

+ Display update times grow proportionally to the number of modules in a display, so some timing 
parameters may need to adapt. Hardware SPI runs approximately 10 times faster and the delay 
increase is not appreciable with up to 12 modules in length. For the arbitrary pin outs, using 
shiftout(), a 6 module chain updates in approximately 14ms on an Uno, while a 12 module display 
takes around 25ms. Most of the time taken is to physically update the display, as animating frames 
takes about 1-2ms to update in the MD_MAX72XX display buffers.

\page pageHardware Parola Hardware
Design
------
The aim was to create a 'lego-like' approach to LED matrix display, using standard 8x8 on
LED matrices. The software supports this flexibility through a scalable approach that
only requires the definition of the number of modules to adapt existing software to 
a new configuration.

![Completed Parola module with and without the LED matrix] (PCB_Actual.jpg "Parola Modules")
___

Circuit Schematic
-----------------
The schematic is the basic application circuit that is found on the MAX7219 datasheet, 
adapted to the LED matrix. Each Parola Module consists of an 8x8 LED Common Anode matrix
controlled by a MAX7219 LED controller and a few passive components. These controllers 
can be daisy chained, making them ideal for the purpose.

![Parola Circuit Schematic] (Circuit_Schematic.jpg "Parola Schematic")
___

PCB Design
----------
The PCD design was executed using the autorouting facility in Eagle CAD, and the PCB was 
manufactured by SeeedStudio. The Eagle CAD files for the layout and the Gerber files 
suitable for SeeedStudio are found on the [Parola website] (http://code.google.com/p/parola-for-arduino).
The final design includes edge connections that allow many modules to be connected
together into an extended display, one LED module in height.

![PCB layout ready for manufacture] (PCB_Layout.jpg "PCB Design")

\page pageConnection System Connections
Connecting Parola to the Arduino
--------------------------------
The modules are connected through a 4-wire serial interface (SPI), and devices are cascaded, 
with communications passed through the first device in the chain to all others. The Arduino 
should be connected to the IN side (shown in the figure below) of the first module in the chain. 

![Module Input Header] (Connector.jpg "Connection In")

The Arduino interface is implemented with 3 digital outputs that are passed through to 
the class constructor. The digital outputs define the SPI interface as follows:
- DIN (MOSI) - the Data IN signal shifts data into the display module. Data is loaded into 
the device's internal 16-bit shift register on CLK's rising edge.
- CLK (SCK) - the CLocK signal that is used to time the data for the device. 
- LD (SS) - the interface is active when LoaD signal is LOW. Serial data are loaded into the 
device shift register while LOAD is LOW and latched in on the rising edge.

Note that the LD signal is used to select the entire device chain. This allows separate LD 
outputs to control multiple Parola displays sharing the same DIN and CLK signals. The 
software needs to instantiate a separate object for each display.

The remaining interface pins are for +5V and GND. The power supply must be able to supply 
enough current for the number of connected Parola modules. The central position of the GND
connector provides some protection for accidentally reversing the connector.
___

Connecting Parola Modules
-------------------------
Separate Parola modules are connected by the plugging them together edge to edge, with the 
OUT side of one module plugged to the IN side of the next, as shown in the figure below.

![Connecting Parola modules] (Modules_conn.jpg "Modules connected")
*/
#ifndef _MD_PAROLA_H
#define _MD_PAROLA_H

#include <Arduino.h>
#include <MD_MAX72xx.h>
/**
 * \file
 * \brief Main header file for the MD_Parola library
 */

// Miscellaneous defines
#define	ARRAY_SIZE(x)	(sizeof(x)/sizeof(x[0]))	///< Generic macro for obtaining number of elements of an array

/**
 * Core object for the Parola library
 */
class MD_Parola 
{
  public:
  /**
   * Text alignment specification.
   *
   * Used to define the display text alignment and to specify direction for 
   * scrolling and animations. In the situation where LEFT AND RIGHT are the only sensible 
   * options (eg, text scrolling direction), CENTER will behave the same as LEFT.
   */
	enum textPosition_t 
	{
		LEFT,	///< The leftmost column for the first character will be on the left side of the display
		CENTER,	///< The text will be placed with equal number of blank display columns either side
		RIGHT	///< The rightmost column of the last character will be on the right side of the display
	};

  /**
   * Special effect specification.
   *
   * Used to define the effects to be used for the entry and exit of text in the display area.
   */
	enum textEffect_t 
	{
		NO_EFFECT,		///< Used as a place filler, executes no operation
		PRINT,			///< Text just appears (printed)
		SLICE,			///< Text enters and exits a slice (column) at a time from the right
		WIPE,			///< Text appears/disappears one column at a time, looks like it is wiped on and off
		WIPE_CURSOR,	///< WIPE with a light bar ahead of the change
		OPENING,		///< Appear and disappear from the center of the display, towards the ends
		OPENING_CURSOR,	///< OPENING with light bars ahead of the change
		CLOSING,		///< Appear and disappear from the ends of the display, towards the middle
		CLOSING_CURSOR,	///< CLOSING with light bars ahead of the change
		BLINDS,			///< Text is replaced behind vertical blinds
		DISSOLVE,		///< Text dissolves from one display to another
		SCROLL_UP,		///< Text scrolls up through the display
		SCROLL_DOWN,	///< Text scrolls down through the display
		SCROLL_LEFT,	///< Text scrolls right to left on the display
		SCROLL_RIGHT,	///< Text scrolls left to right on the display
		SCROLL_UP_LEFT,		///< Text moves in/out in a diagonal path up and left (North East)
		SCROLL_UP_RIGHT,	///< Text moves in/out in a diagonal path up and right (North West)
		SCROLL_DOWN_LEFT,	///< Text moves in/out in a diagonal path down and left (South East)
		SCROLL_DOWN_RIGHT,	///< Text moves in/out in a diagonal path down and right (North West)
		SCAN_HORIZ,		///< Scan one column at a time then appears/disappear at end
		SCAN_VERT,		///< Scan one row at a time then appears/disappear at end
		GROW_UP,		///< Text grows from the bottom up and shrinks from the top down 
		GROW_DOWN,		///< Text grows from the top down and and shrinks from the bottom up 
	};

  /** 
   * Class Constructor - arbitrary output pins.
   *
   * Instantiate a new instance of the class. The parameters passed are used to 
   * connect the software to the hardware using the MD_MAX72XX class.
   * 
   * See documentation for the MD_MAX72XX library for detailed explanation of parameters.
   * 
   * \param dataPin		output on the Arduino where data gets shifted out.
   * \param clkPin		output for the clock signal.
   * \param csPin		output for selecting the device.
   * \param numDevices	number of devices connected. Default is 1 if not supplied. 
   */
	MD_Parola(uint8_t dataPin, uint8_t clkPin, uint8_t csPin, uint8_t numDevices=1);

  /** 
   * Class Constructor - SPI hardware interface.
   *
   * Instantiate a new instance of the class. The parameters passed are used to 
   * connect the software to the hardware using the MD_MAX72XX class.
   * 
   * See documentation for the MD_MAX72XX library for detailed explanation of parameters.
   * 
   * \param csPin		output for selecting the device.
   * \param numDevices	number of devices connected. Default is 1 if not supplied. 
   */
	MD_Parola(uint8_t csPin, uint8_t numDevices=1);

  /** 
   * Initialise the object.
   *
   * Initialise the object data. This needs to be called during setup() to initialise new 
   * data for the class that cannot be done during the object creation.
   */
  void begin(void);

  /** 
   * Class Destructor.
   *
   * Release allocated memory and does the necessary to clean up once the object is
   * no longer required.
   */
	~MD_Parola(void);

  //--------------------------------------------------------------
  /** \name Methods for core object control.
   * @{
   */
  /** 
   * Animate the display.
   *
   * Animate the display using the currently specified text and animation parameters.
	 * This method needs to be invoked as often as possible to ensure smooth animation. 
	 * The animation is governed by a time tick that is set by the setSpeed() method
	 * and it will pause between entry and exit using the time set by the setPause() method.
	 * 
	 * The calling program should monitor the return value for 'true' in order to either
	 * reset the animation or supply another string for display.
   *
   * \return bool	true if the current animation has competed, false otherwise.
   */
	bool displayAnimate(void);

  /** 
   * Clear the display.
   *
   * Clear all the data from the current display.
   * 
   * \return No return value.
   */
	inline void displayClear(void) { _D.clear(); if (_inverted) _D.transform(MD_MAX72XX::TINV); };

  /**
   * Reset the current animation to restart.
   *
   * This method is used to reset an animation back to the start of the current cycle.
	 * It is normally invoked after all the parameters for a display are set and the 
	 * animation needs to be started (or restarted).
   *
   * \return No return value.
   */
	void displayReset(void);

  /**
   * Easy start for a scrolling text display.
   *
   * This method is a convenient way to set up a scrolling display. All the data
   * necessary for setup is passed through as parameters and the display animation
   * is started.
   *
   * \param pText	parameter suitable for the setTextBuffer() method.
   * \param align	parameter suitable for the the setTextAlignment() method.
   * \param effect	parameter suitable for the the setTextEffect() method.
   * \param speed	parameter suitable for the setSpeed() method.
   * \return No return value.
   */
	void displayScroll(char *pText, textPosition_t align, textEffect_t effect, uint16_t speed);

  /** 
   * Suspend or resume display updates.
   *
   * Stop the current animation at the point it is at. When pausing it leaves the 
	 * display showing the current text. Resuming will restart the animation where 
	 * it left off. To reset the animation back to the beginning, use the 
	 * displayReset() method.
   * 
   * \param b	boolean value to suspend (true) or resume (false).
   * \return No return value.
   */
	inline void displaySuspend(bool b) { _suspend = b; };
	
  /**
   * Easy start for a non-scrolling text display.
   *
   * This method is a convenient way to set up a static text display. All the data
   * necessary for setup is passed through as parameters and the display animation
   * is started.
   *
   * \param pText	parameter suitable for the setTextBuffer() method.
   * \param align	parameter suitable for the the setTextAlignment() method.
   * \param speed	parameter suitable for the setSpeed() method.
   * \param pause	parameter suitable for the setPause() method.
   * \param	effectIn	parameter suitable for the setTextEffect() method.
   * \param	effectOut	parameter suitable for the setTextEffect() method.
   * \return No return value.
   */
	void displayText(char *pText, textPosition_t align, uint16_t speed, uint16_t pause, textEffect_t effectIn, textEffect_t effectOut = NO_EFFECT);

  /** @} */
  //--------------------------------------------------------------
  /** \name Support methods for visually adjusting the display.
   * @{
   */

  /** 
   * Get the inter-character spacing in columns.
   * 
   * \return the current setting for the space between characters in columns.
   */
	inline uint8_t getCharSpacing(void) { return _charSpacing; };

  /** 
   * Get the current display invert state.
   * 
   * See the setInvert() method.
   *
   * \return the inverted boolean value.
   */
	inline bool getInvert(void) { return _inverted; };

  /** 
   * Get the current pause time.
   * 
   * See the setPause() method.
   *
   * \return the pause value in milliseconds.
   */
	inline uint16_t getPause(void) { return _pauseTime; };

  /** 
   * Get the current animation speed.
   * 
   * See the setSpeed() method
   * 
   * \return the speed value.
   */
	inline uint16_t getSpeed(void) { return _tickTime; };

  /** 
   * Get the current text alignment specification.
   * 
   * \return the current text alignment setting.
   */
	inline textPosition_t getTextAlignment(void) { return _textAlignment; };

  /** 
   * Set the inter-character spacing in columns.
   * 
   * Set the number of blank columns between characters when they are displayed.
   * 
   * \param cs	space between characters in columns.
   * \return No return value.
   */
	inline void setCharSpacing(uint8_t cs) { _charSpacing = cs; };

  /** 
   * Set the display brightness.
   * 
   * Set the intensity (brightness) of the display.
   * 
   * \param intensity	the intensity to set the display (0-15).
   * \return No return value.
   */
	inline void setIntensity(uint8_t intensity) { _D.control(MD_MAX72XX::INTENSITY, intensity); };

  /** 
   * Invert the display.
   * 
   * Set the display to inverted (ON LED turns OFF and vice versa).
   * 
   * \param invert	true for inverted display, false for normal display
   * \return No return value.
   */
	inline void setInvert(uint8_t invert) { _inverted = invert; };

  /** 
   * Set the pause between ENTER and EXIT animations.
   * 
   * Between each entry and exit, the library will pause by the number of milliseconds
   * specified to allow the viewer to read the message. For continuous scrolling displays
   * this should be set to the same value as the display speed.
   *
   * \param pause	the time, in milliseconds, between animations.
   * \return No return value.
   */
	inline void setPause(uint16_t pause) { _pauseTime = pause; /*(pause > _tickTime ? pause : _tickTime);*/ };

  /** 
   * Set the animation frame speed.
   * 
   * The speed of the display is the 'tick' time between animation frames. The lower this time 
   * the faster the animation; set it to zero to run as fast as possible.
   * 
   * \param speed	the time, in milliseconds, between animation frames.
   * \return No return value.
   */
	inline void setSpeed(uint16_t speed) { _tickTime = speed; /*_pauseTime = (_pauseTime > _tickTime ? _pauseTime : _tickTime); */ };

  /** 
   * Set the text alignment.
   *
   * Text alignment is specified as one of the values in textPosition_t.
   * 
   * \param ta	the required text alignment.
   * \return No return value.
   */
	inline void setTextAlignment(textPosition_t ta) { _textAlignment = ta; };

  /** 
   * Set the pointer to the text buffer.
   *
   * Sets the text buffer to be a pointer to user data. The library does not allocate
   * any memory for the text message, rather it is the calling program that supplies
   * a pointer to a buffer. This reduces memory requirements and offers the flexibility 
   * to keep a single buffer or rotate buffers with different messages, all under calling
   * program control, with no library limit to the size or numbers of buffers. The text 
   * placed in the buffer must be properly terminated by the NUL ('\0') character or 
   * processing will overrun the end of the message.
   * 
   * \param pb	pointer to the text buffer to be used.
   * \return No return value.
   */
	inline void setTextBuffer(char *pb) { _pText = pb; };

  /**
   * Set the entry and exit text effects.
   *
   * The 'in' and 'out' text effects are specified using the textEffect_t enumerated 
   * type. If no effect is required, NO_EFFECT should be specified. NO_EFFECT 
   * is most useful when no exit effect is required (e.g., when DISSOLVE is used) and
   * the entry effect is sufficient.
   *
   * \param	effectIn	the entry effect, one of the textEffect_t enumerated values.
   * \param	effectOut	the exit effect, one of the textEffect_t enumerated values.
   * \return No return value.
   */
	inline void setTextEffect(textEffect_t effectIn, textEffect_t effectOut) { _effectIn = effectIn, _effectOut = effectOut; };

  /** @} */
  //--------------------------------------------------------------
  /** \name Support methods for fonts and characters.
   * @{
   */

  /** 
   * Add a user defined character to the replacement list.
   * 
   * Add a replacement characters to the user defined list. The character data must be 
   * the same as for a single character in the font definition file. If a character is 
   * specified with a code the same as an existing character the existing data will be 
   * data will be substitited for the new data. A character code of 0 is illegal as this 
   * denotes the end of string character for C++ and cannot be used in an actual string.
   * The library does not copy the in the data in the data definition but only retains 
   * a pointer to the data, so any changes to the data storage in the calling program will
   * be reflected in the library.
   * 
   * \param code	ASCII code for the character data.
   * \param data	pointer to the character data.
   * \return true of the character was inserted in the substitution list.
   */
	bool addChar(uint8_t code, uint8_t *data);

  /** 
   * Delete a user defined character to the replacement list.
   * 
   * Delete a replacement character to the user defined list. A character code of 0 is 
   * illegal as this denotes the end of string character for C++ and cannot be used in 
   * an actual string.
   * 
   * \param code	ASCII code for the character data.
   * \return true of the character was found in the substitution list.
   */
	bool delChar(uint8_t code);

  /** 
   * Set the display font.
   * 
   * Set the display font to a user defined font table. This can be created using the 
   * MD_MAX72xx font builder (refer to documentation for the tool and the MD_MAX72xx library).
   * Passing NULL resets to the library default font. 
   * 
   * \param fontDef	Pointer to the font definition to be used.
   * \return No return value.
   */
	inline void setFont(uint8_t PROGMEM * fontDef) { _D.setFont(fontDef); };

  /** @} */

  private:
  /***
    *  Finite State machine states enumerated type.
	*/
	enum fsmState_t 
	{
		INITIALISE,		///< Initialise all variables
		GET_FIRST_CHAR,	///< Get the first character
		GET_NEXT_CHAR,	///< Get the next character
		PUT_CHAR,		///< Placing a character
		PUT_FILLER,		///< Placing filler (blank) columns 
		PAUSE,			///< Pausing between animations
		END				///< Display cycle has completed
	};

  /***
    *  Structure for list of user defined characters substitutions.
	*/
	typedef struct charDef
	{
		uint8_t	code;	///< the ASCII code for the user defined character
		uint8_t	*data;	///< user supplied data
		charDef *next;	///< next in the list
	};

	// The display hardware controlled by this library
	MD_MAX72XX	_D;

	// Time and speed controlling data and methods
	bool		_suspend;		// don't do anything
	uint32_t	_lastRunTime;	// the millis() value for when the animation was last run
	uint16_t	_tickTime;		// the time between animations in milliseconds 
	uint16_t	_pauseTime;		// time to pause the animation between 'in' and 'out'

	// Display control data and methods
	fsmState_t	_fsmState;			// fsm state for all FSMs used to display text
	uint16_t	_textLen;			// length of current text in columns
	int16_t		_limitLeft;			// leftmost limit for the current display effect
	int16_t		_limitRight;		// rightmost limit for the current display effect
	bool		_limitOverflow;		// true if the text will overflow the display
	textPosition_t	_textAlignment;	// current text alignment
	textEffect_t	_effectIn;		// the effect for text entering the display
	textEffect_t	_effectOut;		// the effect for text exiting the display
	bool		_moveIn;			// animation is moving IN when true, OUT when false
	bool		_inverted;			// true if the display needs to be inverted

	void		setInitialConditions(void);	// set up initial conditions for an effect
	uint16_t	getTextWidth(char *p);		// width of text in columns
	bool		calcTextLimits(char *p);	// calculate the right and left limits for the text

	// Variables used in the scrolling routines
	int16_t		_nextPos;			// Next position for animation. Can be used in several different ways depending on the function.
	int8_t		_posOffset;			// Looping increment depends on the direction of the scan for animation
	uint16_t	_startPos;			// Start position for the text LED
	uint16_t	_endPos;			// End limit for the text LED

	void		setInitialEffectConditions(void);	// set the initial conditions for loops in the FSM

	// Character buffer handling data and methods
	char		*_pText;		// pointer to text buffer from user call
	char		*_pCurChar;		// the current character being processed in the text
	bool		_endOfText;		// true when the end of the text string has been reached.
	void		moveTextPointer(void);	// move the text pointer depending on direction of buffer scan

	uint8_t		getFirstChar(void);	// put the first Text char into the char buffer
	uint8_t		getNextChar(void);	// put the next Text char into the char buffer

	// Font character handling data and methods
	charDef		*_userChars;		// the root of the list of user defined characters
	uint8_t		_cBuf[15];			// buffer for loading character font
	uint8_t		_charSpacing;		// spacing in columns between characters
	uint8_t		_charCols;			// number of columns for this character
	int16_t		_countCols;			// count of number of columns already shown

	uint8_t		findChar(uint8_t code, uint8_t size, uint8_t *cBuf);	// look for user defined character
	uint8_t		makeChar(char c);	// load a character bitmap and add in trailing _charSpacing blanks
	uint8_t		reverseBuf(uint8_t *p, uint8_t size);	// reverse the elements of the buffer

	// Effect functions
	void	commonPrint(void);
	void	effectPrint(bool bIn);
	void	effectSlice(bool bIn);
	void	effectWipe(bool bLightBar, bool bIn);
	void	effectOpen(bool bLightBar, bool bIn);
	void	effectClose(bool bLightBar, bool bIn);
	void	effectBlinds(bool bIn);
	void	effectDissolve(bool bIn);
	void	effectVScroll(bool bUp, bool bIn);
	void	effectHScroll(bool bLeft, bool bIn);
	void	effectHScan(bool bIn);
	void	effectVScan(bool bIn);
	void	effectGrow(bool bUp, bool bIn);
	void	effectDiag(bool bUp, bool bLeft, bool bIn);	
};

#endif
