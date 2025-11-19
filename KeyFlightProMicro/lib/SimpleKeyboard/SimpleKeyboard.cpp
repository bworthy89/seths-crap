#include "SimpleKeyboard.h"

SimpleKeyboard_::SimpleKeyboard_() {
    // Initialize report
    memset(_keyReport, 0, sizeof(_keyReport));
}

void SimpleKeyboard_::begin() {
    // Nothing needed, HID is auto-initialized
}

void SimpleKeyboard_::end() {
    releaseAll();
}

void SimpleKeyboard_::press(uint8_t k) {
    uint8_t i;

    // Handle modifier keys (0x80-0x87)
    if (k >= 0x80 && k <= 0x87) {
        // Modifiers byte is _keyReport[0]
        _keyReport[0] |= (1 << (k - 0x80));
        sendReport();
        return;
    }

    // Handle other special keys (translate to HID usage codes)
    uint8_t hidCode = 0;

    // Convert our key codes to HID usage codes
    if (k >= KEY_F1 && k <= KEY_F12) {
        // F1-F12: HID 0x3A-0x45
        hidCode = 0x3A + (k - KEY_F1);
    } else if (k == KEY_UP_ARROW) {
        hidCode = 0x52;
    } else if (k == KEY_DOWN_ARROW) {
        hidCode = 0x51;
    } else if (k == KEY_LEFT_ARROW) {
        hidCode = 0x50;
    } else if (k == KEY_RIGHT_ARROW) {
        hidCode = 0x4F;
    } else if (k == KEY_BACKSPACE) {
        hidCode = 0x2A;
    } else if (k == KEY_TAB) {
        hidCode = 0x2B;
    } else if (k == KEY_RETURN) {
        hidCode = 0x28;
    } else if (k == KEY_ESC) {
        hidCode = 0x29;
    } else if (k == KEY_INSERT) {
        hidCode = 0x49;
    } else if (k == KEY_DELETE) {
        hidCode = 0x4C;
    } else if (k == KEY_PAGE_UP) {
        hidCode = 0x4B;
    } else if (k == KEY_PAGE_DOWN) {
        hidCode = 0x4E;
    } else if (k == KEY_HOME) {
        hidCode = 0x4A;
    } else if (k == KEY_END) {
        hidCode = 0x4D;
    } else if (k == KEY_CAPS_LOCK) {
        hidCode = 0x39;
    } else if (k == KEY_PRINT_SCREEN) {
        hidCode = 0x46;
    } else if (k == KEY_SCROLL_LOCK) {
        hidCode = 0x47;
    } else if (k == KEY_PAUSE) {
        hidCode = 0x48;
    }
    // ASCII characters
    else if (k >= 'a' && k <= 'z') {
        // Lowercase a-z: HID 0x04-0x1D
        hidCode = 0x04 + (k - 'a');
    } else if (k >= 'A' && k <= 'Z') {
        // Uppercase A-Z: same as lowercase but set shift
        hidCode = 0x04 + (k - 'A');
        _keyReport[0] |= 0x02; // Set shift modifier
    } else if (k >= '0' && k <= '9') {
        // Numbers 1-9: HID 0x1E-0x26, 0: HID 0x27
        if (k == '0') {
            hidCode = 0x27;
        } else {
            hidCode = 0x1E + (k - '1');
        }
    } else if (k == ' ') {
        hidCode = 0x2C; // Space
    } else if (k == '-') {
        hidCode = 0x2D; // Minus
    } else if (k == '=') {
        hidCode = 0x2E; // Equal
    } else if (k == '[') {
        hidCode = 0x2F; // Left bracket
    } else if (k == ']') {
        hidCode = 0x30; // Right bracket
    } else if (k == '\\') {
        hidCode = 0x31; // Backslash
    } else if (k == ';') {
        hidCode = 0x33; // Semicolon
    } else if (k == '\'') {
        hidCode = 0x34; // Apostrophe
    } else if (k == '`') {
        hidCode = 0x35; // Grave
    } else if (k == ',') {
        hidCode = 0x36; // Comma
    } else if (k == '.') {
        hidCode = 0x37; // Dot
    } else if (k == '/') {
        hidCode = 0x38; // Slash
    }

    if (hidCode == 0) {
        return; // Unknown key
    }

    // Add key to report (slots 2-7, max 6 keys)
    for (i = 2; i < 8; i++) {
        if (_keyReport[i] == 0) {
            _keyReport[i] = hidCode;
            break;
        } else if (_keyReport[i] == hidCode) {
            // Already pressed
            break;
        }
    }

    sendReport();
}

