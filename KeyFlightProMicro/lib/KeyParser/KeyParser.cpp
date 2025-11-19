#include "KeyParser.h"
#include <string.h>
#include <ctype.h>

// Arduino Keyboard library key codes
// From Keyboard.h in Arduino AVR Boards
#define KEY_UP_ARROW       0xDA
#define KEY_DOWN_ARROW     0xD9
#define KEY_LEFT_ARROW     0xD8
#define KEY_RIGHT_ARROW    0xD7
#define KEY_BACKSPACE      0xB2
#define KEY_TAB            0xB3
#define KEY_RETURN         0xB0
#define KEY_ESC            0xB1
#define KEY_INSERT         0xD1
#define KEY_DELETE         0xD4
#define KEY_PAGE_UP        0xD3
#define KEY_PAGE_DOWN      0xD6
#define KEY_HOME           0xD2
#define KEY_END            0xD5
#define KEY_CAPS_LOCK      0xC1
#define KEY_F1             0xC2
#define KEY_F2             0xC3
#define KEY_F3             0xC4
#define KEY_F4             0xC5
#define KEY_F5             0xC6
#define KEY_F6             0xC7
#define KEY_F7             0xC8
#define KEY_F8             0xC9
#define KEY_F9             0xCA
#define KEY_F10            0xCB
#define KEY_F11            0xCC
#define KEY_F12            0xCD
#define KEY_PRINT_SCREEN   0xCE
#define KEY_SCROLL_LOCK    0xCF
#define KEY_PAUSE          0xD0

KeyParser::KeyParser() {
}

