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
    // Set header
    strncpy(_header.signature, SIGNATURE, 8);
    _header.version = 1;
    _header.moduleCount = 0;
}

void KeyConfig::save() {
    // Write header to EEPROM
    EEPROM.put(EEPROM_START_ADDR, _header);
}

bool KeyConfig::load() {
    // Read header from EEPROM
    KeyConfigHeader tempHeader;
    EEPROM.get(EEPROM_START_ADDR, tempHeader);

    // Validate signature
    if (strncmp(tempHeader.signature, SIGNATURE, 6) != 0) {
        return false;
    }

    // Validate version
    if (tempHeader.version != 1) {
        return false;
    }

    // Validate module count
    if (tempHeader.moduleCount > 40) {
        return false;
    }

    // Header is valid, copy it
    memcpy(&_header, &tempHeader, sizeof(KeyConfigHeader));
    return true;
}

void KeyConfig::reset() {
    setDefaults();
    save();
}

bool KeyConfig::setModuleKey(uint8_t moduleId, const char* keyCommand, const char* keyCommand2) {
    // Find existing module or create new one
    int index = findModuleIndex(moduleId);
    ModuleKeyMap module;

    if (index == -1) {
        // Module not found, add new one
        if (_header.moduleCount >= 40) {
            return false; // Max modules reached
        }
        index = _header.moduleCount;
        _header.moduleCount++;
        save(); // Save updated header
    } else {
        // Read existing module
        readModule(index, module);
    }

    // Set module configuration
    module.moduleId = moduleId;
    strncpy(module.keyCommand, keyCommand, 31);
    module.keyCommand[31] = '\0';

    if (keyCommand2 != nullptr && keyCommand2[0] != '\0') {
        strncpy(module.keyCommand2, keyCommand2, 31);
        module.keyCommand2[31] = '\0';
    } else {
        module.keyCommand2[0] = '\0';
    }

    // Write module back to EEPROM
    writeModule(index, module);

    return true;
}

const char* KeyConfig::getModuleKey(uint8_t moduleId, int value) {
    int index = findModuleIndex(moduleId);
    if (index == -1) return nullptr;

    // Read module from EEPROM
    static ModuleKeyMap module;  // Static to persist after return
    readModule(index, module);

    // For encoders: value = 1 (CW) uses keyCommand, value = -1 (CCW) uses keyCommand2
    // For switches: value = 1 (ON) uses keyCommand, value = 0 (OFF) uses keyCommand2
    // For buttons: always use keyCommand
    if (value < 0 || value == 0) {
        // Use keyCommand2 if available
        if (module.keyCommand2[0] != '\0') {
            return module.keyCommand2;
        }
    }

    // Default: use keyCommand
    return module.keyCommand;
}

bool KeyConfig::isValid() const {
    return (strncmp(_header.signature, SIGNATURE, 6) == 0) &&
           (_header.version == 1) &&
           (_header.moduleCount <= 40);
}

int KeyConfig::findModuleIndex(uint8_t moduleId) {
    ModuleKeyMap module;
    for (int i = 0; i < _header.moduleCount; i++) {
        readModule(i, module);
        if (module.moduleId == moduleId) {
            return i;
        }
    }
    return -1;
}

void KeyConfig::readModule(int index, ModuleKeyMap& module) {
    // Calculate EEPROM address for this module
    int addr = EEPROM_START_ADDR + sizeof(KeyConfigHeader) + (index * sizeof(ModuleKeyMap));
    EEPROM.get(addr, module);
}

void KeyConfig::writeModule(int index, const ModuleKeyMap& module) {
    // Calculate EEPROM address for this module
    int addr = EEPROM_START_ADDR + sizeof(KeyConfigHeader) + (index * sizeof(ModuleKeyMap));
    EEPROM.put(addr, module);
}
