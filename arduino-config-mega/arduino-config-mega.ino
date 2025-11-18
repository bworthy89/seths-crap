/*
 * Arduino Mega 2560 - Input Handler & Configuration Manager
 *
 * This sketch handles all physical inputs (buttons, encoders, switches, potentiometers)
 * and sends keyboard commands to the Arduino Pro Micro via Serial1.
 *
 * Features:
 * - Support for 30+ inputs (buttons, rotary encoders, switches, potentiometers)
 * - Rotary encoder with 4 increment modes (1s, 10s, 100s, 1000s)
 * - User-configurable pin assignments and names
 * - EEPROM storage for persistent configuration
 * - JSON configuration protocol
 * - Flexible keyboard command mapping (single keys, multiple keys, modifiers)
 *
 * Hardware: Arduino Mega 2560 (ELEGOO MEGA R3)
 *
 * Required Libraries:
 * - ArduinoJson (install via Library Manager)
 * - Bounce2 (install via Library Manager)
 * - Encoder (install via Library Manager)
 */

#include <EEPROM.h>
#include <ArduinoJson.h>
#include <Bounce2.h>
#include <Encoder.h>

// ========== CONFIGURATION CONSTANTS ==========
const unsigned long USB_BAUD_RATE = 115200;     // USB Serial (PC connection)
const unsigned long PROMICRO_BAUD_RATE = 115200; // Serial1 (Pro Micro connection)
const int MAX_INPUTS = 40;                       // Maximum number of configurable inputs
const int DEBOUNCE_MS = 5;                       // Button debounce time
const int ANALOG_THRESHOLD = 10;                 // Potentiometer noise threshold
const int EEPROM_CONFIG_ADDRESS = 0;             // EEPROM start address
const uint16_t CONFIG_MAGIC = 0xAC01;            // Magic number for config validation

// ========== INPUT TYPES ==========
enum InputType {
  INPUT_NONE = 0,
  INPUT_BUTTON = 1,
  INPUT_ENCODER = 2,
  INPUT_SWITCH = 3,
  INPUT_POT = 4
};

// ========== ENCODER INCREMENT MODES ==========
enum EncoderMode {
  MODE_1X = 0,    // Increment by 1
  MODE_10X = 1,   // Increment by 10
  MODE_100X = 2,  // Increment by 100
  MODE_1000X = 3  // Increment by 1000
};

// ========== CONFIGURATION STRUCTURES ==========
struct InputConfig {
  uint8_t pin;           // Primary pin (or first pin for encoder)
  uint8_t pin2;          // Secondary pin (for encoder only)
  uint8_t type;          // InputType
  uint8_t mode;          // EncoderMode (for encoders only)
  char name[20];         // User-defined name
  char keyCommand[16];   // Keyboard command (e.g., "A", "CTRL+F", "L+N+A+V")
  bool enabled;          // Is this input active?
};

struct Config {
  uint16_t magic;                  // Magic number for validation
  uint8_t version;                 // Config version
  InputConfig inputs[MAX_INPUTS];  // Array of input configurations
  uint16_t checksum;               // Simple checksum
};

// ========== GLOBAL VARIABLES ==========
Config config;
Bounce* buttons[MAX_INPUTS];         // Bounce objects for buttons/switches
Encoder* encoders[MAX_INPUTS];       // Encoder objects
int encoderPositions[MAX_INPUTS];    // Last encoder positions
int potValues[MAX_INPUTS];           // Last potentiometer values
bool configLoaded = false;
const int LED_PIN = 13;

// ========== SETUP ==========
void setup() {
  // Initialize serial ports
  Serial.begin(USB_BAUD_RATE);      // USB connection to PC
  Serial1.begin(PROMICRO_BAUD_RATE); // UART connection to Pro Micro

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // Initialize arrays
  for (int i = 0; i < MAX_INPUTS; i++) {
    buttons[i] = nullptr;
    encoders[i] = nullptr;
    encoderPositions[i] = 0;
    potValues[i] = 0;
  }

  // Load configuration from EEPROM
  loadConfiguration();

  // Initialize inputs based on configuration
  if (configLoaded) {
    initializeInputs();
    Serial.println(F("Configuration loaded from EEPROM"));
  } else {
    Serial.println(F("No valid configuration found. Please configure via GUI."));
  }

  Serial.println(F("Arduino Mega Input Handler Ready"));
  Serial.println(F("Send JSON configuration to update settings"));

  digitalWrite(LED_PIN, LOW);
}

