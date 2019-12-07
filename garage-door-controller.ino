// Digital Pins
// #define unused 0         // no interrupt, shared with A5 and SETUP button (PWM)
// #define unused 1         // interrupt shared with A4 (PWM)
#define motorDriverPin 2    // interrupt shared with A0, A3 (PWM with A5)
#define ledLampPin 3        // interrupt shared with DAC (PWM with A4)
// #define  unused 4        // interrupt shared with A1
#define relayOnePin 5       // unrestricted interrupt
#define relayTwoPin 6       // unrestricted interrupt
// #define unused 7         // unrestricted interrupt

// Analog Pins
// #define unused A0        // interrupt shared with D2, A3
#define codeButtonPin A1     // interrupt shared with D4
#define rotationInputPin A2 // unrestricted interrupt
#define upButtonPin A3      // interrupt shared with D2, A0
#define downButtonPin A4    // interrupt shared with D1 (PWM with D3)
#define setButtonPin A5     // no interrupt, shared with D0 and SETUP button. (PWM with D2)
#define ledRedPin WKP       // unrestricted interrupt (PWM)
#define ledGreenPin TX      // unrestricted interrupt (PWM)
#define ledBluePin RX       // unrestricted interrupt (PWM)
#define mButtonPin DAC      // interrupt shared with D3

// Status
enum sys_status {
  sys_normal,
  sys_reset,
  sys_configure_endpoints,
  sys_error
};

sys_status status = sys_error;

// Memory addresses
int MEM_currentPosition = 1 * sizeof(int);
int MEM_endPointOpenPosition = 2 * sizeof(int);
int MEM_endPointClosedPosition = 3 * sizeof(int);

// Position variables
int currentPosition = 0;
int targetPosition = 0;
int endPointOpenPosition = 0;
int endPointClosedPosition = 0;

// A timestamp used to watch the movement speed of the motor to detect if something has gone wrong,
// E.g. the motor should be turning but is stuck.
unsigned long lastIncrementTimeStamp = 0;



void setup() {

    // Sensors
    pinMode(rotationInputPin, INPUT_PULLUP);
    
    // Motor
    pinMode(motorDriverPin, OUTPUT);
    pinMode(relayTwoPin, OUTPUT);
    pinMode(relayOnePin, OUTPUT);
    
    // LEDs
    pinMode(ledLampPin, OUTPUT);
    pinMode(ledBluePin, OUTPUT);
    pinMode(ledRedPin, OUTPUT);
    pinMode(ledGreenPin, OUTPUT);
    
    // Buttons
    pinMode(upButtonPin, INPUT_PULLUP);
    pinMode(downButtonPin, INPUT_PULLUP);
    pinMode(setButtonPin, INPUT_PULLUP);
    pinMode(codeButtonPin, INPUT_PULLUP);
    pinMode(mButtonPin, INPUT_PULLUP);

    // Attach an interrupt to count incrememental movement of the motor
    attachInterrupt(rotationInputPin, motorDidChangeIncrement, CHANGE);
    
    stopMotor();
    lightOn("");

    // Load previous state
    restoreFromMemory();

    // Light control
    Particle.function("lightOn", lightOn);
    Particle.function("lightOff", lightOff);

    // Position getters    
    Particle.function("getOpenPt", getEndPointOpenPosition);
    Particle.function("getClosedPt", getEndPointClosedPosition);
    Particle.function("getCurPt", getCurrentPosition);
    Particle.function("getCurPerc", getCurrentPositionPercentage);
    Particle.function("getTargetPt", getTargetPosition);
    
    // Motor control
    Particle.function("setPosition", setDoorPosition);
    
    // Memory control
    Particle.function("resetMemory", resetMemory);
}

void loop() {
    
    switch (status) {
        // Everything is normal    
        case sys_normal:
            handleButtons();
            syncDoorPosition();
            break;
            
        // The system has been reset
        case sys_reset:
            handleButtons();
            break;
            
        // Configuring the door endpoints
        case sys_configure_endpoints:
            handleButtons();
            syncDoorPosition();
            break;
        
        // Error
        case sys_error:
            stopMotor();
            break;
    }
    updateStatusLED();
    
}

