#ifndef UserInterface_h
#define UserInterface_h

#include "Arduino.h"
#include "Motor.h"
#include "SystemState.h"
#include "DoorController.h"

class UserInterface {
public:
    UserInterface(SystemState *sysState,
                  DoorController *doorController,
                  int upButtonPin, 
                  int downButtonPin,
                  int codeButtonPin,
                  int setButtonPin,
                  int mButtonPin);
    
    virtual void loop();

private:
    Motor *_motor;
    SystemState *_sysState;
    DoorController *_doorController;
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
