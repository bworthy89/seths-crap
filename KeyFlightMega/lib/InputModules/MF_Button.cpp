#include "MF_Button.h"

MF_Button::MF_Button(int id, int pin)
    : InputModule(id, BUTTON, pin, 0), _lastState(HIGH), _currentState(HIGH), _lastDebounceTime(0) {
}

void MF_Button::init() {
    pinMode(_pin, INPUT_PULLUP);
    _lastState = digitalRead(_pin);
    _currentState = _lastState;
    _triggered = false;
}

void MF_Button::update() {
    if (!_enabled) return;

    int reading = digitalRead(_pin);

    // If the reading changed, reset debounce timer
    if (reading != _lastState) {
        _lastDebounceTime = millis();
    }

    // If debounce time has passed, update current state
    if ((millis() - _lastDebounceTime) > DEBOUNCE_DELAY) {
        // If state changed from HIGH to LOW (button pressed)
        if (reading != _currentState) {
            _currentState = reading;

            // Trigger on button press (HIGH to LOW transition)
            if (_currentState == LOW) {
                _triggered = true;
            }
        }
    }

    _lastState = reading;
}

bool MF_Button::hasTriggered() {
    return _triggered;
}

void MF_Button::resetTrigger() {
    _triggered = false;
}
