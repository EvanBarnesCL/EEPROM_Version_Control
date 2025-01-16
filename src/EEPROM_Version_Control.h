/**
 * Library for storing and retrieving version control data in the EEPROM.
 * 
 * The data that you can store in EEPROM can be defined in two ways:
 * - First (better way): edit the values directly. They can be found in the CL_Version_Data.conf file.
 * ---- This is better because it uses slightly less RAM, and just keeps all these function calls behind the scenes and 
 *      out of your main code.
 * - Second: use the setter methods in your main.cpp file (e.g. EEPROMVersionControl::setProjectName())
 * ---- If this is more convenient for you, these functions are available. It requires a few more bytes of RAM and a
 *      bunch of extra function calls in your main code, though.
 * 
 * Currently this stores several values in EEPROM:
 *  dataWritten: a flag used to determine if data was previously stored on the EEPROM in this location
 *  libraryVersion: the version number of this library that wrote the data into the EEPROM. Used to ensure compatibility with future versions
 *  projectName: the project projectName, not to exceed 1 character
 *  vendor: the name of the vendor this was provided to, not to exceed 7 characters
 *  projectVersion: the version number of the project (e.g., set to 2 if this is v2 of PLANT)
 *  softwareVersion: the current version of the software for the project, up to 7 characters long
 *  finalSoftwareDate: the date that the compiled version of your project code was made and supplied to the vendor. Spell month names for clarity. 18 characters max.
 */

#include <Arduino.h>
#include <EEPROM.h>
#include <CL_Version_Data.conf>

namespace EEPROMVersionControl {
    // DO NOT CHANGE
    // important values for data storage
    constexpr uint16_t EEPROM_SIZE_BYTES = E2END;            // size of EEPROM on the ATMega328P Arduino Nano
    constexpr uint16_t RESERVED_BYTES = 60;                  // the number of bytes reserved for this data at the end of the EEPROM   
    constexpr uint16_t VERSION_DATA_START_ADDRESS = EEPROM_SIZE_BYTES - RESERVED_BYTES;  // starting address for this data block
    constexpr uint16_t DATA_EXISTS_MAGIC_NUMBER = 42;       // this serves as a flag to indicate that data was previously stored in EEPROM
    constexpr uint8_t LIBRARY_VERSION = 1;                  // DO NOT CHANGE - USED TO TRACK COMPATIBILITY WITH FUTURE VERSIONS OF THIS LIBRARY

    // store strings for print debugs in PROGMEM with constants. reduces RAM useage.
    const char PROGMEM PRINT_PROJECT_NAME[] = "Project Name: ";
    const char PROGMEM PRINT_VENDOR_NAME[] = "Vendor: ";
    const char PROGMEM PRINT_PROJECT_NAME_VERSION[] = "Project Version: ";
    const char PROGMEM PRINT_SOFTWARE_VERSION[] = "Software Version: ";
    const char PROGMEM PRINT_SOFTWARE_DATE[] = "Software Date: ";
    const char PROGMEM PRINT_DATA_DNE[] = "Version data does not exist.";


    /**
     * @brief wrapper function for strncpy() that adds a null termination character
     */ 
    inline void safeStrCopy(char *dest, const char *src, size_t destSize) {
        strncpy(dest, src, destSize - 1);
        dest[destSize - 1] = '\0';  // Ensure null termination
    }

