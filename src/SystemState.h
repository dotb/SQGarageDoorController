#ifndef SystemState_h
#define SystemState_h

#include "Arduino.h"

// System status
enum sys_status {
  sys_normal,
  sys_reset,
  sys_configure_endpoints,
  sys_error
};

class SystemState {
  public:
    // Overall system status
    sys_status systemStatus = sys_error;
    String stateAsString = "OK";
    String userMessage = "OK";
    int currentPositionPercentage = 0;

    // Position variables
    int currentPosition = 0;
    int targetPosition = 0;
    int endPointOpenPosition = 0;
    int endPointClosedPosition = 0;

    virtual void restoreFromMemory();
    virtual void resetMemory();
    virtual void throwFatalError(String userErrorMessage);
    virtual void loop();

private:
    // Memory addresses
    int _MEM_currentPosition = 1 * sizeof(int);
    int _MEM_endPointOpenPosition = 2 * sizeof(int);
    int _MEM_endPointClosedPosition = 3 * sizeof(int);

    virtual void saveToMemory();
    virtual void updateStateAsString();
    virtual void updateCurrentPositionPercentage();
};

#endif