// ========== MAIN LOOP ==========
void loop() {
  // Check for configuration updates from PC
  if (Serial.available() > 0) {
    handleSerialCommand();
  }

  // Process all configured inputs
  if (configLoaded) {
    processInputs();
  }

  delay(1); // Small delay to prevent overwhelming the system
}

// ========== CONFIGURATION MANAGEMENT ==========
void loadConfiguration() {
  EEPROM.get(EEPROM_CONFIG_ADDRESS, config);

  // Validate configuration
  if (config.magic == CONFIG_MAGIC && config.version == 1) {
    uint16_t calculatedChecksum = calculateChecksum();
    if (calculatedChecksum == config.checksum) {
      configLoaded = true;
    } else {
      Serial.println(F("Config checksum mismatch"));
      configLoaded = false;
    }
  } else {
    Serial.println(F("Invalid config magic/version"));
    configLoaded = false;
  }

  // Initialize default config if invalid
  if (!configLoaded) {
    initializeDefaultConfig();
  }
}

void saveConfiguration() {
  config.magic = CONFIG_MAGIC;
  config.version = 1;
  config.checksum = calculateChecksum();

  EEPROM.put(EEPROM_CONFIG_ADDRESS, config);
  Serial.println(F("Configuration saved to EEPROM"));
}

uint16_t calculateChecksum() {
  uint16_t checksum = 0;
  checksum += config.magic;
  checksum += config.version;

  for (int i = 0; i < MAX_INPUTS; i++) {
    if (config.inputs[i].enabled) {
      checksum += config.inputs[i].pin;
      checksum += config.inputs[i].pin2;
      checksum += config.inputs[i].type;
      checksum += config.inputs[i].mode;
    }
  }

  return checksum;
}

void initializeDefaultConfig() {
  config.magic = CONFIG_MAGIC;
  config.version = 1;

  for (int i = 0; i < MAX_INPUTS; i++) {
    config.inputs[i].enabled = false;
    config.inputs[i].pin = 0;
    config.inputs[i].pin2 = 0;
    config.inputs[i].type = INPUT_NONE;
    config.inputs[i].mode = MODE_1X;
    strcpy(config.inputs[i].name, "");
    strcpy(config.inputs[i].keyCommand, "");
  }

  config.checksum = calculateChecksum();
}

// ========== INPUT INITIALIZATION ==========
void initializeInputs() {
  // Clean up existing objects
  cleanupInputs();

  // Initialize each configured input
  for (int i = 0; i < MAX_INPUTS; i++) {
    if (!config.inputs[i].enabled) continue;

    switch (config.inputs[i].type) {
      case INPUT_BUTTON:
      case INPUT_SWITCH:
        initializeButton(i);
        break;

      case INPUT_ENCODER:
        initializeEncoder(i);
        break;

      case INPUT_POT:
        initializePotentiometer(i);
        break;
    }
  }
}

void cleanupInputs() {
  for (int i = 0; i < MAX_INPUTS; i++) {
    if (buttons[i] != nullptr) {
      delete buttons[i];
      buttons[i] = nullptr;
    }
    if (encoders[i] != nullptr) {
      delete encoders[i];
      encoders[i] = nullptr;
    }
  }
}

void initializeButton(int index) {
  uint8_t pin = config.inputs[index].pin;

  buttons[index] = new Bounce();
  buttons[index]->attach(pin, INPUT_PULLUP);
  buttons[index]->interval(DEBOUNCE_MS);
}

void initializeEncoder(int index) {
  uint8_t pinA = config.inputs[index].pin;
  uint8_t pinB = config.inputs[index].pin2;

  encoders[index] = new Encoder(pinA, pinB);
  encoderPositions[index] = 0;
}

void initializePotentiometer(int index) {
  uint8_t pin = config.inputs[index].pin;
  pinMode(pin, INPUT);
  potValues[index] = analogRead(pin);
}

// ========== INPUT PROCESSING ==========
void processInputs() {
  for (int i = 0; i < MAX_INPUTS; i++) {
    if (!config.inputs[i].enabled) continue;

    switch (config.inputs[i].type) {
      case INPUT_BUTTON:
      case INPUT_SWITCH:
        processButton(i);
        break;

      case INPUT_ENCODER:
        processEncoder(i);
        break;

      case INPUT_POT:
        processPotentiometer(i);
        break;
    }
  }
}

void processButton(int index) {
  if (buttons[index] == nullptr) return;

  buttons[index]->update();

  if (buttons[index]->fell()) {
    // Button pressed
    sendKeyCommand(config.inputs[index].keyCommand);
    blinkLED();
  }
}

