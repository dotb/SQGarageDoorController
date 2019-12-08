#ifndef SafetyTests_h
#define SafetyTests_h

#include "Arduino.h"
#include "Motor.h"
#include "DoorController.h"
#include "SystemState.h"

class SafetyTests {
public:
    SafetyTests(Motor *motor, 
                DoorController *doorController, 
                SystemState *sysState);
    virtual bool ensureEverythingIsSafe();

private:
    DoorController *_doorController;
    Motor *_motor;
    SystemState *_sysState;
    unsigned int _motorRunStartTime = 0;
};

#endif
