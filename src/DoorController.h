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
        virtual unsigned long movementSensorSpeed();
        virtual unsigned long timeSinceMotorMoved();
        virtual unsigned long motorRunningTimeMicros();
        virtual void loop();

        motor_status motorStatus = motor_stopped;
        unsigned long lastMotorMovementTimeStamp = 0;
        unsigned long motorRunningStartTimeStamp = 0;
        String movementSensorSpeedStr = "0";

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
        int CONST_MOTOR_SPEED_FAST = 0;
        int CONST_MOTOR_SPEED_MEDIUM = 180;
        int CONST_MOTOR_SPEED_SLOW = 200;

        // Time constants
        unsigned long CONST_TIME_ZERO = 0;
        unsigned long CONST_MILLIS_AS_MICROS_500 = 500000;
        unsigned long CONST_SECONDS_AS_MICROS_1 = 1000000;
        unsigned long CONST_SENSOR_DEBOUNCE_TIME_MICROS = 1000;

        // Pinch detection parameters
        // 7500us between ticks when fast
        // 14000us between ticks when slow
        unsigned long CONST_PINCH_MAX_SPEED_WHEN_MOTOR_FAST = 9100;
        unsigned long CONST_PINCH_MAX_SPEED_WHEN_MOTOR_MEDIUM = 17000;
        unsigned long CONST_PINCH_MAX_SPEED_WHEN_MOTOR_SLOW = 30000;
        unsigned long CONST_PINCH_MAX_SPEED_WHEN_MOTOR_SLOW_CONFIG = 45000;
        int CONST_PINCH_ROLL_BACK_AMOUNT = 100;

        virtual void syncDoorPosition();
        virtual int getAppropriateSpeed();
        virtual void updateMotorStatus();
        virtual void checkPinchDetection();
        virtual void checkPinchDetectionWithSensorSpeed(unsigned long maxAllowedSensorSpeed);
};

#endif