# Library for modular scrolling LED matrix text displays

[Version 1.0 Video](http://www.youtube.com/watch?v=JgzVCSFaz3I)

[Version 2.0 Video](http://www.youtube.com/watch?v=u1iELyROjW8)

[Library Documentation](https://majicdesigns.github.io/Parola/)

Parola is a modular scrolling text display using MAX7219 or MAX7221 LED matrix display controllers using Arduino. The display is made up of any number of identical modules that are plugged together to create a wider/longer display.
* Text left, right or center justification in the display
* Text scrolling, entry and exit effects
* Control display parameters and animation speed
* Support for hardware SPI interface
* Multiple virtual displays (zones) in each string of LED modules
* User defined fonts and/or individual characters substitutions
* Support for double height displays

The aim was to create a 'lego-like' LED matrix display, using standard 8x8 LED matrices. The software supports this flexibility through a scalable approach that only requires the definition of the number of modules to adapt existing software to a new configuration.

The Parola software has a dependency on the [MD_MAX72xx Arduino library](https://github.com/MajicDesigns/MAX72xx).  The library implements functions to simplify the implementation of text special effects on the LED matrix.

Parola discussion on the [Arduino forum](http://forum.arduino.cc/index.php?topic=171056.0) and kits available from [ElectroDragon](http://www.electrodragon.com/product/dot-matrix-chain-display-kit-max7219-v2).

Additional information also at [my blog](http://arduinoplusplus.wordpress.com).
