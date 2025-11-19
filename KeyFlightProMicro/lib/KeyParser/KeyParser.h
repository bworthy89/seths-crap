#ifndef KEYPARSER_H
#define KEYPARSER_H

#include <Arduino.h>

// Key command structure
struct KeyCommand {
    uint8_t modifiers;      // Bitmask: CTRL, ALT, SHIFT, WIN
    uint8_t keys[6];        // Up to 6 simultaneous keys (USB HID limit)
    uint8_t keyCount;       // Number of keys in keys[]
};

// Modifier masks
#define MOD_CTRL   0x01
#define MOD_SHIFT  0x02
#define MOD_ALT    0x04
#define MOD_WIN    0x08

class KeyParser {
public:
    KeyParser();

    // Parse keyboard command string
    // Format: "A", "CTRL+C", "A+B+C", "CTRL+ALT+DEL", etc.
    bool parse(const char* commandStr, KeyCommand& cmd);

private:
    // Parse individual token (could be modifier or key)
    bool parseToken(const char* token, KeyCommand& cmd);

    // Check if token is a modifier
    bool isModifier(const char* token, uint8_t& modifier);

    // Get key code for token
    bool getKeyCode(const char* token, uint8_t& keyCode);

    // Helper: case-insensitive string compare
    bool strcasecmp_P(const char* str1, const char* str2);
};

#endif // KEYPARSER_H
