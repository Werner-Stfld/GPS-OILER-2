#include <arduino.h>
#include "globals.h"

const char *Rev_OILER = "GPS-OILER-2"; // Bezeichnung der Hardware
const char *firmware_Vers = "3.0.1-develop"; // Aktuelle Firmwareversion

#if FALSE
const int RAIN_SENSOR_PIN = A0; // Pin für Regensensor
const int RAIN_SENSOR_VOLTAGE_PIN = 15;    // Spannung für den Regensensor
const int WLAN_RESET_PIN = 12;  // Taster zum Resetten des AP, Setzt ssid auf "OILER" und Passwort wird deaktiviert

const int OIL_PIN = 0;       // Pin an dem die Pumpe angesteuert wird
const int LED_PIN = 2;       // LED auf der Platine
const int U_VCC2 = 13;      // TODO: Check: Unused! Prüft die Spannung an Klemme 15 
const int RXPin = 14;       // Für Software Serial
const uint32_t GPSBaud = 9600; // Baudrate für Software Serial
#endif
