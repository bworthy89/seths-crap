#include <Arduino.h>
#include "CommandMessenger.h"
#include "ConfigManager.h"
#include "InputModule.h"
#include "MF_Button.h"
#include "MF_Encoder.h"
#include "MF_Switch.h"
#include "MF_Analog.h"

// Serial communication
#define BAUD_RATE 115200
#define PROMICRO_SERIAL Serial1  // Serial1 for Mega-to-ProMicro communication

// Global objects
CommandMessenger cmdMessenger(Serial);
ConfigManager configManager;
InputModule* modules[40];  // Max 40 modules
uint8_t moduleCount = 0;

// Function prototypes
void setupCommandCallbacks();
void onStatus(char* args[], int argCount);
void onSetPin(char* args[], int argCount);
void onSetKey(char* args[], int argCount);
void onResetConfig(char* args[], int argCount);
void onSaveConfig(char* args[], int argCount);
void onGetConfig(char* args[], int argCount);
void sendDeviceInfo();
void loadModulesFromConfig();
void updateModules();
void checkModuleTriggers();
void sendTriggerToProMicro(uint8_t moduleId, int value);

void setup() {
    // Initialize serial communication with PC
    Serial.begin(BAUD_RATE);
    while (!Serial && millis() < 3000); // Wait up to 3 seconds for serial

    // Initialize serial communication with Pro Micro
    PROMICRO_SERIAL.begin(BAUD_RATE);

    // Initialize CommandMessenger
    cmdMessenger.init();
    setupCommandCallbacks();

    // Initialize configuration manager
    configManager.init();

    // Load modules from configuration
    loadModulesFromConfig();

    // Send startup message
    cmdMessenger.sendCommand(kDebug, "KeyFlight Mega v1.0.0 ready");
}

void loop() {
    // Process incoming commands from PC
    cmdMessenger.processCommands();

    // Update all input modules
    updateModules();

    // Check for triggered inputs
    checkModuleTriggers();

    // Small delay to prevent overwhelming the serial buffer
    delay(1);
}

void setupCommandCallbacks() {
    cmdMessenger.attach(kStatus, onStatus);
    cmdMessenger.attach(kSetPin, onSetPin);
    cmdMessenger.attach(kSetKey, onSetKey);
    cmdMessenger.attach(kResetConfig, onResetConfig);
    cmdMessenger.attach(kSaveConfig, onSaveConfig);
    cmdMessenger.attach(kGetConfig, onGetConfig);
}

// Command: kStatus - Send device information
void onStatus(char* args[], int argCount) {
    sendDeviceInfo();
}

// Command: kSetPin - Set module pin configuration
// Format: kSetPin,<id>,<type>,<pin>,<pin2>
void onSetPin(char* args[], int argCount) {
    if (argCount < 4) {
        cmdMessenger.sendCommand(kError, "Invalid kSetPin arguments");
        return;
    }

    uint8_t id = atoi(args[0]);
    uint8_t type = 0;
    uint8_t pin = atoi(args[2]);
    uint8_t pin2 = atoi(args[3]);

    // Parse type string to enum
    if (strcmp(args[1], "BUTTON") == 0) type = BUTTON;
    else if (strcmp(args[1], "ENCODER") == 0) type = ENCODER;
    else if (strcmp(args[1], "SWITCH") == 0) type = SWITCH;
    else if (strcmp(args[1], "ANALOG") == 0) type = ANALOG;
    else {
        cmdMessenger.sendCommand(kError, "Invalid module type");
        return;
    }

    // Set module configuration
    if (configManager.setModule(id, type, pin, pin2)) {
        cmdMessenger.sendCommand(kAck, "Module configured");
    } else {
        cmdMessenger.sendCommand(kError, "Failed to configure module");
    }
}

// Command: kSetKey - Set module keyboard command
// Format: kSetKey,<id>,<keyCommand>
void onSetKey(char* args[], int argCount) {
    if (argCount < 2) {
        cmdMessenger.sendCommand(kError, "Invalid kSetKey arguments");
        return;
    }

    uint8_t id = atoi(args[0]);

    // For encoders and switches, we might receive two commands
    // First call sets keyCommand, second call (if sent) sets keyCommand2
    if (configManager.setModuleKey(id, args[1])) {
        // If there's a third argument, it's keyCommand2
        if (argCount >= 3) {
            configManager.setModuleKey2(id, args[2]);
        }
        cmdMessenger.sendCommand(kAck, "Key command set");
    } else {
        cmdMessenger.sendCommand(kError, "Failed to set key command");
    }
}

