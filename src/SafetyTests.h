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
    virtual bool isMotorSupposedToBeMoving();

    // Constants
    unsigned long CONST_MAX_SENSOR_EVENT_TIME = 45000;
    unsigned long CONST_MAX_MOTOR_RUN_TIME = 10 * 1000000; // Max time to fully open or close (10 seconds)
    int CONST_MAX_ENDPONT_DISTANCE = 2000;
    int CONST_MAX_ENDPONT_OPEN_POS = -2000;
    int CONST_MAX_ENDPONT_CLOSED_POS = 2000;
};

#endif
