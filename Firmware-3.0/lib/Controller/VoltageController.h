#pragma once

#include <arduino.h>
#include "globals.h"
#include "userVar.h"
#include "timer.h"

class VoltageController: public VarContainer {
  float voltageAverage;
  public:
  FloatVar scale = FloatVar(12.7/2.7/1000, PrefKeys::voltageScale); // Anzeigefaktor für die Batteriespannung

  VoltageController() : voltageAverage(12.0) {
    add(&scale);
  }

  float voltage() {
    return voltageAverage;
  }

  // Voltage should be greater 13 V, if motor is running
  // Used to enable emergency mode, if no gps is available
  bool assumeMotorOn() {
    return voltageAverage > 13.0; 
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
    float v = scale.get() * vccSensor;

    voltageAverage = v * 0.15 + voltageAverage * 0.85;
  }
};

extern VoltageController voltageController;
