#ifndef UserInterface_h
#define UserInterface_h

#include "Arduino.h"
#include "Motor.h"
#include "SystemState.h"

class UserInterface {
public:
    UserInterface(Motor *motor, 
                  SystemState *sysState, 
                  int upButtonPin, 
                  int downButtonPin,
                  int codeButtonPin,
                  int setButtonPin,
                  int mButtonPin);
    
    virtual void loop();

private:
    Motor *_motor;
    SystemState *_sysState;
    int _upButtonPin;
    int _downButtonPin;
    int _codeButtonPin;
    int _setButtonPin;
    int _mButtonPin;

    virtual void handleButtons();
    virtual bool upButtonIsPressed();
    virtual bool downButtonIsPressed();
    virtual bool setButtonIsPressed();
    virtual bool codeButtonIsPressed();
    virtual bool mButtonIsPressed();
};

#endif
