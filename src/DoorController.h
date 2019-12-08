#ifndef DoorController_h
#define DoorController_h

#include "Arduino.h"
#include "SystemState.h"
#include "Motor.h"

// Motor status
enum motor_status {
  motor_stopped,
  motor_running_forward_closing,
  motor_running_backward_opening,
  motor_freewheeling_forward_closing,
  motor_freewheeling_backward_opening,
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

        motor_status motorStatus = motor_stopped;
        unsigned long lastMotorMovementTimeStamp = 0;
        unsigned int movementSensorSpeed = 0;
    private:
        Motor *_motor; 
        SystemState *_sysState;
        virtual void syncDoorPosition();
        virtual int getAppropriateSpeed();
        virtual void updateMotorStatus();
};

#endif