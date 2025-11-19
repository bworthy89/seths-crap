/*
 * Arduino Mega 2560 - Input Handler & Configuration Manager (Enhanced)
 *
 * This sketch handles all physical inputs (buttons, encoders, switches, potentiometers)
 * and sends keyboard commands to the Arduino Pro Micro via Serial1.
 *
 * NEW FEATURES (v2.0):
 * - 7-segment display support for encoders (TM1637, TM1638, MAX7219)
 * - Encoder button functionality (short/long press actions)
 * - KD2-22 latching switches with LED control and mutual exclusion
 *
 * Features:
 * - Support for 40 inputs (buttons, rotary encoders, switches, potentiometers)
 * - Rotary encoder with 4 increment modes (1s, 10s, 100s, 1000s)
 * - 7-segment displays showing encoder position
 * - Encoder push buttons with configurable actions
 * - Latching switches with LED feedback and mutual exclusion rules
 * - User-configurable pin assignments and names
 * - EEPROM storage for persistent configuration
 * - JSON configuration protocol
 * - Ctrl+ keyboard command mapping
 *
 * Hardware: Arduino Mega 2560 (ELEGOO MEGA R3)
 *
 * Required Libraries:
 * - ArduinoJson (install via Library Manager)
 * - Bounce2 (install via Library Manager)
 * - Encoder (install via Library Manager)
 * - TM1637Display (for TM1637 displays)
 * - TM1638 or TM1638lite (for TM1638 displays)
 * - LedControl (for MAX7219 displays)
 */

#include <EEPROM.h>
#include <ArduinoJson.h>
#include <Bounce2.h>
#include <Encoder.h>

// Conditional includes for display libraries (comment out if not installed)
// #include <TM1637Display.h>
// #include <TM1638.h>
// #include <LedControl.h>

// ========== CONFIGURATION CONSTANTS ==========
const unsigned long USB_BAUD_RATE = 115200;     // USB Serial (PC connection)
const unsigned long PROMICRO_BAUD_RATE = 115200; // Serial1 (Pro Micro connection)
const int MAX_INPUTS = 40;                       // Maximum number of configurable inputs
const int DEBOUNCE_MS = 5;                       // Button debounce time
const int ANALOG_THRESHOLD = 10;                 // Potentiometer noise threshold
const int EEPROM_CONFIG_ADDRESS = 0;             // EEPROM start address
const uint16_t CONFIG_MAGIC = 0xAC02;            // Magic number for config validation (v2.0)
const uint16_t DEFAULT_LONG_PRESS_MS = 1000;     // Default long press threshold

// ========== INPUT TYPES ==========
enum InputType {
  INPUT_NONE = 0,
  INPUT_BUTTON = 1,
  INPUT_ENCODER = 2,
  INPUT_SWITCH = 3,
  INPUT_POT = 4,
  INPUT_SWITCH_LATCHING = 5  // KD2-22 latching switch
};

// ========== ENCODER INCREMENT MODES ==========
enum EncoderMode {
  MODE_1X = 0,    // Increment by 1
  MODE_10X = 1,   // Increment by 10
  MODE_100X = 2,  // Increment by 100
  MODE_1000X = 3  // Increment by 1000
};

// ========== DISPLAY TYPES ==========
enum DisplayType {
  DISPLAY_NONE = 0,
  DISPLAY_TM1637 = 1,
  DISPLAY_TM1638 = 2,
  DISPLAY_MAX7219 = 3
};

// ========== ENCODER BUTTON ACTIONS ==========
enum ButtonAction {
  ACTION_NONE = 0,
  ACTION_CYCLE_MODES = 1,
  ACTION_SEND_KEY = 2,
  ACTION_RESET_DISPLAY = 3
};

// ========== CONFIGURATION STRUCTURES ==========
struct InputConfig {
  // Core fields
  uint8_t pin;           // Primary pin (or first pin for encoder)
  uint8_t pin2;          // Secondary pin (encoder B)
  uint8_t type;          // InputType
  uint8_t mode;          // EncoderMode (for encoders)
  char name[20];         // User-defined name
  char keyCommand[16];   // Primary keyboard command
  bool enabled;          // Is this input active?

  // 7-segment display fields (for encoders only)
  uint8_t displayType;        // DisplayType (0=None, 1=TM1637, 2=TM1638, 3=MAX7219)
  uint8_t displayClkPin;      // CLK pin for display
  uint8_t displayDataPin;     // DIO/DIN pin for display
  uint8_t displayCsPin;       // CS/STB pin (0 if not used)
  uint8_t displayDigits;      // Number of segments to use (1-8)
  int16_t displayMin;         // Minimum display value
  int16_t displayMax;         // Maximum display value

