#include "SafetyTests.h"
#include "SystemState.h"

SafetyTests::SafetyTests(Motor *motor, 
                        DoorController *doorController, 
                        SystemState *sysState) {
    _sysState = sysState;
    _doorController = doorController;
    _motor = motor;
}

bool SafetyTests::ensureEverythingIsSafe() {
    // Ensure the door never goes past the open or closed positions
    if (_sysState->currentPosition > _sysState->endPointClosedPosition || _sysState->currentPosition < _sysState->endPointOpenPosition) {
        _sysState->throwFatalError("Door went past open or closed endpoints.");
        return false;
    }

    // Ensure the target position never goes past the open or closed positions
    if (_sysState->targetPosition > _sysState->endPointClosedPosition || _sysState->targetPosition < _sysState->endPointOpenPosition) {
        _sysState->throwFatalError("The target position went past open or closed endpoints.");
        return false;
    }

    // Ensure the closed position is never behind the open position
    if (_sysState->endPointClosedPosition < _sysState->endPointOpenPosition) {
        _sysState->throwFatalError("The endpoints crossed each other. The closed pos is < the open pos.");
        return false;
    }

    /* 
     * Ensure the motor sensor is triggering if the motor is running.
     * Monitor the motor start time, if after 100ms the sensor has not raised
     * a notification, throw an error.
     */ 
    unsigned int timeNow = millis();
    if (motor_stopped == _motor->lastMotorCommand) {
        _motorRunStartTime = timeNow;
    } else if ((motor_run_forward_closing == _motor->lastMotorCommand || 
        motor_run_backward_opening == _motor->lastMotorCommand) &&
        timeNow - _doorController->lastMotorMovementTimeStamp > 100000 &&
        timeNow - _motorRunStartTime > 100000) {
            _sysState->throwFatalError("The motor sensor is reading too slow or not working.");
            return false;
        }

    // Everything is safe
    return true;
}
