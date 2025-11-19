#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <Arduino.h>
#include <EEPROM.h>

// Module configuration structure
struct ModuleConfig {
    uint8_t id;
    uint8_t type;        // InputType enum value
    uint8_t pin;
    uint8_t pin2;
    char keyCommand[32];
    char keyCommand2[32];
    uint16_t sensitivity;
    bool enabled;
};

// Device configuration structure
struct DeviceConfig {
    char signature[8];     // "KEYFLT" signature for validation
    uint8_t version;       // Config version
    uint8_t moduleCount;   // Number of modules
    ModuleConfig modules[40]; // Max 40 modules
};

class ConfigManager {
public:
    ConfigManager();

    // Initialize configuration (load from EEPROM or set defaults)
    void init();

    // Save configuration to EEPROM
    void save();

    // Load configuration from EEPROM
    bool load();

    // Reset configuration to defaults
    void reset();

    // Add or update module configuration
    bool setModule(uint8_t id, uint8_t type, uint8_t pin, uint8_t pin2);
    bool setModuleKey(uint8_t id, const char* keyCommand);
    bool setModuleKey2(uint8_t id, const char* keyCommand2);
    bool setModuleSensitivity(uint8_t id, uint16_t sensitivity);
    bool setModuleEnabled(uint8_t id, bool enabled);

    // Get module configuration
    ModuleConfig* getModule(uint8_t id);
    uint8_t getModuleCount() const { return _config.moduleCount; }

    // Remove module
    void removeModule(uint8_t id);

    // Validate configuration
    bool isValid() const;

private:
    DeviceConfig _config;
    static const int EEPROM_START_ADDR = 0;
    static const char* SIGNATURE;

    void setDefaults();
    int findModuleIndex(uint8_t id);
};

#endif // CONFIGMANAGER_H
