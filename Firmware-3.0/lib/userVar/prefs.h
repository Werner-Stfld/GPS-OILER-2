#pragma once
#include <Arduino.h>
#include <Preferences.h>

typedef const char* PrefKey;

namespace PrefKeys {
  extern PrefKey Noeeprom;
  extern PrefKey Factory_init;
  extern PrefKey anzahl_Pump;                 // Adresse für Pumpanzahl
  extern PrefKey tankinhalt_ml;              //
  extern PrefKey pumps_ml;                   //
  extern PrefKey tankinhalt_ml_akt;          //
  extern PrefKey pumpDistanz;                // Abstand in m zwischen den einzelnen Ölungen
  extern PrefKey minGeschwindigkeit;         // Mindestgeschwindigkeit zum Ölen
  extern PrefKey oilsymbol_Zeit;             // Zeit des Ölsymbol in
  extern PrefKey geschwindigkeit_Notbetrieb; // Geschwindigkeit die angenommen wird wenn kein Sat-Empfang ist (Notbetrieb)
  extern PrefKey zeit_bis_notbetrieb;        // Zeit bis Notbetrieb in ms
  extern PrefKey zeit_pumpe_ein;             // Zeit in ms wie lage die Pumpe eineschaltet ist
  extern PrefKey zeit_pumpe_pause;           // Zeit zwischen den einzelnen Pumpimpulsen
  extern PrefKey sw_Regensensor_ein;         //
  extern PrefKey sw_Regensensor_aus;         //
  extern PrefKey regenmodus;                 //
  extern PrefKey rainMulti;                  //
  extern PrefKey TimeAPout;                  // Zeit bis zum abschalter des AP
  extern PrefKey init_pump_anzahl;           //
  extern PrefKey oilingDistance;            // Wird zur Berechnung der Zurückgelegten entfernung benötigt
  extern PrefKey Start_disp_1;              //
  extern PrefKey Start_disp_2;              //
  extern PrefKey pump_nach_Regen;           // Pumpimpulse wenn der Regenmodus abgeschaltet wird
  extern PrefKey ssid_ap;                   // Name des AP
  extern PrefKey password_ap;               // Password AP
  extern PrefKey Gefahrene_km;              //
  extern PrefKey Gefahrene_km_Ges;          //
  extern PrefKey timezone;                  //
  extern PrefKey tankinhalt_Aktuell;        //
  extern PrefKey pump_pending;              // Anzahl noch ausstehender Pump Pulse nach reset
}

class PrefsNamespace {
  bool prefsAccessible=false;
  Preferences prefs=Preferences();
  const char *ns;

  void assertOpen() {
    if (prefsAccessible)
      return;
    prefs.begin(ns); // use only RW-Mode
    prefsAccessible = true;
  }

public: 
  PrefsNamespace(const char *name): ns(name) {
  }

  void AssertClosed() {
    if (!prefsAccessible) 
      return;
    prefs.end();
    prefsAccessible = false;
  }

  size_t putInt(const char* key, int32_t value) {
    if (key == PrefKeys::Noeeprom)
      return 0;
    assertOpen();
    return prefs.putInt(key, value);
  }
  size_t putFloat(const char* key, float_t value) {
    if (key == PrefKeys::Noeeprom)
      return 0;
    assertOpen();
    return prefs.putFloat(key, value);
  };
  size_t putString(const char* key, const char* value) {
    if (key == PrefKeys::Noeeprom)
      return 0;
    assertOpen();
    return prefs.putString(key, value);
  }
  int32_t getInt(const char* key, int32_t defaultValue = 0) {
    if (key == PrefKeys::Noeeprom)
      return defaultValue;
    assertOpen();
    return prefs.getInt(key);
  }
  float_t getFloat(const char* key, float_t defaultValue = NAN) {
    if (key == PrefKeys::Noeeprom)
      return defaultValue;
    assertOpen();
    return prefs.getFloat(key);
  }
  size_t getString(const char* key, char* value, size_t maxLen) {
    if (key == PrefKeys::Noeeprom)
      return 0;
    assertOpen();
    return prefs.getString(key, value, maxLen);
  }
  bool isKey(const char* key) {
    if (key == PrefKeys::Noeeprom)
      return false;
    assertOpen();
    return prefs.isKey(key);
  }
};

extern PrefsNamespace prefs;

class IntPrefVar
{
  const PrefKey key;
  int cache;
  bool dirty;

public:
  IntPrefVar(PrefKey  a);
  int get();
  void set(int value);
  void flush();
  void restore();
};

class FloatPrefVar
{
  const PrefKey key;
  float cache;
  bool dirty;

public:
  FloatPrefVar(PrefKey  a);
  float get();
  void set(float value);
  void flush();
  void restore();
};

class StringPrefVar
{
  static const size_t maxLen = 32;
  const PrefKey key;
  char cache[maxLen + 1];
  bool dirty;

public:
  StringPrefVar(PrefKey  a);
  const char *get();
  void set(const char *value);
  void flush();
  void restore();
};

void disableEepromWriting();
