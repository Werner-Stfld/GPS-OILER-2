#pragma once

#include <Arduino.h>
#include "globals.h"

class RainController {

  int rainAverage = 0;
  unsigned long tmo = 0;
  bool raining = false;
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
    IntUserVar pump_nach_Regen = IntUserVar(String("Pumpstoesse nach Regen:"), 5, eepromAddr::pump_nach_Regen);             // Pumpimpulse wenn der Regenmodus abgeschaltet wird
    IntUserVar sw_Regensensor_ein = IntUserVar(String("Schwelle Regenmodus ein:"), 600, eepromAddr::sw_Regensensor_ein);        // Schwellwert Regenmodus ein
    IntUserVar sw_Regensensor_aus = IntUserVar(String("Schwelle Regenmodus aus:"), 800, eepromAddr::sw_Regensensor_aus);        // Schwellwert Regenmodus aus
    FloatUserVar rainMulti = FloatUserVar(String("Regen Multiplikator:"), (float)2.0, eepromAddr::rainMulti, &checkBoundsRainMulti); // Multiplikator für Regenmodus in Promille

  RainController() {
    _initAverageLoop = 10; // loop 10 times before using the average value
  }

  bool isRaining() {
    return raining;
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

  void flush() {
    pump_nach_Regen.flush();
    sw_Regensensor_ein.flush();
    sw_Regensensor_aus.flush();
    rainMulti.flush();
    write_int(eepromAddr::regenmodus, 0);
  }

  void setup() {

    pinMode(RAIN_SENSOR_PIN, INPUT);
    pinMode(RAIN_SENSOR_VOLTAGE_PIN, OUTPUT);
    digitalWrite(RAIN_SENSOR_VOLTAGE_PIN, HIGH);

    tmo = millis() + 1000;
    pump_nach_Regen.read();
    sw_Regensensor_ein.read();
    sw_Regensensor_aus.read();
    rainMulti.read();
    raining = read_int(eepromAddr::regenmodus);
  }

  void loop() {
    if (millis() < tmo)
      return;
    tmo = millis() + 1000;

    rainAverage = 0.85 * rainAverage + 0.15 * analogRead(RAIN_SENSOR_PIN);

    if (_initAverageLoop > 0) { // wait until the average has been build up
      _initAverageLoop--;
      return;
    }

    if (rainAverage <= sw_Regensensor_ein.get())
    {
      raining = true;
      write_int(eepromAddr::regenmodus, raining);
    }
    if (rainAverage >= sw_Regensensor_aus.get() && raining)
    {
      raining = false;
      write_int(eepromAddr::regenmodus, raining);
      _remainingPulsesAfterRain = pump_nach_Regen.get();
    }
  }
};

extern RainController rainController;