bool KeyParser::parse(const char* commandStr, KeyCommand& cmd) {
    // Clear command structure
    cmd.modifiers = 0;
    cmd.keyCount = 0;
    memset(cmd.keys, 0, sizeof(cmd.keys));

    if (commandStr == nullptr || commandStr[0] == '\0') {
        return false;
    }

    // Create a copy of the command string for tokenization
    char buffer[64];
    strncpy(buffer, commandStr, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // Tokenize by '+'
    char* token = strtok(buffer, "+");
    while (token != nullptr) {
        // Trim whitespace
        while (*token == ' ') token++;

        if (!parseToken(token, cmd)) {
            return false;
        }

        token = strtok(nullptr, "+");
    }

    // Must have at least one key (or modifier alone is okay for testing)
    return true;
}

bool KeyParser::parseToken(const char* token, KeyCommand& cmd) {
    uint8_t modifier;
    uint8_t keyCode;

    // Check if it's a modifier
    if (isModifier(token, modifier)) {
        cmd.modifiers |= modifier;
        return true;
    }

    // Check if it's a key
    if (getKeyCode(token, keyCode)) {
        if (cmd.keyCount < 6) {
            cmd.keys[cmd.keyCount++] = keyCode;
            return true;
        } else {
            // Too many keys (USB HID limit is 6)
            return false;
        }
    }

    // Unknown token
    return false;
}

bool KeyParser::isModifier(const char* token, uint8_t& modifier) {
    if (strcasecmp_P(token, "CTRL") || strcasecmp_P(token, "CONTROL")) {
        modifier = MOD_CTRL;
        return true;
    }
    if (strcasecmp_P(token, "SHIFT")) {
        modifier = MOD_SHIFT;
        return true;
    }
    if (strcasecmp_P(token, "ALT")) {
        modifier = MOD_ALT;
        return true;
    }
    if (strcasecmp_P(token, "WIN") || strcasecmp_P(token, "WINDOWS") || strcasecmp_P(token, "GUI")) {
        modifier = MOD_WIN;
        return true;
    }
    return false;
}

bool KeyParser::getKeyCode(const char* token, uint8_t& keyCode) {
    // Convert to uppercase for comparison
    char upper[32];
    int i = 0;
    while (token[i] && i < 31) {
        upper[i] = toupper(token[i]);
        i++;
    }
    upper[i] = '\0';

    // Single letter A-Z (return lowercase ASCII for Keyboard library)
    if (strlen(upper) == 1 && upper[0] >= 'A' && upper[0] <= 'Z') {
        keyCode = upper[0] - 'A' + 'a'; // Convert to lowercase ASCII
        return true;
    }

    // Single digit 0-9 (return ASCII code)
    if (strlen(upper) == 1 && upper[0] >= '0' && upper[0] <= '9') {
        keyCode = upper[0]; // Use ASCII code directly
        return true;
    }

    // Function keys F1-F12
    if (upper[0] == 'F' && strlen(upper) >= 2) {
        int fNum = atoi(upper + 1);
        if (fNum >= 1 && fNum <= 12) {
            keyCode = KEY_F1 + (fNum - 1);
            return true;
        }
    }

    // Special keys
    if (strcmp(upper, "ENTER") == 0 || strcmp(upper, "RETURN") == 0) {
        keyCode = KEY_RETURN;
        return true;
    }
    if (strcmp(upper, "ESC") == 0 || strcmp(upper, "ESCAPE") == 0) {
        keyCode = KEY_ESC;
        return true;
    }
    if (strcmp(upper, "BACKSPACE") == 0) {
        keyCode = KEY_BACKSPACE;
        return true;
    }
    if (strcmp(upper, "TAB") == 0) {
        keyCode = KEY_TAB;
        return true;
    }
    if (strcmp(upper, "SPACE") == 0) {
        keyCode = ' '; // Space is ASCII 32
        return true;
    }
    if (strcmp(upper, "DELETE") == 0 || strcmp(upper, "DEL") == 0) {
        keyCode = KEY_DELETE;
        return true;
    }
    if (strcmp(upper, "INSERT") == 0 || strcmp(upper, "INS") == 0) {
        keyCode = KEY_INSERT;
        return true;
    }
    if (strcmp(upper, "HOME") == 0) {
        keyCode = KEY_HOME;
        return true;
    }
    if (strcmp(upper, "END") == 0) {
        keyCode = KEY_END;
        return true;
    }
    if (strcmp(upper, "PAGEUP") == 0 || strcmp(upper, "PGUP") == 0) {
        keyCode = KEY_PAGE_UP;
        return true;
    }
    if (strcmp(upper, "PAGEDOWN") == 0 || strcmp(upper, "PGDN") == 0) {
        keyCode = KEY_PAGE_DOWN;
        return true;
    }

    // Arrow keys
    if (strcmp(upper, "UPARROW") == 0 || strcmp(upper, "UP") == 0) {
        keyCode = KEY_UP_ARROW;
        return true;
    }
    if (strcmp(upper, "DOWNARROW") == 0 || strcmp(upper, "DOWN") == 0) {
        keyCode = KEY_DOWN_ARROW;
        return true;
    }
    if (strcmp(upper, "LEFTARROW") == 0 || strcmp(upper, "LEFT") == 0) {
        keyCode = KEY_LEFT_ARROW;
        return true;
    }
    if (strcmp(upper, "RIGHTARROW") == 0 || strcmp(upper, "RIGHT") == 0) {
        keyCode = KEY_RIGHT_ARROW;
        return true;
    }

    // Other special keys
    if (strcmp(upper, "CAPSLOCK") == 0) {
        keyCode = KEY_CAPS_LOCK;
        return true;
    }
    if (strcmp(upper, "PRINTSCREEN") == 0) {
        keyCode = KEY_PRINT_SCREEN;
        return true;
    }
    if (strcmp(upper, "SCROLLLOCK") == 0) {
        keyCode = KEY_SCROLL_LOCK;
        return true;
    }
    if (strcmp(upper, "PAUSE") == 0) {
        keyCode = KEY_PAUSE;
        return true;
    }

    // Punctuation (use ASCII codes)
    if (strcmp(upper, "MINUS") == 0 || strcmp(upper, "-") == 0) {
        keyCode = '-';
        return true;
    }
    if (strcmp(upper, "EQUAL") == 0 || strcmp(upper, "=") == 0) {
        keyCode = '=';
        return true;
    }
    if (strcmp(upper, "LEFTBRACE") == 0 || strcmp(upper, "[") == 0) {
        keyCode = '[';
        return true;
    }
    if (strcmp(upper, "RIGHTBRACE") == 0 || strcmp(upper, "]") == 0) {
        keyCode = ']';
        return true;
    }
    if (strcmp(upper, "BACKSLASH") == 0 || strcmp(upper, "\\") == 0) {
        keyCode = '\\';
        return true;
    }
    if (strcmp(upper, "SEMICOLON") == 0 || strcmp(upper, ";") == 0) {
        keyCode = ';';
        return true;
    }
    if (strcmp(upper, "APOSTROPHE") == 0 || strcmp(upper, "'") == 0) {
        keyCode = '\'';
        return true;
    }
    if (strcmp(upper, "GRAVE") == 0 || strcmp(upper, "`") == 0) {
        keyCode = '`';
        return true;
    }
    if (strcmp(upper, "COMMA") == 0 || strcmp(upper, ",") == 0) {
        keyCode = ',';
        return true;
    }
    if (strcmp(upper, "DOT") == 0 || strcmp(upper, "PERIOD") == 0 || strcmp(upper, ".") == 0) {
        keyCode = '.';
        return true;
    }
    if (strcmp(upper, "SLASH") == 0 || strcmp(upper, "/") == 0) {
        keyCode = '/';
        return true;
    }

    // Unknown key
    return false;
}

bool KeyParser::strcasecmp_P(const char* str1, const char* str2) {
    while (*str1 && *str2) {
        if (toupper(*str1) != toupper(*str2)) {
            return false;
        }
        str1++;
        str2++;
    }
    return (*str1 == *str2);
}
