#ifndef Motor_h
#define Motor_h

#include "Arduino.h"

class Motor {
  public:
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