/**
 * Button Handling code
 */
 void handleButtons() {
     
    // Increment the up / open / backward point
    if (sys_configure_endpoints == status           && upButtonIsPressed()) {
        endPointOpenPosition = currentPosition - 2;
        targetPosition = endPointOpenPosition;
        
    // Increment the down / closed / forward point    
    } else if (sys_configure_endpoints == status    && downButtonIsPressed()) {
        endPointClosedPosition = currentPosition + 2;
        targetPosition = endPointClosedPosition;
    
    // Enter configure mode
    } else if ((sys_normal == status || sys_reset == status) && setButtonIsPressed()) {
        status = sys_configure_endpoints;
        stopMotor();
        updateStatusLED();
        delay(500);
        
    // Exit configure mode
    } else if (sys_configure_endpoints == status    && setButtonIsPressed()) {
        status = sys_normal;
        stopMotor();
        targetPosition = currentPosition;
        updateStatusLED();
        delay(500);
        
    } else if (                                        codeButtonIsPressed()) {
        
    } else if (                                        mButtonIsPressed()) {
        
    }
 }
 
 // Set the up / open / backward point
 bool upButtonIsPressed() {
     return digitalRead(upButtonPin) == LOW;
 }
 
 // Set the down / closed / forward point
 bool downButtonIsPressed() {
     return digitalRead(downButtonPin) == LOW;
 }
 
 bool setButtonIsPressed() {
     return digitalRead(setButtonPin) == LOW;
 }
 
 bool codeButtonIsPressed() {
     return digitalRead(codeButtonPin) == LOW;
 }

bool mButtonIsPressed() {
     return digitalRead(mButtonPin) == LOW;
 }

/**
 * Door position code
 */ 
// 0 = closed and 100 = open
int setDoorPosition(String newPercentage) {
    int newTargetPercentage = newPercentage.toInt();
    int newTargetPosition = map(newTargetPercentage, 0, 100, endPointClosedPosition, endPointOpenPosition);
    
    // Check that the targetPosition is value
    if (newTargetPercentage <= 100 && 
        newTargetPercentage >= 0 &&
        newTargetPosition <= endPointClosedPosition && 
        newTargetPosition >= endPointOpenPosition) {
            
            targetPosition = newTargetPosition;
            return 0;
            
    } else {
        status = sys_error;
        return -1;
    }
    
}

// Called from the main loop
void syncDoorPosition() {
    if (ensureEverythingIsSafe()) {
        
        // move the door toward the destimation position
        int speed = getAppropriateSpeed();
        if (currentPosition < targetPosition) {
            // Move the door forward
            turnMotorForward(speed);
            
        } else if (currentPosition > targetPosition) {
            // Move the door backward
            turnMotorBackward(speed);
            
        } else {
            // We've reached the destination, stop the motor
            stopMotor();
        }
        
        // Save the latest positions if the motor is not moving
        if (motorIsStopped) {
            saveToMemory();
        }
    }
}


/**
 * Motor driver code
 */ 
 // Move forward / closed
void turnMotorForward(int speed) {
    if (ensureEverythingIsSafe()) {
        digitalWrite(relayOnePin, HIGH);
        digitalWrite(relayTwoPin, LOW);
        analogWrite(motorDriverPin, speed);
    }
}

// Move backward / open
void turnMotorBackward(int speed) {
    if (ensureEverythingIsSafe()) {
        digitalWrite(relayOnePin, LOW);
        digitalWrite(relayTwoPin, HIGH);
        analogWrite(motorDriverPin, speed);
    }
}

void stopMotor() {
    digitalWrite(relayOnePin, LOW);
    digitalWrite(relayTwoPin, LOW);
    digitalWrite(motorDriverPin, HIGH);
}

bool motorIsStopped() {
    int relayOneValue = digitalRead(relayOnePin);
    int relayTwoValue = digitalRead(relayTwoPin);
    return (relayOneValue == LOW && relayTwoValue == LOW);
}

/* Work out the appropriate speed
 * Large numbers are slow
 * Small numbers are fast
 */
int getAppropriateSpeed() {

    // Calculate the distance to the stop position to see how close the door is.        
    int distanceFromStart = abs(targetPosition - currentPosition);
    int distanceToEnd = abs(targetPosition - currentPosition);
    
    // Move slowly when setting end points
    if (sys_configure_endpoints == status) {
        return 200;
    
    // Move fast if we need to cover distance
    } else if (sys_normal == status && distanceToEnd > 300) {
        return 100;
        
    // Move fast if we need to cover distance    
    } else if (sys_normal == status && distanceToEnd > 100) {
        return 180;
        
    // By default move very slowly, i.e. when close to the endpoint
    } else {
        return 210;
    }
}


