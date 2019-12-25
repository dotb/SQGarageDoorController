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

    // Move the door forward (close the door) if we're less than 2 increments away from the  closed position
    if (_sysState->currentPosition < _sysState->targetPosition - CONST_MOTOR_INCREMENTS_2) {
        _motor->turnMotorForward(speed);

    // Move the door backward (open the door) if we're more than 2 increments away from the open position
    } else if (_sysState->currentPosition > _sysState->targetPosition + CONST_MOTOR_INCREMENTS_2) {
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
        return CONST_MOTOR_SPEED_MEDIUM;

    // Move fast if we need to cover distance
    } else if (sys_normal == _sysState->systemStatus 
                && distanceToEnd > CONST_DOOR_DISTANCE_NEAR) {
        return CONST_MOTOR_SPEED_FAST;

    // Move at a medium speed when we get closer to the resting point
    } else if (sys_normal == _sysState->systemStatus 
                && distanceToEnd > CONST_DOOR_DISTANCE_VERY_CLOSE) {
        return CONST_MOTOR_SPEED_MEDIUM;

    // By default move slowly, i.e. when close to the resting point
    } else {
        return CONST_MOTOR_SPEED_SLOW;
    }
}

/*
 * This method is called when the hall effect sensor is triggered
 * through an interrupt.
 */
void DoorController::motorDidChangeIncrement() {
    // Debounce the interrupt
    unsigned long timeStampNow = micros();
    _movementSensorSpeed = timeStampNow - lastMotorMovementTimeStamp;
    if (_movementSensorSpeed > CONST_SENSOR_DEBOUNCE_TIME_MICROS) {
        _movementSensorTriggerCount++;
        lastMotorMovementTimeStamp = timeStampNow;

        switch (motorStatus) {
            case motor_stopped:
                _sysState->systemStatus = sys_error;
                break;

            case motor_running_forward_closing:
                _sysState->currentPosition++;
                break;

            case motor_freewheeling_forward_closing:
                _sysState->currentPosition++;
                break;

            case motor_running_backward_opening:
                _sysState->currentPosition--;
                break;

            case motor_freewheeling_backward_opening:
                _sysState->currentPosition--;
                break;
        }
    }
}

// Increment the up / open / backward point
void DoorController::nudgeOpenEndpoint() {
    _sysState->endPointOpenPosition = _sysState->currentPosition - CONST_MOTOR_INCREMENTS_4;
    _sysState->targetPosition = _sysState->endPointOpenPosition;
}

// Increment the down / closed / forward point
void DoorController::nudgeClosedEndpoint() {
    _sysState->endPointClosedPosition = _sysState->currentPosition + CONST_MOTOR_INCREMENTS_4;
    _sysState->targetPosition = _sysState->endPointClosedPosition;
}

void DoorController::stopMotor() {
    _motor->stopMotor();
    _sysState->targetPosition = _sysState->currentPosition;
}

/* Get the last known sensor trigger time.
 * The lower the number, the faster the motor is turning.
 * The larger the number, the slower the motor is turning.
 * 0 Means the motor is not turning.
 * We need at least one previous reading to return an accurate result.
 */
unsigned long DoorController::movementSensorSpeed() {
    if (_movementSensorTriggerCount >= 2) {
        return _movementSensorSpeed;
    } else {
        return CONST_TIME_ZERO;
    }
}

/* Return the time since the motor last moved.
 * This will be short if the motor is moving, and long if it has stopped.
 */
unsigned long DoorController::timeSinceMotorMoved() {
    return micros() - lastMotorMovementTimeStamp;
}

unsigned long DoorController::motorRunningTimeMicros() {
    return micros() - motorRunningStartTimeStamp;
}

/* This method manages the state transition of the motor / door. It
 * wrapps the motor state and allows:
 *  - detection for when the motor is freewheeling,
 *  - detection of transitions between states.
 */
void DoorController::updateMotorStatus() {
    switch (_motor->lastMotorCommand) {
        case cmd_motor_run_forward_closing:
            if (motor_running_forward_closing != motorStatus) {
                motorRunningStartTimeStamp = micros();
            }
            motorStatus = motor_running_forward_closing;
            break;

        case cmd_motor_run_backward_opening:
            if (motor_running_backward_opening != motorStatus) {
                motorRunningStartTimeStamp = micros();
            }
            motorStatus = motor_running_backward_opening;
            break;

        case cmd_motor_stop:
            if (timeSinceMotorMoved() > CONST_MILLIS_AS_MICROS_500) {
                _movementSensorTriggerCount = 0; // Reset the sensor count.
                motorStatus = motor_stopped;

            } else if (motor_running_forward_closing == motorStatus) {
                motorStatus = motor_freewheeling_forward_closing;

            } else if (motor_running_backward_opening == motorStatus) {
                motorStatus = motor_freewheeling_backward_opening;
            }
            break;
    }
}

/*
 * Check if the motor is moving too slowly because it's become stuck.
 * If the sensor hasn't fired in more than 3 seconds, the motor probably isn't moving yet.
 * If the sensor speed is between 50ms and 3 seconds, there's probably a pinch issue
 */
void DoorController::checkPinchDetection() {
    int speed = getAppropriateSpeed();
    if (sys_configure_endpoints == _sysState->systemStatus) {
        checkPinchDetectionWithSensorSpeed(CONST_PINCH_MAX_SPEED_WHEN_MOTOR_SLOW_CONFIG);
        
    } else if (CONST_MOTOR_SPEED_SLOW == speed) {
        checkPinchDetectionWithSensorSpeed(CONST_PINCH_MAX_SPEED_WHEN_MOTOR_SLOW);

    } else if (CONST_MOTOR_SPEED_MEDIUM == speed) {
        checkPinchDetectionWithSensorSpeed(CONST_PINCH_MAX_SPEED_WHEN_MOTOR_MEDIUM);

    } else {
        checkPinchDetectionWithSensorSpeed(CONST_PINCH_MAX_SPEED_WHEN_MOTOR_FAST);
    }
}

void DoorController::checkPinchDetectionWithSensorSpeed(unsigned long maxAllowedSensorSpeed) {    
    if (motorRunningTimeMicros() > CONST_SECONDS_AS_MICROS_1
        && movementSensorSpeed() > maxAllowedSensorSpeed) {
        _sysState->userMessage = String::format("Change %d to %d.", maxAllowedSensorSpeed, movementSensorSpeed());
        stopMotor();
        
        // Move backward a few increments to release the pinched object
        switch (motorStatus) {
            case motor_running_forward_closing:
                _sysState->targetPosition = _sysState->currentPosition - CONST_PINCH_ROLL_BACK_AMOUNT;
                break;
            case motor_running_backward_opening:
                _sysState->targetPosition = _sysState->currentPosition + CONST_PINCH_ROLL_BACK_AMOUNT;
                break;
        }
    }
}
    
void DoorController::loop() {
    syncDoorPosition();
    updateMotorStatus();
    checkPinchDetection();
}