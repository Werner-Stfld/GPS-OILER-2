#pragma once

#include <Arduino.h>                  
#include <TinyGPS++.h>                // Für das GPS
#include <SoftwareSerial.h>           // Wird für den Serial für den GPS-Empfänger benötigt
struct gpsTime {
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

class GpsController {
  TinyGPSPlus gps;                   // gps data interpreter
  SoftwareSerial gpsSerial;
  uint32 rxBaudrate;
  unsigned long tmo;                // waiting to complete initialization
  bool _gpsStarted = false;
  public:
  IntUserVar minGeschwindigkeit = IntUserVar(String("Min. Geschwindigkeit:"), 20, eepromAddr::minGeschwindigkeit); // Mindestgeschwindigkeit zum Ölen
  IntUserVar zeit_bis_notbetrieb = IntUserVar(String("Zeit bis Notbetrieb:"), 180, eepromAddr::zeit_bis_notbetrieb);       // Zeit bis Notbetrieb in Sekunden

  bool gpsStarted() {
    return _gpsStarted;
  }
  bool gpsAvailable() {
    return gps.speed.isValid() && gps.course.isValid() && gps.satellites.isValid() && gps.satellites.value()>=3;
  }

  // int state = 0;
  // bool read(uint &vSattelites, float &vSpeed, uint &vCourse, gpsTime &vTime) {
  //   if (state < 5) {
  //     state++;
  //     vSattelites=4;
  //     vSpeed=120;
  //     vCourse=165;
  //     vTime=gpsTime{9,5,13};;
  //     return true;
  //   }
  //   state = state >=10?0:state+1;
  //   vSattelites=5;
  //   vSpeed=0;
  //   vCourse=285;
  //   vTime=gpsTime{21,31,59};;
  //   return true;
  // }

  bool read(uint &vSattelites, float &vSpeed, uint &vCourse, gpsTime &vTime) {
    vSattelites=0;
    vSpeed=0;
    vCourse=0;
    vTime=gpsTime{0,0,0};

    if (!gps.speed.isValid())
      return false;
    vSpeed = gps.speed.kmph();
    if (vSpeed < minGeschwindigkeit.get())
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

  GpsController(int rxPin, uint32 baudrate): gpsSerial(SoftwareSerial(rxPin)) {
    rxBaudrate = baudrate;
  }

  void flush() { // reset eeprom vars
    minGeschwindigkeit.flush();
    zeit_bis_notbetrieb.flush();
  }

  void setup() {
    minGeschwindigkeit.read();
    zeit_bis_notbetrieb.read();
    gpsSerial.begin(rxBaudrate);
    _gpsStarted = false;
    tmo = millis() + zeit_bis_notbetrieb.get() * 1000; // 180 Sekunden bis Init abgeschlossen sein sollte.
  }

  void loop() {
    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }
    if (_gpsStarted) 
      return;
    if (millis() > tmo)
      _gpsStarted = true;
  }
};