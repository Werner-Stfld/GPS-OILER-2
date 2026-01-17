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

  Timer emergencyTimer = Timer(0);                // timer to delay emergency state
  Timer updateTimer = Timer(0);                // timer to poll gps

  bool _emergency = false;
  uint _sattelites;
  float _speed;
  uint _course;
  gpsTime _time;
  int _alt;

  public:
  IntVar zeit_bis_notbetrieb = IntVar(String("Zeit bis Notbetrieb:"), 180, PrefKeys::zeit_bis_notbetrieb);       // Zeit bis Notbetrieb in Sekunden

  bool emergency() {
    return _emergency;
  }

  bool speed() {
    return _speed;
  }

  int altitude() {
    return _alt;
  }

  uint course() {
    return _course;
  }
  gpsTime time() {
    return _time;
  }

  uint sattelites() {
    return _sattelites;
  }

  GpsController(int _rxPin, uint baudrate): gpsSerial(Serial1) {
    rxBaudrate = baudrate;
    rxPin = _rxPin;
    add(&zeit_bis_notbetrieb);
  }

  void setup() {
    restore();
    gpsSerial.begin(rxBaudrate, SERIAL_8N1, rxPin);
    _sattelites = 0;
    _speed = 0.0;
    _course = 0;
    _time = gpsTime{0,0,0};
    _alt = 0;
    _emergency = false;
    emergencyTimer = Timer(zeit_bis_notbetrieb.get() * 1000);
    updateTimer = Timer(500);
  }

  const bool gpsEmulation = false;

  void loop() {

    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }

    if (updateTimer.timedOut()) {
      if (gpsEmulation) {
        emulatedRead ();
        return;
      }
      if (gps.speed.isValid() && gps.satellites.isValid() && gps.satellites.value()>=3) {

    #if false
        Serial.println("GPS");
        Serial.println(gps.charsProcessed());
        Serial.print("sattelites.isValid: ");
        Serial.println(gps.satellites.isValid());
        Serial.print("sattelites: ");
        Serial.println(gps.satellites.value());
    #endif
        _speed = gps.speed.kmph();
        if (_speed < 2) // don't flicker if too slow
          _speed = 0;  

        if (gps.altitude.isValid()) 
          _alt = gps.altitude.meters();

        if (gps.course.isValid()) {
          _course = unifyCourse(gps.course.deg());
        }

        _sattelites = gps.satellites.value();
        if (gps.time.isValid()) {
          _time.hour = gps.time.hour();
          _time.minute = gps.time.minute();
          _time.second = gps.time.second();
        } 
        _emergency = false;
        // retrigger emergency timer
        emergencyTimer.nextTimeout(zeit_bis_notbetrieb.get() * 1000);
        return;
      } 
    }
    if (emergencyTimer.timedOut()) {
      _emergency = true;
      _sattelites = 0;
      _speed = 0.0;
      _course = 0;
      _time = gpsTime{0,0,0};
      _alt = 0;
    }
  }

  int state = 0;
  int aSpeed[10] = {0,5,60,90,120,150,180,210,240,270};
  int aCourse[10] = {0,5,60,90,120,150,180,210,240,270};
  int aAlt[10] = {-5, 0, 123, 234, 850, 1254, 2589, 3456, 6543, 8167};
  bool emulatedRead() {
    if (state < 5) {
      _emergency = false;
      _sattelites = 4;
      _speed = aSpeed[state];
      _course = aCourse[state];
      _time = gpsTime{9,5,13};
      _alt = aAlt[state];
      state++;
      return true;
    }
    _sattelites=5;
    _speed=aSpeed[state];
    _course=aCourse[state];
    _alt=aAlt[state];
    _time=gpsTime{21,31,59};;
    state++;
    if (state >=10) state = 0;
    return true;
  }

  static double unifyCourse (double v) {
    if (v >= 0 || v < 360)
      return v;

    bool negative = v < 0;        // v: -410 => negative = true;
    if (negative)
      v = -v;                     // v = 410;
    int multipleOf360 = v/360;    // => 1;
    v = v - 360 * multipleOf360;  // v = 50;
    if (negative) 
      v = 360 - v;                // 310;
    return v;
  }
};
