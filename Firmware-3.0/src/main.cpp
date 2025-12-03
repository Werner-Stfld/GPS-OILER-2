#include <Arduino.h>
#include <Preferences.h>

#include "globals.h"
#include "eepromStorage.h"
#include "userVar.h"
#include "WebController.h"

WebController webController = WebController();       

JsonEndpoint getEndpoints[1] = {
    JsonEndpoint(nullptr, nullptr)
};

JsonEndpoint putEndpoints[1] = {
    JsonEndpoint(nullptr, nullptr)
};

Preferences pref;
int bootCounter;
void setup() {
  // put your setup code here, to run once:
  pinMode(8, OUTPUT);
  Serial.begin(115200);
  Serial.setTxTimeoutMs(10); // kurze Wartezeit
  delay(1000);

  pref.begin("settings", false);

  bootCounter = pref.getInt("bootCounter", 0);
  bootCounter ++;
  pref.putInt("bootCounter", bootCounter);
  webController.setup(getEndpoints, putEndpoints);

}

void loop() {
  digitalWrite(8,LOW);
  delay(500);
  
  digitalWrite(8,HIGH);
  delay(500);

  webController.loop();     // process web requests

  if (Serial) {
    Serial.print("Servus: ");
    Serial.println(bootCounter);
  }
  // put your main code here, to run repeatedly:
}

#if FALSE
#include <Arduino.h>
#include <ArduinoJson.h>
#include <preferences.h>

#include "Timer.h"
#include "GpsController.h"
#include "RainController.h"
#include "DisplayController.h"
#include "PumpController.h"
#include "TankController.h"
#include "DistanceController.h"

// Benutzerdefinierte Variablen
// Änderbar über stdin oder über WEB
// Werte sind im eeprom und werden gecached
IntUserVar geschwindigkeit_Notbetrieb = IntUserVar(String("Geschwindigkeit Notbetrieb:"),80,eepromAddr::geschwindigkeit_Notbetrieb); // Geschwindigkeit die angenommen wird wenn kein Sat-Empfang ist (Notbetrieb)
FloatUserVar geschwindigkeit = FloatUserVar(String( "Geschwindigkeit:"), 0, eepromAddr::Noeeprom);         // Aktuelle Geschwindigkeit
IntUserVar Factory_init = IntUserVar(String("Factory Reset:"),0,eepromAddr::Factory_init); // flag zum Rücksetzen der Einstellungen

TankController tankController = TankController();
DistanceController distanceController = DistanceController();
RainController rainController = RainController();
GpsController gpsController = GpsController(RXPin, GPSBaud);
PumpController pumpController = PumpController([]() {
  tankController.getOil(1);
  displayController.triggerShowOiling();
});
WebController webController = WebController();       
DisplayController displayController = DisplayController();

void getDisplay(JsonDocument &doc) {
  doc["screen1"] = displayController.Start_disp_1.get();;
  doc["screen2"] = displayController.Start_disp_2.get();
  doc["oilSymbol"] = displayController.oilsymbol_Zeit.get();
  doc["timeZone"] = displayController.timeZone.get();
}

void putDisplay(JsonDocument &doc) {
  displayController.Start_disp_1.write(doc["screen1"]);
  displayController.Start_disp_2.write(doc["screen2"]);
  displayController.oilsymbol_Zeit.write(doc["oilSymbol"]);
  displayController.timeZone.write(doc["timeZone"]);
}

void getRain(JsonDocument &doc) {
  doc["onThreshold"] = rainController.sw_Regensensor_ein.get();;
  doc["offThreshold"] = rainController.sw_Regensensor_aus.get();
  doc["distanceMultiplier"] = rainController.rainMulti.get();
  doc["afterRainOilingPulses"] = rainController.pump_nach_Regen.get();
}

void putRain(JsonDocument &doc) {
  rainController.sw_Regensensor_ein.write( doc["onThreshold"]);
  rainController.sw_Regensensor_aus.write( doc["offThreshold"]);
  rainController.rainMulti.write( doc["distanceMultiplier"]);
  rainController.pump_nach_Regen.write( doc["afterRainOilingPulses"]);
}

void getPump(JsonDocument &doc) {
  doc["pulsesPerMl"] = tankController.pumps_ml.get();
  doc["pulseOn"] = pumpController.zeit_pumpe_ein.get();
  doc["pulseOff"] = pumpController.zeit_pumpe_pause.get();
}

void putPump(JsonDocument &doc) {
  tankController.pumps_ml.write(doc["pulsesPerMl"]);
  pumpController.zeit_pumpe_ein.write( doc["pulseOn"] );
  pumpController.zeit_pumpe_pause.write(doc["pulseOff"]);
}

void getWifi(JsonDocument &doc) {
  doc["name"] = webController.ssid_ap.get();
  doc["password"] = webController.password_ap.get();
  doc["timeout"] = webController.TimeAPout.get();
}

void putWifi(JsonDocument &doc) {
  const char *str=doc["name"];
  int len = strlen(str);
  if (len > 0) 
    webController.ssid_ap.write(str, len);
  str=doc["password"];
  len = strlen(str);
  webController.password_ap.write( str, len );
  webController.TimeAPout.write(doc["timeout"]);
}

