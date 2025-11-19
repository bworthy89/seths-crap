#include "MF_Switch.h"

MF_Switch::MF_Switch(int id, int pin)
    : InputModule(id, SWITCH, pin, 0), _lastState(HIGH), _currentState(HIGH),
      _lastDebounceTime(0), _triggerValue(0) {
}

void MF_Switch::init() {
    pinMode(_pin, INPUT_PULLUP);
    _lastState = digitalRead(_pin);
    _currentState = _lastState;
    _triggered = false;
    _triggerValue = (_currentState == LOW) ? 1 : 0; // LOW = ON, HIGH = OFF
}

void MF_Switch::update() {
    if (!_enabled) return;

    int reading = digitalRead(_pin);

    // If the reading changed, reset debounce timer
    if (reading != _lastState) {
        _lastDebounceTime = millis();
    }

    // If debounce time has passed, update current state
    if ((millis() - _lastDebounceTime) > DEBOUNCE_DELAY) {
        // If state changed
        if (reading != _currentState) {
            _currentState = reading;
            _triggered = true;

            // Set trigger value based on switch position
            // LOW = ON (switch closed), HIGH = OFF (switch open)
            _triggerValue = (_currentState == LOW) ? 1 : 0;
        }
    }

    _lastState = reading;
}

bool MF_Switch::hasTriggered() {
    return _triggered;
}

int MF_Switch::getTriggerValue() {
    return _triggerValue;
}

void MF_Switch::resetTrigger() {
    _triggered = false;
}
