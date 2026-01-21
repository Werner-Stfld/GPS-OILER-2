#undef test
#ifdef test
#include <TFT_eSPI.h>
#include "ScreenBase.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

extern void *_spi_user;

void setup() {
  
  delay(500); // wird auf dem c3 benötigt, wenn CDC eingeschaltet ist.
  Serial.begin(115200);
  delay(300);
  while (!Serial) {
    delay(10);
  }
  //ToDo: Check Wire.begin();
  Serial.println("setup 01");
  Serial.println(USER_SETUP_ID);

  Serial.print("MOSI: ");
  Serial.println(TFT_MOSI);
  Serial.print("SCLK: ");
  Serial.println(TFT_SCLK);
  Serial.print("DC: ");
  Serial.println(TFT_DC);
  Serial.print("CS: ");
  Serial.println(TFT_CS);

  tft.init(INITR_GREENTAB2);

  Serial.println((unsigned long)_spi_user,HEX);
  tft.setRotation(1);
  tft.fillScreen(tft.color565(0,255,0));
  spr.createSprite(TFT_HEIGHT, TFT_WIDTH);  // Vollbild-Sprite
}

uint8_t i=0;
void IRAM_ATTR  loop() {
  delay(100);
  
  spr.fillRect(0,0,160,40, tft.color565(i,0,0));
  spr.fillRect(0,41,160,40,tft.color565(0,i,0));
  spr.fillRect(0,81,160,40, tft.color565(0,0,i));
  spr.setTextColor(TFT_GREEN, TFT_BLACK);
  spr.setTextFont(4);
  String txt = "FF: ";
  txt += i++;

  Serial.println(txt.c_str());
  spr.drawString(txt.c_str(), 1, 10);
  spr.pushSprite(0, 0);  // In einem Rutsch aufs Display
}
#else
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
IntVar initFromPreferences = IntVar(String("Factory Reset:"),0,PrefKeys::init_from_preferences); // flag zum Rücksetzen der Einstellungen

// The endpoints for the web-server
JsonEndpoint *getEndpoints(); 
JsonEndpoint *putEndpoints();

// The controllers
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
VoltageController voltageController = VoltageController();

// Variable für die Seriellen Eingabe
String serialInput = ""; // a String to hold incoming data
void serialStatus();

void tankReset() {
  Serial.println("Tank Reset");
  tankController.reset();
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

// setup the oiler scetch

void setup()
{
  delay(500); // wird auf dem c3 benötigt, wenn CDC eingeschaltet ist.
  Serial.begin(115200);
  delay(300);

  initFromPreferences.restore();
  if (initFromPreferences.get()==0) // Default after flush of memory. May also be forced by web UI.
  { // flush all vars with initial values of the userVars
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

  // setups are loading the userVars from the preference storage
  webController.setup(getEndpoints(), putEndpoints());
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
  displayController.OnWiFiReset(wiFiReset);
  displayController.OnSettingsReset(settingsReset);
  displayController.OnWiFiOnOff(wiFiOnOff);

  serialStatus();

  pinMode(BUTTON_PIN, INPUT);
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

Timer secondTimer = Timer(300);
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
  float oilingSpeed = 0.0;
  if (secondTimer.timedOut())
  {
    geschwindigkeit.set(gpsController.speed());
    oilingSpeed = gpsController.speed(); // oilingSpeed: speed to evaluate distance for oiling
    oilingSpeed = gpsController.emergency()? geschwindigkeit_Notbetrieb.get(): // if no gps available after startup => use emergency speed
                                             gpsController.speed(); 
    oilingSpeed = rainController.getSpeed(oilingSpeed); 
    distanceController.update(oilingSpeed, gpsController.speed());
    pumpController.RequestPulses(distanceController.pulses() + rainController.pulses()); // pass requested pulses to pumpController

    if (minuteTimer.timedOut() || standStillEdgeDetected(!gpsController.emergency(), gpsController.speed())) { 
      // Flush automatically changing values once per minute or when gps is active indicating speed less then 2 km/h
      distanceController.Gefahrene_km.flush();
      tankController.tankinhalt_Aktuell.flush();
      distanceController.oilingDistance.flush();
      rainController.raining.flush();
      pumpController.pendingPulses.flush();
    }

    // update display data
    displayController.setBatteryVoltage(voltageController.voltage());
  }

  pumpController.loop(oilingSpeed);                     // process pump requests
  displayController.loop(digitalRead(BUTTON_PIN)==LOW); // display and input button handling
  webController.loop();                                 // process web requests
  prefs.AssertClosed();                                 // Assert that Pref storage is closed
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
  if (displayController.currScreen.evaluate(input)) return; // Startdisplay 1
  if (displayController.brightness.evaluate(input)) return; // Startdisplay 2
  if (geschwindigkeit.evaluate(input))return; // Geschwindigkeit
  if (rainController.rainMulti.evaluate(input)) return; // RainMulti

  if (input.startsWith("Status")) // Status ausgeben
  {
    serialStatus();
    return;
  }
  
  if (input.startsWith("Set_Factory")||input.startsWith("EEPROM_init"))
  {
    initFromPreferences.set(0, SetMode::flush);
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
  displayController.currScreen.status();
  displayController.brightness.status();
  rainController.rainMulti.status();
  geschwindigkeit.status();
  tankController.tankinhalt_Aktuell.status();
  initFromPreferences.status();
}

void getDisplay(JsonDocument &doc) {
  doc["brightness"] = displayController.brightness.get();;
  doc["currScreen"] = displayController.currScreen.get();
  doc["oilSymbol"] = displayController.oilsymbol_Zeit.get();
  doc["timeZone"] = displayController.timeZone.get();
}

void putDisplay(JsonDocument &doc) {
  displayController.brightness.set(doc["brightness"], SetMode::flush);
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
  doc["init"] = (bool) initFromPreferences.get();
}

void getStates(JsonDocument &doc) {
  doc["oiling"] = pumpController.isOiling();
  doc["extraOiling"] = distanceController.extraOilen;
  doc["emergency"] = gpsController.emergency();
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
    initFromPreferences.set(doc["init"], SetMode::flush); // need 0 value to force reinitialization
}

JsonEndpoint *getEndpoints() {
  static JsonEndpoint ep[9] = {
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
  return ep;
};

JsonEndpoint *putEndpoints() {
  static JsonEndpoint ep[10] = {
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
  return ep;
};
#endif
