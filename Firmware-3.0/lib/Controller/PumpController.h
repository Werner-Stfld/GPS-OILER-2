#pragma once

#include <arduino.h>
#include "globals.h"
#include "userVar.h"

class PumpController: public VarContainer {

  enum {
    pulseIdle,
    pulseOn,
    pulseOff
  } state = pulseIdle;
  unsigned long tmo = 0;
  bool spuelen = false;
  void (*notifyPulse)();

  public:
  IntVar zeit_pumpe_ein = IntVar(50, PrefKeys::zeit_pumpe_ein); // Zeit in ms wie lange die Pumpe eineschaltet ist
  IntVar zeit_pumpe_pause = IntVar(500, PrefKeys::zeit_pumpe_pause); // Zeit zwischen den einzelnen Pumpimpulsen
  IntVar minGeschwindigkeit = IntVar(5, PrefKeys::minGeschwindigkeit); // Mindestgeschwindigkeit zum Ölen
  IntVar pendingPulses =  IntVar(0, PrefKeys::pump_pending); // Noch zu erledigende Pulse

  PumpController(void (*onPulse)()=[]() {}) : notifyPulse(onPulse) {
    add(&zeit_pumpe_ein);
    add(&zeit_pumpe_pause);
    add(&minGeschwindigkeit);
    add(&pendingPulses);
  }

  void setSpuelen(bool v) {
    spuelen = v;
  }

  bool getSpuelen() {
    return spuelen;
  }

  void RequestPulses(unsigned int n) {
    pendingPulses.set(pendingPulses.get() + n);
  }

  bool isOiling() {
    return state != pulseIdle;
  }

  void setPin(int v) {
#ifdef HW_PINS_DEFINED
    digitalWrite(OIL_PIN, v);
#endif
  }

  void setup() {

#ifdef HW_PINS_DEFINED
    pinMode(OIL_PIN, OUTPUT);
#endif
    setPin(HIGH);
    state = pulseIdle;
    restore();
  }

  void loop(float speed) {
    if (state == pulseIdle) {
      if (spuelen) {
        Serial.println("spuelen");
        state = pulseOn;
      } else {
        if (pendingPulses.get() > 0 && (speed >= minGeschwindigkeit.get())) { // Don't oil in standstill
          state = pulseOn;
        }
      }
      if (state == pulseOn) {
        setPin(LOW);
        tmo = millis() + zeit_pumpe_ein.get();
      }
      return;
    } 

    if (millis() < tmo)
      return; // wait a while

    if (state == pulseOn) {
      state = pulseOff;
      setPin(HIGH);
      tmo = millis() + zeit_pumpe_pause.get();
      return;
    }
    if (state == pulseOff) {
      state = pulseIdle;
      notifyPulse();
      int pp =pendingPulses.get(); 
      if (pp > 0 && !spuelen) 
        pendingPulses.set(pp-1);
      return;
    }
  }
};