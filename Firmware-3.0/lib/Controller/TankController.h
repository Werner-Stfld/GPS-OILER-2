#pragma once

#include <arduino.h>
#include "userVar.h"

class TankController {

  public:
  IntUserVar pumps_ml = IntUserVar(String("Pump Impulse pro ml:"), 50, eepromAddr::pumps_ml);                   // Anzahl der Pumpimpulse pro ml
  FloatUserVar tankinhalt_ml = FloatUserVar(String("Tankinhalt ml:"),150, eepromAddr::tankinhalt_ml);             // Tankinhalt in ml
  FloatUserVar tankinhalt_Aktuell =  FloatUserVar(String("Tankinhalt akt. (ml):"), 150, eepromAddr::tankinhalt_Aktuell);        // Wert des Aktuellen Tankinhalts in ml

  void flush() {
      pumps_ml.flush();
      tankinhalt_ml.flush();
      tankinhalt_Aktuell.flush();
  }

  void setup() {
    pumps_ml.read();
    tankinhalt_ml.read();
    tankinhalt_Aktuell.read();
  };

  unsigned int fillGradeInPercent() {
    return tankinhalt_Aktuell.get()/tankinhalt_ml.get() * 100;
  }

  void getOil(int pumps) {
    tankinhalt_Aktuell.set(tankinhalt_Aktuell.get() - ((float)pumps / pumps_ml.get()));
  }

  void reset() {
    tankinhalt_Aktuell.write(tankinhalt_ml.get());
  }
};

