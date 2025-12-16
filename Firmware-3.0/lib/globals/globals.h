#pragma once

extern const char *Rev_OILER; // Bezeichnung der Hardware
extern const char *firmware_Vers; // Aktuelle Firmwareversion

#undef HW_PINS_DEFINED

#ifdef HW_PINS_DEFINED
extern const int RAIN_SENSOR_PIN; // Pin für Regensensor
extern const int RAIN_SENSOR_VOLTAGE_PIN;    // Spannung für den Regensensor
extern const int WLAN_RESET_PIN;  // Taster zum Resetten des AP, Setzt ssid auf "OILER" und Passwort wird deaktiviert

extern const int OIL_PIN;       // Pin an dem die Pumpe angesteuert wird
extern const int LED_PIN;       // LED auf der Platine
extern const int U_VCC2;      // TODO: Check: Unused! Prüft die Spannung an Klemme 15 
extern const int RXPin;       // Für Software Serial
const uint32_t GPSBaud; // Baudrate für Software Serial
#endif

