#include <Arduino.h>
#include <ArduinoJson.h>

#include "globals.h"
#include "prefs.h"
#include "userVar.h"
#include "WebController.h"
#include "Timer.h"
#include "GpsController.h"
#include "RainController.h"
#include "DisplayController.h"
#include "PumpController.h"
#include "TankController.h"
#include "DistanceController.h"
#include "VoltageController.h"

// Benutzerdefinierte Variablen
// Änderbar über stdin oder über WEB
// Werte sind in prefs und werden gecached
IntVar geschwindigkeit_Notbetrieb = IntVar(String("Geschwindigkeit Notbetrieb:"),80,PrefKeys::geschwindigkeit_Notbetrieb); // Geschwindigkeit die angenommen wird wenn kein Sat-Empfang ist (Notbetrieb)
FloatVar geschwindigkeit = FloatVar(String( "Geschwindigkeit:"), 0, PrefKeys::Noeeprom);         // Aktuelle Geschwindigkeit
IntVar Factory_init = IntVar(String("Factory Reset:"),0,PrefKeys::Factory_init); // flag zum Rücksetzen der Einstellungen

TankController tankController = TankController();
DistanceController distanceController = DistanceController();
RainController rainController = RainController();
#ifdef HW_PINS_DEFINED
  GpsController gpsController = GpsController(RXPin, GPSBaud);
#else
  GpsController gpsController = GpsController();
#endif
  PumpController pumpController = PumpController([]() {
  tankController.getOil(1);
  displayController.triggerShowOiling();
});
WebController webController = WebController();       
DisplayController displayController = DisplayController();
VoltageController voltageController = VoltageController();

void getDisplay(JsonDocument &doc) {
  doc["screen1"] = displayController.Start_disp_1.get();;
  doc["screen2"] = displayController.Start_disp_2.get();
  doc["oilSymbol"] = displayController.oilsymbol_Zeit.get();
  doc["timeZone"] = displayController.timeZone.get();
}

void putDisplay(JsonDocument &doc) {
  displayController.Start_disp_1.set(doc["screen1"], SetMode::flush);
  displayController.Start_disp_2.set(doc["screen2"], SetMode::flush);
  displayController.oilsymbol_Zeit.set(doc["oilSymbol"], SetMode::flush);
  displayController.timeZone.set(doc["timeZone"], SetMode::flush);
}

void getRain(JsonDocument &doc) {
  doc["onThreshold"] = rainController.sw_Regensensor_ein.get();;
  doc["offThreshold"] = rainController.sw_Regensensor_aus.get();
  doc["distanceMultiplier"] = rainController.rainMulti.get();
  doc["afterRainOilingPulses"] = rainController.pump_nach_Regen.get();
}

void putRain(JsonDocument &doc) {
  rainController.sw_Regensensor_ein.set( doc["onThreshold"], SetMode::flush);
  rainController.sw_Regensensor_aus.set( doc["offThreshold"], SetMode::flush);
  rainController.rainMulti.set( doc["distanceMultiplier"], SetMode::flush);
  rainController.pump_nach_Regen.set( doc["afterRainOilingPulses"], SetMode::flush);
}

void getPump(JsonDocument &doc) {
  doc["pulsesPerMl"] = tankController.pumps_ml.get();
  doc["pulseOn"] = pumpController.zeit_pumpe_ein.get();
  doc["pulseOff"] = pumpController.zeit_pumpe_pause.get();
}

void putPump(JsonDocument &doc) {
  tankController.pumps_ml.set(doc["pulsesPerMl"], SetMode::flush);
  pumpController.zeit_pumpe_ein.set( doc["pulseOn"] , SetMode::flush);
  pumpController.zeit_pumpe_pause.set(doc["pulseOff"], SetMode::flush);
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
    webController.ssid_ap.set(str, SetMode::flush);
  str=doc["password"];
  webController.password_ap.set(str, SetMode::flush);
  webController.TimeAPout.set(doc["timeout"], SetMode::flush);
}

void getTank(JsonDocument &doc) {
  doc["content"] = tankController.tankinhalt_Aktuell.get();
  doc["capacity"] = tankController.tankinhalt_ml.get();
}

void putTank(JsonDocument &doc) {
  tankController.tankinhalt_ml.set(doc["capacity"], SetMode::flush);
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
  distanceController.pumpDistanz.set(doc["pumpDistance"], SetMode::flush);
}

void getEmergency(JsonDocument &doc) {
  doc["timeout"] = gpsController.zeit_bis_notbetrieb.get();
  doc["speed"] = geschwindigkeit_Notbetrieb.get();
}

