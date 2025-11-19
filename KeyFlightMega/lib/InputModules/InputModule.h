#ifndef INPUTMODULE_H
#define INPUTMODULE_H

#include <Arduino.h>

// Input type enumeration matching C# InputType.cs
enum InputType {
    BUTTON = 1,
    ENCODER = 2,
    SWITCH = 3,
    ANALOG = 4
};

// Base class for all input modules
class InputModule {
public:
    InputModule(int id, InputType type, int pin, int pin2 = 0);
    virtual ~InputModule() {}

    // Initialize the module (setup pins, etc.)
    virtual void init() = 0;

    // Update module state (called in main loop)
    virtual void update() = 0;

    // Check if module has triggered
    virtual bool hasTriggered() = 0;

    // Get trigger value (for encoders: direction, for analog: value)
    virtual int getTriggerValue() { return 0; }

    // Reset trigger state
    virtual void resetTrigger() = 0;

    // Getters
    int getId() const { return _id; }
    InputType getType() const { return _type; }
    int getPin() const { return _pin; }
    int getPin2() const { return _pin2; }
    bool isEnabled() const { return _enabled; }

    // Enable/disable module
    void setEnabled(bool enabled) { _enabled = enabled; }

protected:
    int _id;
    InputType _type;
    int _pin;
    int _pin2;
    bool _enabled;
    bool _triggered;
};

#endif // INPUTMODULE_H
