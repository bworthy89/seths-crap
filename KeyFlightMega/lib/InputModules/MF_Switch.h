#ifndef MF_SWITCH_H
#define MF_SWITCH_H

#include "InputModule.h"

class MF_Switch : public InputModule {
public:
    MF_Switch(int id, int pin);

    void init() override;
    void update() override;
    bool hasTriggered() override;
    int getTriggerValue() override;
    void resetTrigger() override;

private:
    static const unsigned long DEBOUNCE_DELAY = 50; // 50ms debounce
    bool _lastState;
    bool _currentState;
    unsigned long _lastDebounceTime;
    int _triggerValue; // 1 for ON, 0 for OFF
};

#endif // MF_SWITCH_H
