#ifndef KEYEXECUTOR_H
#define KEYEXECUTOR_H

#include <Arduino.h>
#include "SimpleKeyboard.h"
#include "KeyParser.h"

class KeyExecutor {
public:
    KeyExecutor();

    // Initialize keyboard HID
    void init();

    // Execute a parsed key command
    void execute(const KeyCommand& cmd);

    // Execute a key command string
    bool execute(const char* commandStr);

    // Release all keys
    void releaseAll();

private:
    KeyParser _parser;

    // Press modifier keys
    void pressModifiers(uint8_t modifiers);

    // Press regular keys
    void pressKeys(const uint8_t* keys, uint8_t keyCount);

    // Map our modifier bits to Keyboard library modifiers
    uint8_t mapModifier(uint8_t modifier);
};

#endif // KEYEXECUTOR_H