void processEncoder(int index) {
  if (encoders[index] == nullptr) return;

  long newPosition = encoders[index]->read() / 4; // Divide by 4 for detent count
  int change = newPosition - encoderPositions[index];

  if (change != 0) {
    // Apply increment mode multiplier
    int multipliers[] = {1, 10, 100, 1000};
    int actualChange = change * multipliers[config.inputs[index].mode];

    // Send multiple key commands based on change
    int absChange = abs(actualChange);
    for (int j = 0; j < absChange && j < 100; j++) { // Limit to 100 commands
      sendKeyCommand(config.inputs[index].keyCommand);
      delay(20); // Small delay between commands
    }

    encoderPositions[index] = newPosition;
    blinkLED();
  }
}

void processPotentiometer(int index) {
  uint8_t pin = config.inputs[index].pin;
  int currentValue = analogRead(pin);

  if (abs(currentValue - potValues[index]) > ANALOG_THRESHOLD) {
    // Potentiometer value changed significantly
    sendKeyCommand(config.inputs[index].keyCommand);
    potValues[index] = currentValue;
    blinkLED();
  }
}

// ========== KEYBOARD COMMAND SENDING ==========
void sendKeyCommand(const char* command) {
  // Send command to Pro Micro via Serial1
  Serial1.println(command);

  // Echo to USB Serial for debugging
  Serial.print(F("Sent: "));
  Serial.println(command);
}

// ========== SERIAL COMMAND HANDLING ==========
void handleSerialCommand() {
  String jsonString = Serial.readStringUntil('\n');
  jsonString.trim();

  if (jsonString.length() == 0) return;

  // Parse JSON
  StaticJsonDocument<4096> doc;
  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    Serial.print(F("JSON parse error: "));
    Serial.println(error.c_str());
    return;
  }

  // Check command type
  const char* cmdType = doc["type"];
  if (cmdType == nullptr) {
    Serial.println(F("Missing 'type' field"));
    return;
  }

  if (strcmp(cmdType, "config") == 0) {
    handleConfigCommand(doc);
  } else if (strcmp(cmdType, "test") == 0) {
    handleTestCommand(doc);
  } else if (strcmp(cmdType, "status") == 0) {
    sendStatus();
  } else {
    Serial.println(F("Unknown command type"));
  }
}

void handleConfigCommand(JsonDocument& doc) {
  // Initialize new config
  initializeDefaultConfig();

  // Parse inputs array
  JsonArray inputs = doc["inputs"];
  if (inputs.isNull()) {
    Serial.println(F("Missing 'inputs' array"));
    return;
  }

  int inputCount = 0;
  for (JsonObject input : inputs) {
    if (inputCount >= MAX_INPUTS) break;

    config.inputs[inputCount].enabled = true;
    config.inputs[inputCount].pin = input["pin"];
    config.inputs[inputCount].pin2 = input["pin2"] | 0;
    config.inputs[inputCount].type = input["type"];
    config.inputs[inputCount].mode = input["mode"] | 0;

    const char* name = input["name"];
    if (name) strncpy(config.inputs[inputCount].name, name, 19);

    const char* key = input["key"];
    if (key) strncpy(config.inputs[inputCount].keyCommand, key, 15);

    inputCount++;
  }

  // Save to EEPROM
  saveConfiguration();
  configLoaded = true;

  // Reinitialize inputs
  initializeInputs();

  Serial.print(F("Configuration updated: "));
  Serial.print(inputCount);
  Serial.println(F(" inputs configured"));
}

void handleTestCommand(JsonDocument& doc) {
  const char* key = doc["key"];
  if (key) {
    sendKeyCommand(key);
    Serial.println(F("Test command sent"));
  }
}

void sendStatus() {
  StaticJsonDocument<1024> doc;

  doc["status"] = "ok";
  doc["configLoaded"] = configLoaded;
  doc["maxInputs"] = MAX_INPUTS;

  int activeInputs = 0;
  for (int i = 0; i < MAX_INPUTS; i++) {
    if (config.inputs[i].enabled) activeInputs++;
  }
  doc["activeInputs"] = activeInputs;

  serializeJson(doc, Serial);
  Serial.println();
}

// ========== UTILITY FUNCTIONS ==========
void blinkLED() {
  digitalWrite(LED_PIN, HIGH);
  delay(50);
  digitalWrite(LED_PIN, LOW);
}
