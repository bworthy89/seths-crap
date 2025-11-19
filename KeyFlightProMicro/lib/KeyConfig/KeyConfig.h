#ifndef KEYCONFIG_H
#define KEYCONFIG_H

#include <Arduino.h>
#include <EEPROM.h>

// Module key mapping
struct ModuleKeyMap {
    uint8_t moduleId;
    char keyCommand[32];
    char keyCommand2[32];  // For encoders (CW/CCW) and switches (ON/OFF)
};

// Configuration header (stored in RAM)
struct KeyConfigHeader {
    char signature[8];           // "KEYFPM" signature
    uint8_t version;             // Config version
    uint8_t moduleCount;         // Number of modules
};

// Full configuration structure (stored in EEPROM only)
struct KeyConfiguration {
    KeyConfigHeader header;
    ModuleKeyMap modules[40];    // Max 40 modules
};

class KeyConfig {
public:
    KeyConfig();

    // Initialize configuration
    void init();

    // Save configuration to EEPROM
    void save();

    // Load configuration from EEPROM
    bool load();

    // Reset configuration
    void reset();

    // Set module key command
    bool setModuleKey(uint8_t moduleId, const char* keyCommand, const char* keyCommand2 = "");

    // Get module key command
    const char* getModuleKey(uint8_t moduleId, int value = 0);

    // Get module count
    uint8_t getModuleCount() const { return _header.moduleCount; }

    // Check if configuration is valid
    bool isValid() const;

private:
    KeyConfigHeader _header;  // Only header in RAM
    static const int EEPROM_START_ADDR = 0;
    static const char* SIGNATURE;

    void setDefaults();
    int findModuleIndex(uint8_t moduleId);
    void readModule(int index, ModuleKeyMap& module);
    void writeModule(int index, const ModuleKeyMap& module);
};

#endif // KEYCONFIG_H
