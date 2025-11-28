#pragma once

#include <arduino.h>
#include "globals.h"
#include "userVar.h"

class PumpController {
  unsigned int pendingPulses = 0;

  enum {
    pulseIdle,
    pulseOn,
    pulseOff
  } state = pulseIdle;
  unsigned long tmo = 0;
  bool spuelen = false;
  void (*notifyPulse)();

  public:
  IntUserVar zeit_pumpe_ein = IntUserVar(String("Zeit Pumpdauer:"), 50, eepromAddr::zeit_pumpe_ein); // Zeit in ms wie lange die Pumpe eineschaltet ist
  IntUserVar zeit_pumpe_pause = IntUserVar(String("Zeit Pumppause:"), 500, eepromAddr::zeit_pumpe_pause); // Zeit zwischen den einzelnen Pumpimpulsen

  PumpController(void (*onPulse)()=[]() {}) : notifyPulse(onPulse) {}

  void setSpuelen(bool v) {
    spuelen = v;
  }

  bool getSpuelen() {
    return spuelen;
  }

  void RequestPulses(unsigned int n) {
    pendingPulses += n;
  }

  bool isOiling() {
    return state != pulseIdle;
  }

  void setup() {
    pinMode(OIL_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(OIL_PIN, HIGH);
    state = pulseIdle;

    zeit_pumpe_ein.read();
    zeit_pumpe_pause.read();
  }

  void flush() {
    zeit_pumpe_ein.flush();
    zeit_pumpe_pause.flush();
  }

  void loop() {
    if ((state == pulseIdle) && ((pendingPulses > 0) || spuelen)) {
      state = pulseOn;
      digitalWrite(OIL_PIN, LOW);
      digitalWrite(LED_PIN, LOW);
      tmo = millis() + zeit_pumpe_ein.get();
      return;
    }

    if (millis() < tmo)
      return; // wait a while

    if (state == pulseOn) {
      state = pulseOff;
      digitalWrite(OIL_PIN, HIGH);
      digitalWrite(LED_PIN, HIGH);
      tmo = millis() + zeit_pumpe_pause.get();
      return;
    }
    if (state == pulseOff) {
      state = pulseIdle;
      notifyPulse();
      if (pendingPulses > 0 && !spuelen) 
        pendingPulses -= 1;
      return;
    }
  }
};