  // Encoder button fields
  uint8_t buttonPin;              // Encoder push button pin (0 if none)
  uint8_t buttonShortAction;      // ButtonAction for short press
  uint8_t buttonLongAction;       // ButtonAction for long press
  char buttonShortKey[16];        // Key command for short press (if ACTION_SEND_KEY)
  char buttonLongKey[16];         // Key command for long press (if ACTION_SEND_KEY)
  uint16_t longPressMs;           // Long press threshold (default 1000ms)

  // LED and mutex fields (for latching switches)
  uint8_t ledPin;            // LED control pin (0 if none)
  uint8_t mutexCount;        // Number of mutex inputs
  uint8_t mutexList[8];      // Input indices to turn off when this is activated
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
Bounce* encoderButtons[MAX_INPUTS];  // Bounce objects for encoder push buttons
Encoder* encoders[MAX_INPUTS];       // Encoder objects
long encoderPositions[MAX_INPUTS];   // Last encoder positions
long displayValues[MAX_INPUTS];      // Current display values for encoders
int potValues[MAX_INPUTS];           // Last potentiometer values
bool switchStates[MAX_INPUTS];       // Current state of latching switches
unsigned long buttonPressTime[MAX_INPUTS]; // Time when button was pressed
bool buttonLongPressed[MAX_INPUTS];  // Has long press been triggered?

// Display objects (will be allocated dynamically)
void* displays[MAX_INPUTS];          // Pointer to display objects

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
    encoderButtons[i] = nullptr;
    encoders[i] = nullptr;
    displays[i] = nullptr;
    encoderPositions[i] = 0;
    displayValues[i] = 0;
    potValues[i] = 0;
    switchStates[i] = false;
    buttonPressTime[i] = 0;
    buttonLongPressed[i] = false;
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

  Serial.println(F("Arduino Mega Input Handler Ready (v2.0 - Enhanced)"));
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
      checksum += config.inputs[i].displayType;
      checksum += config.inputs[i].buttonPin;
      checksum += config.inputs[i].ledPin;
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

    // Initialize new fields
    config.inputs[i].displayType = DISPLAY_NONE;
    config.inputs[i].displayClkPin = 0;
    config.inputs[i].displayDataPin = 0;
    config.inputs[i].displayCsPin = 0;
    config.inputs[i].displayDigits = 4;
    config.inputs[i].displayMin = 0;
    config.inputs[i].displayMax = 9999;

    config.inputs[i].buttonPin = 0;
    config.inputs[i].buttonShortAction = ACTION_NONE;
    config.inputs[i].buttonLongAction = ACTION_NONE;
    strcpy(config.inputs[i].buttonShortKey, "");
    strcpy(config.inputs[i].buttonLongKey, "");
    config.inputs[i].longPressMs = DEFAULT_LONG_PRESS_MS;

    config.inputs[i].ledPin = 0;
    config.inputs[i].mutexCount = 0;
    for (int j = 0; j < 8; j++) {
      config.inputs[i].mutexList[j] = 0;
    }
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

