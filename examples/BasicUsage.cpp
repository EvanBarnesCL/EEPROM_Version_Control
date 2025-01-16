#include <Arduino.h>
#include <EEPROM_Version_Control.h>

/**
 * Notes on use:
 * 
 * If you only need to write data to EEPROM for your project and don't need to retrieve it, be sure not to use the
 * EEPROMVersionControl::printVersionData() function. This function relies on Serial.print statements. If you exclude
 * this function and don't use the Serial library anywhere else in your program, you can save about 6% of available RAM. 
 * 
 * If you avoid using the setter functions to create the version data you plan to write to EEPROM, you can save a few more
 * bytes of available RAM. I put the setter functions in here for convenience, but it's better to write the data directly
 * in the EEPROM_Version_Control.h file. 
*/

EEPROMVersionControl::versionData projectVersionData;     // struct for storing the data we plan to write to EEPROM
EEPROMVersionControl::versionData retrievedData;          // struct for retrieving already written data from EEPROM

void setup() {
  Serial.begin(115200);
  
  // first, examine the default data:
  Serial.println("First, let's see what data will be written to EEPROM: ");
  EEPROMVersionControl::printVersionData(projectVersionData);

  // If needed, you can use the setter methods to safely change the fields that will be written.
  // If you change it to a value that is too large for the field you're writing to, the compiler
  // will throw an error. For example, uncomment the following lines to change the matching fields:
  
  // EEPROMVersionControl::setProjectName(projectVersionData, "Tank Plant");             // Project name (not SKU)
  EEPROMVersionControl::setProjectVersion(projectVersionData, 2);                     // V2 project
  EEPROMVersionControl::setFinalSoftwareDate(projectVersionData, "April 3, 2025");    // a new date for when the final version of the software was provided to the vendor
  EEPROMVersionControl::setSoftwareVersion(projectVersionData, "3.1.1");              // new version of the software, provided to the vendor on the date above
  EEPROMVersionControl::setVendor(projectVersionData, "N");                           // new vendor


  // Now write the data to EEPROM for long term storage:
  EEPROMVersionControl::writeDataToEEPROM(projectVersionData, true);    // Setting second parameter to true forces an overwrite of previous data

  // Finally, you can retrieve previously written data from EEPROM:
  Serial.println("\nNow retrieve the data from EEPROM: ");
  EEPROMVersionControl::getVersionData(retrievedData);        // getVersionData returns false if no valid data exists to retrieve
  EEPROMVersionControl::printVersionData(retrievedData);
}





void loop() {

}
