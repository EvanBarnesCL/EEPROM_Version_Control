# EEPROM Version Control Library

The EEPROM Version Control Library allows you to store and retrieve structured version control data, such as project name, version, vendor, and release date, in the EEPROM of Arduino-compatible microcontrollers.
Designed for internal CrunchLabs use on Hack Pack.

## Features

- Store structured version control data, including:
  - Project name
  - Vendor name
  - Project version
  - Software version
  - Date final software provided to vendor
- Use safe setter methods with compile-time checks for string lengths.
- Optimized for low memory usage on devices like the ATmega328P.
- Compatible with the Arduino IDE and PlatformIO.

## Installation

1. **Install in the Arduino IDE**:
   - Download the library as a ZIP file from the [GitHub repository](https://github.com/EvanBarnesCL/EEPROM_Version_Control).
   - Open the Arduino IDE.
   - Go to `Sketch -> Include Library -> Add .ZIP Library`.
   - Select the downloaded ZIP file.

OR:

2. **Install in PlatformIO**:
   - Add the following to your `platformio.ini` file:
     ```
     lib_deps =
         https://github.com/EvanBarnesCL/EEPROM_Version_Control
     ```

## Usage

Include the library in your sketch:
```cpp
#include <EEPROM_Version_Control.h>

Modify the version control data found in CL_Version_Data.conf to suit your needs.

In your main program, write the version control data to EEPROM with EEPROMVersionControl::writeDataToEEPROM().

See the example BasicUsage.cpp for a complete example of setting and retrieving data.