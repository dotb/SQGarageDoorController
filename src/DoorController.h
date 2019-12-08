#ifndef DoorController_h
#define DoorController_h

#include "Arduino.h"
#include "SystemState.h"
#include "Motor.h"

// Motor status
enum motor_status {
  motor_stopped,
  motor_running_forward_closing,
  motor_running_backward_opening
};

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
        Motor *_motor; 
        SystemState *_sysState;
        motor_status _motorStatus = motor_stopped;
        unsigned long _lastMotorMovementTimeStamp = 0;
        virtual void syncDoorPosition();
        virtual int getAppropriateSpeed();
};

#endif