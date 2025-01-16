#include <Arduino.h>
#include <EEPROM_Version_Control.h>

/**
 * EEPROM data dumper
*/

EEPROMVersionControl::versionData retrievedData;          // struct for retrieving already written data from EEPROM

void setup() {
  Serial.begin(115200);
  EEPROMVersionControl::getVersionData(retrievedData);        // getVersionData returns false if no valid data exists to retrieve
  EEPROMVersionControl::printVersionData(retrievedData);
}

void loop() {

}
