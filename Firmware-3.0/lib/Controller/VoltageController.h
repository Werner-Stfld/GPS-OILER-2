#pragma once

#include <arduino.h>
#include "globals.h"
#include "userVar.h"
#include "timer.h"

class VoltageController: public VarContainer {
  float voltageAverage;
  public:
  FloatVar displayScale = FloatVar(String(""), 12.7/2.7/1000, PrefKeys::voltageScale); // Anzeigefaktur für die Batteriespannung

  VoltageController() : voltageAverage(2.4) {
    add(&displayScale);
  }

  float voltage() {
    return voltageAverage*displayScale.get();
  }

  void setup() {

#ifdef HW_PINS_DEFINED
    pinMode(VCC_SENSOR_PIN, INPUT);
#endif
    
  }

  Timer secondTick = Timer(300);
  void loop() {
    if (!secondTick.timedOut())
      return;
    int32_t vccSensor = analogReadMilliVolts(VCC_SENSOR_PIN);

    voltageAverage = vccSensor * 0.15 + voltageAverage * 0.85;
  }
};

extern VoltageController voltageController;
