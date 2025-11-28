#pragma once

#include <Arduino.h>
#include "userVar.h"

// Limit pumpDistance to be not less than 500 m
int checkPumpDistanz(int i) {
  return i > 500?i:500;
}

class DistanceController {
  unsigned long start = 0;
public:
  
  IntUserVar pumpDistanz = IntUserVar(String("Pumpdistanz:"),2000, eepromAddr::pumpDistanz, checkPumpDistanz);              // Abstand in m zwischen den einzelnen Ölungen
  FloatUserVar Gefahrene_km =FloatUserVar(String( "Gefahrene Km:"), 0, eepromAddr::Gefahrene_km);         // Für die Anzeige der gefahrenen Strecke seit letztem Tank Reset.
  FloatUserVar oilingDistance = FloatUserVar(String( "Cannot be modified:"), 0, eepromAddr::oilingDistance);
  boolean extraOilen = false;        // Variable für extraSpülen

  void setup() {
    pumpDistanz.read ();
    Gefahrene_km.read();
    oilingDistance.read();


    start = millis();
  }

  void flush() {
    pumpDistanz.flush ();
    Gefahrene_km.flush();
    oilingDistance.flush();
  }

  void update (float oilingSpeed, float speed) {
    unsigned long now = millis();
    float secondsToIntegrate = (now - start);
    start = now;
    secondsToIntegrate = secondsToIntegrate/1000;
    float distance = oilingDistance.get();
    distance += (oilingSpeed / 3600) * secondsToIntegrate * (extraOilen?1.5:1);
    oilingDistance.set(distance);
    Gefahrene_km.set((speed / 3600) * secondsToIntegrate + Gefahrene_km.get());
  }

  int oilingDistanceInPercent() {
    float pumpDistance = pumpDistanz.get();
    pumpDistance = pumpDistance / 1000;
    return oilingDistance.get() / pumpDistance *100;
  }

  unsigned int pulses() {
    float pumpDistance = pumpDistanz.get();
    pumpDistance = pumpDistance / 1000;
    unsigned int pulses = oilingDistance.get() / pumpDistance;
    float distance = oilingDistance.get();
    distance -= pulses * pumpDistance;
    oilingDistance.set(distance); // Keep rest to accumulate further distances.
    return pulses;
  }
};
