#ifndef SafetyTests_h
#define SafetyTests_h

#include "Arduino.h"
#include "SystemState.h"

class SafetyTests {
public:
    SafetyTests(SystemState *sysState);
    virtual bool ensureEverythingIsSafe();

private:
    SystemState *_sysState;
};

#endif
