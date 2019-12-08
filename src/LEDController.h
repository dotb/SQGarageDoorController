#ifndef LEDController_h
#define LEDController_h

#include "Arduino.h"
#include "SystemState.h"
#include "Motor.h"

class LEDController {
    public:
        LEDController(SystemState *sysState,
                            int ledLampPin,
                            int ledRedPin,
                            int ledGreenPin,
                            int ledBluePin);
        virtual void lightOn();    
        virtual void lightOff();
        virtual void loop();
    
    private:
        SystemState *_sysState;
        int _ledLampPin;
        int _ledRedPin;
        int _ledGreenPin;
        int _ledBluePin;
        unsigned int _lampOnTimestamp = 0;

        virtual void displayNormal();
        virtual void displayReset();
        virtual void displayConfigure();
        virtual void displayError();
        virtual void updateStatusLED();
};

#endif