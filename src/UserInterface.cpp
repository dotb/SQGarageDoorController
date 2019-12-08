#include "UserInterface.h"
#include "Motor.h"
#include "SystemState.h"

UserInterface::UserInterface(SystemState *sysState,
                            DoorController *doorController,
                            int upButtonPin,
                            int downButtonPin,
                            int codeButtonPin,
                            int setButtonPin,
                            int mButtonPin) {
    _sysState = sysState;
    _doorController = doorController;
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
        _doorController->nudgeOpenEndpoint();

    // Increment the down / closed / forward point
    } else if (sys_configure_endpoints == _sysState->systemStatus && 
                                                downButtonIsPressed()) {
        _doorController->nudgeClosedEndpoint();

    // Enter configure mode
    } else if ((sys_normal == _sysState->systemStatus || 
                sys_reset == _sysState->systemStatus) && 
                                                 setButtonIsPressed()) {
        _doorController->stopMotor();
        _sysState->systemStatus = sys_configure_endpoints;
        delay(500);

    // Exit configure mode
    } else if (sys_configure_endpoints == _sysState->systemStatus && 
                                                setButtonIsPressed()) {
        _doorController->stopMotor();
        _sysState->systemStatus = sys_normal;
        _sysState->targetPosition = _sysState->currentPosition;
        delay(500);

    } else if (                                codeButtonIsPressed()) {
        _sysState->throwFatalError("User pressed CODE button");
        
    } else if (                                   mButtonIsPressed()) {
        _sysState->throwFatalError("User pressed M button");
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
