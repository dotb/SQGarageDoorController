#include "Motor.h"
#include "SystemState.h"
#include "SafetyTests.h"
#include "DoorController.h"
#include "UserInterface.h"
#include "LEDController.h"

// Digital Pins
#define motorDriverPin 2    // interrupt shared with A0, A3 (PWM with A5)
#define ledLampPin 3        // interrupt shared with DAC (PWM with A4)
#define relayOnePin 5       // unrestricted interrupt
#define relayTwoPin 6       // unrestricted interrupt

// Analog Pins
#define codeButtonPin A1    // interrupt shared with D4
#define rotationInputPin A2 // unrestricted interrupt
#define upButtonPin A3      // interrupt shared with D2, A0
#define downButtonPin A4    // interrupt shared with D1 (PWM with D3)
#define setButtonPin A5     // no interrupt, shared with D0 and SETUP button. (PWM with D2)
#define ledRedPin WKP       // unrestricted interrupt (PWM)
#define ledGreenPin TX      // unrestricted interrupt (PWM)
#define ledBluePin RX       // unrestricted interrupt (PWM)
#define mButtonPin DAC      // interrupt shared with D3

SystemState systemState;
Motor motor(relayOnePin,
            relayTwoPin,
            motorDriverPin);
SafetyTests safetyTests(&systemState);
DoorController doorController(&motor,
                             &systemState);
UserInterface userInterface(&systemState,
                            &doorController,
                            upButtonPin, 
                            downButtonPin,
                            codeButtonPin,
                            setButtonPin,
                            mButtonPin);
LEDController ledController(&systemState,
                            ledLampPin,
                            ledRedPin,
                            ledGreenPin,
                            ledBluePin);

void setup() {

    // Ensure the motor is off
    doorController.stopMotor();

    // Load previous stored state
    systemState.restoreFromMemory();

    // Turn on the light
    ledController.lightOn();    

    // Attach an interrupt to count incrememental movement of the motor
    pinMode(rotationInputPin, INPUT_PULLUP);
    attachInterrupt(rotationInputPin, motorDidChangeIncrement, CHANGE);
    
    // Map particle functions
    Particle.function("lightOn", lightOn);
    Particle.function("lightOff", lightOff);
    Particle.function("setPosition", setDoorPosition);
    Particle.function("resetMemory", resetMemory);
    Particle.variable("state", systemState.stateAsString);
    Particle.variable("userMessage", systemState.userMessage);
}

void loop() {
    if (safetyTests.ensureEverythingIsSafe()) {
        switch (systemState.systemStatus) {
            // Everything is normal, allow user operation and setup
            case sys_normal:
                userInterface.loop();
                doorController.loop();
                break;

            // The system has been reset, allow user setup
            case sys_reset:
                userInterface.loop();
                break;

            // User is configuring the door endpoints
            case sys_configure_endpoints:
                userInterface.loop();
                doorController.loop();
                break;

            // Error, everybody out of the pool
            case sys_error:
                doorController.stopMotor();
                break;
        }
    } else {
        // Ensure the motor is stopped
        doorController.stopMotor();
    }

    systemState.loop();
    ledController.loop();
}


/**
 * Interrupt proxy functions
 */
void motorDidChangeIncrement() {
    doorController.motorDidChangeIncrement();
} 

/**
 * Particle proxy functions 
 */
int lightOn(String param) {
    ledController.lightOn();
    return 0;
}

int lightOff(String param) {
    ledController.lightOff();
    return 0;
}

int setDoorPosition(String param) {
    doorController.setDoorPosition(param);
    return 0;
}

int resetMemory(String param) {
    systemState.resetMemory();
    return 0;
}
