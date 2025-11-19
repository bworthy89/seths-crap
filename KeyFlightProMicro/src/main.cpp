#include <Arduino.h>
#include "SimpleKeyboard.h"
#include "KeyConfig.h"
#include "KeyExecutor.h"

// Serial communication
#define BAUD_RATE 115200
#define MEGA_SERIAL Serial1  // Serial1 for communication with Mega

// Global objects
KeyConfig keyConfig;
KeyExecutor keyExecutor;

// Serial buffer (reduced for RAM savings)
char serialBuffer[64];
int bufferIndex = 0;

// Function prototypes
void processSerialData();
void processCommand(char* commandStr);
void processTrigger(uint8_t moduleId, int value);
void processConfig(char* args[], int argCount);
void sendDebug(const char* message);

void setup() {
    // Initialize serial communication with Mega
    MEGA_SERIAL.begin(BAUD_RATE);

    // Initialize USB HID keyboard
    keyExecutor.init();

    // Initialize configuration
    keyConfig.init();

    // Built-in LED for status indication
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);  // LED on when ready

    // Small delay to ensure everything is initialized
    delay(1000);

    sendDebug("KeyFlight ProMicro v1.0.0 ready");
}

void loop() {
    // Process incoming data from Mega
    processSerialData();

    // Small delay to prevent overwhelming the serial buffer
    delay(1);
}

void processSerialData() {
    while (MEGA_SERIAL.available() > 0) {
        char c = MEGA_SERIAL.read();

        // Check for command separator
        if (c == ';' || c == '\n') {
            if (bufferIndex > 0) {
                serialBuffer[bufferIndex] = '\0';
                processCommand(serialBuffer);
                bufferIndex = 0;
            }
        }
        // Check for carriage return (ignore)
        else if (c == '\r') {
            // Ignore
        }
        // Add to buffer
        else if (bufferIndex < sizeof(serialBuffer) - 1) {
            serialBuffer[bufferIndex++] = c;
        }
        // Buffer overflow
        else {
            sendDebug("Buffer overflow");
            bufferIndex = 0;
        }
    }
}

void processCommand(char* commandStr) {
    // Parse command
    // Format options:
    // 1. Trigger: "<moduleId>,<value>" e.g., "0,0" or "1,1"
    // 2. Config: "CFG,<moduleId>,<keyCommand>,<keyCommand2>" e.g., "CFG,0,SPACE"
    // 3. Save: "SAVE"
    // 4. Reset: "RESET"

    // Check for special commands
    if (strcmp(commandStr, "SAVE") == 0) {
        keyConfig.save();
        sendDebug("Config saved");
        return;
    }

    if (strcmp(commandStr, "RESET") == 0) {
        keyConfig.reset();
        sendDebug("Config reset");
        return;
    }

    // Tokenize by comma
    char* args[5];
    int argCount = 0;
    char* token = strtok(commandStr, ",");
    while (token != nullptr && argCount < 5) {
        args[argCount++] = token;
        token = strtok(nullptr, ",");
    }

    if (argCount == 0) return;

    // Check if first argument is "CFG"
    if (strcmp(args[0], "CFG") == 0) {
        processConfig(args, argCount);
        return;
    }

    // Otherwise, treat as trigger: <moduleId>,<value>
    if (argCount >= 1) {
        uint8_t moduleId = atoi(args[0]);
        int value = (argCount >= 2) ? atoi(args[1]) : 0;
        processTrigger(moduleId, value);
    }
}

void processTrigger(uint8_t moduleId, int value) {
    // Get key command for module
    const char* keyCommand = keyConfig.getModuleKey(moduleId, value);

    if (keyCommand == nullptr || keyCommand[0] == '\0') {
        // No key command configured for this module
        sendDebug("No key command for module");
        return;
    }

    // Execute keyboard command
    if (keyExecutor.execute(keyCommand)) {
        // Success - blink LED
        digitalWrite(LED_BUILTIN, LOW);
        delay(50);
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
        // Failed to parse command
        sendDebug("Failed to execute key command");
    }
}

void processConfig(char* args[], int argCount) {
    // Format: CFG,<moduleId>,<keyCommand>,<keyCommand2>
    // argCount: 1=CFG, 2=moduleId, 3=keyCommand, 4=keyCommand2 (optional)

    if (argCount < 3) {
        sendDebug("Invalid CFG command");
        return;
    }

    uint8_t moduleId = atoi(args[1]);
    const char* keyCommand = args[2];
    const char* keyCommand2 = (argCount >= 4) ? args[3] : "";

    if (keyConfig.setModuleKey(moduleId, keyCommand, keyCommand2)) {
        sendDebug("Module configured");
    } else {
        sendDebug("Failed to configure module");
    }
}

void sendDebug(const char* message) {
    MEGA_SERIAL.print("[PM] ");
    MEGA_SERIAL.println(message);
}
