#ifndef SafetyTests_h
#define SafetyTests_h

#include "Arduino.h"
#include "Motor.h"
#include "SystemState.h"

class SafetyTests {
public:
    SafetyTests(Motor *motor, SystemState *sysState);
    virtual bool ensureEverythingIsSafe();

private:
    Motor *_motor;
    SystemState *_sysState;
    virtual bool runSafetyTestsAndReturnIsSafe();
};

#endif
