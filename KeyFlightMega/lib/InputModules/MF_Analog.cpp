#include "MF_Analog.h"

MF_Analog::MF_Analog(int id, int pin, int threshold)
    : InputModule(id, ANALOG, pin, 0), _threshold(threshold), _lastValue(0),
      _currentValue(0), _triggerValue(0) {
}

void MF_Analog::init() {
    pinMode(_pin, INPUT);
    _lastValue = analogRead(_pin);
    _currentValue = _lastValue;
    _triggered = false;
    _triggerValue = 0;
}

void MF_Analog::update() {
    if (!_enabled) return;

    _currentValue = analogRead(_pin);

    // Check if value changed beyond threshold
    int delta = abs(_currentValue - _lastValue);
    if (delta >= _threshold) {
        _triggered = true;
        _triggerValue = _currentValue;
        _lastValue = _currentValue;
    }
}

bool MF_Analog::hasTriggered() {
    return _triggered;
}

int MF_Analog::getTriggerValue() {
    return _triggerValue;
}

void MF_Analog::resetTrigger() {
    _triggered = false;
    _triggerValue = 0;
}
