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
IntVar geschwindigkeit_Notbetrieb = IntVar(60,PrefKeys::geschwindigkeit_Notbetrieb); // Geschwindigkeit die angenommen wird wenn kein Sat-Empfang ist (Notbetrieb)
FloatVar geschwindigkeit = FloatVar(0, PrefKeys::Noeeprom);         // Aktuelle Geschwindigkeit
IntVar initFromPreferences = IntVar(0,PrefKeys::init_from_preferences); // flag zum Rücksetzen der Einstellungen

// The endpoints for the web-server
JsonEndpoint *endpoints(); 

// The controllers
TankController tankController = TankController();
DistanceController distanceController = DistanceController();
RainController rainController = RainController();
GpsController gpsController = GpsController(RXPin, GPSBaud);
PumpController pumpController = PumpController([]() {
  tankController.getOil(1);
  displayController.triggerShowOiling();
});
WebController webController = WebController(endpoints());
DisplayController displayController = DisplayController();
VoltageController voltageController = VoltageController();

void tankReset() {
  Serial.println("Tank Reset");
  tankController.reset();
}

void wiFiReset() {
  webController.resetWiFi();
}

void startWiFi() {
  webController.startWiFi();
}

// setup the oiler scetch

void setup()
{
  delay(500); // wird auf dem c3 benötigt, wenn CDC eingeschaltet ist.
  Serial.begin(115200);
  delay(300);

  Serial.println("setup-01");
  initFromPreferences.restore();
  if (initFromPreferences.get()==0) // Default after flush of memory. May also be forced by web UI.
  { // flush all vars with initial values of the userVars
    Serial.println("setup-02");
    initFromPreferences.set(610118, SetMode::flush);
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
  }
  Serial.println("setup-03");

  // setups are loading the userVars from the preference storage
  webController.setup();
  gpsController.setup();
  rainController.setup();
  distanceController.setup();
  displayController.setup();
  pumpController.setup();
  tankController.setup();
  voltageController.setup();

  geschwindigkeit_Notbetrieb.restore();

  delay(20);

  displayController.OnTankReset(tankReset);
  displayController.OnResetWiFi(wiFiReset);
  displayController.OnStartWiFi(startWiFi);

  pinMode(BUTTON_PIN, INPUT);
}

