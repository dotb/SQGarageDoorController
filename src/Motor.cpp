#include "Motor.h"

Motor::Motor(int relayOnePin, int relayTwoPin, int motorDriverPin) {
    _relayOnePin = relayOnePin;
    _relayTwoPin = relayTwoPin;
    _motorDriverPin = motorDriverPin;

    pinMode(_motorDriverPin, OUTPUT);
    pinMode(_relayTwoPin, OUTPUT);
    pinMode(_relayOnePin, OUTPUT);

    stopMotor();
}

 // Move forward / closed
void Motor::turnMotorForward(int speed) {
    digitalWrite(_relayOnePin, HIGH);
    digitalWrite(_relayTwoPin, LOW);
    analogWrite(_motorDriverPin, speed);
    motorStatus = motor_running_forward_closing;
}

// Move backward / open
void Motor::turnMotorBackward(int speed) {
    digitalWrite(_relayOnePin, LOW);
    digitalWrite(_relayTwoPin, HIGH);
    analogWrite(_motorDriverPin, speed);
    motorStatus = motor_running_backward_opening;
}

// Stop the motor
void Motor::stopMotor() {
    digitalWrite(_relayOnePin, LOW);
    digitalWrite(_relayTwoPin, LOW);
    digitalWrite(_motorDriverPin, HIGH);
    motorStatus = motor_stopped;
}
