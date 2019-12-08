#include "UserInterface.h"
#include "Motor.h"
#include "SystemState.h"

UserInterface::UserInterface(Motor *motor, 
                  SystemState *sysState, 
                  int upButtonPin, 
                  int downButtonPin,
                  int codeButtonPin,
                  int setButtonPin,
                  int mButtonPin) {
    _motor = motor;
    _sysState = sysState;
    _upButtonPin = upButtonPin;
    _downButtonPin = downButtonPin;
    _codeButtonPin = codeButtonPin;
    _setButtonPin = setButtonPin;
    _mButtonPin = mButtonPin;

    pinMode(_upButtonPin, INPUT_PULLUP);
    pinMode(_downButtonPin, INPUT_PULLUP);
    pinMode(_setButtonPin, INPUT_PULLUP);
    pinMode(_codeButtonPin, INPUT_PULLUP);
    pinMode(_mButtonPin, INPUT_PULLUP);
}

void UserInterface::handleButtons() {
    // Increment the up / open / backward point
    if (sys_configure_endpoints == _sysState->systemStatus && 
                                                 upButtonIsPressed()) {
        _sysState->endPointOpenPosition = _sysState->currentPosition - 2;
        _sysState->targetPosition = _sysState->endPointOpenPosition;

    // Increment the down / closed / forward point
    } else if (sys_configure_endpoints == _sysState->systemStatus && 
                                                downButtonIsPressed()) {
        _sysState->endPointClosedPosition = _sysState->currentPosition + 2;
        _sysState->targetPosition = _sysState->endPointClosedPosition;

    // Enter configure mode
    } else if ((sys_normal == _sysState->systemStatus || 
                sys_reset == _sysState->systemStatus) && 
                                                 setButtonIsPressed()) {
        _motor->stopMotor();
        _sysState->systemStatus = sys_configure_endpoints;
        delay(500);

    // Exit configure mode
    } else if (sys_configure_endpoints == _sysState->systemStatus && 
                                                setButtonIsPressed()) {
        _motor->stopMotor();
        _sysState->systemStatus = sys_normal;
        _sysState->targetPosition = _sysState->currentPosition;
        delay(500);

    } else if (                                codeButtonIsPressed()) {
        _motor->stopMotor();
        
    } else if (                                   mButtonIsPressed()) {
        _motor->stopMotor();
    }
}

// Set the up / open / backward point
bool UserInterface::upButtonIsPressed() {
    return digitalRead(_upButtonPin) == LOW;
}

// Set the down / closed / forward point
bool UserInterface::downButtonIsPressed() {
    return digitalRead(_downButtonPin) == LOW;
}

bool UserInterface::setButtonIsPressed() {
    return digitalRead(_setButtonPin) == LOW;
}

bool UserInterface::codeButtonIsPressed() {
    return digitalRead(_codeButtonPin) == LOW;
}

bool UserInterface::mButtonIsPressed() {
    return digitalRead(_mButtonPin) == LOW;
}

void UserInterface::loop() {
    handleButtons();
}