void getTank(JsonDocument &doc) {
  doc["content"] = tankController.tankinhalt_Aktuell.get();
  doc["capacity"] = tankController.tankinhalt_ml.get();
}

void putTank(JsonDocument &doc) {
  tankController.tankinhalt_ml.write(doc["capacity"]);
}

void getSystem(JsonDocument &doc) {
  String rev = Rev_OILER;
  rev += ": ";
  rev += firmware_Vers;
  doc["rev"] = rev;
  doc["init"] = (bool) Factory_init.get();
}

void getStates(JsonDocument &doc) {
  doc["oiling"] = pumpController.isOiling();
  doc["extraOiling"] = distanceController.extraOilen;
  doc["emergency"] = !gpsController.gpsAvailable() && gpsController.gpsStarted();
  doc["raining"] = rainController.isRaining();
  doc["wifi"] = true; // ToDo:
  doc["washing"] = pumpController.getSpuelen();
  doc["distance"] = distanceController.Gefahrene_km.get();
  doc["pumpDistance"] = distanceController.pumpDistanz.get();
}

void putStates(JsonDocument &doc) {
  distanceController.extraOilen = doc["extraOiling"];
  pumpController.setSpuelen(doc["washing"]);
  distanceController.pumpDistanz.write(doc["pumpDistance"]);
}

void getEmergency(JsonDocument &doc) {
  doc["timeout"] = gpsController.zeit_bis_notbetrieb.get();
  doc["speed"] = geschwindigkeit_Notbetrieb.get();
}

void putEmergency(JsonDocument &doc) {
  gpsController.zeit_bis_notbetrieb.write(doc["timeout"]);
  geschwindigkeit_Notbetrieb.write(doc["speed"]);
}

// Refill tank
void putTankReset(JsonDocument &doc) {
  tankController.reset();
  distanceController.Gefahrene_km.write(0.0); // Reset km 
}

// Force system defaults during next startup
void putSystemDefaults(JsonDocument &doc) {
  write_int(eepromAddr::Factory_init, doc["init"]);
}

JsonEndpoint getEndpoints[9] = {
    JsonEndpoint("/api/system", getSystem),
    JsonEndpoint("/api/display", getDisplay),
    JsonEndpoint("/api/rain", getRain),
    JsonEndpoint("/api/pump", getPump),
    JsonEndpoint("/api/wifi", getWifi),
    JsonEndpoint("/api/tank", getTank),
    JsonEndpoint("/api/states", getStates),
    JsonEndpoint("/api/emergency", getEmergency),
    JsonEndpoint(nullptr, nullptr)
};

JsonEndpoint putEndpoints[10] = {
    JsonEndpoint("/api/system/defaults", putSystemDefaults),
    JsonEndpoint("/api/display", putDisplay),
    JsonEndpoint("/api/rain", putRain),
    JsonEndpoint("/api/pump", putPump),
    JsonEndpoint("/api/wifi", putWifi),
    JsonEndpoint("/api/tank", putTank),
    JsonEndpoint("/api/tank/reset", putTankReset),
    JsonEndpoint("/api/states", putStates),
    JsonEndpoint("/api/emergency", putEmergency),
    JsonEndpoint(nullptr, nullptr)
};

// Variable für die Seriellen Eingabe
String serialInput = ""; // a String to hold incoming data
void serialStatus();

void setup()
{
  delay(300);
  Serial.begin(115200);

  Wire.begin();
  delay(200);

  if (Factory_init.read()>0)
  {
    Factory_init.write(0);
    gpsController.flush();
    rainController.flush();
    distanceController.flush();
    displayController.flush();
    pumpController.flush();
    tankController.flush();
    geschwindigkeit_Notbetrieb.flush();
    webController.flush();
    delay(1000);
    //ESP.restart(); // Reset wird durchgeführt
    return;
  }
  serialInput.reserve(200);

  gpsController.setup();
  rainController.setup();
  distanceController.setup();
  displayController.setup();
  pumpController.setup();
  tankController.setup();
  geschwindigkeit_Notbetrieb.read();

  pinMode(U_VCC2, INPUT);
  pinMode(WLAN_RESET_PIN, INPUT);

  delay(20);

  displayController.setTankPercent(tankController.fillGradeInPercent());
  webController.setup(getEndpoints, putEndpoints);
  serialStatus();
}

Timer secondTimer = Timer(1000);
Timer minuteTimer = Timer(60000);

static bool speedIsGreaterLimit = true;

// standStillEdgeDetected detects transition from speed > 3,0 km/h to <= 3.0. Triggers saving the distance.
bool standStillEdgeDetected(bool gpsAvailable, float speed) {
  if (!gpsAvailable)  
    return false; // no edge detection, if gps is not available.
  if (speed > 3.0)
  {
    speedIsGreaterLimit = true;
    return false;
  }
  // speed <= 3;
  if (speedIsGreaterLimit) {
    speedIsGreaterLimit = false;
    return true;
  }
  return false;
}

