#pragma once

// Speicheraddressen der benutzerdefinierten Variablen
enum class eepromAddr : int
{
  Noeeprom = -1,
  Factory_init = 0,
  anzahl_Pump = 50,                 // Adresse für Pumpanzahl
  tankinhalt_ml = 100,              //
  pumps_ml = 150,                   //
  tankinhalt_ml_akt = 200,          //
  pumpDistanz = 250,                // Abstand in m zwischen den einzelnen Ölungen
  minGeschwindigkeit = 300,         // Mindestgeschwindigkeit zum Ölen
  oilsymbol_Zeit = 350,             // Zeit des Ölsymbol in
  geschwindigkeit_Notbetrieb = 450, // Geschwindigkeit die angenommen wird wenn kein Sat-Empfang ist (Notbetrieb)
  zeit_bis_notbetrieb = 500,        // Zeit bis Notbetrieb in ms
  zeit_pumpe_ein = 550,             // Zeit in ms wie lage die Pumpe eineschaltet ist
  zeit_pumpe_pause = 600,           // Zeit zwischen den einzelnen Pumpimpulsen
  sw_Regensensor_ein = 650,         //
  sw_Regensensor_aus = 700,         //
  regenmodus = 750,                 //
  rainMulti = 800,                  //
  TimeAPout = 850,                  // Zeit bis zum abschalter des AP
  init_pump_anzahl = 900,           //
  oilingDistance = 1100,            // Wird zur Berechnung der Zurückgelegten entfernung benötigt
  Start_disp_1 = 1150,              //
  Start_disp_2 = 1200,              //
  pump_nach_Regen = 1350,           // Pumpimpulse wenn der Regenmodus abgeschaltet wird
  ssid_ap = 1500,                   // Name des AP
  password_ap = 1800,               // Password AP
  Gefahrene_km = 2000,              //
  Gefahrene_km_Ges = 2100,          //
  timezone = 2200,                  //
  tankinhalt_Aktuell = 2300,        //
  pump_anzahl_Ges = 2400,           //
};

class IntEepromVar
{
  const eepromAddr address;
  int cache;
  bool dirty;

public:
  IntEepromVar(eepromAddr a);
  int get();
  void set(int value);
  void flush();
  void init();
};

class FloatEepromVar
{
  const eepromAddr address;
  float cache;
  bool dirty;

public:
  FloatEepromVar(eepromAddr a);
  float get();
  void set(float value);
  void flush();
  void init();
};

class Char20EepromVar
{
  const eepromAddr address;
  char cache[20];
  bool dirty;

public:
  Char20EepromVar(eepromAddr a);
  const char *get();
  void set(const char *value, int length);
  void flush();
  void init();
};

int read_int(eepromAddr address);
void write_int(eepromAddr address, unsigned int value);
void write_char_array(eepromAddr address, const char *value, int length);
void read_char_array(eepromAddr address, int length, char *rcvData);
float read_float(eepromAddr addr);
void write_float(eepromAddr addr, float value);

void disableEepromWriting();
