#ifndef MF_ENCODER_H
#define MF_ENCODER_H

#include "InputModule.h"

class MF_Encoder : public InputModule {
public:
    MF_Encoder(int id, int pinA, int pinB, int sensitivity = 1);

    void init() override;
    void update() override;
    bool hasTriggered() override;
    int getTriggerValue() override;
    void resetTrigger() override;

    void setSensitivity(int sensitivity) { _sensitivity = sensitivity; }

private:
    int _sensitivity;
    int _lastStateA;
    int _lastStateB;
    int _position;
    int _triggerDirection; // 1 for CW, -1 for CCW, 0 for no trigger
};

#endif // MF_ENCODER_H