void loop()
{
  if (Serial.available() > 0){
    Serial.println("something available");
    String input = Serial.readStringUntil('\n');
    Serial.println(input);
  }

  gpsController.loop();
  rainController.loop();

  if (secondTimer.timedOut())
  {
    uint sattelites;
    float speed;
    uint direction;
    gpsTime time;

    bool gpsAvailable = gpsController.read(sattelites, speed, direction, time);

    geschwindigkeit.set(speed);
    float oilingSpeed = speed; // oilingSpeed: speed to evaluate distance for oiling
    if (!gpsAvailable && gpsController.gpsStarted())
      oilingSpeed = geschwindigkeit_Notbetrieb.get(); // if no gps available after startup => use emergency speed
    oilingSpeed = rainController.getSpeed(oilingSpeed);
    distanceController.update(oilingSpeed, speed);
    pumpController.RequestPulses(distanceController.pulses() + rainController.pulses()); // pass requested pulses to pumpController

    if (minuteTimer.timedOut() || standStillEdgeDetected(gpsAvailable, speed)) { // Store automatically changing values per minute of when speed lowers 3 km/h
      distanceController.Gefahrene_km.flush();
      tankController.tankinhalt_Aktuell.flush();
      distanceController.oilingDistance.flush();
    }

    // update display data
    displayController.setShowSattelite(gpsAvailable);
    displayController.setDirection(direction);
    displayController.setNoSattelite(sattelites);
    displayController.setSpeed(speed);
    displayController.setDistance(distanceController.Gefahrene_km.get());
    displayController.setTime(time);
    displayController.setTankPercent(tankController.fillGradeInPercent());
    displayController.setOilingDistanceInPercent(distanceController.oilingDistanceInPercent());
  }
  pumpController.loop(geschwindigkeit.get());    // process pumping requests
  displayController.loop(); // update display
  webController.loop();     // process web requests
}

/////////////////////////////////
// Serial support
/////////////////////////////////

// Serielle Eingaben
void evaluate(String input)
{
  if (pumpController.zeit_pumpe_ein.evaluate(input)) return; // Pumpzeit
  if (pumpController.zeit_pumpe_pause.evaluate(input)) return; // Pumppause
  if (gpsController.zeit_bis_notbetrieb.evaluate(input)) return; // Zeit bis Notbetrieb
  if (geschwindigkeit_Notbetrieb.evaluate(input)) return; // Geschwindigkeit Notbetrieb
  if (rainController.pump_nach_Regen.evaluate(input)) return; // Pumpimpulse nach Regenmodus
  if (pumpController.minGeschwindigkeit.evaluate(input)) return; // Min Gescchwindigkeit
  if (distanceController.Gefahrene_km.evaluate(input)) return; // Gefahrene Km
  if (distanceController.pumpDistanz.evaluate(input)) return; // Pumpdistanz
  if (displayController.oilsymbol_Zeit.evaluate(input)) return; // Ölsymbol Anzeigezeit
  if (displayController.Start_disp_1.evaluate(input)) return; // Startdisplay 1
  if (displayController.Start_disp_2.evaluate(input)) return; // Startdisplay 2
  if (geschwindigkeit.evaluate(input))return; // Geschwindigkeit
  if (rainController.rainMulti.evaluate(input)) return; // RainMulti

  if (input.startsWith("Status")) // Status ausgeben
  {
    serialStatus();
    return;
  }
  
  if (input.startsWith("Set_Factory")||input.startsWith("EEPROM_init"))
  {
    write_int(eepromAddr::Factory_init, 1);
    ESP.restart();
    return;
  }
}

void serialEvent()
{
  while (Serial.available())
  {
    // get the new byte:
    char inChar = (char)Serial.read();

    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n')
    {
      // Serial.println(inputString);
      evaluate(serialInput);
      // clear the string:
      serialInput = "";
    }
    else
    {
      // add it to the inputString:
      serialInput += inChar;
    }
  }
}

// Serielle Ausgabe aller Benutzerdefinierten Einstellungen
void serialStatus()
{
  Serial.println("");
  Serial.println("Um die Variable zu ändern z.B. Hell:100 eingeben");
  Serial.println("Für Statusabfrage Status eingeben");
  Serial.println("Um den EEPROM neu zu Initialisieren: EEPROM_init eingeben");
  Serial.println("Im Anschluss muss 2x neu gestartet werden");
  Serial.println("");
  distanceController.Gefahrene_km.status();
  pumpController.zeit_pumpe_ein.status();  
  pumpController.zeit_pumpe_pause.status(); 
  displayController.oilsymbol_Zeit.status();
  gpsController.zeit_bis_notbetrieb.status();
  geschwindigkeit_Notbetrieb.status();
  distanceController.pumpDistanz.status();
  rainController.pump_nach_Regen.status();
  pumpController.minGeschwindigkeit.status();
  displayController.Start_disp_1.status();
  displayController.Start_disp_2.status();
  rainController.rainMulti.status();
  geschwindigkeit.status();
  tankController.tankinhalt_Aktuell.status();
  Factory_init.status();
}
#endif