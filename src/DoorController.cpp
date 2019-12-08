#include "DoorController.h"

DoorController::DoorController(Motor *motor, SystemState *sysState) {
    _motor = motor;
    _sysState = sysState;
}

// Set the door position as a %. 0% = closed and 100% = open
void DoorController::setDoorPosition(String newPercentage) {
    int newTargetPercentage = newPercentage.toInt();
    int newTargetPosition = map(newTargetPercentage, 0, 100, _sysState->endPointClosedPosition, _sysState->endPointOpenPosition);

    // Check that the targetPosition is a valid value
    if (newTargetPercentage <= 100 &&
        newTargetPercentage >= 0 &&
        newTargetPosition <= _sysState->endPointClosedPosition &&
        newTargetPosition >= _sysState->endPointOpenPosition) {

            _sysState->targetPosition = newTargetPosition;
    } else {
            _sysState->throwFatalError(String::format("Requested position is out of bounds %s", newPercentage));
    }
}

// Called from the main loop
void DoorController::syncDoorPosition() {

    // move the door toward the destimation position
    int speed = getAppropriateSpeed();

    // Move the door forward (close the door)
    if (_sysState->currentPosition < _sysState->targetPosition) {
        _motor->turnMotorForward(speed);

    // Move the door backward (open the door)
    } else if (_sysState->currentPosition > _sysState->targetPosition) {
        _motor->turnMotorBackward(speed);

    // We've reached the destination, stop the motor
    } else {    
        stopMotor();
    }
}

/* 
 * Work out the appropriate motor speed
 * Large numbers are slow
 * Small numbers are fast
 */
int DoorController::getAppropriateSpeed() {

    // Calculate the distance to the stop position to see how close the door is the resting point
    int distanceToEnd = abs(_sysState->targetPosition - _sysState->currentPosition);

    // Move slowly when setting end points
    if (sys_configure_endpoints == _sysState->systemStatus) {
        return 220;

    // Move fast if we need to cover distance
    } else if (sys_normal == _sysState->systemStatus && distanceToEnd > 300) {
        return 50;

    // Move at a medium speed when we get closer to the resting point
    } else if (sys_normal == _sysState->systemStatus && distanceToEnd > 100) {
        return 180;

    // By default move slowly, i.e. when close to the resting point
    } else {
        return 210;
    }
}

/*
 * This method is called when the hall effect sensor is triggered
 * through an interrupt.
 */
void DoorController::motorDidChangeIncrement() {
    // Debounce the interrupt
    unsigned long timeStampNow = micros();
    if (timeStampNow - _lastMotorMovementTimeStamp > 10) {
        _lastMotorMovementTimeStamp = timeStampNow;

        switch (_motor->motorStatus) {
            case motor_stopped:
                _sysState->systemStatus = sys_error;
                break;
            case motor_running_forward_closing:
                _sysState->currentPosition++;
                break;
            case motor_running_backward_opening:
                _sysState->currentPosition--;
                break;
        }

    }
}

// Increment the up / open / backward point
void DoorController::nudgeOpenEndpoint() {
    _sysState->endPointOpenPosition = _sysState->currentPosition - 2;
    _sysState->targetPosition = _sysState->endPointOpenPosition;
}

// Increment the down / closed / forward point
void DoorController::nudgeClosedEndpoint() {
    _sysState->endPointClosedPosition = _sysState->currentPosition + 2;
    _sysState->targetPosition = _sysState->endPointClosedPosition;
}

void DoorController::stopMotor() {
    _motor->stopMotor();
    _sysState->targetPosition = _sysState->currentPosition;
}

void DoorController::loop() {
    syncDoorPosition();
}