static bool edgeSignalled = true;
// standStillEdgeDetected detects transition from speed > 3,0 km/h to <= 2.0. Triggers saving the vars modified so far.
bool standStillEdgeDetected(float speed) {
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

Timer secondTimer = Timer(300);
Timer minuteTimer = Timer(60000);

float getOilingSpeed() {
  float oilingSpeed;
  if (gpsController.validData()) {
    // speed from gps
    oilingSpeed = gpsController.speed();
    geschwindigkeit.set(oilingSpeed);
  } else {
    oilingSpeed = voltageController.assumeMotorOn()? geschwindigkeit_Notbetrieb.get(): 0.0;
  }
  return oilingSpeed;
}

void loop()
{
  gpsController.loop();
  rainController.loop();
  voltageController.loop();
  float oilingSpeed = 0.0; // oilingSpeed: speed to evaluate distance for oiling
  if (secondTimer.retriggered())
  {
    oilingSpeed = getOilingSpeed();
    oilingSpeed = rainController.getSpeed(oilingSpeed); 
    distanceController.update(oilingSpeed);
    pumpController.RequestPulses(distanceController.pulses() + rainController.pulses()); // pass requested pulses to pumpController

    if (minuteTimer.retriggered() || (gpsController.validData() && standStillEdgeDetected(gpsController.speed()))) { 
      // Flush automatically changing values once per minute or when gps is active indicating speed less then 2 km/h
      // remember: all flush() are writing only, if values have been modified 
      tankController.tankinhalt_Aktuell.flush();
      distanceController.oilingDistance.flush();
      rainController.raining.flush();
      pumpController.pendingPulses.flush();
    }
  }

  pumpController.loop(oilingSpeed);                     // process pump requests
  displayController.loop(digitalRead(BUTTON_PIN)==LOW); // display and input button handling
  webController.loop();                                 // process web requests
  prefs.AssertClosed();                                 // Assert that Pref storage is closed
}

void getDisplay(JsonObject doc) {
  doc["brightness"] = displayController.brightness.get();;
  doc["currScreen"] = displayController.currScreen.get();
  doc["timeZone"] = displayController.timeZone.get();
}

void putDisplay(JsonObject doc) {
  displayController.brightness.set(doc["brightness"], SetMode::flush);
  displayController.timeZone.set(doc["timeZone"], SetMode::flush);
}

void getRain(JsonObject doc) {
  doc["onThreshold"] = rainController.sw_Regensensor_ein.get();;
  doc["offThreshold"] = rainController.sw_Regensensor_aus.get();
  doc["distanceMultiplier"] = rainController.rainMulti.get();
  doc["afterRainOilingPulses"] = rainController.pump_nach_Regen.get();
}

void putRain(JsonObject doc) {
  rainController.sw_Regensensor_ein.set( doc["onThreshold"], SetMode::flush);
  rainController.sw_Regensensor_aus.set( doc["offThreshold"], SetMode::flush);
  rainController.rainMulti.set( doc["distanceMultiplier"], SetMode::flush);
  rainController.pump_nach_Regen.set( doc["afterRainOilingPulses"], SetMode::flush);
}

void getPump(JsonObject doc) {
  doc["pulsesPerMl"] = tankController.pumps_ml.get();
  doc["pulseOn"] = pumpController.zeit_pumpe_ein.get();
  doc["pulseOff"] = pumpController.zeit_pumpe_pause.get();
}

void putPump(JsonObject doc) {
  tankController.pumps_ml.set(doc["pulsesPerMl"], SetMode::flush);
  pumpController.zeit_pumpe_ein.set( doc["pulseOn"] , SetMode::flush);
  pumpController.zeit_pumpe_pause.set(doc["pulseOff"], SetMode::flush);
}

void getWifi(JsonObject doc) {
  doc["name"] = webController.ssid_ap.get();
  doc["password"] = webController.password_ap.get();
}

void putWifi(JsonObject doc) {
  const char *str=doc["name"];
  int len = strlen(str);
  if (len > 0) 
    webController.ssid_ap.set(str, SetMode::flush);
  str=doc["password"];
  webController.password_ap.set(str, SetMode::flush);
  // Serial.print("putWiFi: ssid: ");
  // Serial.print(webController.ssid_ap.get());
  // Serial.print(", pw: ");
  // Serial.println(webController.password_ap.get());
}

void getTank(JsonObject doc) {
  doc["content"] = tankController.tankinhalt_Aktuell.get();
  doc["capacity"] = tankController.tankinhalt_ml.get();
}

void putTank(JsonObject doc) {
  tankController.tankinhalt_ml.set(doc["capacity"], SetMode::flush);
}

void getSystem(JsonObject doc) {
  String rev = Rev_OILER;
  rev += ": ";
  rev += firmware_Vers;
  doc["rev"] = rev;
  doc["init"] = (bool) initFromPreferences.get();
}

void getStates(JsonObject doc) {
  doc["oiling"] = pumpController.isOiling();
  doc["extraOiling"] = distanceController.extraOilen;
  doc["raining"] = rainController.isRaining();
  doc["wifi"] = true; // ToDo:
  doc["washing"] = pumpController.getSpuelen();
  doc["pumpDistance"] = distanceController.pumpDistanz.get();
}

void putStates(JsonObject doc) {
  distanceController.extraOilen = doc["extraOiling"];
  pumpController.setSpuelen(doc["washing"]);
  distanceController.pumpDistanz.set(doc["pumpDistance"], SetMode::flush);
}

void getEmergency(JsonObject doc) {
  doc["timeout"] = gpsController.zeit_bis_notbetrieb.get();
  doc["speed"] = geschwindigkeit_Notbetrieb.get();
}

void putEmergency(JsonObject doc) {
  gpsController.zeit_bis_notbetrieb.set(doc["timeout"], SetMode::flush);
  geschwindigkeit_Notbetrieb.set(doc["speed"], SetMode::flush);
}

// Refill tank
void putTankReset(JsonObject doc) {
  tankController.reset();
}

// Force system defaults during next startup
void putSystemDefaults(JsonObject doc) {
  if (doc["init"].is<JsonVariant>())
    initFromPreferences.set(doc["init"], SetMode::flush); // need 0 value to force reinitialization
}

void getBackup(JsonObject doc) {
  getDisplay(doc["display"].to<JsonObject>());
  getEmergency(doc["emergency"].to<JsonObject>());
  getPump(doc["pump"].to<JsonObject>());
  getRain(doc["rain"].to<JsonObject>());
  getStates(doc["states"].to<JsonObject>());
  getSystem(doc["system"].to<JsonObject>());
  getTank(doc["tank"].to<JsonObject>());
  getWifi(doc["wifi"].to<JsonObject>());
}

// Restore
void restore(JsonObject doc) { 
  JsonObject o;
  o = doc["display"].as<JsonObject>();
  if (o != nullptr) {
    Serial.println("restore display");
    putDisplay(o);
  }
  o = doc["emergency"].as<JsonObject>();
  if (o != nullptr) {
    Serial.println("restore emergency");
    putEmergency(o);
  }
  o = doc["pump"].as<JsonObject>();
  if (o != nullptr) {
    Serial.println("restore pump");
    putPump(o);
  }
  o = doc["rain"].as<JsonObject>();
  if (o != nullptr) {
    Serial.println("restore rain");
    putRain(o);
  }
  o = doc["states"].as<JsonObject>();
  if (o != nullptr) {
    Serial.println("restore states");
    putStates(o);
  }
  o = doc["tank"].as<JsonObject>();
  if (o != nullptr) {
    Serial.println("restore tank");
    putTank(o);
  }
  o = doc["wifi"].as<JsonObject>();
  if (o != nullptr) {
    Serial.println("restore wifi");
    putWifi(o);
  }
}

JsonEndpoint *endpoints() {
  static JsonEndpoint ep[20] = {
    JsonEndpoint("/api/system",HTTP_GET, getSystem),
    JsonEndpoint("/api/display",HTTP_GET, getDisplay),
    JsonEndpoint("/api/display",HTTP_PUT, putDisplay),
    JsonEndpoint("/api/rain",HTTP_GET, getRain),
    JsonEndpoint("/api/rain",HTTP_PUT, putRain),
    JsonEndpoint("/api/pump",HTTP_GET, getPump),
    JsonEndpoint("/api/pump",HTTP_PUT, putPump),
    JsonEndpoint("/api/wifi",HTTP_GET, getWifi),
    JsonEndpoint("/api/wifi",HTTP_PUT, putWifi),
    JsonEndpoint("/api/tank",HTTP_GET, getTank),
    JsonEndpoint("/api/tank",HTTP_PUT, putTank),
    JsonEndpoint("/api/tank/reset",HTTP_PUT, putTankReset),
    JsonEndpoint("/api/states",HTTP_GET, getStates),
    JsonEndpoint("/api/states",HTTP_PUT, putStates),
    JsonEndpoint("/api/emergency",HTTP_GET, getEmergency),
    JsonEndpoint("/api/emergency",HTTP_PUT, putEmergency),
    JsonEndpoint("/api/system/defaults",HTTP_PUT, putSystemDefaults),
    JsonEndpoint("/api/backup",HTTP_GET, getBackup),
    JsonEndpoint("/api/backup",HTTP_POST, restore),
    JsonEndpoint(nullptr,HTTP_GET, nullptr)
  };
  return ep;
};
