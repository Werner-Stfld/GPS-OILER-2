#pragma once

#include <arduino.h>
#include "userVar.h"

class TankController: public VarContainer {

  public:
  IntVar pumps_ml = IntVar(String("Pump Impulse pro ml:"), 50, PrefKeys::pumps_ml);                   // Anzahl der Pumpimpulse pro ml
  FloatVar tankinhalt_ml = FloatVar(String("Tankinhalt ml:"),150, PrefKeys::tankinhalt_ml);             // Tankinhalt in ml
  FloatVar tankinhalt_Aktuell =  FloatVar(String("Tankinhalt akt. (ml):"), 150, PrefKeys::tankinhalt_Aktuell);        // Wert des Aktuellen Tankinhalts in ml

  TankController() {
    add(&pumps_ml);
    add(&tankinhalt_ml);
    add(&tankinhalt_Aktuell);
  }

  void setup() {
    restore();
  };

  unsigned int fillGradeInPercent() {
    return tankinhalt_Aktuell.get()/tankinhalt_ml.get() * 100;
  }

  void getOil(int pumps) {
    tankinhalt_Aktuell.set(tankinhalt_Aktuell.get() - ((float)pumps / pumps_ml.get()));
  }

  void reset() {
    tankinhalt_Aktuell.set(tankinhalt_ml.get(), SetMode::flush);
  }
};

