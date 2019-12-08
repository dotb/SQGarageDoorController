#ifndef Motor_h
#define Motor_h

#include "Arduino.h"

// Motor status
enum motor_status {
  motor_stopped,
  motor_running_forward_closing,
  motor_running_backward_opening
};

class Motor {
  public:
      motor_status motorStatus = motor_stopped;

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