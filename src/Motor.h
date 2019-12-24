#ifndef Motor_h
#define Motor_h

#include "Arduino.h"

enum motor_command {
  cmd_motor_run_forward_closing,
  cmd_motor_run_backward_opening,
  cmd_motor_stop
};

class Motor {
  public:
    motor_command lastMotorCommand = cmd_motor_stop;
    Motor(int relayOnePin, int relayTwoPin, int motorDriverPin);
    virtual void turnMotorForward(int speed);
    virtual void turnMotorBackward(int speed);
    virtual void stopMotor();

  private:
    int _relayOnePin;
    int _relayTwoPin;
    int _motorDriverPin;
};

#endif