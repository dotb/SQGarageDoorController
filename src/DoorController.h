#ifndef DoorController_h
#define DoorController_h

#include "Arduino.h"
#include "SystemState.h"
#include "Motor.h"

class DoorController {
    public:
        DoorController(Motor *motor, SystemState *sysState);
        virtual void setDoorPosition(String newPercentage);
        virtual void motorDidChangeIncrement();
        virtual void nudgeOpenEndpoint();
        virtual void nudgeClosedEndpoint();
        virtual void stopMotor();
        virtual void loop();

    private:
        SystemState *_sysState;
        Motor *_motor; 
        unsigned long _lastMotorMovementTimeStamp = 0;
        virtual void syncDoorPosition();
        virtual int getAppropriateSpeed();
};

#endif