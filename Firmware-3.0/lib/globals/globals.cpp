#include <arduino.h>
#include "globals.h"

const char *Rev_OILER = "GPS-OILER-2"; // Bezeichnung der Hardware
const char *firmware_Vers = "3.0.1-develop"; // Aktuelle Firmwareversion

const int RAIN_SENSOR_PIN = A1; // Pin für Regensensor
const int VCC_SENSOR_PIN = A3; // Pin für VCC Messung

const int WLAN_RESET_PIN = 20;  // Taster zum Resetten des AP, Setzt ssid auf "OILER" und Passwort wird deaktiviert

const int RXPin = 7;       // Für Software Serial
const uint32_t GPSBaud = 9600; // Baudrate für Software Serial

const int OIL_PIN = 2;       // Pin an dem die Pumpe angesteuert wird

