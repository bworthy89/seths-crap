#include "KeyExecutor.h"

// Arduino Keyboard library modifier key codes
#define KEY_LEFT_CTRL   0x80
#define KEY_LEFT_SHIFT  0x81
#define KEY_LEFT_ALT    0x82
#define KEY_LEFT_GUI    0x83

KeyExecutor::KeyExecutor() {
}

void KeyExecutor::init() {
    Keyboard.begin();
}

void KeyExecutor::execute(const KeyCommand& cmd) {
    // Release any previous keys
    Keyboard.releaseAll();

    // Press modifiers
    if (cmd.modifiers != 0) {
        pressModifiers(cmd.modifiers);
    }

    // Press keys
    if (cmd.keyCount > 0) {
        pressKeys(cmd.keys, cmd.keyCount);
    }

    // Small delay to ensure HID report is sent
    delay(10);

    // Release all keys
    Keyboard.releaseAll();
}

bool KeyExecutor::execute(const char* commandStr) {
    KeyCommand cmd;

    // Parse command string
    if (!_parser.parse(commandStr, cmd)) {
        return false;
    }

    // Execute parsed command
    execute(cmd);
    return true;
}

void KeyExecutor::releaseAll() {
    Keyboard.releaseAll();
}

void KeyExecutor::pressModifiers(uint8_t modifiers) {
    if (modifiers & MOD_CTRL) {
        Keyboard.press(KEY_LEFT_CTRL);
    }
    if (modifiers & MOD_SHIFT) {
        Keyboard.press(KEY_LEFT_SHIFT);
    }
    if (modifiers & MOD_ALT) {
        Keyboard.press(KEY_LEFT_ALT);
    }
    if (modifiers & MOD_WIN) {
        Keyboard.press(KEY_LEFT_GUI);
    }
}

void KeyExecutor::pressKeys(const uint8_t* keys, uint8_t keyCount) {
    for (uint8_t i = 0; i < keyCount; i++) {
        // The key codes from KeyParser are already in Arduino Keyboard library format
        Keyboard.press(keys[i]);
    }
}

uint8_t KeyExecutor::mapModifier(uint8_t modifier) {
    switch (modifier) {
        case MOD_CTRL:  return KEY_LEFT_CTRL;
        case MOD_SHIFT: return KEY_LEFT_SHIFT;
        case MOD_ALT:   return KEY_LEFT_ALT;
        case MOD_WIN:   return KEY_LEFT_GUI;
        default:        return 0;
    }
}
