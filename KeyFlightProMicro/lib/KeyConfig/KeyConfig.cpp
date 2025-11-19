#include "KeyConfig.h"
#include <string.h>

const char* KeyConfig::SIGNATURE = "KEYFPM"; // KeyFlight Pro Micro

KeyConfig::KeyConfig() {
    setDefaults();
}

void KeyConfig::init() {
    // Try to load from EEPROM
    if (!load()) {
        // If load failed, use defaults and save
        setDefaults();
        save();
    }
}

void KeyConfig::setDefaults() {
    // Set signature
    strncpy(_config.signature, SIGNATURE, 8);
    _config.version = 1;
    _config.moduleCount = 0;

    // Clear all modules
    memset(_config.modules, 0, sizeof(_config.modules));
}

void KeyConfig::save() {
    // Write configuration to EEPROM
    EEPROM.put(EEPROM_START_ADDR, _config);
}

bool KeyConfig::load() {
    // Read configuration from EEPROM
    KeyConfiguration tempConfig;
    EEPROM.get(EEPROM_START_ADDR, tempConfig);

    // Validate signature
    if (strncmp(tempConfig.signature, SIGNATURE, 6) != 0) {
        return false;
    }

    // Validate version
    if (tempConfig.version != 1) {
        return false;
    }

    // Validate module count
    if (tempConfig.moduleCount > 40) {
        return false;
    }

    // Configuration is valid, copy it
    memcpy(&_config, &tempConfig, sizeof(KeyConfiguration));
    return true;
}

void KeyConfig::reset() {
    setDefaults();
    save();
}

bool KeyConfig::setModuleKey(uint8_t moduleId, const char* keyCommand, const char* keyCommand2) {
    // Find existing module or create new one
    int index = findModuleIndex(moduleId);

    if (index == -1) {
        // Module not found, add new one
        if (_config.moduleCount >= 40) {
            return false; // Max modules reached
        }
        index = _config.moduleCount;
        _config.moduleCount++;
    }

    // Set module configuration
    _config.modules[index].moduleId = moduleId;
    strncpy(_config.modules[index].keyCommand, keyCommand, 31);
    _config.modules[index].keyCommand[31] = '\0';

    if (keyCommand2 != nullptr && keyCommand2[0] != '\0') {
        strncpy(_config.modules[index].keyCommand2, keyCommand2, 31);
        _config.modules[index].keyCommand2[31] = '\0';
    } else {
        _config.modules[index].keyCommand2[0] = '\0';
    }

    return true;
}

const char* KeyConfig::getModuleKey(uint8_t moduleId, int value) {
    int index = findModuleIndex(moduleId);
    if (index == -1) return nullptr;

    // For encoders: value = 1 (CW) uses keyCommand, value = -1 (CCW) uses keyCommand2
    // For switches: value = 1 (ON) uses keyCommand, value = 0 (OFF) uses keyCommand2
    // For buttons: always use keyCommand
    if (value < 0 || value == 0) {
        // Use keyCommand2 if available
        if (_config.modules[index].keyCommand2[0] != '\0') {
            return _config.modules[index].keyCommand2;
        }
    }

    // Default: use keyCommand
    return _config.modules[index].keyCommand;
}

bool KeyConfig::isValid() const {
    return (strncmp(_config.signature, SIGNATURE, 6) == 0) &&
           (_config.version == 1) &&
           (_config.moduleCount <= 40);
}

int KeyConfig::findModuleIndex(uint8_t moduleId) {
    for (int i = 0; i < _config.moduleCount; i++) {
        if (_config.modules[i].moduleId == moduleId) {
            return i;
        }
    }
    return -1;
}