void SimpleKeyboard_::release(uint8_t k) {
    uint8_t i;

    // Handle modifier keys
    if (k >= 0x80 && k <= 0x87) {
        _keyReport[0] &= ~(1 << (k - 0x80));
        sendReport();
        return;
    }

    // For other keys, convert to HID code and search for it
    uint8_t hidCode = 0;

    // Convert our key codes to HID usage codes (same logic as press())
    if (k >= KEY_F1 && k <= KEY_F12) {
        hidCode = 0x3A + (k - KEY_F1);
    } else if (k == KEY_UP_ARROW) {
        hidCode = 0x52;
    } else if (k == KEY_DOWN_ARROW) {
        hidCode = 0x51;
    } else if (k == KEY_LEFT_ARROW) {
        hidCode = 0x50;
    } else if (k == KEY_RIGHT_ARROW) {
        hidCode = 0x4F;
    } else if (k == KEY_BACKSPACE) {
        hidCode = 0x2A;
    } else if (k == KEY_TAB) {
        hidCode = 0x2B;
    } else if (k == KEY_RETURN) {
        hidCode = 0x28;
    } else if (k == KEY_ESC) {
        hidCode = 0x29;
    } else if (k == KEY_INSERT) {
        hidCode = 0x49;
    } else if (k == KEY_DELETE) {
        hidCode = 0x4C;
    } else if (k == KEY_PAGE_UP) {
        hidCode = 0x4B;
    } else if (k == KEY_PAGE_DOWN) {
        hidCode = 0x4E;
    } else if (k == KEY_HOME) {
        hidCode = 0x4A;
    } else if (k == KEY_END) {
        hidCode = 0x4D;
    } else if (k == KEY_CAPS_LOCK) {
        hidCode = 0x39;
    } else if (k == KEY_PRINT_SCREEN) {
        hidCode = 0x46;
    } else if (k == KEY_SCROLL_LOCK) {
        hidCode = 0x47;
    } else if (k == KEY_PAUSE) {
        hidCode = 0x48;
    } else if (k >= 'a' && k <= 'z') {
        hidCode = 0x04 + (k - 'a');
    } else if (k >= 'A' && k <= 'Z') {
        hidCode = 0x04 + (k - 'A');
    } else if (k >= '0' && k <= '9') {
        if (k == '0') {
            hidCode = 0x27;
        } else {
            hidCode = 0x1E + (k - '1');
        }
    } else if (k == ' ') {
        hidCode = 0x2C;
    } else if (k == '-') {
        hidCode = 0x2D;
    } else if (k == '=') {
        hidCode = 0x2E;
    } else if (k == '[') {
        hidCode = 0x2F;
    } else if (k == ']') {
        hidCode = 0x30;
    } else if (k == '\\') {
        hidCode = 0x31;
    } else if (k == ';') {
        hidCode = 0x33;
    } else if (k == '\'') {
        hidCode = 0x34;
    } else if (k == '`') {
        hidCode = 0x35;
    } else if (k == ',') {
        hidCode = 0x36;
    } else if (k == '.') {
        hidCode = 0x37;
    } else if (k == '/') {
        hidCode = 0x38;
    }

    if (hidCode == 0) {
        return;
    }

    // Remove key from report
    for (i = 2; i < 8; i++) {
        if (_keyReport[i] == hidCode) {
            _keyReport[i] = 0;
        }
    }

    sendReport();
}

void SimpleKeyboard_::releaseAll() {
    memset(_keyReport, 0, sizeof(_keyReport));
    sendReport();
}

size_t SimpleKeyboard_::write(uint8_t c) {
    press(c);
    release(c);
    return 1;
}

void SimpleKeyboard_::sendReport() {
    // Send HID keyboard report
    // Report structure:
    // [0] Modifiers
    // [1] Reserved
    // [2-7] Key codes (up to 6 simultaneous keys)
    HID().SendReport(2, _keyReport, sizeof(_keyReport));
}

SimpleKeyboard_ Keyboard;