// Command: kResetConfig - Reset configuration to defaults
void onResetConfig(char* args[], int argCount) {
    configManager.reset();

    // Clear all modules
    for (int i = 0; i < moduleCount; i++) {
        if (modules[i] != nullptr) {
            delete modules[i];
            modules[i] = nullptr;
        }
    }
    moduleCount = 0;

    cmdMessenger.sendCommand(kAck, "Configuration reset");
}

// Command: kSaveConfig - Save configuration to EEPROM
void onSaveConfig(char* args[], int argCount) {
    configManager.save();

    // Reload modules from saved configuration
    loadModulesFromConfig();

    cmdMessenger.sendCommand(kAck, "Configuration saved");
}

// Command: kGetConfig - Send current configuration
void onGetConfig(char* args[], int argCount) {
    char buffer[64];

    for (uint8_t i = 0; i < configManager.getModuleCount(); i++) {
        ModuleConfig* module = configManager.getModule(i);
        if (module != nullptr) {
            // Send module configuration
            // Format: kConfigInfo,<id>,<type>,<pin>,<pin2>,<keyCommand>,<keyCommand2>
            const char* typeStr = "UNKNOWN";
            switch (module->type) {
                case BUTTON: typeStr = "BUTTON"; break;
                case ENCODER: typeStr = "ENCODER"; break;
                case SWITCH: typeStr = "SWITCH"; break;
                case ANALOG: typeStr = "ANALOG"; break;
            }

            snprintf(buffer, sizeof(buffer), "%d", module->id);
            cmdMessenger.sendCommand(kConfigInfo, buffer, typeStr,
                itoa(module->pin, buffer, 10),
                itoa(module->pin2, buffer, 10));
        }
    }

    cmdMessenger.sendCommand(kAck, "Configuration sent");
}

void sendDeviceInfo() {
    // Send: kInfo,<boardType>,<firmwareVersion>,<maxModules>
    cmdMessenger.sendCommand(kInfo, BOARD_TYPE, VERSION, "40");
}

void loadModulesFromConfig() {
    // Clear existing modules
    for (int i = 0; i < moduleCount; i++) {
        if (modules[i] != nullptr) {
            delete modules[i];
            modules[i] = nullptr;
        }
    }
    moduleCount = 0;

    // Load modules from configuration
    for (uint8_t i = 0; i < configManager.getModuleCount(); i++) {
        ModuleConfig* config = configManager.getModule(i);
        if (config != nullptr && config->enabled) {
            InputModule* module = nullptr;

            // Create appropriate module type
            switch (config->type) {
                case BUTTON:
                    module = new MF_Button(config->id, config->pin);
                    break;
                case ENCODER:
                    module = new MF_Encoder(config->id, config->pin, config->pin2, config->sensitivity);
                    break;
                case SWITCH:
                    module = new MF_Switch(config->id, config->pin);
                    break;
                case ANALOG:
                    module = new MF_Analog(config->id, config->pin);
                    break;
            }

            if (module != nullptr) {
                module->init();
                modules[moduleCount++] = module;
            }
        }
    }

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Loaded %d modules", moduleCount);
    cmdMessenger.sendCommand(kDebug, buffer);
}

void updateModules() {
    for (int i = 0; i < moduleCount; i++) {
        if (modules[i] != nullptr) {
            modules[i]->update();
        }
    }
}

void checkModuleTriggers() {
    for (int i = 0; i < moduleCount; i++) {
        if (modules[i] != nullptr && modules[i]->hasTriggered()) {
            uint8_t moduleId = modules[i]->getId();
            int value = modules[i]->getTriggerValue();

            // Send trigger to Pro Micro
            sendTriggerToProMicro(moduleId, value);

            // Send trigger event to PC for logging
            char idStr[12], valueStr[12];
            itoa(moduleId, idStr, 10);
            itoa(value, valueStr, 10);
            cmdMessenger.sendCommand(kTrigger, idStr, valueStr);

            // Reset trigger
            modules[i]->resetTrigger();
        }
    }
}

void sendTriggerToProMicro(uint8_t moduleId, int value) {
    // Send trigger command to Pro Micro via Serial1
    // Format: <moduleId>,<value>;
    PROMICRO_SERIAL.print(moduleId);
    PROMICRO_SERIAL.print(',');
    PROMICRO_SERIAL.print(value);
    PROMICRO_SERIAL.println(';');
    PROMICRO_SERIAL.flush();
}
