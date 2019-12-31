#include "SafetyTests.h"
#include "SystemState.h"

SafetyTests::SafetyTests(Motor *motor, 
                        DoorController *doorController, 
                        SystemState *sysState) {
    _sysState = sysState;
    _doorController = doorController;
    _motor = motor;
}

bool SafetyTests::isMotorSupposedToBeMoving() {
    bool isMotorStopped = cmd_motor_stop == _motor->lastMotorCommand;
    return !isMotorStopped;
}

bool SafetyTests::ensureEverythingIsSafe() {
    // Ensure the door never goes past the open or closed positions
    if (_sysState->currentPosition < _sysState->endPointOpenPosition) {
        _sysState->throwFatalError("Door went past open endpoint.");
        return false;
    }

    if (_sysState->currentPosition > _sysState->endPointClosedPosition) {
        _sysState->throwFatalError("Door went past closed endpoint.");
        return false;
    }

    // Ensure the target position never goes past the open or closed positions
    if (_sysState->targetPosition < _sysState->endPointOpenPosition) {
        _sysState->throwFatalError("The target position went past the open endpoint.");
        return false;
    }

    if (_sysState->targetPosition > _sysState->endPointClosedPosition) {
        _sysState->throwFatalError("The target position went past the closed endpoint.");
        return false;
    }

    // Ensure the closed position is never behind the open position
    if (_sysState->endPointClosedPosition < _sysState->endPointOpenPosition) {
        _sysState->throwFatalError("The endpoints crossed each other. The closed pos is < the open pos.");
        return false;
    }

    // Ensure the endpoint positions are not too far apart
    if (_sysState->endPointOpenPosition - _sysState->endPointClosedPosition > CONST_MAX_ENDPONT_DISTANCE) {
        _sysState->throwFatalError("The endpoints are too far apart.");
        return false;
    }

    // Ensure the open endpoint is not a way off number
    if (_sysState->endPointOpenPosition < CONST_MAX_ENDPONT_OPEN_POS) {
        _sysState->throwFatalError("The open endpoint is too low.");
        return false;
    }

    // Ensure the closed endpoint is not a way off number
    if (_sysState->endPointClosedPosition > CONST_MAX_ENDPONT_CLOSED_POS) {
        _sysState->throwFatalError("The closed endpoint is too high.");
        return false;
    }

    /* 
     * Ensure the motor sensor is triggering if the motor is running.
     * Monitor the motor start time, if after 100ms the sensor has not raised
     * a notification, throw an error.
     */ 
    unsigned int timeNow = millis();

    // Keep a note of the time the motor started running
    if (!isMotorSupposedToBeMoving()) {
        _motorRunStartTime = timeNow;
    }


    // Everything is safe
    return true;
}
