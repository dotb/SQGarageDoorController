#include "SystemState.h"

void SystemState::saveToMemory() {
    EEPROM.put(_MEM_endPointOpenPosition, endPointOpenPosition);
    EEPROM.put(_MEM_endPointClosedPosition, endPointClosedPosition);
    EEPROM.put(_MEM_currentPosition, currentPosition);
}

void SystemState::restoreFromMemory() {
    // Setup endpoints and positions
    EEPROM.get(_MEM_endPointOpenPosition, endPointOpenPosition);
    EEPROM.get(_MEM_endPointClosedPosition, endPointClosedPosition);
    EEPROM.get(_MEM_currentPosition, currentPosition);

    // Check that the memory doesn't contain defaults.
    if (currentPosition >= 0xFFFF ||
        endPointOpenPosition >= 0xFFFF ||
        endPointClosedPosition >= 0xFFFF || 
        (endPointOpenPosition == 0 && endPointClosedPosition == 0)) {
        resetMemory();
    } else {
        targetPosition = currentPosition;
        systemStatus = sys_normal;
    }
}

void SystemState::resetMemory() {
    currentPosition = 0;
    targetPosition = 0;
    endPointOpenPosition = 0;
    endPointClosedPosition = 0;
    EEPROM.put(_MEM_endPointOpenPosition, 0);
    EEPROM.put(_MEM_endPointClosedPosition, 0);
    EEPROM.put(_MEM_currentPosition, 0);
    systemStatus = sys_reset;
}

void SystemState::throwFatalError(String userErrorMessage) {
    systemStatus = sys_error;
    userMessage = userErrorMessage;
}

void SystemState::updateStateAsString() {
    String status = "";
    switch (systemStatus) {
        case sys_normal:
            status = "sys_normal";
            break;
        case sys_reset:
            status = "sys_reset";
            break;
        case sys_configure_endpoints:
            status = "sys_configure_endpoints";
            break;
        case sys_error:
            status = "sys_error";
            break;
    }
    stateAsString = String::format("*%s* (endPointOpenPos %d) - (currentPos %d) (targetPos %d) -  (endPointClosedPosition %d)", status.c_str(), endPointOpenPosition, currentPosition, targetPosition, endPointClosedPosition);
}

void SystemState::loop() {
    // Save the latest positions if the motor is not moving
    if (targetPosition == currentPosition) {
        saveToMemory();
    }
    updateStateAsString();
}
