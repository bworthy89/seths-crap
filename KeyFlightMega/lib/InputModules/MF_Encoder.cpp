#include "MF_Encoder.h"

MF_Encoder::MF_Encoder(int id, int pinA, int pinB, int sensitivity)
    : InputModule(id, ENCODER, pinA, pinB), _sensitivity(sensitivity), _lastStateA(HIGH),
      _lastStateB(HIGH), _position(0), _triggerDirection(0) {
}

void MF_Encoder::init() {
    pinMode(_pin, INPUT_PULLUP);   // Pin A
    pinMode(_pin2, INPUT_PULLUP);  // Pin B
    _lastStateA = digitalRead(_pin);
    _lastStateB = digitalRead(_pin2);
    _position = 0;
    _triggerDirection = 0;
    _triggered = false;
}

void MF_Encoder::update() {
    if (!_enabled) return;

    int currentStateA = digitalRead(_pin);
    int currentStateB = digitalRead(_pin2);

    // Detect rotation using quadrature encoding
    if (currentStateA != _lastStateA) {
        // A changed
        if (currentStateA == LOW) {
            // Falling edge on A
            if (currentStateB == HIGH) {
                // CW rotation
                _position++;
            } else {
                // CCW rotation
                _position--;
            }
        }
    }

    // Check if we've exceeded sensitivity threshold
    if (_position >= _sensitivity) {
        _triggerDirection = 1;  // CW
        _triggered = true;
        _position = 0;
    } else if (_position <= -_sensitivity) {
        _triggerDirection = -1; // CCW
        _triggered = true;
        _position = 0;
    }

    _lastStateA = currentStateA;
    _lastStateB = currentStateB;
}

bool MF_Encoder::hasTriggered() {
    return _triggered;
}

int MF_Encoder::getTriggerValue() {
    return _triggerDirection;
}

void MF_Encoder::resetTrigger() {
    _triggered = false;
    _triggerDirection = 0;
}