    /**
     * @brief Struct for storing version control data in EEPROM.
     * 
     * This struct holds information about the projectName, vendor, project version, 
     * software version, and the final software date. It is designed to fit 
     * within the reserved EEPROM space. Currently 55 bytes of 60 reserved bytes are used.
     */
    struct versionData {
        uint16_t dataWritten;          // if set to exactly DATA_EXISTS_MAGIC_NUMBER, there is version control data written. if false, data needs to be written. 
        uint8_t libraryVersion;        // version of this library code (EEPROM_Version_Control.h) that was used to store the data in EEPROM
        char projectName[21];          // e.g., "Tank Plant". Use official name, not the SKU. Max 20 characters.
        char vendor[2];                // "M" or "N" (or for future vendors, single first letter of their name)
        uint8_t projectVersion;        // 1 for version 1, 2 for version 2, 3 for reorder.
        char softwareVersion[8];       // e.g., "1.0.0.0", or similar for a max of 7 characters (honestly however you want to do it, within 7 characters)
        char finalSoftwareDate[19];    // e.g., "September 23, 2024" (this example is longest possible at 18 bytes) (I like writing month name for clarity)

        // constructor
        versionData()
            : dataWritten(DATA_EXISTS_MAGIC_NUMBER),
              libraryVersion(LIBRARY_VERSION),
              projectVersion(PROJECT_VERSION) {
                safeStrCopy(projectName, PROJECT_NAME, sizeof(projectName));
                safeStrCopy(vendor, VENDOR, sizeof(vendor));
                safeStrCopy(softwareVersion, SOFTWARE_VERSION, sizeof(softwareVersion));
                safeStrCopy(finalSoftwareDate, SOFTWARE_DATE, sizeof(finalSoftwareDate));
                finalSoftwareDate[sizeof(finalSoftwareDate) - 1] = '\0';  // Ensure null termination
              }
    };

    // Compile-time assertion to ensure the struct fits within the reserved EEPROM space
    static_assert(sizeof(versionData) <= RESERVED_BYTES, "versionData exceeds reserved EEPROM size!");

    /**
     * @brief check to see if version data is stored in the last 50 bytes of the EEPROM.
     * @return returns true iff the data written flag == DATA_EXISTS_MAGIC_NUMBER
     */
    inline bool dataIsWritten() {
        uint16_t dataWrittenFlag = EEPROM.read(VERSION_DATA_START_ADDRESS);
    return (dataWrittenFlag == DATA_EXISTS_MAGIC_NUMBER);
}


    /**
     * @brief Writes version data to EEPROM.
     * 
     * This function writes the `versionData` struct to the reserved EEPROM space.
     * It will overwrite existing data only if the `overwrite` parameter is set to `true` 
     * or if no data has been written yet.
     * 
     * @param dataBlock The struct of type `versionData` containing the information to store.
     * @param overwrite Set to `true` to overwrite previously written data (default: `false`).
     */
    void writeDataToEEPROM(const versionData &dataBlock, bool overwrite = false) {
        if (!dataIsWritten() || overwrite) {
            EEPROM.put(VERSION_DATA_START_ADDRESS, dataBlock);
        }
    }


    /**
     * @brief Retrieves version data from EEPROM.
     * 
     * This function reads the `versionData` struct from the reserved EEPROM space and 
     * populates the provided `versionData` object.
     * 
     * @param storedData Reference to a `versionData` object where the retrieved data will be stored.
     * @return `true` if data was successfully retrieved, `false` if no valid data exists.
     */
    bool getVersionData(versionData &storedData) {
        if (dataIsWritten()) {
            EEPROM.get(VERSION_DATA_START_ADDRESS, storedData);
            return true;
        }
        return false;
    }

    /**
     * @brief Retrieves the version number of this library (EEPROM_Version_Control.h) that was used to write data to EEPROM.
     * 
     * This is mainly provided in case we later change the fields of the versionData struct in some way that breaks
     * compatibility with the setting and retrieving functions provided here.
     * 
     * @param data Reference to a `versionData` object where the retrieved data is stored.
     * @return the version numer of the library that wrote this data into the EEPROM.
     */
    uint8_t getLibraryVersion(const versionData &data) {
        if (data.dataWritten == DATA_EXISTS_MAGIC_NUMBER) {
            Serial.print("Library version: "); Serial.println(data.libraryVersion);
        } else {
            Serial.println("Version data does not exist.");
        }
        return data.libraryVersion;
    }


