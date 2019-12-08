#include "SafetyTests.h"
#include "Motor.h"
#include "SystemState.h"

SafetyTests::SafetyTests(Motor *motor, SystemState *sysState) {
    _motor = motor;
    _sysState = sysState;
}

bool SafetyTests::runSafetyTestsAndReturnIsSafe() {
    // Ensure the door never goes past the open or closed positions
    if (_sysState->currentPosition > _sysState->endPointClosedPosition || _sysState->currentPosition < _sysState->endPointOpenPosition) {
        _sysState->throwFatalError("Door went past open or closed endpoints");
        return false;
    }

    // Ensure the target position never goes past the open or closed positions
    if (_sysState->targetPosition > _sysState->endPointClosedPosition || _sysState->targetPosition < _sysState->endPointOpenPosition) {
        _sysState->throwFatalError("The target position went past open or closed endpoints");
        return false;
    }

    // Ensure the closed position is never behind the open position
    if (_sysState->endPointClosedPosition < _sysState->endPointOpenPosition) {
        _sysState->throwFatalError("The endpoints crossed each other. The closed pos is < the open pos.");
        return false;
    }

    // Everything is safe
    return true;
}

bool SafetyTests::ensureEverythingIsSafe() {
    return runSafetyTestsAndReturnIsSafe();
}
