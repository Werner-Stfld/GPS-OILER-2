#pragma once

#include <Arduino.h>                  
#include <TinyGPS++.h>                // Für das GPS

struct gpsTime {
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

struct gpsDate {
  uint8_t day;
  uint8_t month;
  uint16_t year;
};
struct gpsLocation {
  double lat;
  double lng;
};

class GpsController: public VarContainer {
  TinyGPSPlus gps;                   // gps data interpreter
  HardwareSerial gpsSerial;
  int rxPin;
  uint rxBaudrate;

  Timer updateTimer = Timer(0);                // timer to poll gps

  bool _validData = false;
  uint _sattelites;
  float _speed;
  uint _course;
  gpsTime _time;
  gpsDate _date;
  int _alt;
  gpsLocation _location;
  Timer _emergencyTimer = Timer(0);

  public:
  IntVar zeit_bis_notbetrieb = IntVar(180, PrefKeys::zeit_bis_notbetrieb);       // Zeit bis Notbetrieb in Sekunden

  bool validData() {
    return _validData;
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
  gpsDate date () {
    return _date;
  }
  gpsLocation location () {
    return _location;
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
    _date = gpsDate{0,0,0};
    _location = gpsLocation{0,0};
    _alt = 0;
    _validData = false;
    updateTimer = Timer(500);
    _emergencyTimer = Timer(zeit_bis_notbetrieb.get()*1000);
  }

  const bool gpsEmulation = false;

  void loop() {

    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }

    if (!updateTimer.timedOut()) {
      return;
    }
    if (gpsEmulation) {
      emulatedRead ();
      return;
    }
    #if false
    Serial.println("GPS");
    Serial.println(gps.charsProcessed());
    Serial.print("speed.age: ");
    Serial.println(gps.speed.age());
    Serial.print("sattelites.isValid: ");
    Serial.println(gps.satellites.isValid());
    Serial.print("sattelites: ");
    Serial.println(gps.satellites.value());
    #endif
    if (!gps.satellites.isValid()) { // wait until gps modul has initialized
      return;
    }
    int tmo = zeit_bis_notbetrieb.get() * 1000;

    _validData = gps.speed.age() < tmo;

    if (_validData) {
      if (gps.speed.isValid()) 
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
        _date.day = gps.date.day();
        _date.month = gps.date.month();
        _date.year = gps.date.year();
      } 
      if (gps.location.isValid()){
        _location.lat = gps.location.lat();
        _location.lng = gps.location.lng();
      }
      return;
    }
    _sattelites = 0;
    _speed = 0.0;
    _course = 0;
    _time = gpsTime{0,0,0};
    _date = gpsDate{0,0,0};
    _alt = 0;
  }

  int state = 0;
  int aSpeed[10] = {0,5,60,90,120,150,180,210,240,270};
  int aCourse[10] = {0,5,60,90,120,150,180,210,240,270};
  int aAlt[10] = {-5, 0, 123, 234, 850, 1254, 2589, 3456, 6543, 8167};
  bool emulatedRead() {
    if (state < 5) {
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

extern GpsController gpsController;