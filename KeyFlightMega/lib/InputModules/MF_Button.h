#ifndef MF_BUTTON_H
#define MF_BUTTON_H

#include "InputModule.h"

class MF_Button : public InputModule {
public:
    MF_Button(int id, int pin);

    void init() override;
    void update() override;
    bool hasTriggered() override;
    void resetTrigger() override;

private:
    static const unsigned long DEBOUNCE_DELAY = 50; // 50ms debounce
    bool _lastState;
    bool _currentState;
    unsigned long _lastDebounceTime;
};

#endif // MF_BUTTON_H
