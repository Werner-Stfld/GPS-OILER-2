#pragma once

#include <Arduino.h>
#include "globals.h"
#include "userVar.h"

class RainController: public VarContainer {

  int rainAverage = 0;
  unsigned long tmo = 0;
  int _remainingPulsesAfterRain;
  int _initAverageLoop;

    // lmiit the rainMultiplier between 1 and 5
  static float checkBoundsRainMulti (float v) {
    if (v < 1) {
      return 1;
    }
    if (v > 5) {
      return 5;
    }
    return v;
  };

  public:
    IntVar pump_nach_Regen = IntVar(String("Pumpstoesse nach Regen:"), 5, PrefKeys::pump_nach_Regen);             // Pumpimpulse wenn der Regenmodus abgeschaltet wird
    IntVar sw_Regensensor_ein = IntVar(String("Schwelle Regenmodus ein:"), 600, PrefKeys::sw_Regensensor_ein);    // Schwellwert Regenmodus ein
    IntVar sw_Regensensor_aus = IntVar(String("Schwelle Regenmodus aus:"), 800, PrefKeys::sw_Regensensor_aus);    // Schwellwert Regenmodus aus
    FloatVar rainMulti = FloatVar(String("Regen Multiplikator:"), (float)2.0, PrefKeys::rainMulti, &checkBoundsRainMulti); // Multiplikator für Regenmodus in Promille
    IntVar raining = IntVar(String("Regenmodus:"), 0, PrefKeys::regenmodus);                                      // Regenmodus Zustand beim Ausschalten

  RainController() {
    _initAverageLoop = 10; // loop 10 times before using the average value
    add(&pump_nach_Regen);
    add(&sw_Regensensor_ein);
    add(&sw_Regensensor_aus);
    add(&rainMulti);
    add(&raining);
  }

  bool isRaining() {
    return raining.get() != 0;
  }

  int pulses() {
    unsigned int pulses = _remainingPulsesAfterRain;
    _remainingPulsesAfterRain = 0;
    return pulses;
  }

  float getSpeed(float oilingSpeed) {
    if (isRaining()) {
      return oilingSpeed * rainMulti.get(); // if raining, use rain multiplier    
    }
    return oilingSpeed;
  }

  void setup() {
#ifdef HW_PINS_DEFINED
    pinMode(RAIN_SENSOR_PIN, INPUT);
#endif
    tmo = millis() + 1000;
    restore();
  }

  void loop() {
    if (millis() < tmo)
      return;
    tmo = millis() + 1000;

#ifdef HW_PINS_DEFINED
    rainAverage = 0.85 * rainAverage + 0.15 * analogRead(RAIN_SENSOR_PIN);
#else
    rainAverage = 650;
#endif

    if (_initAverageLoop > 0) { // wait until the average has been build up
      _initAverageLoop--;
      return;
    }

    if (rainAverage <= sw_Regensensor_ein.get())
    {
      raining.set(1);
    }
    if (rainAverage >= sw_Regensensor_aus.get() && isRaining())
    {
      raining.set(0);
      _remainingPulsesAfterRain = pump_nach_Regen.get();
    }
  }
};

extern RainController rainController;