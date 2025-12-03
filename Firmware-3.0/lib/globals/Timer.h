#pragma once

#include <arduino.h>

class Timer {
  unsigned long _timeOut;
  unsigned long tmo;
  public:
  
  // timeOut: 0 => no timeout at all
  Timer(unsigned long timeOut) : _timeOut(timeOut) {
    tmo = (timeOut != 0)?millis():0;
  }

  // timeOut: 0 => no timeout at all
  // else retriggers timer
  void nextTimeout (unsigned long timeOut) {
    _timeOut = timeOut;
    tmo = (timeOut != 0)?millis() + _timeOut:0;
  }

  bool timedOut() 
  {
    if (_timeOut == 0)
        return false;
    unsigned long now = millis();
    if (now < tmo) {
      return false;
    }
    tmo = now + _timeOut;
    return true;
  }
};
