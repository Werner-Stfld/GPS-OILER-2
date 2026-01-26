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

void getDisplay(JsonDocument &doc) {
  doc["brightness"] = displayController.brightness.get();;
  doc["currScreen"] = displayController.currScreen.get();
  doc["timeZone"] = displayController.timeZone.get();
}

void putDisplay(JsonDocument &doc) {
  displayController.brightness.set(doc["brightness"], SetMode::flush);
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
}

void putWifi(JsonDocument &doc) {
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
  doc["raining"] = rainController.isRaining();
  doc["wifi"] = true; // ToDo:
  doc["washing"] = pumpController.getSpuelen();
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
}

// Force system defaults during next startup
void putSystemDefaults(JsonDocument &doc) {
  if (doc["init"].is<JsonVariant>())
    initFromPreferences.set(doc["init"], SetMode::flush); // need 0 value to force reinitialization
}

JsonEndpoint *endpoints() {
  static JsonEndpoint ep[18] = {
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
    JsonEndpoint(nullptr,HTTP_GET, nullptr)
  };
  return ep;
};
#endif
