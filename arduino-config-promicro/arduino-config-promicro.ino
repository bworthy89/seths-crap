/*
 * Arduino Pro Micro - USB HID Keyboard Bridge
 *
 * This sketch receives serial commands from the Arduino Mega 2560
 * and converts them to USB HID keyboard commands (Ctrl+ combinations).
 *
 * Hardware: Arduino Pro Micro (ATmega32u4 with native USB HID support)
 *
 * Serial Protocol:
 * Format: "CTRL+<key>\n"
 * Example: "CTRL+F\n" - sends Ctrl+F
 *          "CTRL+UPARROW\n" - sends Ctrl+Up Arrow
 *
 * Wiring:
 * - Connect Mega Serial1 TX (18) to Pro Micro RX
 * - Connect Mega Serial1 RX (19) to Pro Micro TX
 * - Connect GND between boards
 */

#include <Keyboard.h>

const unsigned long BAUD_RATE = 115200;
const int LED_PIN = LED_BUILTIN;  // LED for status indication

String inputBuffer = "";
bool ledState = false;
unsigned long lastBlinkTime = 0;
const unsigned long BLINK_INTERVAL = 500;

void setup() {
  // Initialize USB HID Keyboard
  Keyboard.begin();

  // Initialize Serial communication with Mega
  Serial1.begin(BAUD_RATE);

  // Initialize LED for status
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // Turn on LED when ready

  // Short delay for stability
  delay(100);
}

void loop() {
  // Blink LED to show activity
  if (millis() - lastBlinkTime >= BLINK_INTERVAL) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    lastBlinkTime = millis();
  }

  // Read from Serial1 (connected to Mega)
  while (Serial1.available() > 0) {
    char inChar = Serial1.read();

    if (inChar == '\n') {
      // Process complete command
      if (inputBuffer.length() > 0) {
        processCommand(inputBuffer);
        inputBuffer = "";

        // Flash LED on command
        digitalWrite(LED_PIN, HIGH);
        delay(50);
        digitalWrite(LED_PIN, LOW);
      }
    } else {
      inputBuffer += inChar;

      // Prevent buffer overflow
      if (inputBuffer.length() > 50) {
        inputBuffer = "";
      }
    }
  }
}

void processCommand(String command) {
  command.trim();

  // All commands should start with "CTRL+"
  if (!command.startsWith("CTRL+")) {
    return;
  }

  // Extract the key part after "CTRL+"
  String keyPart = command.substring(5);

  if (keyPart.length() == 0) {
    return;
  }

  // Press Ctrl modifier
  Keyboard.press(KEY_LEFT_CTRL);
  delay(5);  // Small delay for key registration

  // Press the specified key
  if (keyPart.length() == 1) {
    // Single character key
    char key = keyPart.charAt(0);
    Keyboard.press(key);
  } else {
    // Special keys
    pressSpecialKey(keyPart);
  }

  delay(10);  // Hold time

  // Release all keys
  Keyboard.releaseAll();
}

void pressSpecialKey(String keyName) {
  // Convert to uppercase for comparison
  keyName.toUpperCase();

  // Map special key names to keyboard codes
  if (keyName == "UPARROW" || keyName == "UP") {
    Keyboard.press(KEY_UP_ARROW);
  } else if (keyName == "DOWNARROW" || keyName == "DOWN") {
    Keyboard.press(KEY_DOWN_ARROW);
  } else if (keyName == "LEFTARROW" || keyName == "LEFT") {
    Keyboard.press(KEY_LEFT_ARROW);
  } else if (keyName == "RIGHTARROW" || keyName == "RIGHT") {
    Keyboard.press(KEY_RIGHT_ARROW);
  } else if (keyName == "ENTER" || keyName == "RETURN") {
    Keyboard.press(KEY_RETURN);
  } else if (keyName == "ESC" || keyName == "ESCAPE") {
    Keyboard.press(KEY_ESC);
  } else if (keyName == "TAB") {
    Keyboard.press(KEY_TAB);
  } else if (keyName == "SPACE") {
    Keyboard.press(' ');
  } else if (keyName == "BACKSPACE") {
    Keyboard.press(KEY_BACKSPACE);
  } else if (keyName == "DELETE" || keyName == "DEL") {
    Keyboard.press(KEY_DELETE);
  } else if (keyName == "HOME") {
    Keyboard.press(KEY_HOME);
  } else if (keyName == "END") {
    Keyboard.press(KEY_END);
  } else if (keyName == "PAGEUP" || keyName == "PGUP") {
    Keyboard.press(KEY_PAGE_UP);
  } else if (keyName == "PAGEDOWN" || keyName == "PGDN") {
    Keyboard.press(KEY_PAGE_DOWN);
  } else if (keyName == "F1") {
    Keyboard.press(KEY_F1);
  } else if (keyName == "F2") {
    Keyboard.press(KEY_F2);
  } else if (keyName == "F3") {
    Keyboard.press(KEY_F3);
  } else if (keyName == "F4") {
    Keyboard.press(KEY_F4);
  } else if (keyName == "F5") {
    Keyboard.press(KEY_F5);
  } else if (keyName == "F6") {
    Keyboard.press(KEY_F6);
  } else if (keyName == "F7") {
    Keyboard.press(KEY_F7);
  } else if (keyName == "F8") {
    Keyboard.press(KEY_F8);
  } else if (keyName == "F9") {
    Keyboard.press(KEY_F9);
  } else if (keyName == "F10") {
    Keyboard.press(KEY_F10);
  } else if (keyName == "F11") {
    Keyboard.press(KEY_F11);
  } else if (keyName == "F12") {
    Keyboard.press(KEY_F12);
  }
  // If key not recognized, it won't be pressed
}