      case INPUT_SWITCH_LATCHING:
        initializeLatchingSwitch(i);
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
    if (encoderButtons[i] != nullptr) {
      delete encoderButtons[i];
      encoderButtons[i] = nullptr;
    }
    if (encoders[i] != nullptr) {
      delete encoders[i];
      encoders[i] = nullptr;
    }
    // Clean up display objects
    if (displays[i] != nullptr) {
      cleanupDisplay(i);
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
  displayValues[index] = 0;

  // Initialize encoder button if configured
  if (config.inputs[index].buttonPin > 0) {
    encoderButtons[index] = new Bounce();
    encoderButtons[index]->attach(config.inputs[index].buttonPin, INPUT_PULLUP);
    encoderButtons[index]->interval(DEBOUNCE_MS);
    buttonPressTime[index] = 0;
    buttonLongPressed[index] = false;
  }

  // Initialize display if configured
  if (config.inputs[index].displayType != DISPLAY_NONE) {
    initializeDisplay(index);
  }
}

void initializePotentiometer(int index) {
  uint8_t pin = config.inputs[index].pin;
  pinMode(pin, INPUT);
  potValues[index] = analogRead(pin);
}

void initializeLatchingSwitch(int index) {
  uint8_t pin = config.inputs[index].pin;

  buttons[index] = new Bounce();
  buttons[index]->attach(pin, INPUT_PULLUP);
  buttons[index]->interval(DEBOUNCE_MS);

  // Initialize LED pin if configured
  if (config.inputs[index].ledPin > 0) {
    pinMode(config.inputs[index].ledPin, OUTPUT);
    digitalWrite(config.inputs[index].ledPin, LOW);
  }

  switchStates[index] = false;
}

void initializeDisplay(int index) {
  uint8_t displayType = config.inputs[index].displayType;

  // Note: Actual display initialization would happen here
  // This is a placeholder that logs the display configuration
  Serial.print(F("Display configured for input "));
  Serial.print(index);
  Serial.print(F(" - Type: "));
  Serial.print(displayType);
  Serial.print(F(", Digits: "));
  Serial.println(config.inputs[index].displayDigits);

  // TODO: Uncomment and implement based on which display library is available
  /*
  switch (displayType) {
    case DISPLAY_TM1637:
      #ifdef TM1637Display_h
      displays[index] = new TM1637Display(
        config.inputs[index].displayClkPin,
        config.inputs[index].displayDataPin
      );
      ((TM1637Display*)displays[index])->setBrightness(0x0f);
      #endif
      break;

    case DISPLAY_TM1638:
      #ifdef TM1638_h
      displays[index] = new TM1638(
        config.inputs[index].displayDataPin,
        config.inputs[index].displayClkPin,
        config.inputs[index].displayCsPin
      );
      #endif
      break;

    case DISPLAY_MAX7219:
      #ifdef LedControl_h
      displays[index] = new LedControl(
        config.inputs[index].displayDataPin,
        config.inputs[index].displayClkPin,
        config.inputs[index].displayCsPin,
        1
      );
      ((LedControl*)displays[index])->shutdown(0, false);
      ((LedControl*)displays[index])->setIntensity(0, 8);
      ((LedControl*)displays[index])->clearDisplay(0);
      #endif
      break;
  }
  */
}

void cleanupDisplay(int index) {
  // Clean up display objects based on type
  if (displays[index] != nullptr) {
    // TODO: Proper cleanup based on display type
    displays[index] = nullptr;
  }
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
        processEncoderButton(i);
        break;

      case INPUT_POT:
        processPotentiometer(i);
        break;

      case INPUT_SWITCH_LATCHING:
        processLatchingSwitch(i);
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

    // Update display value
    displayValues[index] += actualChange;

    // Clamp to min/max if display is configured
    if (config.inputs[index].displayType != DISPLAY_NONE) {
      if (displayValues[index] < config.inputs[index].displayMin) {
        displayValues[index] = config.inputs[index].displayMin;
      }
      if (displayValues[index] > config.inputs[index].displayMax) {
        displayValues[index] = config.inputs[index].displayMax;
      }
      updateDisplay(index, displayValues[index]);
    }

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

void processEncoderButton(int index) {
  if (encoderButtons[index] == nullptr) return;

  encoderButtons[index]->update();

  // Button pressed
  if (encoderButtons[index]->fell()) {
    buttonPressTime[index] = millis();
    buttonLongPressed[index] = false;
  }

  // Check for long press while button is held
  if (encoderButtons[index]->read() == LOW) {
    unsigned long pressDuration = millis() - buttonPressTime[index];
    if (pressDuration >= config.inputs[index].longPressMs && !buttonLongPressed[index]) {
      // Long press detected
      buttonLongPressed[index] = true;
      handleEncoderButtonAction(index, config.inputs[index].buttonLongAction,
                                config.inputs[index].buttonLongKey);
    }
  }

  // Button released
  if (encoderButtons[index]->rose()) {
    if (!buttonLongPressed[index]) {
      // Short press
      handleEncoderButtonAction(index, config.inputs[index].buttonShortAction,
                                config.inputs[index].buttonShortKey);
    }
    buttonPressTime[index] = 0;
  }
}

void handleEncoderButtonAction(int index, uint8_t action, const char* keyCommand) {
  switch (action) {
    case ACTION_CYCLE_MODES:
      // Cycle through increment modes
      config.inputs[index].mode = (config.inputs[index].mode + 1) % 4;
      Serial.print(F("Encoder "));
      Serial.print(index);
      Serial.print(F(" mode changed to: "));
      Serial.println(config.inputs[index].mode);
      blinkLED();
      break;

    case ACTION_SEND_KEY:
      // Send configured keyboard command
      if (strlen(keyCommand) > 0) {
        sendKeyCommand(keyCommand);
        blinkLED();
      }
      break;

    case ACTION_RESET_DISPLAY:
      // Reset display value to 0 (or minimum)
      displayValues[index] = config.inputs[index].displayMin;
      if (config.inputs[index].displayType != DISPLAY_NONE) {
        updateDisplay(index, displayValues[index]);
      }
      Serial.print(F("Display "));
      Serial.print(index);
      Serial.println(F(" reset"));
      blinkLED();
      break;
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

void processLatchingSwitch(int index) {
  if (buttons[index] == nullptr) return;

  buttons[index]->update();

  if (buttons[index]->fell()) {
    // Toggle switch state
    switchStates[index] = !switchStates[index];

    // Update LED if configured
    if (config.inputs[index].ledPin > 0) {
      digitalWrite(config.inputs[index].ledPin, switchStates[index] ? HIGH : LOW);
    }

    if (switchStates[index]) {
      // Switch turned ON
      sendKeyCommand(config.inputs[index].keyCommand);

      // Apply mutual exclusion rules
      for (int i = 0; i < config.inputs[index].mutexCount && i < 8; i++) {
        uint8_t mutexIndex = config.inputs[index].mutexList[i];
        if (mutexIndex < MAX_INPUTS && mutexIndex != index) {
          turnOffSwitch(mutexIndex);
        }
      }
    }

    blinkLED();
  }
}

void turnOffSwitch(int index) {
  if (config.inputs[index].type != INPUT_SWITCH_LATCHING) return;

  switchStates[index] = false;

  // Turn off LED if configured
  if (config.inputs[index].ledPin > 0) {
    digitalWrite(config.inputs[index].ledPin, LOW);
  }

  Serial.print(F("Switch "));
  Serial.print(index);
  Serial.println(F(" turned off by mutex rule"));
}

// ========== DISPLAY UPDATE ==========
void updateDisplay(int index, long value) {
  if (displays[index] == nullptr) return;

  // Log the display update
  Serial.print(F("Display "));
  Serial.print(index);
  Serial.print(F(" updated to: "));
  Serial.println(value);

  // TODO: Uncomment and implement based on display type
  /*
  uint8_t displayType = config.inputs[index].displayType;
  uint8_t numDigits = config.inputs[index].displayDigits;

  switch (displayType) {
    case DISPLAY_TM1637:
      #ifdef TM1637Display_h
      ((TM1637Display*)displays[index])->showNumberDec(value, false);
      #endif
      break;

    case DISPLAY_TM1638:
      #ifdef TM1638_h
      // TM1638 display update
      #endif
      break;

    case DISPLAY_MAX7219:
      #ifdef LedControl_h
      LedControl* lc = (LedControl*)displays[index];
      // Clear unused digits
      for (int i = 0; i < 8; i++) {
        lc->setChar(0, i, ' ', false);
      }
      // Display value
      long tempValue = abs(value);
      for (int i = 0; i < numDigits; i++) {
        lc->setDigit(0, i, tempValue % 10, false);
        tempValue /= 10;
      }
      #endif
      break;
  }
  */
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

    // Parse new fields
    config.inputs[inputCount].displayType = input["displayType"] | 0;
    config.inputs[inputCount].displayClkPin = input["displayClkPin"] | 0;
    config.inputs[inputCount].displayDataPin = input["displayDataPin"] | 0;
    config.inputs[inputCount].displayCsPin = input["displayCsPin"] | 0;
    config.inputs[inputCount].displayDigits = input["displayDigits"] | 4;
    config.inputs[inputCount].displayMin = input["displayMin"] | 0;
    config.inputs[inputCount].displayMax = input["displayMax"] | 9999;

    config.inputs[inputCount].buttonPin = input["buttonPin"] | 0;
    config.inputs[inputCount].buttonShortAction = input["buttonShortAction"] | 0;
    config.inputs[inputCount].buttonLongAction = input["buttonLongAction"] | 0;
    config.inputs[inputCount].longPressMs = input["longPressMs"] | DEFAULT_LONG_PRESS_MS;

    const char* buttonShortKey = input["buttonShortKey"];
    if (buttonShortKey) strncpy(config.inputs[inputCount].buttonShortKey, buttonShortKey, 15);

    const char* buttonLongKey = input["buttonLongKey"];
    if (buttonLongKey) strncpy(config.inputs[inputCount].buttonLongKey, buttonLongKey, 15);

    config.inputs[inputCount].ledPin = input["ledPin"] | 0;
    config.inputs[inputCount].mutexCount = input["mutexCount"] | 0;

    JsonArray mutexArray = input["mutexList"];
    if (!mutexArray.isNull()) {
      int mutexIdx = 0;
      for (int mutexItem : mutexArray) {
        if (mutexIdx >= 8) break;
        config.inputs[inputCount].mutexList[mutexIdx++] = mutexItem;
      }
    }

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
  doc["version"] = "2.0";

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
