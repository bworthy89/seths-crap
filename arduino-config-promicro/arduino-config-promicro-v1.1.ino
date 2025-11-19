/*
 * Arduino Pro Micro - USB HID Keyboard Bridge
 *
 * This sketch receives serial commands from the Arduino Mega 2560
 * and converts them to USB HID keyboard commands with support for
 * multiple simultaneous key presses.
 *
 * Hardware: Arduino Pro Micro (ATmega32u4 with native USB HID support)
 *
 * Serial Protocol:
 * Format: "KEY1+KEY2+KEY3\n"
 * Examples:
 *   "A\n" - sends key A
 *   "L+N+A+V\n" - sends L, N, A, V simultaneously
 *   "CTRL+A\n" - sends Ctrl+A
 *   "CTRL+SHIFT+S\n" - sends Ctrl+Shift+S
 *   "UPARROW\n" - sends Up Arrow
 *   "CTRL+F1\n" - sends Ctrl+F1
 *
 * Supported Modifiers: CTRL, SHIFT, ALT, GUI (Windows key)
 *
 * Wiring:
 * - Connect Mega Serial1 TX (18) to Pro Micro RXI
 * - Connect Mega Serial1 RX (19) to Pro Micro TXO
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
      if (inputBuffer.length() > 100) {
        inputBuffer = "";
      }
    }
  }
}

void processCommand(String command) {
  command.trim();

  if (command.length() == 0) {
    return;
  }

  // Convert to uppercase for consistent parsing
  command.toUpperCase();

  // Split command by '+' delimiter
  int keyCount = 0;
  String keys[10];  // Support up to 10 simultaneous keys
  int startPos = 0;

  // Parse all keys separated by '+'
  for (int i = 0; i <= command.length(); i++) {
    if (i == command.length() || command.charAt(i) == '+') {
      if (i > startPos && keyCount < 10) {
        keys[keyCount++] = command.substring(startPos, i);
      }
      startPos = i + 1;
    }
  }

  if (keyCount == 0) {
    return;
  }

  // Press all keys
  delay(5);  // Small delay for stability

  for (int i = 0; i < keyCount; i++) {
    pressKey(keys[i]);
  }

  delay(10);  // Hold time

  // Release all keys
  Keyboard.releaseAll();
}

void pressKey(String keyName) {
  keyName.trim();

  if (keyName.length() == 0) {
    return;
  }

  // Check for modifiers
  if (keyName == "CTRL" || keyName == "CONTROL") {
    Keyboard.press(KEY_LEFT_CTRL);
  } else if (keyName == "SHIFT") {
    Keyboard.press(KEY_LEFT_SHIFT);
  } else if (keyName == "ALT") {
    Keyboard.press(KEY_LEFT_ALT);
  } else if (keyName == "GUI" || keyName == "WIN" || keyName == "WINDOWS") {
    Keyboard.press(KEY_LEFT_GUI);
  }
  // Check for single character keys
  else if (keyName.length() == 1) {
    char key = keyName.charAt(0);
    Keyboard.press(key);
  }
  // Check for special keys
  else {
    pressSpecialKey(keyName);
  }
}

void pressSpecialKey(String keyName) {
  // Arrow keys
  if (keyName == "UPARROW" || keyName == "UP") {
    Keyboard.press(KEY_UP_ARROW);
  } else if (keyName == "DOWNARROW" || keyName == "DOWN") {
    Keyboard.press(KEY_DOWN_ARROW);
  } else if (keyName == "LEFTARROW" || keyName == "LEFT") {
    Keyboard.press(KEY_LEFT_ARROW);
  } else if (keyName == "RIGHTARROW" || keyName == "RIGHT") {
    Keyboard.press(KEY_RIGHT_ARROW);
  }
  // Action keys
  else if (keyName == "ENTER" || keyName == "RETURN") {
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
  }
  // Navigation keys
  else if (keyName == "HOME") {
    Keyboard.press(KEY_HOME);
  } else if (keyName == "END") {
    Keyboard.press(KEY_END);
  } else if (keyName == "PAGEUP" || keyName == "PGUP") {
    Keyboard.press(KEY_PAGE_UP);
  } else if (keyName == "PAGEDOWN" || keyName == "PGDN") {
    Keyboard.press(KEY_PAGE_DOWN);
  } else if (keyName == "INSERT" || keyName == "INS") {
    Keyboard.press(KEY_INSERT);
  }
  // Function keys F1-F12
  else if (keyName == "F1") {
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
  // Function keys F13-F24
  else if (keyName == "F13") {
    Keyboard.press(KEY_F13);
  } else if (keyName == "F14") {
    Keyboard.press(KEY_F14);
  } else if (keyName == "F15") {
    Keyboard.press(KEY_F15);
  } else if (keyName == "F16") {
    Keyboard.press(KEY_F16);
  } else if (keyName == "F17") {
    Keyboard.press(KEY_F17);
  } else if (keyName == "F18") {
    Keyboard.press(KEY_F18);
  } else if (keyName == "F19") {
    Keyboard.press(KEY_F19);
  } else if (keyName == "F20") {
    Keyboard.press(KEY_F20);
  } else if (keyName == "F21") {
    Keyboard.press(KEY_F21);
  } else if (keyName == "F22") {
    Keyboard.press(KEY_F22);
  } else if (keyName == "F23") {
    Keyboard.press(KEY_F23);
  } else if (keyName == "F24") {
    Keyboard.press(KEY_F24);
  }
  // If key not recognized, it won't be pressed
}
