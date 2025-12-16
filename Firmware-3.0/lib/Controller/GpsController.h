#pragma once

#include <Arduino.h>                  
#include <TinyGPS++.h>                // Für das GPS

struct gpsTime {
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

class GpsController: public VarContainer {
  TinyGPSPlus gps;                   // gps data interpreter
  HardwareSerial gpsSerial;
  int rxPin;
  uint rxBaudrate;
  unsigned long tmo;                // waiting to complete initialization
  bool _gpsStarted = false;
  public:
  IntVar zeit_bis_notbetrieb = IntVar(String("Zeit bis Notbetrieb:"), 180, PrefKeys::zeit_bis_notbetrieb);       // Zeit bis Notbetrieb in Sekunden

  bool gpsStarted() {
    return _gpsStarted;
  }
  bool gpsAvailable() {
    return gps.speed.isValid() && gps.course.isValid() && gps.satellites.isValid() && gps.satellites.value()>=3;
  }

  int state = 0;
  int aSpeed[10] = {0,5,60,90,120,150,180,210,240,270};
  int aCourse[10] = {0,5,60,90,120,150,180,210,240,270};
  bool emulatedRead(uint &vSattelites, float &vSpeed, uint &vCourse, gpsTime &vTime) {
    if (state < 5) {
      vSattelites=4;
      vSpeed=aSpeed[state];
      vCourse=aCourse[state];
      vTime=gpsTime{9,5,13};;
      state++;
      return true;
    }
    vSattelites=5;
    vSpeed=aSpeed[state];
    vCourse=aCourse[state];
    vTime=gpsTime{21,31,59};;
    state++;
    if (state >=10) state = 0;
    return true;
  }

  const bool gpsEmulation = true;
  bool read(uint &vSattelites, float &vSpeed, uint &vCourse, gpsTime &vTime) {
    if (gpsEmulation) {
      return emulatedRead(vSattelites, vSpeed, vCourse, vTime);
    }
    vSattelites=0;
    vSpeed=0;
    vCourse=0;
    vTime=gpsTime{0,0,0};

    if (!gps.speed.isValid())
      return false;
    if (gps.speed.age() > 10000)
      return false; // More than 10 seconds no update
    vSpeed = gps.speed.kmph();
    if (vSpeed < 2) // don't flicker if too slow
      vSpeed = 0;
    if (!gps.course.isValid())
      return false;
    vCourse = gps.course.deg();
    if (vCourse < 0 || vCourse >359)
      vCourse = unifyCourse(vCourse);
    if (!gps.satellites.isValid())
      return false;
    vSattelites = gps.satellites.value();
    if (vSattelites >= 3) 
      _gpsStarted = true;
    if (gps.time.isValid()) {
      vTime.hour = gps.time.hour();
      vTime.minute = gps.time.minute();
      vTime.second = gps.time.second();
    } 
    return vSattelites >= 3;
  }

  static double unifyCourse (double v) {
    bool negative = v < 0;        // v: -410 => negative = true;
    if (negative)
      v = -v;                     // v = 410;
    int multipleOf360 = v/360;    // => 1;
    v = v - 360 * multipleOf360;  // v = 50;
    if (negative) 
      v = 360 - v;                // 310;
    return v;
  }
  
#ifdef HW_PINS_DEFINED
  GpsController(int _rxPin, uint baudrate): gpsSerial(Serial1) {
    rxBaudrate = baudrate;
    rxPin = _rxPin;
    add(&zeit_bis_notbetrieb);
  }
#else
  GpsController() : gpsSerial(Serial1) {
    add(&zeit_bis_notbetrieb);
  }
#endif


  void setup() {
    restore();
#ifdef HW_PINS_DEFINED
    gpsSerial.begin(rxBaudrate, rxPin);
#endif
    _gpsStarted = false;
    tmo = millis() + zeit_bis_notbetrieb.get() * 1000; // 180 Sekunden bis Init abgeschlossen sein sollte.
  }

  void loop() {

#ifdef HW_PINS_DEFINED
    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }
#endif
    if (_gpsStarted) 
      return;
    if (millis() > tmo)
      _gpsStarted = true;
  }
};