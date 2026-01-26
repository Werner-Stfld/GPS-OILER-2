#pragma once

#include <arduino.h>

class Timer {
  unsigned long _retriggerTimeout;
  unsigned long _activeTimeout;
  public:
  
  Timer(unsigned long retriggerTimeout) : _retriggerTimeout(retriggerTimeout) {
    _activeTimeout = 0;
    retrigger();
  }

  Timer() : _retriggerTimeout(0), _activeTimeout(0) {  }

  // void dump() {
  //   Serial.print("retriggerTimeout: ");
  //   Serial.println(_retriggerTimeout);
  //   Serial.print("activeTimeout: ");
  //   Serial.println(_activeTimeout);
  //   Serial.print("now: ");
  //   Serial.println(millis());
  // }

  void setRetriggerTimeout (unsigned long timeOut) {
    _retriggerTimeout = timeOut;
  }

  void retrigger() {
    _activeTimeout = (_retriggerTimeout != 0)?millis() + _retriggerTimeout:0;
  }

  void stop () {
    _activeTimeout = 0;
  }

  // Timer has been initialized to wait and timeout has currently not been reached
  bool isWaiting() {
    if (!isActive()) 
        return false; // timer inactive
    unsigned long now = millis();
    if (now < _activeTimeout) {
      return true;
    }
    return false;
  }

  // Timer is running
  bool isActive() {
    return _activeTimeout != 0;
  }

  bool retriggered() {
    if (!isActive()) 
        return false; // timer inactive
    if (isWaiting())
      return false;
    retrigger();
    return true;
  }

  // return true, if timeout has been reached. Retriggers timeout, if retriggerTimeout is set
  bool timedOut() 
  {
    if (!isActive()) 
        return false; // timer inactive
    if (isWaiting())
      return false;
    return true;
  }
};
