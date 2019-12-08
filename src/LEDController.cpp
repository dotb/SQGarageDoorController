#include "LEDController.h"

LEDController::LEDController(SystemState *sysState,
                                    int ledLampPin,
                                    int ledRedPin,
                                    int ledGreenPin,
                                    int ledBluePin) {
    _sysState = sysState;
    _ledLampPin = ledLampPin;
    _ledRedPin = ledRedPin;
    _ledGreenPin = ledGreenPin;
    _ledBluePin = ledBluePin;
    
    pinMode(ledLampPin, OUTPUT);
    pinMode(ledBluePin, OUTPUT);
    pinMode(ledRedPin, OUTPUT);
    pinMode(ledGreenPin, OUTPUT);
}

void LEDController::lightOn() {
    _lampOnTimestamp = millis();
    analogWrite(_ledLampPin, 255);
}

void LEDController::lightOff() {
    analogWrite(_ledLampPin, 0);
}

void LEDController::displayNormal() {
    analogWrite(_ledRedPin, 0);
    analogWrite(_ledGreenPin, 255);
    analogWrite(_ledBluePin, 0);
}

void LEDController::displayReset() {
    analogWrite(_ledRedPin, 0);
    analogWrite(_ledGreenPin, 0);
    analogWrite(_ledBluePin, 255);
}

void LEDController::displayConfigure() {
    analogWrite(_ledRedPin, 128);
    analogWrite(_ledGreenPin, 128);
    analogWrite(_ledBluePin, 128);
}

void LEDController::displayError() {
    analogWrite(_ledRedPin, 255);
    analogWrite(_ledGreenPin, 0);
    analogWrite(_ledBluePin, 0);
}

void LEDController::updateStatusLED() {
    switch (_sysState->systemStatus) {
      case  sys_normal:
        displayNormal();
        break;
      case sys_reset:
        displayReset();
        break;
      case sys_configure_endpoints:
        displayConfigure();
        break;
      case sys_error:
        displayError();
        break;
    }
}

void LEDController::loop() {
    updateStatusLED();

    // Turn off the lamp if it's been on for 5 minutes
    if (millis() - _lampOnTimestamp > 5 * 60000) {
        lightOff();
    }
}