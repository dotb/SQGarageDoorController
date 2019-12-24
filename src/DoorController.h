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
        virtual unsigned long sensorTriggerSpeed();
        virtual void loop();

        motor_status motorStatus = motor_stopped;
        unsigned long lastMotorMovementTimeStamp = 0;
        String movementSensorSpeedStr = "0";
        unsigned int motorRunningStartTime = 0;

    private:
        Motor *_motor; 
        SystemState *_sysState;
        unsigned int _movementSensorTriggerCount = 0;
        unsigned int _movementSensorSpeed = 0;
        
        // Position constants
        int CONST_MOTOR_INCREMENTS_2 = 2;
        int CONST_MOTOR_INCREMENTS_4 = 4;
        int CONST_DOOR_DISTANCE_NEAR = 300;
        int CONST_DOOR_DISTANCE_VERY_CLOSE = 50;

        // Speed constants
        unsigned int CONST_MOTOR_SPEED_FAST = 0;
        unsigned int CONST_MOTOR_SPEED_MEDIUM = 180;
        unsigned int CONST_MOTOR_SPEED_SLOW = 200;

        // Time constants
        unsigned int CONST_MILLIS_AS_MICROS_500 = 500000;
        unsigned int CONST_SENSOR_DEBOUNCE_TIME_MICROS = 1000;

        virtual void syncDoorPosition();
        virtual int getAppropriateSpeed();
        virtual void updateMotorStatus();
        virtual void checkPinchDetection();
        virtual void checkPinchDetectionWithSpeed(unsigned int speed);  
};

#endif