#include "ConfigManager.h"
#include <string.h>

const char* ConfigManager::SIGNATURE = "KEYFLT";

ConfigManager::ConfigManager() {
    setDefaults();
}

void ConfigManager::init() {
    // Try to load from EEPROM
    if (!load()) {
        // If load failed, use defaults and save
        setDefaults();
        save();
    }
}

void ConfigManager::setDefaults() {
    // Set signature
    strncpy(_config.signature, SIGNATURE, 8);
    _config.version = 1;
    _config.moduleCount = 0;

    // Clear all modules
    memset(_config.modules, 0, sizeof(_config.modules));
}

void ConfigManager::save() {
    // Write configuration to EEPROM
    EEPROM.put(EEPROM_START_ADDR, _config);
}

bool ConfigManager::load() {
    // Read configuration from EEPROM
    DeviceConfig tempConfig;
    EEPROM.get(EEPROM_START_ADDR, tempConfig);

    // Validate signature
    if (strncmp(tempConfig.signature, SIGNATURE, 6) != 0) {
        return false;
    }

    // Validate version (for future compatibility)
    if (tempConfig.version != 1) {
        return false;
    }

    // Validate module count
    if (tempConfig.moduleCount > 40) {
        return false;
    }

    // Configuration is valid, copy it
    memcpy(&_config, &tempConfig, sizeof(DeviceConfig));
    return true;
}

void ConfigManager::reset() {
    setDefaults();
    save();
}

bool ConfigManager::setModule(uint8_t id, uint8_t type, uint8_t pin, uint8_t pin2) {
    // Find existing module or create new one
    int index = findModuleIndex(id);

    if (index == -1) {
        // Module not found, add new one
        if (_config.moduleCount >= 40) {
            return false; // Max modules reached
        }
        index = _config.moduleCount;
        _config.moduleCount++;
    }

    // Set module configuration
    _config.modules[index].id = id;
    _config.modules[index].type = type;
    _config.modules[index].pin = pin;
    _config.modules[index].pin2 = pin2;
    _config.modules[index].enabled = true;
    _config.modules[index].sensitivity = 1;

    // Clear key commands
    _config.modules[index].keyCommand[0] = '\0';
    _config.modules[index].keyCommand2[0] = '\0';

    return true;
}

bool ConfigManager::setModuleKey(uint8_t id, const char* keyCommand) {
    int index = findModuleIndex(id);
    if (index == -1) return false;

    strncpy(_config.modules[index].keyCommand, keyCommand, 31);
    _config.modules[index].keyCommand[31] = '\0';
    return true;
}

bool ConfigManager::setModuleKey2(uint8_t id, const char* keyCommand2) {
    int index = findModuleIndex(id);
    if (index == -1) return false;

    strncpy(_config.modules[index].keyCommand2, keyCommand2, 31);
    _config.modules[index].keyCommand2[31] = '\0';
    return true;
}

bool ConfigManager::setModuleSensitivity(uint8_t id, uint16_t sensitivity) {
    int index = findModuleIndex(id);
    if (index == -1) return false;

    _config.modules[index].sensitivity = sensitivity;
    return true;
}

bool ConfigManager::setModuleEnabled(uint8_t id, bool enabled) {
    int index = findModuleIndex(id);
    if (index == -1) return false;

    _config.modules[index].enabled = enabled;
    return true;
}

ModuleConfig* ConfigManager::getModule(uint8_t id) {
    int index = findModuleIndex(id);
    if (index == -1) return nullptr;

    return &_config.modules[index];
}

void ConfigManager::removeModule(uint8_t id) {
    int index = findModuleIndex(id);
    if (index == -1) return;

    // Shift all modules after this one down
    for (int i = index; i < _config.moduleCount - 1; i++) {
        memcpy(&_config.modules[i], &_config.modules[i + 1], sizeof(ModuleConfig));
    }

    _config.moduleCount--;

    // Clear last module
    memset(&_config.modules[_config.moduleCount], 0, sizeof(ModuleConfig));
}

bool ConfigManager::isValid() const {
    return (strncmp(_config.signature, SIGNATURE, 6) == 0) &&
           (_config.version == 1) &&
           (_config.moduleCount <= 40);
}

int ConfigManager::findModuleIndex(uint8_t id) {
    for (int i = 0; i < _config.moduleCount; i++) {
        if (_config.modules[i].id == id) {
            return i;
        }
    }
    return -1;
}