    /**
     * @brief Prints the version data to the Serial monitor.
     * 
     * This function prints the projectName, vendor name, project version, software version, 
     * and final software date to the Serial monitor. If no data is available, 
     * it prints an appropriate message.
     * 
     * @param data Reference to the `versionData` struct containing the information to print.
     */
    void printVersionData(const versionData &data) {
        if (data.dataWritten == DATA_EXISTS_MAGIC_NUMBER) {
            Serial.print(reinterpret_cast<const __FlashStringHelper *>(PRINT_PROJECT_NAME));
            Serial.println(data.projectName);
            
            Serial.print(reinterpret_cast<const __FlashStringHelper *>(PRINT_VENDOR_NAME));
            Serial.println(data.vendor);

            Serial.print(reinterpret_cast<const __FlashStringHelper *>(PRINT_PROJECT_NAME_VERSION));
            Serial.println(data.projectVersion);

            Serial.print(reinterpret_cast<const __FlashStringHelper *>(PRINT_SOFTWARE_VERSION));
            Serial.println(data.softwareVersion);

            Serial.print(reinterpret_cast<const __FlashStringHelper *>(PRINT_SOFTWARE_DATE));
            Serial.println(data.finalSoftwareDate);
        } else {
            Serial.println("Version data does not exist.");
        }
    }

    ///////////////////////////////////////////////////////////////////////
    // Setter functions for safely changing data field values
    ///////////////////////////////////////////////////////////////////////


    /**
     * @brief Sets the projectName field in the versionData struct.
     * 
     * @param data Reference to the `versionData` struct.
     * @param newSKU A string representing the new projectName (maximum 5 characters).
     */
    template <size_t N>
    void setProjectName(versionData &data, const char (&newSKU)[N]) {
        static_assert(N <= 21, "Error in setProjectName: projectName exceeds maximum length of 20 characters.");
        safeStrCopy(data.projectName, newSKU, sizeof(data.projectName));
    }

    /**
     * @brief Sets the vendor field in the versionData struct.
     * 
     * @param data Reference to the `versionData` struct.
     * @param newVendor A string representing the new vendor name (maximum 7 characters).
     */
    template <size_t N>
    void setVendor(versionData &data, const char (&newVendor)[N]) {
        static_assert(N <= 2, "Error in setVendor: Vendor name exceeds maximum length of 1 character.");
        safeStrCopy(data.vendor, newVendor, sizeof(data.vendor));
    }

    /**
     * @brief Sets the software version field in the versionData struct.
     * 
     * @param data Reference to the `versionData` struct.
     * @param newVersion A string representing the new software version (maximum 7 characters).
     */
    template <size_t N>
    void setSoftwareVersion(versionData &data, const char (&newVersion)[N]) {
        static_assert(N <= 8, "Error in setSoftwareVersion: Software version exceeds maximum length of 7 characters.");
        safeStrCopy(data.softwareVersion, newVersion, sizeof(data.softwareVersion));
    }

    /**
     * @brief Sets the final software date field in the versionData struct.
     * 
     * @param data Reference to the `versionData` struct.
     * @param newDate A string representing the new software date (maximum 18 characters).
     */
    template <size_t N>
    void setFinalSoftwareDate(versionData &data, const char (&newDate)[N]) {
        static_assert(N <= 19, "Error in setFinalSoftwareDate: Final software date exceeds maximum length of 18 characters.");
        safeStrCopy(data.finalSoftwareDate, newDate, sizeof(data.finalSoftwareDate));
    }

    /**
     * @brief Sets the project version field in the versionData struct.
     * 
     * @param data Reference to the `versionData` struct.
     * @param newVersion The new project version (must be greater than 0).
     */
    void setProjectVersion(versionData &data, uint8_t newVersion) {
        data.projectVersion = newVersion;
    }
}