/**
 * Rotation counting and positioning
 */ 
 void motorDidChangeIncrement() {
     
     // Debounce the interrupt
     unsigned long timeStampNow = micros();
     if (timeStampNow - lastIncrementTimeStamp > 10) {
         // Record the last increment timestamp
         lastIncrementTimeStamp = timeStampNow;
         
         // Work out the direction of the motor
        int relayOneValue = digitalRead(relayOnePin);
        int relayTwoValue = digitalRead(relayTwoPin);
        
        // Relay one is activated, the door is moving forward / closed
        if (relayOneValue == HIGH && relayTwoValue == LOW) {
            currentPosition++;
            flopFlopStatusLED();
            
        // Relay two is activated, the door is moving backward / open
        } else if (relayOneValue == LOW && relayTwoValue == HIGH) {
            currentPosition--;
            flopFlopStatusLED();
            
        // Both relays are OFF or ON, the motor shouldn't be moving. Why is the door moving? Error!    
        } else if ((relayOneValue == LOW && relayTwoValue == LOW) ||
            (relayOneValue == HIGH && relayTwoValue == HIGH)) {
            stopMotor();
            status = sys_error;
            
        // We should never get here, if we do something went very wrong.            
        } else {
            stopMotor();
            status = sys_error;
        }
        
        // Stop the motor if we have reached the destination
        if (doorHasReachedDestination()) {
            stopMotor();
        }
    }
    
 }
 
 bool doorHasReachedDestination() {
    return currentPosition == targetPosition;
 }
 
 
 /**
 * Safety and error handling code
 */
bool ensureEverythingIsSafe() {
    bool isSafe = runSafetyTestsAndReturnIsSafe();
    if (!isSafe) {
        stopMotor();
        status = sys_error;
    }
    return isSafe;
}

bool runSafetyTestsAndReturnIsSafe() {
    
    if (sys_error == status) {
        return false;
    }
    
    // Ensure the door never goes past the open or closed positions
    if (currentPosition > endPointClosedPosition || currentPosition < endPointOpenPosition) {
        return false;
    }
    
    // Ensure the target position never goes past the open or closed positions
    if (targetPosition > endPointClosedPosition || targetPosition < endPointOpenPosition) {
        return false;
    }
    
    // Ensure the closed position is never behind the open position
    if (endPointClosedPosition < endPointOpenPosition) {
        return false;
    }
    
    // Everything is safe
    return true;
}
 
 /**
  * Memory functions
  */
  void saveToMemory() {
    EEPROM.put(MEM_endPointOpenPosition, endPointOpenPosition);
    EEPROM.put(MEM_endPointClosedPosition, endPointClosedPosition);
    EEPROM.put(MEM_currentPosition, currentPosition);    
  }
  
  void restoreFromMemory() {
    // Setup endpoints and positions
    EEPROM.get(MEM_endPointOpenPosition, endPointOpenPosition);
    EEPROM.get(MEM_endPointClosedPosition, endPointClosedPosition);
    EEPROM.get(MEM_currentPosition, currentPosition);
    targetPosition = currentPosition;
    status = sys_normal;
    
    // Check that the memory doesn't contain defaults.
    if (currentPosition >= 0xFFFF || 
        endPointOpenPosition >= 0xFFFF || 
        endPointClosedPosition >= 0xFFFF) {
        resetMemory("");
    }
  }
  
  int resetMemory(String cmd) {
    stopMotor();
    currentPosition = 0;
    targetPosition = 0;
    endPointOpenPosition = 0;
    endPointClosedPosition = 0;
    EEPROM.put(MEM_endPointOpenPosition, 0);
    EEPROM.put(MEM_endPointClosedPosition, 0);
    EEPROM.put(MEM_currentPosition, 0);
    status = sys_reset;
    return 0;
  }
 
 /**
 * LED Lamp, status LED and error LEDs
 */
int lightOn(String cmd) {
    digitalWrite(ledLampPin, HIGH);
    return 0;
}

int lightOff(String cmd) {
    digitalWrite(ledLampPin, LOW);
    return 0;
}

void flopFlopStatusLED() {
    digitalWrite(ledBluePin, !digitalRead(ledBluePin));
}

int displayNormal() {
    analogWrite(ledRedPin, 0);
    analogWrite(ledGreenPin, 255);
}

int displayReset() {
    analogWrite(ledRedPin, 0);
    analogWrite(ledGreenPin, 0);
}

int displayConfigure() {
    analogWrite(ledRedPin, 128);
    analogWrite(ledGreenPin, 128);
}

int displayError() {
    analogWrite(ledRedPin, 255);
    analogWrite(ledGreenPin, 0);
}
 
void updateStatusLED() {
    switch (status) {
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
 
/**
 * Getter methods
 */ 
int getEndPointOpenPosition(String cmd) {
    return endPointOpenPosition;
}

int getEndPointClosedPosition(String cmd) {
    return endPointClosedPosition;
}

int getCurrentPosition(String cmd) {
    return currentPosition;
}

int getCurrentPositionPercentage(String cmd) {
    return map(currentPosition, endPointClosedPosition, endPointOpenPosition, 0, 100);
}

int getTargetPosition(String cmd) {
    return targetPosition;
}