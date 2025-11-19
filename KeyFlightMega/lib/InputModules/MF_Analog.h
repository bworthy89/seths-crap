#ifndef MF_ANALOG_H
#define MF_ANALOG_H

#include "InputModule.h"

class MF_Analog : public InputModule {
public:
    MF_Analog(int id, int pin, int threshold = 10);

    void init() override;
    void update() override;
    bool hasTriggered() override;
    int getTriggerValue() override;
    void resetTrigger() override;

    void setThreshold(int threshold) { _threshold = threshold; }

private:
    int _threshold;       // Minimum change to trigger
    int _lastValue;       // Last analog value
    int _currentValue;    // Current analog value
    int _triggerValue;    // Value that triggered
};

#endif // MF_ANALOG_H