void putEmergency(JsonDocument &doc) {
  gpsController.zeit_bis_notbetrieb.set(doc["timeout"], SetMode::flush);
  geschwindigkeit_Notbetrieb.set(doc["speed"], SetMode::flush);
}

// Refill tank
void putTankReset(JsonDocument &doc) {
  tankController.reset();
  distanceController.Gefahrene_km.set(0.0, SetMode::flush); // Reset km 
}

// Force system defaults during next startup
void putSystemDefaults(JsonDocument &doc) {
  if (doc["init"].is<JsonVariant>())
    Factory_init.set(doc["init"], SetMode::flush);
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

void tankReset() {
  Serial.println("Tank Reset");
}

void wiFiReset() {
  Serial.println("WiFi Reset");
}

void settingsReset() {
  Serial.println("Settings Reset");
}

void wiFiOnOff(bool on) {
  if (on) 
    Serial.println("Wifi ON");
    else
    Serial.println("Wifi OFF");
}

void setup()
{
  delay(500); // wird auf dem c3 benötigt, wenn CDC eingeschaltet ist.
  Serial.begin(115200);
  delay(300);
  while (!Serial) {
    delay(10);
  }
  //ToDo: Check Wire.begin();
  Serial.println("setup 01");

  Factory_init.restore();
  if (Factory_init.get()>0)
  { // flush all vars with initial values
    Factory_init.set(0, SetMode::flush);
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
  Serial.println("setup 02");
  
  webController.setup(getEndpoints, putEndpoints);
  gpsController.setup();
  rainController.setup();
  distanceController.setup();
  displayController.setup();
  pumpController.setup();
  tankController.setup();
  voltageController.setup();

  geschwindigkeit_Notbetrieb.restore();

#ifdef HW_PINS_DEFINED
  pinMode(WLAN_RESET_PIN, INPUT);
  pinMode(TANK_RESET_PIN, INPUT);
#endif
  delay(20);

  displayController.setTankPercent(tankController.fillGradeInPercent());
  displayController.OnTankReset(tankReset);
  displayController.OnWiFiReset(wiFiReset);
  displayController.OnSettingsReset(settingsReset);
  displayController.OnWiFiOnOff(wiFiOnOff);

  serialStatus();

  pinMode(TANK_RESET_PIN, INPUT);
  pinMode(WLAN_RESET_PIN, INPUT);
}
static bool edgeSignalled = true;

// standStillEdgeDetected detects transition from speed > 3,0 km/h to <= 2.0. Triggers saving the vars modified so far.
bool standStillEdgeDetected(bool gpsAvailable, float speed) {
  if (!gpsAvailable)  
    return false; // no edge detection, if gps is not available.
  if (speed > 3.0)
  {
    edgeSignalled = false; // prepare detection of new edge
    return false;
  }
  if (edgeSignalled) 
    return false; 
  if (speed < 2.0) {
    edgeSignalled = true; // signal edge only once
    return true; 
  }
  return false;
}

Timer secondTimer = Timer(1000);
Timer minuteTimer = Timer(60000);

void loop()
{
  if (Serial.available() > 0){
    Serial.println("something available");
    String input = Serial.readStringUntil('\n');
    Serial.println(input);
  }

  gpsController.loop();
  rainController.loop();
  voltageController.loop();

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

    if (minuteTimer.timedOut() || standStillEdgeDetected(gpsAvailable, speed)) { 
      // Flush automatically changing values once per minute or when gps is active indicating speed less then 2 km/h
      distanceController.Gefahrene_km.flush();
      tankController.tankinhalt_Aktuell.flush();
      distanceController.oilingDistance.flush();
      rainController.raining.flush();
      pumpController.pendingPulses.flush();
    }

    // update display data
    displayController.setShowSattelite(gpsAvailable);
    displayController.setShowRaining(rainController.isRaining());
    displayController.setDirection(direction);
    displayController.setNoSattelite(sattelites);
    displayController.setSpeed(speed);
    displayController.setDistance(distanceController.Gefahrene_km.get());
    displayController.setTime(time);
    displayController.setTankPercent(tankController.fillGradeInPercent());
    displayController.setOilingDistanceInPercent(distanceController.oilingDistanceInPercent());
    displayController.setBatteryVoltage(voltageController.voltage());
  }

  pumpController.loop(geschwindigkeit.get());    // process pump requests
  displayController.loop(digitalRead(WLAN_RESET_PIN)==LOW); // update display
  webController.loop();     // process web requests
  prefs.AssertClosed();
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
    Factory_init.set(1, SetMode::flush);
    prefs.AssertClosed();
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