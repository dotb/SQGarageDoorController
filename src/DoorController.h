#ifndef DoorController_h
#define DoorController_h

#include "Arduino.h"
#include "SystemState.h"
#include "Motor.h"

class DoorController {
    public:
        DoorController(Motor *motor, SystemState *sysState, int rotationInputPin);
        virtual void setDoorPosition(String newPercentage);
        virtual void motorDidChangeIncrement();
        virtual void loop();

    private:
        SystemState *_sysState;
        Motor *_motor; 
        unsigned long _lastMotorMovementTimeStamp = 0;
        int _rotationInputPin;
        virtual void syncDoorPosition();
        virtual int getAppropriateSpeed();
        virtual bool doorHasReachedDestination();
};

#endif